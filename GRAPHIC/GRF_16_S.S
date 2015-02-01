**************************************************************************************
*	GRF_4_S.S
*
*	4 BitPlane Rendering Functions
*
*	[c] 2002 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	INCLUDES
**************************************************************************************

	include	"GRAPHIC.I"

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	Graphic_16BPP_Blit
	export	Graphic_16BPP_ClearScreen
	export	Graphic_16BPP_CopyScreen
	export	Graphic_16BPP_DrawBox 
	export	Graphic_16BPP_DrawLine 
	export	Graphic_16BPP_DrawPixel 
	export	Graphic_16BPP_DrawSprite 
	export	Graphic_16BPP_DrawTri  
	export	Graphic_16BPP_DrawQuad 

	export	Graphic_16BPP_Blit_Clip
	export	Graphic_16BPP_ClearScreen_Clip
	export	Graphic_16BPP_CopyScreen_Clip
	export	Graphic_16BPP_DrawBox_Clip
	export	Graphic_16BPP_DrawLine_Clip
	export	Graphic_16BPP_DrawPixel_Clip
	export	Graphic_16BPP_DrawSprite_Clip
	export	Graphic_16BPP_DrawTri_Clip
	export	Graphic_16BPP_DrawQuad_Clip


**************************************************************************************
;	DEFINES
**************************************************************************************





**************************************************************************************
	TEXT
**************************************************************************************


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	Blit )( sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc );
* ACTION:   Blits rectangular area from apSrc to apDst
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_Blit:
	movem.l	d3-d7/a2-a6,-(a7)				; save registers

	move.l	11*4(a7),a2						; apRect
	move.l	12*4(a7),a3						; apSrc

	moveq	#0,d2
	moveq	#0,d3
	move.w	sGraphicCanvas_mWidth(a0),d2	; dst width
	move.w	sGraphicCanvas_mWidth(a3),d3	; src width
	add.l	d2,d2
	add.l	d3,d3


	moveq	#0,d0							; clear
	move.w	sGraphicPos_mX(a1),d0			; x pos
	move.w	sGraphicPos_mY(a1),d1			; y pos
	muls.w	sGraphicCanvas_mWidth(a0),d1	; y * width
	add.l	d1,d0
	add.l	d0,d0
	move.l	sGraphicCanvas_mpVRAM(a0),a0	; dst
	add.l	d0,a0							;+offset

	moveq	#0,d0							; clear
	move.w	sGraphicRect_mX(a2),d0			; x pos
	move.w	sGraphicRect_mY(a2),d1			; y pos
	muls.w	sGraphicCanvas_mWidth(a3),d1	; y * width
	add.l	d1,d0
	add.l	d0,d0
	move.l	sGraphicCanvas_mpVRAM(a3),a3	; src
	add.l	d0,a3							;+offset

	move.w	sGraphicRect_mHeight(a2),d1
	bra.s	.blt_first_y
.blt_loop_y:
	move.l	a0,a1							; dst
	move.l	a3,a2							; src
	move.w	sGraphicRect_mWidth(a2),d0
	bra.s	.blt_first_x
.blt_loop_x:
	move.w	(a2)+,(a1)+						; copy pixel
.blt_first_x:
	dbra	d0,.blt_loop_x
	add.l	d2,a0
	add.l	d3,a3
.blt_first_y:
	dbra	d1,.blt_loop_y

	movem.l	(a7)+,d3-d7/a2-a6				; restore registers
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	ClearScreen )( sGraphicCanvas * apCanvas )
* ACTION:   clears screen
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_ClearScreen:

	moveq	#0,d0
	move.w	sGraphicCanvas_mWidth(a0),d0
	mulu.w	sGraphicCanvas_mHeight(a0),d0
	move.l	sGraphicCanvas_mpVRAM(a0),a0
	moveq	#0,d1
.clr_loop:
	move.w	d1,(a0)+
	subq.l	#1,d0
	bne		.clr_loop

	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	CopyScreen )( sGraphicCanvas * apCanvas, void * apSrc )
* ACTION:   clears screen
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_CopyScreen:

	moveq	#0,d0
	move.w	sGraphicCanvas_mWidth(a0),d0
	mulu.w	sGraphicCanvas_mHeight(a0),d0
	move.l	sGraphicCanvas_mpVRAM(a0),a0
	moveq	#0,d1
