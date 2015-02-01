#ifndef	INCLUDED_FONT_H
#define	INCLUDED_FONT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sFont
{
	U32					mID;
	U32					mVersion;

	U8					mCharFirst;
	U8					mCharLast;
	U8					mKerning;
	U8					mSpaceWidth;

	U16					mWidthMax;
	U16					mHeightMax;
	U16					mSpriteCount;
	U16					mPad;

	U8 *				mpCharMap;
	sSprite *			mpSprites;
	sSpriteRegion *		mpRegions;
} sFont;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sFont *			Font_Create( const sSpriteBlock * apBlock, const U8 * apCharMap, const U16 aFixedWidth );
void			Font_Destroy( sFont * apFont );

U16				Font_GetStringWidth( const sFont * apFont, const char * apString );
U16				Font_GetStringCharX( const sFont * apFont, const char * apString, const U16 aIndex );
U16				Font_GetCharWidth( const sFont * apFont, const U8 aChar );
U16				Font_GetCharX0( const sFont * apFont, const U8 aChar );
U16				Font_GetCharX1( const sFont * apFont, const U8 aChar );

sSpriteRegion*	Font_GetpSpriteRegion( const sFont * apFont, const char aChar );
sSprite*		Font_GetpSprite( const sFont * apFont, const char aChar );

void			Font_Delocate( sFont * apFont );
void			Font_Relocate( sFont * apFont );

U32				Font_GetSize( sFont * apFont );
void			Font_ShowInfo( sFont * apFont );


/* ################################################################################ */

#endif	/* INCLUDED_FONT_H */
