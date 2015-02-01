#ifndef	INCLUDED_ACH_GFX_H
#define	INCLUDED_ACH_GFX_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_GFX_YOFF( _a )	((_a<<6)+(_a<<4))
#define	dACH_GFX_XOFF( _a )	((_a>>4)<<2)


/* ###################################################################################
#  DEFINES
################################################################################### */

enum
{
	eACH_GFX_ALIGN_LEFT,
	eACH_GFX_ALIGN_CENTRE,
	eACH_GFX_ALIGN_RIGHT,
};

enum
{
	eACH_GFX_COLOUR_CURSOR = 16
};

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct	sAchieveGfxPos
{
	S16	mX;
	S16	mY;
} sAchieveGfxPos;

typedef struct	sAchieveGfxQuad
{
	S16	mX0;
	S16	mX1;
	S16	mY0;
	S16	mY1;
} sAchieveGfxQuad;

typedef struct	sAchieveGfxSizePos
{
	S16	mX;
	S16	mY;
	S16	mWidth;
	S16	mHeight;
} sAchieveGfxSizePos;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Gfx_AppInit( void );
void	Achieve_Gfx_AppDeInit( void );

void	Achieve_Gfx_Init( void );
void	Achieve_Gfx_DeInit( void );

void	Achieve_Gfx_Update( void * apPhysic, void * apLogic );

void	Achieve_Gfx_HLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour );
void	Achieve_Gfx_VLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour );
void	Achieve_Gfx_Box_Render( const sAchieveGfxQuad * apQuad, const U16 aColour, const sAchieveGfxQuad * apClipRect );

void	Achieve_Gfx_GetPal( U16 * apPal );
void	Achieve_Gfx_SetPal( const U16 * apPal );

void	Achieve_Gfx_Fade_Begin( const U16 * apPal, const U16 aFrameCount );
U8		Achieve_Gfx_Fade_IsFinished( void );

void	Achieve_Gfx_QuadFromSizePos( sAchieveGfxQuad * apQuad, const sAchieveGfxSizePos * apSizePos );
U8		Achieve_Gfx_QuadClip( sAchieveGfxQuad * apTarget, const sAchieveGfxQuad * apSrc, const sAchieveGfxQuad * apClipRect );
void	Achieve_Gfx_FontDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect );
void	Achieve_Gfx_CursorDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect );

void	Achieve_Gfx_Beam_Begin( void );

void	Achieve_Input_Update( const U8 aPacket, const U8 aKey );
U8		Achieve_Input_GetPacket( void );
U8		Achieve_Input_GetKey( void );
U8		Achieve_Input_GetHit( void );
U8		Achieve_Input_GetUnHit( void );


/* ################################################################################ */

#endif	/*	INCLUDED_SIGN_H	*/
