/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 GodPack_Mtf_Decode(const void * apSrc,void * apDst,const U32 aSize)
* ACTION   : GodPack_Mtf_Decode
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 GodPack_Mtf_Decode(const void * apSrc,void * apDst,const U32 aSize)
{
	U8		lOrder[ 256 ];
	U8		lByte;
	U8		lFind;
	U8 *	lpSrc;
	U8 *	lpDst;
	U32		i;

	for( i=0; i<256; i++ )
	{
		lOrder[ i ] = (U8)(i);
	}

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;
	for( i=0; i<aSize; i++ )
	{
		lByte      = lpSrc[ i ];
		lFind      = lOrder[ lByte ];
		lpDst[ i ] = lFind;

		while( lByte )
		{
			lOrder[ lByte ] = lOrder[ lByte - 1 ];
			lByte--;
		}
		lOrder[ 0 ] = lFind;
	}

	return( aSize );
}


/* ################################################################################ */