.cpy_loop:
	move.w	(a1)+,(a0)+
	subq.l	#1,d0
	bne		.cpy_loop

	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	DrawBox )( sGraphicCanvas * apCanvas, sGraphicRect * apRect, U16 aColour )
* ACTION:   draws a box
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_DrawBox:
	move.l	d3,-(a7)

	moveq	#0,d1
	move.w	sGraphicCanvas_mWidth(a0),d1
	sub.w	sGraphicRect_mWidth(a1),d1
	add.l	d1,d1
	move.l	sGraphicCanvas_mpVRAM(a0),a0

	move.w	sGraphicRect_mHeight(a1),d3
	bra.s	.dbx_first_y
.dbx_loop_y:
	move.w	sGraphicRect_mWidth(a1),d2
	bra.s	.dbx_first_x
.dbx_loop_x:
	move.w	d0,(a0)+
.dbx_first_x:
	dbra	d2,.dbx_loop_x
	add.l	d1,a0
.dbx_first_y:
	dbra	d3,.dbx_loop_y

	move.l	(a7)+,d3
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	DrawPixel )( sGraphicCanvas * apCanvas, sGraphicRect * apRect, U16 aColour )
* ACTION:   draws a box
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_DrawPixel:
	moveq	#0,d1
	moveq	#0,d2
	move.w	sGraphicPos_mX(a0),d2
	move.w	sGraphicPos_mY(a0),d1
	muls.w	sGraphicCanvas_mWidth(a0),d1
	add.l	d2,d1
	add.l	d1,d1
	move.l	sGraphicCanvas_mpVRAM(a0),a0
	add.l	d1,a0
	move.w	d0,(a0)
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	DrawSprite )( sGraphicCanvas * apCanvas, sGraphicPos * apPos, sSprite * apSprite )
* ACTION:   draws a box
* CREATION: 25.03.05 PNK
*------------------------------------------------------------------------------------*

Graphic_16BPP_DrawSprite:
	movem.l	d3-d7/a2-a6,-(a7)				; save registers

	move.l	11*4(a7),a2						; 2d0 fix offset

	moveq	#0,d1
	moveq	#0,d2
	move.w	sGraphicPos_mX(a1),d2
	move.w	sGraphicPos_mY(a1),d1
	muls.w	sGraphicCanvas_mWidth(a0),d1
	add.l	d2,d1
	add.l	d1,d1

	moveq	#0,d5
	move.w	sGraphicCanvas_mWidth(a0),d5
	sub.w	sGraphicSprite_mWidth(a2),d5
	add.l	d5,d5

	move.l	sGraphicCanvas_mpVRAM(a0),a0
	add.l	d1,a0
	
	move.l	sGraphicSprite_mpGfx(a2),a3		; gfx
	move.l	sGraphicSprite_mpMask(a2),a4	; mask
	move.w	sGraphicSprite_mHeight(a2),d1	; spr height
	moveq	#0,d3
	tst.w	sGraphicSprite_mMaskPlaneCount(a2)
	bne.s	.dsp_masked
	move.w	#$7FFF,d3
.dsp_masked:
	bra		.dsp_first_y
.dsp_loop_y:
	move.l	d3,d7							; force mask read
	move.w	sGraphicSprite_mWidth(a2),d0
	bra		.dsp_first_x
.dsp_loop_x:
	dbra	d7,.dsp_msk
	move.w	(a4)+,d6						; read word of mask
	moveq	#15,d7							; bits in mask
.dsp_msk:
	add.w	d6,d6
	bcs		.dsp_skip
	move.w	(a3),(a0)
.dsp_skip:
	addq.l	#2,a0							; next pixel of dst
	addq.l	#2,a3							; next pixel of spr
.dsp_first_x:
	dbra	d0,.dsp_loop_x
	add.l	d5,a0							; go to next scanline
.dsp_first_y:
	dbra	d1,.dsp_loop_y

	movem.l	(a7)+,d3-d7/a2-a6				; restore registers
	rts

Graphic_16BPP_DrawLine:
	rts