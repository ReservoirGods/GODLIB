/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DATA
################################################################################### */

U16 *	gpBwtIndices;
U8 *	gpBwtBlockData;
U32		gBwtBlockSize;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

int	GodPack_Bwt_Compare( const void * apIndex0, const void * apIndex1 );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 GodPack_Bwt_Encode(const void * apSrc,void * apDst,const U32 aSize)
* ACTION   : GodPack_Bwt_Encode
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 GodPack_Bwt_Encode(const void * apSrc,void * apDst,const U32 aSize)
{

	U8 *	lpSrc;
	U8 *	lpDst;
	U8 *	lpBase;
	U32		lOffset;
	U32		i;

	gpBwtIndices   = (U16*)mMEMALLOC( (dGODPACK_BWT_BLOCK_SIZE ) * sizeof(U16) );
	gpBwtBlockData = (U8*)mMEMALLOC( (dGODPACK_BWT_BLOCK_SIZE ) * 2           );
	lpBase         = (U8*)apDst;

	lpSrc   = (U8*)apSrc;
	lpDst   = lpBase;
	lOffset = 0;

	while( lOffset < aSize )
	{

		gBwtBlockSize = aSize - lOffset;
		if( gBwtBlockSize > dGODPACK_BWT_BLOCK_SIZE )
		{
			gBwtBlockSize = dGODPACK_BWT_BLOCK_SIZE;
		}
		for( i=0; i<gBwtBlockSize; i++ )
		{
			gpBwtIndices[ i ] = (U16)i;
		}

		Memory_Copy( gBwtBlockSize, &lpSrc[ lOffset ],  gpBwtBlockData );
		Memory_Copy( gBwtBlockSize, &lpSrc[ lOffset ], &gpBwtBlockData[ gBwtBlockSize ] );

        qsort( gpBwtIndices, gBwtBlockSize, sizeof(U16), GodPack_Bwt_Compare );

		i = 0;
		while( (i<gBwtBlockSize) && (gpBwtIndices[i] !=1) )
		{
			i++;
		}

		*(U16*)lpDst = (U16)(i);
		lpDst += 2;
		for( i=0; i<gBwtBlockSize; i++ )
		{
			*lpDst = gpBwtBlockData[ gpBwtIndices[i] + gBwtBlockSize -1 ];
			lpDst++;
		}

		lOffset += gBwtBlockSize;
	}
	

	mMEMFREE( gpBwtIndices );
	mMEMFREE( gpBwtBlockData );
	return( (U32)lpDst - (U32)lpBase );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Bwt_Compare( const void * apIndex0,const void * apIndex1 )
* ACTION   : GodPack_Bwt_Compare
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

int	GodPack_Bwt_Compare( const void * apIndex0,const void * apIndex1 )
{
	const U16 *	lpIndex0;
	const U16 *	lpIndex1;
	U8 *		lp0;
	U8 *		lp1;
	U32			lSize;

	lSize    = gBwtBlockSize;
	lpIndex0 = (const U16*)apIndex0;
	lpIndex1 = (const U16*)apIndex1;
	lp0      =&gpBwtBlockData[ *lpIndex0 ];
	lp1      =&gpBwtBlockData[ *lpIndex1 ];

	while( lSize )
	{
		if( *lp0 < *lp1 )
		{
			return( -1 );
		}
		else if( *lp0 > *lp1 )
		{
			return( 1 );
		}
		lp0++;
		lp1++;
		lSize--;
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Bwt_CalcSize( const U32 aSrcSize )
* ACTION   : GodPack_Bwt_CalcSize
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Bwt_CalcSize( const U32 aSrcSize )
{
	U32	lSize;
	
	lSize  = aSrcSize + dGODPACK_BWT_BLOCK_SIZE - 1;
	lSize /= dGODPACK_BWT_BLOCK_SIZE;
	lSize *= sizeof(U16);
	lSize += aSrcSize;

	return( lSize );
}


/* ################################################################################ */
