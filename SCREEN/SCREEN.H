#ifndef	INCLUDED_SCREEN_H
#define	INCLUDED_SCREEN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/GRAPHIC/GRAPHIC.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eSCREEN_PHYSIC,
	eSCREEN_LOGIC,
	eSCREEN_BACK,
	eSCREEN_MISC,

	eSCREEN_LIMIT
};

enum
{
	eSCREEN_SCROLL_NONE = 0,
	eSCREEN_SCROLL_H    = (1<<0),
	eSCREEN_SCROLL_V    = (1<<1),
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sScreenClass
{
	U16 *	mpMemBase;
	U16 *	mpBuffers[ eSCREEN_LIMIT ];
	U32		mFrameRate;
	U32		mLastVbl;
	U16		mScrollY;
	U16		mPhysicIndex;
	U16		mFirstTimeFlag;
} sScreenClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Screen_Init( const U16 aWidth, const U16 aHeight, const U16 aBitDepth, const U16 aScrollFlags );
void	Screen_Update( void );
void	Screen_DeInit( void );

extern	sGraphicCanvas	gScreenBackGraphic;
extern	sGraphicCanvas	gScreenLogicGraphic;
extern	sGraphicCanvas	gScreenPhysicGraphic;
extern	sGraphicCanvas	gScreenMiscGraphic;

extern	sScreenClass	gScreenClass;


/* ###################################################################################
#  HELPER FUNCTIONS
################################################################################### */

#define	Screen_GetpPhysicGraphic()	(&gScreenPhysicGraphic)
#define	Screen_GetpLogicGraphic()	(&gScreenLogicGraphic)
#define	Screen_GetpBackGraphic()	(&gScreenBackGraphic)
#define	Screen_GetpMiscGraphic()	(&gScreenMiscGraphic)


#define	Screen_GetpPhysic()			gScreenClass.mpBuffers[ gScreenClass.mPhysicIndex ]
#define	Screen_GetpLogic()			gScreenClass.mpBuffers[ gScreenClass.mPhysicIndex ^ 1 ]
#define	Screen_GetpBack()			gScreenClass.mpBuffers[ eSCREEN_BACK ]

#define	Screen_GetPhysicIndex()		gScreenClass.mPhysicIndex
#define	Screen_GetLogicIndex()		(gScreenClass.mPhysicIndex ^ 1)

#define	Screen_SetFrameRate( _aRate )	gScreenClass.mFrameRate = _aRate
#define	Screen_GetScrollY()				gScreenClass.mScrollY
#define Screen_SetScrollY( _aY )		gScreenClass.mScrollY = _aY

#define	Screen_GetpMemBase()			gScreenClass.mpMemBase

#define Screen_Logic_Blit( apCoords, apRect, apSrc )				gScreenLogicGraphic.mpFuncs->Blit( &gScreenLogicGraphic, apCoords, apRect, apSrc )
#define Screen_Logic_ClearScreen()									gScreenLogicGraphic.mpFuncs->ClearScreen( &gScreenLogicGraphic )
#define Screen_Logic_CopyScreen( apSrc )							gScreenLogicGraphic.mpFuncs->CopyScreen( &gScreenLogicGraphic, apSrc )
#define Screen_Logic_ConvertBlit( apCoords, apRect, apSrc )			gScreenLogicGraphic.mpFuncs->ConvertBlit( &gScreenLogicGraphic, apCoords, apRect, apSrc )
#define Screen_Logic_DrawBox( apCoords, aColour )					gScreenLogicGraphic.mpFuncs->DrawBox( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawLine( apCoords, aColour )					gScreenLogicGraphic.mpFuncs->DrawLine( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawPixel( apCoords, aColour )					gScreenLogicGraphic.mpFuncs->DrawPixel( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawSprite( apCoords, apSprite )				gScreenLogicGraphic.mpFuncs->DrawSprite( &gScreenLogicGraphic, apCoords, apSprite )
#define Screen_Logic_DrawTri( apCoords, aColour )					gScreenLogicGraphic.mpFuncs->DrawTri( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawQuad( apCoords, aColour )					gScreenLogicGraphic.mpFuncs->DrawQuad( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_FontPrint( apCoords, apFont, apString )		gScreenLogicGraphic.mpFuncs->FontPrint( &gScreenLogicGraphic, apCoords, apFont, apString )

#define Screen_Logic_Blit_Clip( apCoords, apRect, apSrc )			gScreenLogicGraphic.mpClipFuncs->Blit( &gScreenLogicGraphic, apCoords, apRect, apSrc )
#define Screen_Logic_ClearScreen_Clip()								gScreenLogicGraphic.mpClipFuncs->ClearScreen( &gScreenLogicGraphic )
#define Screen_Logic_CopyScreen_Clip( apSrc )						gScreenLogicGraphic.mpClipFuncs->CopyScreen( &gScreenLogicGraphic, apSrc )
#define Screen_Logic_ConvertBlit_Clip( apCoords, apRect, apSrc )	gScreenLogicGraphic.mpClipFuncs->ConvertBlit( &gScreenLogicGraphic, apCoords, apRect, apSrc )
#define Screen_Logic_DrawBox_Clip( apCoords, aColour )				gScreenLogicGraphic.mpClipFuncs->DrawBox( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawLine_Clip( apCoords, aColour )				gScreenLogicGraphic.mpClipFuncs->DrawLine( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawPixel_Clip( apCoords, aColour )			gScreenLogicGraphic.mpClipFuncs->DrawPixel( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawSprite_Clip( apCoords, apSprite )			gScreenLogicGraphic.mpClipFuncs->DrawSprite( &gScreenLogicGraphic, apCoords, apSprite )
#define Screen_Logic_DrawTri_Clip( apCoords, aColour )				gScreenLogicGraphic.mpClipFuncs->DrawTri( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_DrawQuad_Clip( apCoords, aColour )				gScreenLogicGraphic.mpClipFuncs->DrawQuad( &gScreenLogicGraphic, apCoords, aColour )
#define Screen_Logic_FontPrint_Clip( apCoords, apFont, apString )	gScreenLogicGraphic.mpClipFuncs->FontPrint( &gScreenLogicGraphic, apCoords, apFont, apString )


#define Screen_Physic_Blit( apCoords, apRect, apSrc )				gScreenPhysicGraphic.mpFuncs->Blit( &gScreenPhysicGraphic, apCoords, apRect, apSrc )
#define Screen_Physic_ClearScreen()									gScreenPhysicGraphic.mpFuncs->ClearScreen( &gScreenPhysicGraphic )
#define Screen_Physic_CopyScreen( apSrc )							gScreenPhysicGraphic.mpFuncs->CopyScreen( &gScreenPhysicGraphic, apSrc )
#define Screen_Physic_ConvertBlit( apCoords, apRect, apSrc )		gScreenPhysicGraphic.mpFuncs->ConvertBlit( &gScreenPhysicGraphic, apCoords, apRect, apSrc )
#define Screen_Physic_DrawBox( apCoords, aColour )					gScreenPhysicGraphic.mpFuncs->DrawBox( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawLine( apCoords, aColour )					gScreenPhysicGraphic.mpFuncs->DrawLine( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawPixel( apCoords, aColour )				gScreenPhysicGraphic.mpFuncs->DrawPixel( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawSprite( apCoords, apSprite )				gScreenPhysicGraphic.mpFuncs->DrawSprite( &gScreenPhysicGraphic, apCoords, apSprite )
#define Screen_Physic_DrawTri( apCoords, aColour )					gScreenPhysicGraphic.mpFuncs->DrawTri( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawQuad( apCoords, aColour )					gScreenPhysicGraphic.mpFuncs->DrawQuad( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_FontPrint( apCoords, apFont, apString )		gScreenPhysicGraphic.mpFuncs->FontPrint( &gScreenPhysicGraphic, apCoords, apFont, apString )

#define Screen_Physic_Blit_Clip( apCoords, apRect, apSrc )			gScreenPhysicGraphic.mpClipFuncs->Blit( &gScreenPhysicGraphic, apCoords, apRect, apSrc )
#define Screen_Physic_ClearScreen_Clip()							gScreenPhysicGraphic.mpClipFuncs->ClearScreen( &gScreenPhysicGraphic )
#define Screen_Physic_CopyScreen_Clip( apSrc )						gScreenPhysicGraphic.mpClipFuncs->CopyScreen( &gScreenPhysicGraphic, apSrc )
#define Screen_Physic_ConvertBlit_Clip( apCoords, apRect, apSrc )	gScreenPhysicGraphic.mpClipFuncs->ConvertBlit( &gScreenPhysicGraphic, apCoords, apRect, apSrc )
#define Screen_Physic_DrawBox_Clip( apCoords, aColour )				gScreenPhysicGraphic.mpClipFuncs->DrawBox( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawLine_Clip( apCoords, aColour )			gScreenPhysicGraphic.mpClipFuncs->DrawLine( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawPixel_Clip( apCoords, aColour )			gScreenPhysicGraphic.mpClipFuncs->DrawPixel( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawSprite_Clip( apCoords, apSprite )			gScreenPhysicGraphic.mpClipFuncs->DrawSprite( &gScreenPhysicGraphic, apCoords, apSprite )
#define Screen_Physic_DrawTri_Clip( apCoords, aColour )				gScreenPhysicGraphic.mpClipFuncs->DrawTri( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_DrawQuad_Clip( apCoords, aColour )			gScreenPhysicGraphic.mpClipFuncs->DrawQuad( &gScreenPhysicGraphic, apCoords, aColour )
#define Screen_Physic_FontPrint_Clip( apCoords, apFont, apString )	gScreenPhysicGraphic.mpClipFuncs->FontPrint( &gScreenPhysicGraphic, apCoords, apFont, apString )

#define Screen_Back_Blit( apCoords, apRect, apSrc )					gScreenBackGraphic.mpFuncs->Blit( &gScreenBackGraphic, apCoords, apRect, apSrc )
#define Screen_Back_ClearScreen()									gScreenBackGraphic.mpFuncs->ClearScreen( &gScreenBackGraphic )
#define Screen_Back_CopyScreen( apSrc )								gScreenBackGraphic.mpFuncs->CopyScreen( &gScreenBackGraphic, apSrc )
#define Screen_Back_ConvertBlit( apCoords, apRect, apSrc )			gScreenBackGraphic.mpFuncs->ConvertBlit( &gScreenBackGraphic, apCoords, apRect, apSrc )
#define Screen_Back_DrawBox( apCoords, aColour )					gScreenBackGraphic.mpFuncs->DrawBox( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawLine( apCoords, aColour )					gScreenBackGraphic.mpFuncs->DrawLine( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawPixel( apCoords, aColour )					gScreenBackGraphic.mpFuncs->DrawPixel( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawSprite( apCoords, apSprite )				gScreenBackGraphic.mpFuncs->DrawSprite( &gScreenBackGraphic, apCoords, apSprite )
#define Screen_Back_DrawTri( apCoords, aColour )					gScreenBackGraphic.mpFuncs->DrawTri( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawQuad( apCoords, aColour )					gScreenBackGraphic.mpFuncs->DrawQuad( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_FontPrint( apCoords, apFont, apString )			gScreenBackGraphic.mpFuncs->FontPrint( &gScreenBackGraphic, apCoords, apFont, apString )

#define Screen_Back_Blit_Clip( apCoords, apRect, apSrc )			gScreenBackGraphic.mpClipFuncs->Blit( &gScreenBackGraphic, apCoords, apRect, apSrc )
#define Screen_Back_ClearScreen_Clip()								gScreenBackGraphic.mpClipFuncs->ClearScreen( &gScreenBackGraphic )
#define Screen_Back_CopyScreen_Clip( apSrc )						gScreenBackGraphic.mpClipFuncs->CopyScreen( &gScreenBackGraphic, apSrc )
#define Screen_Back_ConvertBlit_Clip( apCoords, apRect, apSrc )		gScreenBackGraphic.mpClipFuncs->ConvertBlit( &gScreenBackGraphic, apCoords, apRect, apSrc )
#define Screen_Back_DrawBox_Clip( apCoords, aColour )				gScreenBackGraphic.mpClipFuncs->DrawBox( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawLine_Clip( apCoords, aColour )				gScreenBackGraphic.mpClipFuncs->DrawLine( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawPixel_Clip( apCoords, aColour )				gScreenBackGraphic.mpClipFuncs->DrawPixel( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawSprite_Clip( apCoords, apSprite )			gScreenBackGraphic.mpClipFuncs->DrawSprite( &gScreenBackGraphic, apCoords, apSprite )
#define Screen_Back_DrawTri_Clip( apCoords, aColour )				gScreenBackGraphic.mpClipFuncs->DrawTri( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_DrawQuad_Clip( apCoords, aColour )				gScreenBackGraphic.mpClipFuncs->DrawQuad( &gScreenBackGraphic, apCoords, aColour )
#define Screen_Back_FontPrint_Clip( apCoords, apFont, apString )	gScreenBackGraphic.mpClipFuncs->FontPrint( &gScreenBackGraphic, apCoords, apFont, apString )


/* ################################################################################ */

#endif	/*	INCLUDED_SCREEN_H	*/
