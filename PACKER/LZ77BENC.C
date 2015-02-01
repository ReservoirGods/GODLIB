/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dLZ77B_BUILDHASH( _a ) (((U16)(_a[0]))<<8) + (((U16)(_a[1]))<<4) ^ ((U16)(_a[2]))


/* ###################################################################################
#  DATA
################################################################################### */

S32		gGodPackLz77bHashTable[ 0x10000L ];
S32 *	gpGodPackLz77bLinkList;

U32		gGodPackLz77bSymbol0Count;
U32		gGodPackLz77bSymbol1Count;
U32		gGodPackLz77bSymbol2Count;
U32 	gGodPackLz77bCountHistogram[ 32 ];
U32 	gGodPackLz77bLiteralHistogram[ 32 ];
U32 	gGodPackLz77bStringHistogram[ 32 ];
U32 	gGodPackLz77bOffsetHistogram[ 32 ];
U32 	gGodPackLz77bOffsetCountHistogram[ 16 ][ 16 ];
U32 	gGodPackLz77bOffsetWeightHistogram[ 16 ][ 16 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

#if	0

void	GodPack_Lz77b_StatsInit( void );
void	GodPack_Lz77b_StatsDeInit( void );
void	GodPack_Lz77b_StatsLiteralsUpdate( const U32 aLitCount );
void	GodPack_Lz77b_StatsOffsetUpdate( const U32 aOffset );
void	GodPack_Lz77b_StatsOffsetCountUpdate( const U32 aOffset, const U32 aCount );
void	GodPack_Lz77b_StatsCountUpdate( const U32 aCount );
void	GodPack_Lz77b_StatsStringUpdate( const U32 aCount );
U16		GodPack_Lz77b_GetBitCount( const U32 aNumber );

#else

#define	GodPack_Lz77b_StatsInit()
#define	GodPack_Lz77b_StatsDeInit()
#define	GodPack_Lz77b_StatsLiteralsUpdate( a )
#define	GodPack_Lz77b_StatsOffsetUpdate( a )
#define	GodPack_Lz77b_StatsOffsetCountUpdate( a, b )
#define	GodPack_Lz77b_StatsCountUpdate( a );
#define	GodPack_Lz77b_StatsStringUpdate( a );
#define	GodPack_Lz77b_GetBitCount( a )	0


#endif

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77b_Encode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Lz77b_Encode
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Lz77b_Encode( const void * apSrc,void * apDst,const U32 aSize )
{
	U8 *	lpSrc;
	U8 *	lpDst;
	S32		lWindowSize;
	S32		lBackStart;
	S32		lBackOffBest;
	S32		lBackCountBest;
	S32		lBackOff;
	S32		lCount;
	U32		i;
	S32		j;
	S32		lCost;
	S32		lBackCostBest;
	U16		lHash;
	U32		lStringCount;
	U32		lLiteralRun;
	U32		lLiteralStart;
	U32		lStringRun;
	U32		lDstOff;

	DebugLog_Init( "LZ77B.LOG" );

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;

	lDstOff      = 0;
	lStringCount = 0;

	gpGodPackLz77bLinkList = (S32*)mMEMCALLOC( sizeof(U32) * aSize );

	for( i=0; i<0x10000L; i++ )
	{
		gGodPackLz77bHashTable[ i ] = -1;
	}
	for( i=0; i<aSize; i++ )
	{
		gpGodPackLz77bLinkList[ i ] = -1;
	}

	GodPack_Lz77b_StatsInit();

	i           = 0;
	lWindowSize = 0;
	lLiteralStart = 0;
	lLiteralRun = 0;
	lStringRun = 0;
	while( i < aSize )
	{
		lBackStart     = 1;
		lBackOffBest   = 0;
		lBackCountBest = 0;
		lBackCostBest  = dGODPACK_LZ77B_SYMBOL1_COST;
		lCost          = 2;

		lHash      = (U16)(dLZ77B_BUILDHASH( (&lpSrc[i]) ));
		lBackStart = gGodPackLz77bHashTable[ lHash ];


		while( lBackStart >= 0 )
		{
			lBackOff = lBackStart;
			lCount   = 0;
			if( (lpSrc[ lBackStart ] == lpSrc[ i ]) && ((S32)(i-lBackStart) < lWindowSize) && ((i-lBackStart)>0) )
			{
				j = 0;
				while( (lpSrc[ lBackStart + j ] == lpSrc[ i + j ]) && ((i+j) <aSize) )
				{
					j++;
					lCount++;
				}

				lBackOff = i - lBackStart;
				if( (lCount < dGODPACK_LZ77B_COUNT0_LIMIT) && (lBackOff<dGODPACK_LZ77B_OFF0_LIMIT) )
				{
					lCost = dGODPACK_LZ77B_SYMBOL0_COST;
				}
				else if( (lCount < dGODPACK_LZ77B_COUNT1_LIMIT) && (lBackOff<dGODPACK_LZ77B_OFF1_LIMIT) )
				{
					lCost = dGODPACK_LZ77B_SYMBOL1_COST;
				}
				else if( (lCount < dGODPACK_LZ77B_COUNT2_LIMIT) && (lBackOff<dGODPACK_LZ77B_OFF2_LIMIT) )
				{
					lCost = dGODPACK_LZ77B_SYMBOL2_COST;
				}
				else
				{
					if( (lCount > dGODPACK_LZ77B_COUNT2_LIMIT) && (lBackOff<dGODPACK_LZ77B_OFF2_LIMIT) )
					{
						lCount = dGODPACK_LZ77B_COUNT2_LIMIT;
						lCost  = dGODPACK_LZ77B_SYMBOL2_COST;
					}
					else
					{
						lCost = lCount;
					}
				}

				if( (lCount-lCost) > (lBackCountBest-lBackCostBest) )
				{
					lBackOffBest   = lBackOff;
					lBackCountBest = lCount;
					lBackCostBest  = lCost;
				}
			}
			lBackStart = gpGodPackLz77bLinkList[ lBackStart ];
		}



		if( lBackCountBest > lBackCostBest )
		{
			GodPack_Lz77b_StatsLiteralsUpdate( lLiteralRun );
			GodPack_Lz77b_StatsCountUpdate( lBackCountBest );
			GodPack_Lz77b_StatsOffsetUpdate( lBackOffBest );
			GodPack_Lz77b_StatsOffsetCountUpdate( lBackOffBest, lBackCountBest );

			while( lLiteralRun > 3 )
			{
				j = (lLiteralRun>>2)-1;
				if( j > 31 )
				{
					j = 31;
				}
				lpDst[ lDstOff ]  = (U8)(dGODPACK_LZ77B_FLAG_LITERAL + j);
				lDstOff++;

				j = ((j+1)<<2);
				lLiteralRun -= j;

				while( j )
				{
					lpDst[ lDstOff ] = lpSrc[ lLiteralStart ];
					lDstOff++;
					lLiteralStart++;
					j--;
				}
			}

/*			DebugLog_Printf3( "%ld:\t%ld\t%ld", i, lBackOffBest, lBackCountBest );*/

			j = lBackCountBest;
			while( j )
			{
				lHash = (U16)(dLZ77B_BUILDHASH( (&lpSrc[i]) ));
				gpGodPackLz77bLinkList[ i ] = gGodPackLz77bHashTable[ lHash ];
				gGodPackLz77bHashTable[ lHash ] = i;
				i++;
				j--;
			}

			lBackCountBest  -= (lBackCostBest+1);
			lBackCountBest <<= dGODPACK_LZ77B_LRUN_BITS;
			if( dGODPACK_LZ77B_SYMBOL0_COST == lBackCostBest )
			{
/*				DebugLog_Printf0( "\tS0\n" );*/

				lBackOffBest <<= dGODPACK_LZ77B_OFF0_LSHIFT;
				lpDst[ lDstOff ] = (U8)(dGODPACK_LZ77B_FLAG_SYMBOL0 | (lBackOffBest>>dGODPACK_LZ77B_OFF0_RSHIFT));
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackOffBest | lBackCountBest | lLiteralRun);
				lDstOff++;
				gGodPackLz77bSymbol0Count++;
			}
			else if( dGODPACK_LZ77B_SYMBOL1_COST == lBackCostBest )
			{
/*				DebugLog_Printf0( "\tS1\n" );*/

				lBackOffBest <<= dGODPACK_LZ77B_OFF1_LSHIFT;
				lpDst[ lDstOff ] = (U8)(dGODPACK_LZ77B_FLAG_SYMBOL1 | (lBackOffBest>>dGODPACK_LZ77B_OFF1_RSHIFT));
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackOffBest | (lBackCountBest>>dGODPACK_LZ77B_COUNT1_RSHIFT));
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackCountBest | lLiteralRun);
				lDstOff++;
				gGodPackLz77bSymbol1Count++;
			}
			else
			{
/*				DebugLog_Printf0( "\tS2\n" );*/

				lBackOffBest <<= dGODPACK_LZ77B_OFF2_LSHIFT;

				lpDst[ lDstOff ] = (U8)(dGODPACK_LZ77B_FLAG_SYMBOL2 | (lBackOffBest>>dGODPACK_LZ77B_OFF2_RSHIFT0));
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackOffBest>> dGODPACK_LZ77B_OFF2_RSHIFT1);
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackOffBest | (lBackCountBest>>dGODPACK_LZ77B_COUNT2_RSHIFT));
				lDstOff++;
				lpDst[ lDstOff ] = (U8)(lBackCountBest | lLiteralRun);
				lDstOff++;
				gGodPackLz77bSymbol2Count++;
			}


			while( lLiteralRun )
			{
				lpDst[ lDstOff ] = lpSrc[ lLiteralStart ];
				lDstOff++;
				lLiteralStart++;
				lLiteralRun--;
			}

			lStringCount++;


			lStringRun++;
		}
		else
		{
			GodPack_Lz77b_StatsStringUpdate( lStringRun );
			lStringRun = 0;
			if( !lLiteralRun )
			{
				lLiteralStart = i;
			}
			lLiteralRun++;

			gpGodPackLz77bLinkList[ i ] = gGodPackLz77bHashTable[ lHash ];
			gGodPackLz77bHashTable[ lHash ] = i;

			i++;
		}
		lWindowSize = i;
		if( lWindowSize > dGODPACK_LZ77B_OFF2_LIMIT )
		{
			lWindowSize = dGODPACK_LZ77B_OFF2_LIMIT;
		}
	}

	GodPack_Lz77b_StatsLiteralsUpdate( lLiteralRun );

	while( lLiteralRun > 3 )
	{
		j = (lLiteralRun>>2)-1;
		if( j > 31 )
		{
			j = 31;
		}
		lpDst[ lDstOff ]  = (U8)(dGODPACK_LZ77B_FLAG_LITERAL + j);
		lDstOff++;

		j = ((j+1)<<2);
		lLiteralRun -= j;

		while( j )
		{
			lpDst[ lDstOff ] = lpSrc[ lLiteralStart ];
			lDstOff++;
			lLiteralStart++;
			j--;
		}
	}
	if( lLiteralRun )
	{
		lpDst[ lDstOff ]  = (U8)(dGODPACK_LZ77B_FLAG_LITERAL + lLiteralRun);
		lDstOff++;

		while( lLiteralRun )
		{
			lpDst[ lDstOff ] = lpSrc[ lLiteralStart ];
			lDstOff++;
			lLiteralStart++;
			lLiteralRun--;
		}
	}


	GodPack_Lz77b_StatsDeInit();
	mMEMFREE( gpGodPackLz77bLinkList );

	DebugLog_DeInit();

	return( lDstOff );
}

