/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77_Decode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Lz77_Decode
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Lz77_Decode( const void * apSrc,void * apDst,const U32 aSize )
{
	U8 *	lpSrc;
	U8 *	lpDst;
	S32		lBackOff;
	S32		lCount;
	U32		i;
	sGodPackBitStream	lStream;


	lpDst = (U8*)apDst;

	GodPack_BitStream_Init( &lStream, (void*)apSrc, aSize );
	
	i = 0;
	while( i < aSize )
	{
		if( GodPack_BitStream_BitRead( &lStream ) )
		{
			if( GodPack_BitStream_BitRead( &lStream ) )
			{
				lBackOff = GodPack_BitStream_BitsRead( &lStream, dGODPACK_LZ77_OFF1_BITS   );
				lCount   = GodPack_BitStream_BitsRead( &lStream, dGODPACK_LZ77_COUNT1_BITS );
			}
			else
			{
				lBackOff = GodPack_BitStream_BitsRead( &lStream, dGODPACK_LZ77_OFF0_BITS   );
				lCount   = GodPack_BitStream_BitsRead( &lStream, dGODPACK_LZ77_COUNT0_BITS );
			}
			lCount += 3;
			lpSrc   =&lpDst[ i - lBackOff ];
			while( lCount )
			{
				lCount--;
				lpDst[ i ] = *lpSrc++;
				i++;
			}
		}
		else
		{
			lCount = GodPack_BitStream_BitsRead( &lStream, 3 ) + 1;
			while( lCount )
			{
				lpDst[ i ] = GodPack_BitStream_ByteRead( &lStream );
				i++;
				lCount--;
			}
		}
	}

	return( i );
}


/* ################################################################################ */
