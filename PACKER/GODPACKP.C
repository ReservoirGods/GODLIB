/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Pack( const void * apSrc,const U32 aSrcSize )
* ACTION   : GodPack_Pack
* CREATION : 23.02.2005 PNK
*-----------------------------------------------------------------------------------*/

sGodPackHeader *	GodPack_Pack( const void * apSrc,const U32 aSrcSize )
{
	sGodPackHeader *	lpHeader;
	U8 *	lpBuf0;
	U8 *	lpBuf1;
	U32		lRleSize;
	U32		lLzSize;
	U32		lName;
	
	lpBuf0 = (U8*)mMEMCALLOC( (aSrcSize*2) );

	lRleSize = GodPack_Rle_Encode( apSrc, lpBuf0, aSrcSize );
	lpBuf1   = (U8*)mMEMCALLOC( sizeof(sGodPackHeader) + (lRleSize*2) );
	lLzSize  = GodPack_Lz77b_Encode( lpBuf0, lpBuf1 + sizeof(sGodPackHeader), lRleSize );

	lpHeader                = (sGodPackHeader*)lpBuf1;

	lName = mSTRING_TO_U32( 'G', 'D', 'P', 'K' );
	Endian_WriteBigU32( (&lpHeader->mID),           lName    );
	Endian_WriteBigU32( &lpHeader->mPackedSize,   lLzSize  );
	Endian_WriteBigU32( &lpHeader->mUnPackedSize, aSrcSize );
	Endian_WriteBigU32( &lpHeader->mStageSize,    lRleSize );

	mMEMFREE( lpBuf0 );

	return( lpHeader );
}



/* ################################################################################ */
