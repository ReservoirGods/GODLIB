/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  DATA
################################################################################### */


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Rle_Encode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Rle_Encode
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Rle_Encode( const void * apSrc,void * apDst,const U32 aSize )
{
	U32		lFreqs[ 256 ];
	U32		i;
	U32		lSmall;
	S32		lCount;
	U8		lLast;
	U8		lEsc;
	U8 *	lpSrc;
	U8 *	lpDst;

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;

	for( i=0; i<256; i++ )
	{
		lFreqs[ i ] = 0;
	}

	for( i=0; i<aSize; i++ )
	{
		lFreqs[ lpSrc[i] ]++;
	}

	lEsc   = 0;
	lSmall = lFreqs[ 0 ];
	for( i=1; i<256; i++ )
	{
		if( lFreqs[ i ] < lSmall )
		{
			lSmall = lFreqs[ i ];
			lEsc   = (U8)(i);
		}
	}

	*lpDst++ = lEsc;

	lLast  = 0;
	lCount = 0;
	i      = 0;

	while( i < aSize )
	{
		while( (lpSrc[ i ] == lLast) && (i<aSize) )
		{
			lCount++;
			i++;
		}
		if( lCount )
		{
			while( lCount > 0 )
			{
				if( lCount > 255 )
				{
					*lpDst++ = lEsc;
					*lpDst++ = 255;
					*lpDst++ = lLast;
					lCount -= 255;
				}
				else if( lCount > 3 )
				{
					*lpDst++ = lEsc;
					*lpDst++ = (U8)(lCount);
					*lpDst++ = lLast;
					lCount   = 0;
				}
				else
				{
					while( lCount )
					{
						*lpDst++ = lLast;
						lCount--;
					}
				}
			}
		}
		else
		{
			*lpDst++ = lpSrc[ i ];
			if( lpSrc[ i ] == lEsc )
			{
				*lpDst++ = 0;
			}
			i++;
		}
	}

	return( (U32)lpDst - (U32)apDst );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Rle_Decode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Rle_Decode
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Rle_Decode( const void * apSrc,void * apDst,const U32 aSize )
{
	U8 *	lpSrc;	
	U8 *	lpDst;
	U8		lEsc;
	U8		lValue;
	U16		lCount;
	U32		i,j;

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;

	lEsc = lpSrc[ 0 ];

	i = 1;
	j = 0;
	while( j < aSize )
	{
		if( lpSrc[ i ] == lEsc )
		{
			i++;
			lCount = lpSrc[ i ];
			i++;
			if( lCount )
			{
				lValue = lpSrc[ i ];
				i++;
				while( lCount )
				{
					*lpDst++ = lValue;
					j++;
					lCount--;
				}
			}
			else
			{
				*lpDst++ = lEsc;
				j++;
			}
		}
		else
		{
			*lpDst++ = lpSrc[ i ];
			i++;
			j++;
		}
	}

	return( (U32)lpDst - (U32)apDst );
}


/* ################################################################################ */