#if	0

/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsInit(void)
* ACTION   : GodPack_Lz77b_StatsInit
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsInit(void)
{
	U16	i,j;

	for( i=0; i<32; i++ )
	{
		gGodPackLz77bCountHistogram[ i ] = 0;
		gGodPackLz77bLiteralHistogram[ i ] = 0;
		gGodPackLz77bOffsetHistogram[ i ] = 0;
	}
	for( i=0; i<16; i++ )
	{
		for( j=0; j<16; j++ )
		{
			gGodPackLz77bOffsetCountHistogram[ i ][ j ] = 0;
			gGodPackLz77bOffsetWeightHistogram[ i ][ j ] = 0;
		}
	}

	gGodPackLz77bSymbol0Count = 0;
	gGodPackLz77bSymbol1Count = 0;
	gGodPackLz77bSymbol2Count = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsDeInit(void)
* ACTION   : GodPack_Lz77b_StatsDeInit
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsDeInit(void)
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77b_GetBitCount( const U32 aNumber )
* ACTION   : GodPack_Lz77b_GetBitCount
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	GodPack_Lz77b_GetBitCount( const U32 aNumber )
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
* FUNCTION : void GodPack_Lz77b_StatsLiteralsUpdate(const U32 aLitCount)
* ACTION   : GodPack_Lz77b_StatsLiteralsUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsLiteralsUpdate(const U32 aLitCount)
{
	gGodPackLz77bLiteralHistogram[ GodPack_Lz77b_GetBitCount(aLitCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsOffsetUpdate(const U32 aOffset)
* ACTION   : GodPack_Lz77b_StatsOffsetUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsOffsetUpdate(const U32 aOffset)
{
	gGodPackLz77bOffsetHistogram[ GodPack_Lz77b_GetBitCount(aOffset) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsCountUpdate(const U32 aCount)
* ACTION   : GodPack_Lz77b_StatsCountUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsCountUpdate(const U32 aCount)
{
	gGodPackLz77bCountHistogram[ GodPack_Lz77b_GetBitCount(aCount) ]++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsOffsetCountUpdate(const U32 aOffset,const U32 aCount)
* ACTION   : GodPack_Lz77b_StatsOffsetCountUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsOffsetCountUpdate(const U32 aOffset,const U32 aCount)
{
	gGodPackLz77bOffsetCountHistogram[ GodPack_Lz77b_GetBitCount(aOffset) ][ GodPack_Lz77b_GetBitCount(aCount) ]++;
	gGodPackLz77bOffsetWeightHistogram[ GodPack_Lz77b_GetBitCount(aOffset) ][ GodPack_Lz77b_GetBitCount(aCount) ] += aCount;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_Lz77b_StatsStringUpdate(const U32 aCount)
* ACTION   : GodPack_Lz77b_StatsStringUpdate
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_Lz77b_StatsStringUpdate(const U32 aCount)
{
	gGodPackLz77bStringHistogram[ GodPack_Lz77b_GetBitCount(aCount) ]++;
}


#endif



/* ################################################################################ */
