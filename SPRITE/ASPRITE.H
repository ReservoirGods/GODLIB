#ifndef	INCLUDED_ASPRITE_H
#define	INCLUDED_ASPRITE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eASPRITE_COLOUR_D1,
	eASPRITE_COLOUR_D2,
	eASPRITE_COLOUR_D3,
	eASPRITE_COLOUR_D4,
	eASPRITE_COLOUR_D5,
	eASPRITE_COLOUR_D6,
	eASPRITE_COLOUR_D7,

	eASPRITE_COLOUR_A0,
	eASPRITE_COLOUR_A2,
	eASPRITE_COLOUR_A3,
	eASPRITE_COLOUR_A4,
	eASPRITE_COLOUR_A5,
	eASPRITE_COLOUR_A6,

	eASPRITE_COLOUR_LIMIT
};


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	U32	(*fAsmSpriteDraw)( void * apDst, const U32 aScreenWidthBytes );
typedef	U32	(*fAsmSpriteClip)( void * apDst, const U32 aScreenWidthBytes );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sAsmSpriteHeader
{
	U16			mColours[ eASPRITE_COLOUR_LIMIT ][ 2 ];
	U16			mWidth;
	U16			mHeight;
} sAsmSpriteHeader;

typedef struct sAsmSprite
{
	sAsmSpriteHeader	mHeader;
	U16 *				mfLines[ 1 ];
} sAsmSprite;

typedef struct sAsmSpriteBlockHeader
{
	U32	mID;
	U16	mVersion;
	U16	mSpriteCount;
} sAsmSpriteBlockHeader;

typedef struct sAsmSpriteBlock
{
	sAsmSpriteBlockHeader	mHeader;
	sAsmSprite *			mpSprites[ 1 ];
} sAsmSpriteBlock;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sAsmSprite *		AsmSprite_Create( const U16 * apGfx, const sSpriteRect * apRect, const U16 aGfxWidth, const U16 aOpaqueColour, const U8 aOpaqueFlag );
void				AsmSprite_Destroy( sAsmSprite * apSprite );
U32					AsmSprite_GetSize( const sAsmSprite * apSprite );

void				AsmSprite_Delocate( sAsmSprite * apSprite );
void				AsmSprite_Relocate( sAsmSprite * apSprite );

sAsmSpriteBlock *	AsmSpriteBlock_Create( const U16 aSpriteCount );
void				AsmSpriteBlock_Destroy( sAsmSpriteBlock * apBlock );

sAsmSpriteBlock *	AsmSpriteBlock_Serialise( const sAsmSpriteBlock * apBlock );

void				AsmSpriteBlock_Delocate( sAsmSpriteBlock * apBlock );
void				AsmSpriteBlock_Relocate( sAsmSpriteBlock * apBlock );

U32					AsmSpriteBlock_GetSize( const sAsmSpriteBlock * apBlock );

void				AsmSprite_Draw( const sAsmSprite * apSprite, U16 * apScreen, U32 aScreenWidth );
void				AsmSprite_DrawClip( const sAsmSprite * apSprite, U16 * apScreen, U32 aScreenWidth, S16 aTopLine, S16 aBotLine );


/* ################################################################################ */

#endif	/* INCLUDED_ASPRITE_H */
