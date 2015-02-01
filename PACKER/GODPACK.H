#ifndef	INCLUDED_GODPACK_H
#define	INCLUDED_GODPACK_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


typedef	void *	(*fGpStream)( const U32 aBytes, const U32 aUserData );
typedef	U32 	(*fGpTransform)( const void * apSrc, void * apDst, const U32 aSize );


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGODPACK_VERSION			0
#define	dGODPACK_OVERFLOW			32

#define	dGODPACK_BWT_BLOCK_SIZE		256

#define	dGODPACK_ARI_CHAR_LIMIT		256
#define	dGODPACK_ARI_SYMBOL_LIMIT	257
#define	dGODPACK_ARI_FREQ_LIMIT		16383
#define	dGODPACK_ARI_VALUE_LIMIT	0xFFFFL
#define	dGODPACK_ARI_FIRST_QTR		((dGODPACK_ARI_VALUE_LIMIT/4)+1)
#define	dGODPACK_ARI_HALF			(2*dGODPACK_ARI_FIRST_QTR)
#define	dGODPACK_ARI_THIRD_QTR		(3*dGODPACK_ARI_FIRST_QTR)

#define	dGODPACK_LZ77_WINDOW_SIZE	16384
#define	dGODPACK_LZ77_COUNT_LIMIT	256

#define	dGODPACK_LZ77_COUNT0_BITS	4
#define	dGODPACK_LZ77_COUNT1_BITS	8

#define	dGODPACK_LZ77_OFF0_BITS		8
#define	dGODPACK_LZ77_OFF1_BITS		14

#define	dGODPACK_LZ77B_FLAG_LITERAL	(0x80+0x40+0x20)
#define	dGODPACK_LZ77B_FLAG_SYMBOL0	(0x00+0x00+0x00)
#define	dGODPACK_LZ77B_FLAG_SYMBOL1	(0x80+0x00+0x00)
#define	dGODPACK_LZ77B_FLAG_SYMBOL2	(0x80+0x40+0x00)
#define	dGODPACK_LZ77B_FLAG_MASK	(0x80+0x40+0x20)

#define	dGODPACK_LZ77B_SYMBOL0_COST	(2L)
#define	dGODPACK_LZ77B_SYMBOL1_COST	(3L)
#define	dGODPACK_LZ77B_SYMBOL2_COST	(4L)

#define	dGODPACK_LZ77B_OFF0_BITS	9L
#define	dGODPACK_LZ77B_COUNT0_BITS	4L
#define	dGODPACK_LZ77B_OFF1_BITS	14L
#define	dGODPACK_LZ77B_COUNT1_BITS	6L
#define	dGODPACK_LZ77B_OFF2_BITS	14L
#define	dGODPACK_LZ77B_COUNT2_BITS	13L

#define	dGODPACK_LZ77B_OFF0_LIMIT	((1<<dGODPACK_LZ77B_OFF0_BITS)-1L)
#define	dGODPACK_LZ77B_OFF1_LIMIT	((1<<dGODPACK_LZ77B_OFF1_BITS)-1L)
#define	dGODPACK_LZ77B_OFF2_LIMIT	((1<<dGODPACK_LZ77B_OFF2_BITS)-1L)
#define	dGODPACK_LZ77B_COUNT0_LIMIT	((1<<dGODPACK_LZ77B_COUNT0_BITS)+dGODPACK_LZ77B_SYMBOL0_COST)
#define	dGODPACK_LZ77B_COUNT1_LIMIT	((1<<dGODPACK_LZ77B_COUNT1_BITS)+dGODPACK_LZ77B_SYMBOL1_COST)
#define	dGODPACK_LZ77B_COUNT2_LIMIT	((1<<dGODPACK_LZ77B_COUNT2_BITS)+dGODPACK_LZ77B_SYMBOL2_COST)

#define	dGODPACK_LZ77B_LRUN_BITS	(2L)

