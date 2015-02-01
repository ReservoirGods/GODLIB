/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 GodPack_Mtf_Encode(const void * apSrc,void * apDst,const U32 aSize)
* ACTION   : GodPack_Mtf_Encode
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 GodPack_Mtf_Encode(const void * apSrc,void * apDst,const U32 aSize)
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
		lByte = lpSrc[ i ];

		lFind = 0;

		while( lOrder[ lFind ] != lByte )
		{
			lFind++;
		}

		lpDst[ i ] = lFind;

		while( lFind )
		{
			lOrder[ lFind ] = lOrder[ lFind - 1 ];
			lFind--;
		}

		lOrder[ 0 ] = lByte;
	}

	return( aSize );
}


/* ################################################################################ */
