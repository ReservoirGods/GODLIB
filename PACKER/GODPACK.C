/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_Init( sGodPackBitStream * apStream,void * apMem,const U32 aSize )
* ACTION   : GodPack_BitStream_Init
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_Init( sGodPackBitStream * apStream,void * apMem,const U32 aSize )
{
	apStream->mBitsLeft = 0;
	apStream->mByte = 0;
	apStream->mByteBitsLeft = 8;
	apStream->mOffset = 0;
	apStream->mpBase = (U8*)apMem;
	apStream->mSize = aSize;
	apStream->mSwapBitsLeft = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_DeInit( sGodPackBitStream * apStream )
* ACTION   : GodPack_BitStream_DeInit
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_DeInit( sGodPackBitStream * apStream )
{
	if( apStream->mByteBitsLeft < 8 )
	{
		apStream->mByte >>= apStream->mByteBitsLeft;
		apStream->mpBase[ apStream->mOffset ] = apStream->mByte;
		apStream->mOffset++;
		apStream->mByteBitsLeft = 8;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_BitWrite( sGodPackBitStream * apStream,const U8 aBit )
* ACTION   : GodPack_BitStream_BitWrite
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_BitWrite( sGodPackBitStream * apStream,const U8 aBit )
{
	apStream->mByte >>= 1;
	if( aBit )
	{
		apStream->mByte |= 0x80;
	}

	apStream->mByteBitsLeft--;
	if( !apStream->mByteBitsLeft )
	{
		apStream->mpBase[ apStream->mOffset ] = apStream->mByte;
		apStream->mOffset++;
		apStream->mByteBitsLeft = 8;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_MultiBitsWrite( sGodPackBitStream * apStream,const U8 aFirstBit )
* ACTION   : GodPack_BitStream_MultiBitsWrite
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_MultiBitsWrite( sGodPackBitStream * apStream,const U8 aFirstBit )
{
	GodPack_BitStream_BitWrite( apStream, aFirstBit );
	while( apStream->mSwapBitsLeft )
	{
		apStream->mSwapBitsLeft--;
		GodPack_BitStream_BitWrite( apStream, (U8)!aFirstBit );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_BitRead( sGodPackBitStream * apStream )
* ACTION   : GodPack_BitStream_BitRead
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	GodPack_BitStream_BitRead( sGodPackBitStream * apStream )
{
	U8	lBit;

	if( 8 == apStream->mByteBitsLeft )
	{
		apStream->mByte = apStream->mpBase[ apStream->mOffset ];
		apStream->mOffset++;
		apStream->mByteBitsLeft = 0;
	}

	lBit   = (U8)(apStream->mByte & 1);
	apStream->mByte >>= 1;

	apStream->mByteBitsLeft++;
	return( lBit );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_ByteWrite( sGodPackBitStream * apStream,const U8 aByte )
* ACTION   : GodPack_BitStream_ByteWrite
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_ByteWrite( sGodPackBitStream * apStream,const U8 aByte )
{
	U16	i;
	U8	lByte;

	lByte = aByte;
	for( i=0; i<8; i++ )
	{
		GodPack_BitStream_BitWrite( apStream, (U8)((lByte>>7) & 1) );
		lByte <<= 1;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_ByteRead( sGodPackBitStream * apStream )
* ACTION   : GodPack_BitStream_ByteRead
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	GodPack_BitStream_ByteRead( sGodPackBitStream * apStream )
{
	U16	i;
	U8	lByte;

	lByte = 0;
	for( i=0; i<8; i++ )
	{
		lByte <<= 1;
		lByte = (U8)(lByte + GodPack_BitStream_BitRead( apStream ));
	}

	return( lByte );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_WordWrite( sGodPackBitStream * apStream,const U16 aWord )
* ACTION   : GodPack_BitStream_WordWrite
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_BitStream_WordWrite( sGodPackBitStream * apStream,const U16 aWord )
{
	U16	i;
	U16	lWord;

	lWord = aWord;
	for( i=0; i<16; i++ )
	{
		GodPack_BitStream_BitWrite( apStream, (U8)((lWord>>15) & 1) );
		lWord <<= 1;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_BitStream_WordRead( sGodPackBitStream * apStream )
* ACTION   : GodPack_BitStream_WordRead
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	GodPack_BitStream_WordRead( sGodPackBitStream * apStream )
{
	U16	i;
	U16	lWord;

	lWord = 0;
	for( i=0; i<16; i++ )
	{
		lWord <<= 1;
		lWord = (U16)(lWord + GodPack_BitStream_BitRead( apStream ));
	}

	return( lWord );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GodPack_BitStream_BitsWrite(sGodPackBitStream * apStream,const U32 aBits,const U16 aBitCount)
* ACTION   : GodPack_BitStream_BitsWrite
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void GodPack_BitStream_BitsWrite(sGodPackBitStream * apStream,const U32 aBits,const U16 aBitCount)
{
	U16	i;
	U32	lBits;

	lBits = (aBits << (32L-aBitCount));
	for( i=0; i<aBitCount; i++ )
	{
		GodPack_BitStream_BitWrite( apStream, (U8)((lBits>>31L) & 1L) );
		lBits <<= 1;
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 GodPack_BitStream_BitsRead(sGodPackBitStream * apStream,const U16 aBitCount)
* ACTION   : GodPack_BitStream_BitsRead
* CREATION : 22.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 GodPack_BitStream_BitsRead(sGodPackBitStream * apStream,const U16 aBitCount)
{
	U16	i;
	U32	lBits;

	lBits = 0;
	for( i=0; i<aBitCount; i++ )
	{
		lBits <<= 1;
		lBits += GodPack_BitStream_BitRead( apStream );
	}

	return( lBits );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_DePack( const void * apSrc,void * apDst )
* ACTION   : GodPack_DePack
* CREATION : 23.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_DePack( const void * apSrc,void * apDst )
{
	sGodPackHeader *	lpHeader;
	U32					lSize;
	U32					lPakSize;
	U32					lLzSize;
	U8 *				lpDst;
	U8 *				lpStart;


	lSize = 0;

	if( apSrc && apDst )
	{
		lpHeader = (sGodPackHeader*)apSrc;
		Endian_ReadBigU32( &lpHeader->mUnPackedSize, lSize );
		Endian_ReadBigU32( &lpHeader->mPackedSize, lPakSize );
		Endian_ReadBigU32( &lpHeader->mStageSize, lLzSize );

		lpDst    = (U8*)apDst;
		lpStart  = (U8*)apDst;
		lpStart += lSize + dGODPACK_OVERFLOW;
		lpStart -= lPakSize;
		Memory_Copy( lPakSize, &lpHeader[1], lpStart );
#ifndef	dGODLIB_PLATFORM_ATARI
		GodPack_Lz77b_Decode( lpStart, lpDst, lLzSize );
#else
		GodPack_Lz77b_Decode_Asm( lpStart, lpDst, lLzSize );
#endif
		lpStart  = (U8*)apDst;
		lpStart += lSize + dGODPACK_OVERFLOW;
		lpStart -= lLzSize;
		Memory_Copy( lLzSize, lpDst, lpStart );
#ifndef	dGODLIB_PLATFORM_ATARI
		GodPack_Rle_Decode( lpStart, lpDst, lSize );
#else
		GodPack_Rle_Decode_Asm( lpStart, lpDst, lSize );
#endif

	}

	return( lSize );
}


/* ################################################################################ */
