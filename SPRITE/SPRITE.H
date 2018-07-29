#ifndef	INCLUDED_SPRITE_H
#define	INCLUDED_SPRITE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16 *	mpGfx;
	U16 *	mpMask;
	U16		mWidth;
	U16		mHeight;
	U16		mGfxPlaneCount;
	U16		mMaskPlaneCount;
} sSprite;

typedef	struct
{
	U32	mID;
	U16	mVersion;
	U16	mSpriteCount;
} sSpriteBlockHeader;

typedef	struct
{
	sSpriteBlockHeader	mHeader;
	sSprite *			mpSprite[ 1 ];
} sSpriteBlock;

typedef	struct
{
	U16	mX0;
	U16	mY0;
	U16	mX1;
	U16	mY1;
} sSpriteRegion;

typedef	struct sSpriteRect
{
	S16	mX;
	S16	mY;
	S16	mWidth;
	S16	mHeight;
} sSpriteRect;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


sSprite *		Sprite_Create( U16 * apGfx, U16 * apMsk, U16 aWidth, U16 aHeight, U16 aGfxPlaneCount, U16 aMskPlaneCount, const U8 aOpaqueFlag );
sSprite	*		Sprite_CreatePreShifted( sSprite * apSprite, const U16 aShiftAmount );
sSprite *		Sprite_CreateUnaligned( U16 * apGfx, U16 * apMsk, U16 aWidth, U16 aHeight, U16 aGfxPlaneCount, U16 aMskPlaneCount, const U8 aOpaqueFlag, U16 aX );
sSprite *		Sprite_CreateTC( U16 * apGfxBase, U16 aMskColour, sSpriteRect * apDstRect, U16 aSrcWidth, const U8 aOpaqueFlag );
void			Sprite_Destroy( sSprite * apSprite );
U32				Sprite_GetSize( sSprite * apSprite );
U32				Sprite_GetMskSize( sSprite * apSprite );
U32				Sprite_GetGfxSize( sSprite * apSprite );
void			Sprite_Delocate( sSprite * apSprite );
void			Sprite_Relocate( sSprite * apSprite );

U16 *			Sprite_MaskCreate( U16 * apGfx, U16 aWidth, U16 aHeight, U16 aGfxPlaneCount, U16 aMskPlaneCount, const U8 aOpaqueFlag );
void			Sprite_MaskDestroy( U16 * apMsk );

sSpriteBlock *	Sprite_BlockClone( sSpriteBlock * apSrc );
sSpriteBlock *	Sprite_BlockCreate( U16 aSpriteCount );
void			Sprite_BlockDestroy( sSpriteBlock * apBlock );
sSpriteBlock *	Sprite_BlockSerialise( sSpriteBlock * apBlock );
U32				Sprite_BlockGetSize( sSpriteBlock * apBlock );
void			Sprite_BlockDelocate( sSpriteBlock * apBlock );
void			Sprite_BlockRelocate( sSpriteBlock * apBlock );
void			Sprite_BlockGetRegion( sSpriteBlock * apBlock, sSpriteRegion * apRegion );

void			Sprite_FlipH( sSprite * apBlock );
void			Sprite_BlockFlipH( sSpriteBlock * apBlock );

void			Sprite_GetRegion( sSprite * apSprite, sSpriteRegion * apRegion );

sSpriteBlock *	SpriteBlock_Rot90( const sSpriteBlock * apSrc );


/* ################################################################################ */

#endif	/* INCLUDED_SPRITE_H */

