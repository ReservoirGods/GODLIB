/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dLZ77_BUILDHASH( _a ) (((U16)(_a[0]))<<8) + (((U16)(_a[1]))<<4) ^ ((U16)(_a[2]))


/* ###################################################################################
#  DATA
################################################################################### */

S32		gGodPackLz77HashTable[ 0x10000L ];
S32 *	gpGodPackLz77LinkList;

U32 	gGodPackLz77CountHistogram[ 32 ];
U32 	gGodPackLz77LiteralHistogram[ 32 ];
U32 	gGodPackLz77StringHistogram[ 32 ];
U32 	gGodPackLz77OffsetHistogram[ 32 ];
U32 	gGodPackLz77OffsetCountHistogram[ 16 ][ 16 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	GodPack_Lz77_StatsInit( void );
void	GodPack_Lz77_StatsDeInit( void );
void	GodPack_Lz77_StatsLiteralsUpdate( const U32 aLitCount );
void	GodPack_Lz77_StatsOffsetUpdate( const U32 aOffset );
void	GodPack_Lz77_StatsOffsetCountUpdate( const U32 aOffset, const U32 aCount );
void	GodPack_Lz77_StatsCountUpdate( const U32 aCount );
void	GodPack_Lz77_StatsStringUpdate( const U32 aCount );
U16		GodPack_Lz77_GetBitCount( const U32 aNumber );
void	GodPack_Lz77_LiteralRunOutput( sGodPackBitStream * apStream, U8 * apStart, U32 aCount );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77_Encode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Lz77_Encode
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Lz77_Encode( const void * apSrc,void * apDst,const U32 aSize )
{
	U8 *	lpSrc;
	S32		lWindowSize;
	S32		lBackStart;
	S32		lBackOffBest;
	S32		lBackCountBest;
	S32		lCount;
	U32		i;
	S32		j;
	U16		lHash;
	U32		lStringCount;
	U32		lLiteralRun;
	U32		lLiteralStart;
	U32		lStringRun;
	U16		lBitsOff;
	U16		lBitsCount;
	sGodPackBitStream	lStream;


	lpSrc = (U8*)apSrc;
	lStringCount = 0;

	gpGodPackLz77LinkList = (S32*)mMEMCALLOC( sizeof(U32) * aSize );

	for( i=0; i<0x10000L; i++ )
	{
		gGodPackLz77HashTable[ i ] = -1;
	}
	for( i=0; i<aSize; i++ )
	{
		gpGodPackLz77LinkList[ i ] = -1;
	}

	GodPack_Lz77_StatsInit();

	GodPack_BitStream_Init( &lStream, apDst, aSize );

	i           = 0;
	lWindowSize = 0;
	lLiteralRun = 0;
	lStringRun = 0;
	lLiteralStart = 0;
	while( i < aSize )
	{
		lBackStart     = 1;
		lBackOffBest   = 0;
		lBackCountBest = 0;

		lHash      = (U16)(dLZ77_BUILDHASH( (&lpSrc[i]) ));
		lBackStart = gGodPackLz77HashTable[ lHash ];


		while( lBackStart >= 0 )
		{
			lCount   = 0;
			if( (lpSrc[ lBackStart ] == lpSrc[ i ]) && ((S32)(i-lBackStart) < lWindowSize) && ((i-lBackStart)>0) )
			{
				j = 0;
				while( (lpSrc[ lBackStart + j ] == lpSrc[ i + j ]) && ((i+j) <aSize) )
				{
					j++;
					lCount++;
				}
				if( lCount > lBackCountBest )
				{
					lBackOffBest   = i - lBackStart;
					lBackCountBest = lCount;
				}
			}
			lBackStart = gpGodPackLz77LinkList[ lBackStart ];
		}

		if( lBackCountBest > 2 )
		{
			lBackCountBest -= 3;

			GodPack_Lz77_LiteralRunOutput( &lStream, &lpSrc[ lLiteralStart ], lLiteralRun );
			lLiteralRun = 0;

			GodPack_Lz77_StatsOffsetCountUpdate( lBackOffBest, lBackCountBest );
			GodPack_Lz77_StatsOffsetUpdate( lBackOffBest );
			GodPack_Lz77_StatsCountUpdate( lBackCountBest );

			lStringCount++;
			GodPack_BitStream_BitWrite( &lStream, 1 );

			if( lBackCountBest >= dGODPACK_LZ77_COUNT_LIMIT )
			{
				lBackCountBest = dGODPACK_LZ77_COUNT_LIMIT-1;
			}

			lBitsOff   = GodPack_Lz77_GetBitCount( lBackOffBest   );
			lBitsCount = GodPack_Lz77_GetBitCount( lBackCountBest );

			if( (lBitsOff <=dGODPACK_LZ77_OFF0_BITS) && (lBitsCount <=dGODPACK_LZ77_COUNT0_BITS) )
			{
				GodPack_BitStream_BitWrite(  &lStream, 0 );
				GodPack_BitStream_BitsWrite( &lStream, lBackOffBest,   dGODPACK_LZ77_OFF0_BITS    );
				GodPack_BitStream_BitsWrite( &lStream, lBackCountBest, dGODPACK_LZ77_COUNT0_BITS  );
			}
			else
			{
				GodPack_BitStream_BitWrite(  &lStream, 1 );
				GodPack_BitStream_BitsWrite( &lStream, lBackOffBest,   dGODPACK_LZ77_OFF1_BITS );
				GodPack_BitStream_BitsWrite( &lStream, lBackCountBest, dGODPACK_LZ77_COUNT1_BITS  );
			}

			lBackCountBest += 3;
			while( lBackCountBest )
			{
				lHash = (U16)(dLZ77_BUILDHASH( (&lpSrc[i]) ));
				gpGodPackLz77LinkList[ i ] = gGodPackLz77HashTable[ lHash ];
				gGodPackLz77HashTable[ lHash ] = i;
				lBackCountBest--;
				i++;
			}
			lStringRun++;
		}
		else
		{
			GodPack_Lz77_StatsStringUpdate( lStringRun );
			lStringRun = 0;
			if( !lLiteralRun )
			{
				lLiteralStart = i;
			}
			lLiteralRun++;

			gpGodPackLz77LinkList[ i ] = gGodPackLz77HashTable[ lHash ];
			gGodPackLz77HashTable[ lHash ] = i;

			i++;
			lWindowSize++;
		}
		if( lWindowSize > dGODPACK_LZ77_WINDOW_SIZE )
		{
			lWindowSize = dGODPACK_LZ77_WINDOW_SIZE;
		}
	}

	GodPack_Lz77_LiteralRunOutput( &lStream, &lpSrc[ lLiteralStart ], lLiteralRun );

	GodPack_BitStream_DeInit( &lStream );

	GodPack_Lz77_StatsDeInit();
	mMEMFREE( gpGodPackLz77LinkList );

	return( lStream.mOffset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsInit(void)
* ACTION   : GodPack_Lz77_StatsInit
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsInit(void)
{
	U16	i,j;

	for( i=0; i<32; i++ )
	{
		gGodPackLz77CountHistogram[ i ] = 0;
		gGodPackLz77LiteralHistogram[ i ] = 0;
		gGodPackLz77OffsetHistogram[ i ] = 0;
	}
	for( i=0; i<16; i++ )
	{
		for( j=0; j<16; j++ )
		{
			gGodPackLz77OffsetCountHistogram[ i ][ j ] = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsDeInit(void)
* ACTION   : GodPack_Lz77_StatsDeInit
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsDeInit(void)
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77_GetBitCount( const U32 aNumber )
* ACTION   : GodPack_Lz77_GetBitCount
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	GodPack_Lz77_GetBitCount( const U32 aNumber )
{
	U16	lBitCount;
	U32	lNumber;

	lBitCount = 0;
	lNumber   = aNumber;

	while( lNumber )
	{
		lNumber >>= 1L;
		lBitCount++;
	}

	return( lBitCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsLiteralsUpdate(const U32 aLitCount)
* ACTION   : GodPack_Lz77_StatsLiteralsUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsLiteralsUpdate(const U32 aLitCount)
{
	gGodPackLz77LiteralHistogram[ GodPack_Lz77_GetBitCount(aLitCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsOffsetUpdate(const U32 aOffset)
* ACTION   : GodPack_Lz77_StatsOffsetUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsOffsetUpdate(const U32 aOffset)
{
	gGodPackLz77OffsetHistogram[ GodPack_Lz77_GetBitCount(aOffset) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsCountUpdate(const U32 aCount)
* ACTION   : GodPack_Lz77_StatsCountUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsCountUpdate(const U32 aCount)
{
	gGodPackLz77CountHistogram[ GodPack_Lz77_GetBitCount(aCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsOffsetCountUpdate(const U32 aOffset,const U32 aCount)
* ACTION   : GodPack_Lz77_StatsOffsetCountUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsOffsetCountUpdate(const U32 aOffset,const U32 aCount)
{
	gGodPackLz77OffsetCountHistogram[ GodPack_Lz77_GetBitCount(aOffset) ][ GodPack_Lz77_GetBitCount(aCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_StatsStringUpdate(const U32 aCount)
* ACTION   : GodPack_Lz77_StatsStringUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_StatsStringUpdate(const U32 aCount)
{
	gGodPackLz77StringHistogram[ GodPack_Lz77_GetBitCount(aCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77_LiteralRunOutput(sGodPackBitStream * apStream,U8 * apStart,U32 aCount)
* ACTION   : GodPack_Lz77_LiteralRunOutput
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77_LiteralRunOutput(sGodPackBitStream * apStream,U8 * apStart,U32 aCount)
{
	U16	j;

	GodPack_Lz77_StatsLiteralsUpdate( aCount);
	if( aCount )
	{
		while( aCount >= 8 )
		{
			GodPack_BitStream_BitWrite( apStream, 0 );
			GodPack_BitStream_BitsWrite( apStream, 7, 3 );
			for( j=0; j<8; j++ )
			{
				GodPack_BitStream_ByteWrite( apStream, *apStart );
				apStart++;
			}
			aCount -= 8;
		}
		if( aCount )
		{
			GodPack_BitStream_BitWrite( apStream, 0 );
			GodPack_BitStream_BitsWrite( apStream, aCount-1, 3 );
			while( aCount )
			{
				GodPack_BitStream_ByteWrite( apStream, *apStart );
				apStart++;
				aCount--;
			}
		}
	}
}


/* ################################################################################ */
