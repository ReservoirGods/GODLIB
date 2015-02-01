#ifndef	INCLUDED_RSPRITE_H
#define	INCLUDED_RSPRITE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sRleSpriteHeader
{
	U16			mWidth;
	U16			mHeight;
} sRleSpriteHeader;

typedef struct sRleSprite
{
	sRleSpriteHeader	mHeader;
	U16 *				mpLines[ 1 ];
} sRleSprite;

typedef struct sRleSpriteBlockHeader
{
	U32	mID;
	U16	mVersion;
	U16	mSpriteCount;
} sRleSpriteBlockHeader;

typedef struct sRleSpriteBlock
{
	sRleSpriteBlockHeader	mHeader;
	sRleSprite *			mpSprites[ 1 ];
} sRleSpriteBlock;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sRleSprite *		RleSprite_Create( const U16 * apGfx, const sSpriteRect * apRect, const U16 aGfxWidth, const U16 aOpaqueColour, const U8 aOpaqueFlag );
void				RleSprite_Destroy( sRleSprite * apSprite );
U32					RleSprite_GetSize( const sRleSprite * apSprite );

void				RleSprite_Delocate( sRleSprite * apSprite );
void				RleSprite_Relocate( sRleSprite * apSprite );

sRleSpriteBlock *	RleSpriteBlock_Create( const U16 aSpriteCount );
void				RleSpriteBlock_Destroy( sRleSpriteBlock * apBlock );

sRleSpriteBlock *	RleSpriteBlock_Serialise( const sRleSpriteBlock * apBlock );

void				RleSpriteBlock_Delocate( sRleSpriteBlock * apBlock );
void				RleSpriteBlock_Relocate( sRleSpriteBlock * apBlock );

U32					RleSpriteBlock_GetSize( const sRleSpriteBlock * apBlock );

void				RleSprite_Draw( const sRleSprite * apSprite, U16 * apScreen, U32 aScreenWidth );
void				RleSprite_DrawClip( const sRleSprite * apSprite, U16 * apScreen, U32 aScreenWidth, const sSpriteRect * apRect );

void				RleSprite_Clear( const sRleSprite * apSprite, const U16 * apSrc, U16 * apDst, U32 aScreenWidth );


/* ################################################################################ */

#endif	/* INCLUDED_RSPRITE_H */
