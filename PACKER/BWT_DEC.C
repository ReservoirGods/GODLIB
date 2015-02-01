/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DATA
################################################################################### */

U16 *	gpBwtTransform;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	GodPack_Bwt_BlockDecode( const void * apSrc,void * apDest,const U32 aSize );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 GodPack_Bwt_Decode(const void * apSrc,void * apDest, const U32 aSize)
* ACTION   : GodPack_Bwt_Decode
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 GodPack_Bwt_Decode(const void * apSrc,void * apDest, const U32 aSize)
{
	U32		lOffset;
	U32		lSize;
	U32		i;
	U8 *	lpSrc;
	U8 *	lpDst;


	gpBwtTransform = (U16*)mMEMCALLOC( dGODPACK_BWT_BLOCK_SIZE * sizeof(U16) );

	lpSrc      = (U8*)apSrc;
	lpDst      = (U8*)apDest;
	lOffset    = 0;
	i          = 0;

	while( i < aSize )
	{
		lSize      = aSize - i;
		if( lSize > dGODPACK_BWT_BLOCK_SIZE )
		{
			lSize  = dGODPACK_BWT_BLOCK_SIZE;
		}
		lSize += 2;

		GodPack_Bwt_BlockDecode( lpSrc, lpDst, lSize );
		lpSrc     +=(dGODPACK_BWT_BLOCK_SIZE+2);
		lpDst     += dGODPACK_BWT_BLOCK_SIZE;

		lOffset   +=(dGODPACK_BWT_BLOCK_SIZE+2);
		i         +=dGODPACK_BWT_BLOCK_SIZE;
	}

	mMEMFREE( gpBwtTransform );

	return( aSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Bwt_BlockDecode( const void * apSrc,void * apDest,const U32 aSize )
* ACTION   : GodPack_Bwt_BlockDecode
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Bwt_BlockDecode( const void * apSrc,void * apDest,const U32 aSize )
{
	U16		lCounts[ 257 ];
	U16		lTotals[ 257 ];
	U16		lSum;
	U16		lIndex;
	U32		lBwtOff;
	U16		lFirst;
	U32		lSize;
	U8 *	lpSrc;
	U8 *	lpDst;
	U16		i;

	lFirst = *(U16*)apSrc;
	lpSrc  = (U8*)apSrc;
	lpSrc += 2;
	lpDst  = (U8*)apDest;
	lSize  = aSize - 2;

	if( lSize )
	{
        for ( i=0 ; i<256 ; i++ )
		{
            lCounts[ i ] = 0;
		}

        for ( i=0 ; i<lSize ; i++ )
		{
			lCounts[ lpSrc[i] ]++;
		}

		lSum = 0;
        for ( i=0 ; i<256 ; i++ )
		{
			lTotals[ i ]  = lSum;
			lSum          = (U16)(lSum + lCounts[ i ]);
			lCounts[ i ]  = 0;
		}

        for ( i=0 ; i<lSize ; i++ )
		{
			lIndex  = lpSrc[i];
			lBwtOff = lCounts[lIndex] + lTotals[lIndex];
			if( lBwtOff >= dGODPACK_BWT_BLOCK_SIZE )
			{
				printf( "WARNING!!\n" );
			}
			gpBwtTransform[ lBwtOff ] = i;
			lCounts[ lIndex ]++;
		}

		lIndex = lFirst;
		for( i=0; i<lSize; i++ )
		{
			lpDst[ i ] = lpSrc[ lIndex ];
			lIndex     = gpBwtTransform[ lIndex ];
		}
	}

	return( 1 );
}


/* ################################################################################ */