#define	dGODPACK_LZ77B_OFF0_LSHIFT		((dGODPACK_LZ77B_LRUN_BITS + dGODPACK_LZ77B_COUNT0_BITS)&7L)
#define	dGODPACK_LZ77B_OFF1_LSHIFT		((dGODPACK_LZ77B_LRUN_BITS + dGODPACK_LZ77B_COUNT1_BITS)&7L)
#define	dGODPACK_LZ77B_OFF2_LSHIFT		((dGODPACK_LZ77B_LRUN_BITS + dGODPACK_LZ77B_COUNT2_BITS)&7L)

#define	dGODPACK_LZ77B_OFF0_RSHIFT		(8L)
#define	dGODPACK_LZ77B_OFF1_RSHIFT		(8L)
#define	dGODPACK_LZ77B_OFF2_RSHIFT0		(16L)
#define	dGODPACK_LZ77B_OFF2_RSHIFT1		(8L)

#define	dGODPACK_LZ77B_COUNT0_RSHIFT	(8L)
#define	dGODPACK_LZ77B_COUNT1_RSHIFT	(8L)
#define	dGODPACK_LZ77B_COUNT2_RSHIFT	(8L)


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sGodPackHeader
{
	U32	mID;
	U32	mVersion;
	U32	mPackedSize;
	U32	mUnPackedSize;
	U32	mStageSize;
} sGodPackHeader;

typedef	struct	sGodPackBitStream
{
	U8 *	mpBase;
	U32		mOffset;
	U32		mSize;
	U8		mByte;
	U8		mByteBitsLeft;
	U8		mBitsLeft;
	U8		mSwapBitsLeft;
} sGodPackBitStream;

typedef	struct	sGodPackAriTables
{
	S32	mLow;
	S32	mHigh;
	S32	mValue;
	U16	mCharToIndex[ 258 ];
	U16	mSymbolFreq[ 258 ];
	U16	mCumFreq[ 258 ];
	U8	mIndexToChar[ 258 ];
} sGodPackAriTables;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sGodPackHeader *	GodPack_Pack( const void * apSrc, const U32 aSrcSize );
U32					GodPack_DePack( const void * apSrc, void * apDst );

void	GodPack_BitStream_Init( sGodPackBitStream * apStream, void * apMem, const U32 aSize );
void	GodPack_BitStream_DeInit( sGodPackBitStream * apStream );
void	GodPack_BitStream_BitWrite( sGodPackBitStream * apStream, const U8 aBit );
void	GodPack_BitStream_BitsWrite( sGodPackBitStream * apStream, const U32 aBits, const U16 aBitCount );
U32		GodPack_BitStream_BitsRead( sGodPackBitStream * apStream, const U16 aBitCount );
void	GodPack_BitStream_MultiBitsWrite( sGodPackBitStream * apStream, const U8 aFirstBit );
U8		GodPack_BitStream_BitRead( sGodPackBitStream * apStream );
void	GodPack_BitStream_ByteWrite( sGodPackBitStream * apStream, const U8 aByte );
U8		GodPack_BitStream_ByteRead( sGodPackBitStream * apStream );
void	GodPack_BitStream_WordWrite( sGodPackBitStream * apStream, const U16 aWord );
U16		GodPack_BitStream_WordRead( sGodPackBitStream * apStream );

void	GodPack_Ari_ModelUpdate( sGodPackAriTables * apTables, U16 aSymbol );
void	GodPack_Ari_TablesInit( sGodPackAriTables * apTables );

U32		GodPack_Ari_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Ari_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Bwt_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Bwt_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Lz77_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Lz77_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Lz77b_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Lz77b_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Mtf_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Mtf_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Rle_Decode( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Rle_Encode( const void * apSrc,void * apDst,const U32 aDstSize );

U32		GodPack_Lz77_Decode_Asm( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Lz77b_Decode_Asm( const void * apSrc,void * apDst,const U32 aDstSize );
U32		GodPack_Rle_Decode_Asm( const void * apSrc,void * apDst,const U32 aDstSize );


/* ################################################################################ */

#endif	/* INCLUDED_GODPACK_H */
