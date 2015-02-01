**************************************************************************************
*	GRAPHIC.I
*
*	Graphic definitions
*
*	[c] 2005 Reservoir Gods
**************************************************************************************


**************************************************************************************
;	DEFINES
**************************************************************************************


**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET
sGraphicPos_mX:					ds.w	1
sGraphicPos_mY:					ds.w	1
sGraphicPos_sizeof:				ds.w	1

	OFFSET
sGraphicBox_mX0:				ds.w	1
sGraphicBox_mX1:				ds.w	1
sGraphicBox_mY0:				ds.w	1
sGraphicBox_mY1:				ds.w	1
sGraphicBox_sizeof:				ds.w	1

	OFFSET
sGraphicRect_mX:					ds.w	1
sGraphicRect_mY:					ds.w	1
sGraphicRect_mWidth:				ds.w	1
sGraphicRect_mHeight:			ds.w	1
sGraphicRect_sizeof:			ds.w	1


	OFFSET
sGraphicSprite_mpGfx:			ds.l	1
sGraphicSprite_mpMask:			ds.l	1
sGraphicSprite_mWidth:			ds.w	1
sGraphicSprite_mHeight:			ds.w	1
sGraphicSprite_mGfxPlaneCount:	ds.w	1
sGraphicSprite_mMaskPlaneCount:	ds.w	1
sGraphicSprite_sizeof:			ds.w	1


	OFFSET
sGraphicCanvas_mpVRAM:			ds.l	1
sGraphicCanvas_mColourMode:		ds.w	1
sGraphicCanvas_mWidth:			ds.w	1
sGraphicCanvas_mHeight:			ds.w	1
sGraphicCanvas_mpad:			ds.w	1
sGraphicCanvas_mClipBox:		ds.b	sGraphicBox_sizeof
sGraphicCanvas_mpFuncs:			ds.l	1
sGraphicCanvas_mpClipFuncs:		ds.l	1
sGraphicCanvas_mLineOffsets:	ds.l	485
sGraphicCanvas_sizeof:			ds.w	1

	OFFSET
slSpr_Xcount:					ds.w	1
slSpr_Xloop:					ds.w	1
slSpr_Yloop:					ds.w	1
slSpr_LeftMask:					ds.w	1
slSpr_sizeof:					ds.w	1
