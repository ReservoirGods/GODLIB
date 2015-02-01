**************************************************************************************
*	GRF_B4_S.S
*
*	4 BitPlane Blitter Rendering Functions
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

	export	Graphic_4BP_Blit_BLT
	export	Graphic_4BP_ClearScreen_BLT
	export	Graphic_4BP_CopyScreen_BLT
	export	Graphic_4BP_DrawBox_BLT
	export	Graphic_4BP_DrawLine_BLT
	export	Graphic_4BP_DrawPixel_BLT 
	export	Graphic_4BP_DrawSprite_BLT 
	export	Graphic_4BP_DrawTri_BLT  
	export	Graphic_4BP_DrawQuad_BLT 

	export	Graphic_4BP_Blit_Clip_BLT
	export	Graphic_4BP_ClearScreen_Clip_BLT
	export	Graphic_4BP_CopyScreen_Clip_BLT
	export	Graphic_4BP_DrawBox_Clip_BLT
	export	Graphic_4BP_DrawLine_Clip_BLT
	export	Graphic_4BP_DrawPixel_Clip_BLT
	export	Graphic_4BP_DrawSprite_Clip_BLT
	export	Graphic_4BP_DrawTri_Clip_BLT
	export	Graphic_4BP_DrawQuad_Clip_BLT


**************************************************************************************
;	DEFINES
**************************************************************************************

eBLITTERLOP_ZERO				EQU	0
eBLITTERLOP_SRC_AND_DST			EQU	1
eBLITTERLOP_SRC_ANDNOT_DST		EQU	2
eBLITTERLOP_SRC					EQU	3
eBLITTERLOP_NOTSRC_AND_DST		EQU	4
eBLITTERLOP_DST					EQU	5
eBLITTERLOP_SRC_XOR_DST			EQU	6
eBLITTERLOP_SRC_OR_DST			EQU	7
eBLITTERLOP_NOTSRC_ANDNOT_DST	EQU	8
eBLITTERLOP_NOTSRC_XOR_DST		EQU	9
eBLITTERLOP_NOTDST				EQU	10
eBLITTERLOP_SRC_ORNOT_DST		EQU	11
eBLITTERLOP_NOTSRC				EQU	12
eBLITTERLOP_NOTSRC_OR_DST		EQU	13
eBLITTERLOP_NOTSRC_ORNOT_DST	EQU	14
eBLITTERLOP_ONE					EQU	15

eBLITTERHOP_ONE					EQU	0
eBLITTERHOP_HALFTONE			EQU	1
eBLITTERHOP_SRC					EQU	2
eBLITTERHOP_SRC_AND_HALFTONE	EQU	3

eBLITTERMODE_LINENUMBER_MASK	EQU	$F
eBLITTERMODE_SMUDGE_BIT			EQU	$20
eBLITTERMODE_SMUDGE_MASK		EQU	$DF
eBLITTERMODE_HOG_BIT			EQU	$40
eBLITTERMODE_HOG_MASK			EQU	$BF
eBLITTERMODE_BUSY_BIT			EQU	$80
eBLITTERMODE_BUSY_MASK			EQU	$7F

eBLITTERSKEW_SKEW_MASK			EQU	$F
eBLITTERSKEW_NFSR_BIT			EQU	$40
eBLITTERSKEW_NFSR_MASK			EQU	$BF
eBLITTERSKEW_FXSR_BIT			EQU	$80
eBLITTERSKEW_FXSR_MASK			EQU	$7F

eBLITREG_HALFTONE				EQU	$FFFF8A00
eBLITREG_SRC_INC_X				EQU	$FFFF8A20
eBLITREG_SRC_INC_Y				EQU	$FFFF8A22
eBLITREG_pSRC					EQU	$FFFF8A24
eBLITREG_ENDMASK_1				EQU	$FFFF8A28
eBLITREG_ENDMASK_2				EQU	$FFFF8A2A
eBLITREG_ENDMASK_3				EQU	$FFFF8A2C
eBLITREG_DST_INC_X				EQU	$FFFF8A2E
eBLITREG_DST_INC_Y				EQU	$FFFF8A30
eBLITREG_pDST					EQU	$FFFF8A32
eBLITREG_COUNT_X				EQU	$FFFF8A36
eBLITREG_COUNT_Y				EQU	$FFFF8A38
eBLITREG_HOP					EQU	$FFFF8A3A
eBLITREG_LOP					EQU	$FFFF8A3B
eBLITREG_MODE					EQU	$FFFF8A3C
eBLITREG_SKEW					EQU	$FFFF8A3D

eBLITTER_BASE					EQU	$FFFF8A00
eBLITTER_HALFTONE				EQU	$00
eBLITTER_SRC_INC_X				EQU	$20
eBLITTER_SRC_INC_Y				EQU	$22
eBLITTER_pSRC					EQU	$24
eBLITTER_ENDMASK_1				EQU	$28
eBLITTER_ENDMASK_2				EQU	$2A
eBLITTER_ENDMASK_3				EQU	$2C
eBLITTER_DST_INC_X				EQU	$2E
eBLITTER_DST_INC_Y				EQU	$30
eBLITTER_pDST					EQU	$32
eBLITTER_COUNT_X				EQU	$36
eBLITTER_COUNT_Y				EQU	$38
eBLITTER_HOP					EQU	$3A
eBLITTER_LOP					EQU	$3B
eBLITTER_MODE					EQU	$3C
eBLITTER_SKEW					EQU	$3D

*    I n p u t   p a r a m e t e r   b l o c k   o f f s e t s

SRC_FORM  equ  0    ; Base address of source memory form .l
SRC_NXWD  equ  4    ; Offset between words in source plane .w
SRC_NXLN  equ  6    ; Source form width .w
SRC_NXPL  equ  8    ; Offset between source planes .w
SRC_XMIN  equ  10   ; Source blt rectangle minimum X .w
SRC_YMIN  equ  12   ; Source blt rectangle minimum Y .w

DST_FORM  equ  14   ; Base address of destination memory form .l 
DST_NXWD  equ  18   ; Offset between words in destination plane.w
DST_NXLN  equ  20   ; Destination form width .w
DST_NXPL  equ  22   ; Offset between destination planes .w
DST_XMIN  equ  24   ; Destination blt rectangle minimum X .w
DST_YMIN  equ  26   ; Destination blt rectangle minimum Y .w

WIDTH     equ  28   ; Width of blt rectangle .w
HEIGHT    equ  30   ; Height of blt rectangle .w
PLANES    equ  32   ; Number of planes to blt .w
BLIT_SIZEOF	equ 32

**************************************************************************************
;	STRUCTS
**************************************************************************************


**************************************************************************************
;	MACROS
**************************************************************************************

	MACRO	mBlitterWait_a1
	btst	#7,eBLITTER_MODE(a1)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

	MACRO	mBlitterWait_a2
	btst	#7,eBLITTER_MODE(a2)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

	MACRO	mBlitterWait_a6
	btst	#7,eBLITTER_MODE(a6)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

	MACRO	mBlitterGoWait_a1
	move.b	#eBLITTERMODE_BUSY_BIT,eBLITTER_MODE(a1)
	nop
	bset.b	#7,eBLITTER_MODE(a1)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

	MACRO	mBlitterGoWait_a2
	move.b	#eBLITTERMODE_BUSY_BIT,eBLITTER_MODE(a2)
	nop
	bset.b	#7,eBLITTER_MODE(a2)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

	MACRO	mBlitterGoWait_a6
	move.b	#eBLITTERMODE_BUSY_BIT,eBLITTER_MODE(a6)
	nop
	bset.b	#7,eBLITTER_MODE(a6)	; is blitter busy?
	nop
	bne.s	*-8						; yes, wait
	ENDM

**************************************************************************************
	TEXT
**************************************************************************************


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	Blit )(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc, void * apDst );
* ACTION:   Blits rectangular area from apSrc to apDst
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_BlitClip_BLT:
	movem.l	d3-d7/a2-a6,-(a7)				; save registers

	move.l	11*4(a7),a2
	move.l	12*4(a7),a3

	move.w	sGraphicRect_mX(a2),d0			; blit x
	move.w	sGraphicRect_mY(a2),d1			; blit y
	move.w	sGraphicRect_mWidth(a2),d2		; blit width
	move.w	sGraphicRect_mHeight(a2),d3		; blit height

	move.w	sGraphicPos_mX(a1),d4		; X
	move.w	sGraphicPos_mY(a1),d5		; Y

	tst.w	d0								; X < 0?	
	bpl		.rectx0_ok						; X >= 0, no left clip	

	add.w	d0,d2							; Clip Width	
	ble		.clip							; Entirely off left, clip	
	moveq	#0,d0							; Set New X0	
	
.rectx0_ok:

	move.w	d0,d6							; X
	add.w	d2,d6							; X+Width
	sub.w	sGraphicCanvas_mWidth(a3),d6	; Check Amount off right of screen
	ble		.rectx1_ok						; None, no clipping
	
	sub.w	d6,d2							; Clip Width	
	ble		.clip							; Entirely Off Left, Cull	

.rectx1_ok:

	tst.w	d1								; Y < 0 ?	
	bpl		.recty0_ok						; Y >= 0, no top clip	

	add.w	d1,d3							; Clip Height	
	ble		.clip							; Entirely Off Top, Cull	
	moveq	#0,d1							; Clip Y	
	
.recty0_ok:

	move.w	d1,d6							; Y	
	add.w	d3,d6							; Y+Width	
	sub.w	sGraphicCanvas_mHeight(a3),d6	; Check amount off bottom	
	ble		.recty1_ok						; None, no bot clip	
	
	sub.w	d6,d3							; Clip Height	
	ble		.clip							; Entirely off bottom, Cull	

.recty1_ok:


	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d4
	bge		.x0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d4
	add.w	d4,d2												; adjust width
	ble		.clip
	sub.w	d4,d0												; shift src x0
	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d4		; clip dst x0
	
.x0_ok:

	move.w	d4,d6												; dstX0	
	add.w	d2,d6												; dstX0+Width	
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX1(a0),d6		; Check Amount off right of screen	
	ble		.x1_ok												; None, no clipping	
	
	sub.w	d6,d2												; Clip Width	
	ble		.clip												; Entirely Off Left, Cull	

.x1_ok:

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d5
	bge		.y0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d4
	add.w	d5,d3												; adjust height
	ble		.clip
	sub.w	d5,d1												; shift src y0
	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d4		; clip dst y0

	
.y0_ok:

	move.w	d5,d6											; Y	
	add.w	d3,d6											; Y+Width	
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY1(a0),d6	; Check amount off bottom	
	ble		.y1_ok											; None, no bot clip	
	
	sub.w	d6,d3											; Clip Height	
	ble		.clip											; Entirely off bottom, Cull	

.y1_ok:


	bra		Graphic_4BP_Blit_Go					; Blit it	

.clip:
	movem.l	(a7)+,d3-d7/a2-a6				; restore registers	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (*	Blit )( const struct sGraphicCanvas * apCanvas, const sGraphicRect * apCoords, void * apSrc, void * apDst );
* ACTION:   Blits rectangular area from apSrc to apDst
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_Blit_BLT:
	movem.l	d3-d7/a2-a6,-(a7)
Graphic_4BP_Blit_Go:					; Blit it	

	movem.l	(a7)+,d3-d7/a2-a6
	rts



*------------------------------------------------------------------------------------*
* FUNCTION: void (*	ClearScreen )( const struct sGraphicCanvas * apCanvas );
* ACTION:   clears a 320x200 screen
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_ClearScreen_BLT:
	movea.w	#eBLITTER_BASE,a1

	mBlitterWait_a1

	move.w	sGraphicCanvas_mWidth(a0),d0
	lsr.w	#2,d0

	move.w	d0,eBLITTER_COUNT_X(a1)
	move.w	sGraphicCanvas_mHeight(a0),eBLITTER_COUNT_Y(a1)
	move.l	sGraphicCanvas_mpVRAM(a0),eBLITTER_pDST(a1)

	move.l	#$00020002,eBLITTER_DST_INC_X(a1)

	moveq	#-1,d0
	move.l	d0,eBLITTER_ENDMASK_1(a1)
	move.w	d0,eBLITTER_ENDMASK_3(a1)

	move.l	#$00008000,eBLITTER_HOP(a1)					; HOP=1 :LOP=ALL ZERO:MODE=GO|SHAREBUS:SKEW=0

	mBlitterGoWait_a1

	rts



*------------------------------------------------------------------------------------*
* FUNCTION: void (*	CopyScreen )(  const struct sGraphicCanvas * apCanvas, void * apSrc, void * apDst );
* ACTION:   copies a 320x200 screen
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_CopyScreen_BLT:
	move.l	a2,-(a7)							; save regs
	movea.w	#eBLITTER_BASE,a2

	mBlitterWait_a2

	move.w	sGraphicCanvas_mWidth(a0),d0
	lsr.w	#2,d0

	move.w	d0,eBLITTER_COUNT_X(a2)
	move.w	sGraphicCanvas_mHeight(a0),eBLITTER_COUNT_Y(a2)
	move.l	a1,eBLITTER_pSRC(a2)
	move.l	sGraphicCanvas_mpVRAM(a0),eBLITTER_pDST(a2)

	move.l	#$00020002,eBLITTER_SRC_INC_X(a2)
	move.l	#$00020002,eBLITTER_DST_INC_X(a2)

	moveq	#-1,d0
	move.l	d0,eBLITTER_ENDMASK_1(a2)
	move.w	d0,eBLITTER_ENDMASK_3(a2)

	move.l	#$02038000,eBLITTER_HOP(a2)					; HOP=SRC :LOP=SRC:MODE=GO|SHAREBUS:SKEW=0

	mBlitterGoWait_a2

	move.l	(a7)+,a2
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawBox )(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
* ACTION:   draws a box with clipping
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_DrawBox_Clip_BLT:
	movem.l	d3-d7/a2-a3,-(a7)					; save regs

	move.w	sGraphicRect_mX(a1),d1				; Pixel X
	move.w	sGraphicRect_mY(a1),d2				; Pixel Y
	move.w	sGraphicRect_mWidth(a1),d3			; Width
	ble		.clip
	move.w	sGraphicRect_mHeight(a1),d4			; Height
	ble		.clip

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d1
	bge		.x0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d1
	add.w	d1,d3												; adjust width
	ble		.clip
	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d1		; clip dst x0
	
.x0_ok:

	move.w	d1,d5												; X
	add.w	d3,d5												; X+Width
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX1(a0),d5		; Check Amount off right of screen
	ble		.x1_ok												; None, no clipping
	
	sub.w	d5,d3												; Clip Width
	ble		.clip												; Entirely Off Left, Cull

.x1_ok:

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d2
	bge		.y0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d2
	add.w	d2,d4												; adjust height
	ble		.clip
	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d2		; clip dst y0
	
.y0_ok:

	move.w	d2,d5												; Y
	add.w	d4,d5												; Y+Width
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY1(a0),d5		; Check amount off bottom
	ble		.y1_ok												; None, no bot clip
	
	sub.w	d5,d4												; Clip Height
	ble		.clip												; Entirely off bottom, Cull

.y1_ok:

	bra		Graphic_4BP_DrawBox_Go				; Render box

.clip:
	movem.l	(a7)+,d3-d7/a2-a3					; restore registers
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawBox )(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
* ACTION:   draws a box
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_DrawBox_BLT:
	movem.l	d3-d7/a2-a3,-(a7)					; save regs

	move.w	sGraphicRect_mX(a1),d1				; Pixel X
	move.w	sGraphicRect_mY(a1),d2				; Pixel Y
	move.w	sGraphicRect_mWidth(a1),d3			; Width
	move.w	sGraphicRect_mHeight(a1),d4			; Height

Graphic_4BP_DrawBox_Go:
	movea.w	#eBLITTER_BASE,a2

	mBlitterWait_a2

	move.l	sGraphicCanvas_mpVRAM(a0),a3

	add.w	d2,d2								; Y * 2
	add.w	d2,d2								; Y * 4
	add.l	#sGraphicCanvas_mLineOffsets,d2		; Line Offsets Table
	add.l	(a0,d2.w),a3						; Get To Y

	move.w	d1,d2								; X
	and.l	#$0000FFF0,d2						; To Nearest 16
	lsr.w	#1,d2								; 8 Byte Offset for bitplane
	add.l	d2,a3								; Get To X

	subq.w	#1,d3								; width-1
	add.w	d1,d3								; X1=X0+width-1

	move.w	d1,d7								; X0
	move.w	d3,d5								; X1

	and.w	#$FFF0,d7							; X0 to nearest 16
	and.w	#$FFF0,d5							; X1 to nearest 16
	sub.w	d7,d5								; PixelCount
	lsr.w	#4,d5								; /16 = ChunkCount
	addq.w	#1,d5

	move.w	d5,eBLITTER_COUNT_X(a2)				; set x count
	move.w	#8,eBLITTER_DST_INC_X(a2)			; offset to next chunk

	move.w	d5,d7								; xcount
	lsl.w	#3,d7								; xcount * sizeof(bitplane)
	move.w	sGraphicCanvas_mLineOffsets+6(a0),d6	; LineSize
	addq.w	#8,d6								; +8
	sub.w	d7,d6								; -xcount
	move.w	d6,eBLITTER_DST_INC_Y(a2)			; set dst y inc

	move.b	#eBLITTERHOP_ONE,eBLITTER_HOP(a2)
	clr.b	eBLITTER_SKEW(a2)

	and.w	#15,d1								; X0 & 15
	and.w	#15,d3								; X1 & 15
	add.w	d1,d1
	add.w	d3,d3
	lea		gGraphic_4BP_LeftMasks,a0
	move.w	(a0,d1.w),d1
	lea		gGraphic_4BP_RightMasks,a0
	move.w	(a0,d3.w),d3

	cmp.w	#1,d5
	bgt		.xcnot1

	and.w	d1,d3
	move.w	d3,eBLITTER_ENDMASK_1(a2)
	bra		.start

.xcnot1:
	cmp.w	#2,d5
	bgt		.xcnot2

	move.w	d1,eBLITTER_ENDMASK_1(a2)
	move.w	d3,eBLITTER_ENDMASK_2(a2)
	move.w	d3,eBLITTER_ENDMASK_3(a2)

	bra		.start

.xcnot2:

	move.w	d1,eBLITTER_ENDMASK_1(a2)
	move.w	#$FFFF,eBLITTER_ENDMASK_2(a2)
	move.w	d3,eBLITTER_ENDMASK_3(a2)

.start:
	
	lea		Graphic_4BP_LopColours,a0
	add.w	d0,d0
	add.w	d0,d0
	move.l	(a0,d0.w),d0
	

	move.b	d0,eBLITTER_LOP(a2)
	move.l	a3,eBLITTER_pDST(a2)
	move.w	d4,eBLITTER_COUNT_Y(a2)
	mBlitterGoWait_a2

	lsr.l	#8,d0
	addq.l	#2,a3
	move.b	d0,eBLITTER_LOP(a2)
	move.l	a3,eBLITTER_pDST(a2)
	move.w	d4,eBLITTER_COUNT_Y(a2)
	mBlitterGoWait_a2

	lsr.l	#8,d0
	addq.l	#2,a3
	move.b	d0,eBLITTER_LOP(a2)
	move.l	a3,eBLITTER_pDST(a2)
	move.w	d4,eBLITTER_COUNT_Y(a2)
	mBlitterGoWait_a2

	lsr.l	#8,d0
	addq.l	#2,a3
	move.b	d0,eBLITTER_LOP(a2)
	move.l	a3,eBLITTER_pDST(a2)
	move.w	d4,eBLITTER_COUNT_Y(a2)
	mBlitterGoWait_a2


	movem.l	(a7)+,d3-d7/a2-a3					; restore regs
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawPixel )(   const struct sGraphicCanvas * apCanvas, const sGraphicPos * apCoords,  const S16 aColour )
* ACTION:   draws a pixel with clipping
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_DrawPixel_Clip_BLT:

	move.w	sGraphicPos_mX(a1),d1				; Pixel X
	move.w	sGraphicPos_mY(a1),d2				; Pixel Y

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d1
	blt		.clip
	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX1(a0),d1
	bge		.clip
	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d2
	blt		.clip
	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY1(a0),d2
	bge		.clip
	
	
	bra		Graphic_4BP_DrawPixel_Go			; Draw Pixel

.clip:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawPixel )(   const struct sGraphicCanvas * apCanvas, const sGraphicPos * apCoords,  const S16 aColour )
* ACTION:   draws a pixel
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_DrawPixel_BLT:

	move.w	sGraphicPos_mX(a1),d1				; Pixel X
	move.w	sGraphicPos_mY(a1),d2				; Pixel Y

Graphic_4BP_DrawPixel_Go:

	add.w	d2,d2								; Y * 2
	add.w	d2,d2								; Y * 4
	add.l	#sGraphicCanvas_mLineOffsets,d2		; Line Offsets Table
	move.l	(a0,d2.w),a1						; Get To Y

	move.w	d1,d2								; X
	and.l	#$0000FFF0,d2						; To Nearest 16
	lsr.w	#1,d2								; 8 Byte Offset for bitplane
	add.l	d2,a1								; Get To X

	add.l	sGraphicCanvas_mpVRAM(a0),a1			; Get To Position On Screen

	and.w	#15,d1								; X & 15
	add.w	d1,d1								; *2 As Offset In Table
	lea		gGraphic_4BP_Points,a0
	move.w	(a0,d1.w),d1						; Read Point
	move.w	d1,d2								; Copy Point
	not.w	d2									; Make Mask

	and.w	d2,(a1)								; Mask Plane 0
	and.w	d2,2(a1)							; Mask Plane 1
	and.w	d2,4(a1)							; Mask Plane 2
	and.w	d2,6(a1)							; Mask Plane 3

	lsr.w	#1,d0								; Check Bit 0
	bcc.s	.nbp0								; Not Set
	or.w	d1,6(a1)							; Draw To Plane0
.nbp0:
	lsr.w	#1,d0								; Check Bit 1
	bcc.s	.nbp1								; Not Set
	or.w	d1,4(a1)							; Draw To Plane 1
.nbp1:
	lsr.w	#1,d0								; Check Bit 2
	bcc.s	.nbp2								; Not Set
	or.w	d1,2(a1)							; Draw To Plane 2
.nbp2:
	lsr.w	#1,d0								; Check Bit 3
	bcc.s	.nbp3								; Not Set
	or.w	d1,0(a1)							; Draw To Plane 3
.nbp3:

	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawSprite )(  const struct sGraphicCanvas * apCanvas, const sGraphicPos * apCoords,  const void * apSprite );
* ACTION:   draws a sprite with clipping
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*
cunt:
Graphic_4BP_DrawSprite_Clip_BLT:
	movem.l	d3-d7/a2-a6,-(a7)

	move.l	11*4(a7),a2
	move.w	sGraphicPos_mX(a1),d0
	move.w	sGraphicPos_mY(a1),d1
	move.w	sGraphicSprite_mWidth(a2),d2
	move.w	sGraphicSprite_mHeight(a2),d3
	move.l	sGraphicSprite_mpGfx(a2),a3
	move.l	sGraphicSprite_mpMask(a2),a4


	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX1(a0),d0
	bge		.clip
	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY1(a0),d1
	bge		.clip

	move.w	d2,d7
	subq.w	#1,d7
	lsr.w	#4,d7

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d1
	bge		.y0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d1
	add.w	d1,d3												; adjust height
	ble		.clip


	move.w	sGraphicSprite_mWidth(a2),d4
	add.w	#15,d4
	and.l	#$0000FFF0,d4
	lsr.w	#1,d4
	move.w	d4,d5
	lsr.w	#2,d5

	neg.w	d1
	mulu.w	d1,d4
	mulu.w	d1,d5
	add.l	d4,a3
	add.l	d5,a4

	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mY0(a0),d1		; clip dst y0
.y0_ok:

	move.w	d1,d4												;y
	add.w	d3,d4												;y2
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mY1(a0),d4
	ble		.y1_ok

	sub.w	d4,d3											; clip height
	ble		.clip

.y1_ok:

	movea.w	#eBLITTER_BASE,a6

	mBlitterWait_a6
	moveq			#$f,d5
	and.w			d0,d5
	move.b			d5,eBLITTER_SKEW(a6)

	cmp.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d0
	bge		.x0_ok
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d0
	add.w	d0,d2												; adjust height
	ble		.clip

	neg.w	d0
	lsr.w	#4,d0
	sub.w	d0,d7
	lsl.w	#3,d0
	add.w	d0,a3
	lsr.w	#2,d0
	add.w	d0,a4

	move.w	sGraphicCanvas_mClipBox+sGraphicBox_mX0(a0),d0
.x0_ok:

	move.w	d0,d4							;x
	add.w	d2,d4							;x2
	sub.w	sGraphicCanvas_mClipBox+sGraphicBox_mX1(a0),d4
	ble		.x1_ok

	sub.w	d4,d2							; clip width
	ble		.clip

	lsr.w	#4,d4
	sub.w	d4,d7

.x1_ok:
	move.w	#2,eBLITTER_SRC_INC_X(a6)			; offset to next chunk
	move.w	d0,d5
	move.w	d0,d6
	add.w	d2,d6
	subq.w	#1,d6
	lsr.w	#4,d5
	lsr.w	#4,d6
	sub.w	d5,d6
	cmp.w	d7,d6
	beq.s	.same
	bgt.s	.dstbig

	or.b	#eBLITTERSKEW_FXSR_BIT,eBLITTER_SKEW(a6)	
	bra		.go
.dstbig:
	or.b	#eBLITTERSKEW_NFSR_BIT,eBLITTER_SKEW(a6)	
	bra		.go
.same:
	moveq	#$f,d7
	and.w	d0,d7
	move.b	eBLITTER_SKEW(a6),d5
	cmp.b	d5,d7
	bgt		.go

	tst.w	d6
	beq.s	.singlespan

	or.b	#eBLITTERSKEW_FXSR_BIT+eBLITTERSKEW_NFSR_BIT,eBLITTER_SKEW(a6)	
	bra		.go
.singlespan:

	or.b	#eBLITTERSKEW_FXSR_BIT,eBLITTER_SKEW(a6)	
	move.w	#0,eBLITTER_SRC_INC_X(a6)			; offset to next chunk

.go:
	bra		Graphic_4BP_DrawSprite_Go

.clip:

	movem.l	(a7)+,d3-d7/a2-a6
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void (* DrawSprite )(  const struct sGraphicCanvas * apCanvas, const sGraphicPos * apCoords,  const void * apSprite );
* ACTION:   draws a sprite
* CREATION: 01.02.02 PNK
*------------------------------------------------------------------------------------*

Graphic_4BP_DrawSprite_BLT:
	movem.l	d3-d7/a2-a6,-(a7)

	move.l	11*4(a7),a2
	move.w	sGraphicPos_mX(a1),d0
	move.w	sGraphicPos_mY(a1),d1
	move.w	sGraphicSprite_mWidth(a2),d2
	move.w	sGraphicSprite_mHeight(a2),d3
	move.l	sGraphicSprite_mpGfx(a2),a3
	move.l	sGraphicSprite_mpMask(a2),a4

	movea.w	#eBLITTER_BASE,a6

	mBlitterWait_a6
	moveq			#$f,d4
	and.w			d0,d4
	beq.s			.noNFSR
	or.b			#eBLITTERSKEW_NFSR_BIT,d4
.noNFSR:
	move.b			d4,eBLITTER_SKEW(a6)
	move.w			#2,eBLITTER_SRC_INC_X(a6)			; offset to next chunk

Graphic_4BP_DrawSprite_Go:


	move.l	sGraphicCanvas_mpVRAM(a0),a5

	add.w	d1,d1								; Y * 2
	add.w	d1,d1								; Y * 4
	add.l	#sGraphicCanvas_mLineOffsets,d1		; Line Offsets Table
	add.l	(a0,d1.w),a5						; Get To Y

	move.w	d0,d1								; X
	and.l	#$0000FFF0,d1						; To Nearest 16
	lsr.w	#1,d1								; 8 Byte Offset for bitplane
	add.l	d1,a5								; Get To X

;	bsr		Atari_BlitInit

	subq.w	#1,d2								; width-1

	move.w	sGraphicSprite_mWidth(a2),d6		; Sprite Width
	lsr.w	#4,d6
	move.w	d2,d7
	lsr.w	#4,d7
	sub.w	d7,d6								; -xcount
	add.w	d6,d6
	move.w	d6,eBLITTER_SRC_INC_Y(a6)			; set dst y inc

	add.w	d0,d2								; X1=X0+width-1

	move.w	d0,d7								; X0
	move.w	d2,d5								; X1

	and.w	#$FFF0,d7							; X0 to nearest 16
	and.w	#$FFF0,d5							; X1 to nearest 16
	sub.w	d7,d5								; PixelCount
	lsr.w	#4,d5								; /16 = ChunkCount

	move.w	d5,eBLITTER_COUNT_X(a6)				; set x count
	addq.w	#1,eBLITTER_COUNT_X(a6)
	move.w	#8,eBLITTER_DST_INC_X(a6)			; offset to next chunk

	move.w	d5,d7								; xcount
	lsl.w	#3,d7								; xcount * sizeof(bitplane)
	move.w	sGraphicCanvas_mLineOffsets+6(a0),d6	; LineSize
	sub.w	d7,d6								; -xcount
	move.w	d6,eBLITTER_DST_INC_Y(a6)			; set dst y inc



	move.b	#eBLITTERHOP_SRC,eBLITTER_HOP(a6)

	and.w	#15,d0								; X0 & 15
	and.w	#15,d2								; width

	add.w	d0,d0
	add.w	d2,d2
	lea		gGraphic_4BP_LeftMasks,a1
	move.w	(a1,d0.w),d0
	lea		gGraphic_4BP_RightMasks,a1
	move.w	(a1,d2.w),d2

	tst.w	d5
	bne		.xcnot1

	and.w	d0,d2
	move.w	d2,eBLITTER_ENDMASK_1(a6)
	bra		.start

.xcnot1:
	cmp.w	#1,d5
	bgt		.xcnot2

	move.w	d0,eBLITTER_ENDMASK_1(a6)
	move.w	d2,eBLITTER_ENDMASK_2(a6)
	move.w	d2,eBLITTER_ENDMASK_3(a6)

	bra		.start

.xcnot2:

	move.w	d0,eBLITTER_ENDMASK_1(a6)
	move.w	#$FFFF,eBLITTER_ENDMASK_2(a6)
	move.w	d2,eBLITTER_ENDMASK_3(a6)

.start:
		
	move.b	#eBLITTERLOP_SRC,eBLITTER_LOP(a6)
	tst.w	sGraphicSprite_mMaskPlaneCount(a2)
	beq		.noMASKING

	move.b	#eBLITTERLOP_SRC_AND_DST,eBLITTER_LOP(a6)

	move.l	a4,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a5
	move.l	a4,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a5
	move.l	a4,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a5
	move.l	a4,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	subq.l	#6,a5
	move.b	#eBLITTERLOP_SRC_OR_DST,eBLITTER_LOP(a6)



.noMASKING:
;	lea		gAtariBlit,a4
;	bsr		BLiT_iT
;	bra		G4BP_DrawSprite_RTS
	move.w	eBLITTER_SRC_INC_Y(a6),d0
	mulu	sGraphicSprite_mGfxPlaneCount(a2),d0
;	lsl.w	#2,d0
	move.w	d0,eBLITTER_SRC_INC_Y(a6)
	move.w	eBLITTER_SRC_INC_X(a6),d0			; offset to next chunk
	mulu	sGraphicSprite_mGfxPlaneCount(a2),d0
	move.w	d0,eBLITTER_SRC_INC_X(a6)

	move.l	a3,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a3
	addq.l	#2,a5
	move.l	a3,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a3
	addq.l	#2,a5
	move.l	a3,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6

	addq.l	#2,a3
	addq.l	#2,a5
	move.l	a3,eBLITTER_pSRC(a6)
	move.l	a5,eBLITTER_pDST(a6)
	move.w	d3,eBLITTER_COUNT_Y(a6)
	mBlitterGoWait_a6


G4BP_DrawSprite_RTS:

	movem.l	(a7)+,d3-d7/a2-a6
	rts


Atari_BlitInit:
	move.l	a4,-(a7)
	lea	gAtariBlit,a4

	move.l	a3,SRC_FORM(a4)
;	move.w	#8,SRC_NXWD(a4)
	move.w	#0,SRC_NXWD(a4)
	move.w	#8,SRC_NXLN(a4)
	move.w	#2,SRC_NXPL(a4)
	move.w	#4,SRC_XMIN(a4)
	move.w	#0,SRC_YMIN(a4)

	move.l	a5,DST_FORM(a4)
	move.w	#8,DST_NXWD(a4)
	move.w	#160,DST_NXLN(a4)
	move.w	#2,DST_NXPL(a4)
	move.w	d0,DST_XMIN(a4)
	move.w	d1,DST_YMIN(a4)

	move.w	d2,WIDTH(a4)
	move.w	d3,HEIGHT(a4)
	move.w	#4,PLANES(a4)
	move.l	(a7)+,a4
	rts

* (c) 1987 Atari Corporation
*    All Rights Reserved.

* BLiTTER BASE ADDRESS

BLiTTER   equ  $FF8A00

* BLiTTER REGISTER OFFSETS

Halftone  equ  0
Src_Xinc  equ  32
Src_Yinc  equ  34
Src_Addr  equ  36
Endmask1  equ  40
Endmask2  equ  42
Endmask3  equ  44
Dst_Xinc  equ  46
Dst_Yinc  equ  48
Dst_Addr  equ  50
X_Count   equ  54
Y_Count   equ  56
HOP       equ  58
OP        equ  59
Line_Num  equ  60
Skew      equ  61

* BLiTTER REGISTER FLAGS

fHOP_Source    equ  1
fHOP_Halftone  equ  0

fSkewFXSR      equ  7
fSkewNFSR      equ  6

fLineBusy      equ  7
fLineHog       equ  6
fLineSmudge    equ  5

* BLiTTER REGISTER MASKS

mHOP_Source    equ  $02
mHOP_Halftone  equ  $01

mSkewFXSR      equ  $80
mSkewNFSR      equ  $40

mLineBusy      equ  $80
mLineHog       equ  $40
mLineSmudge    equ  $20

*         E n D m A s K   d A t A
*
* These tables are referenced by PC relative instructions.  Thus,
* the labels on these tables must remain within 128 bytes of the
* referencing instructions forever.  Amen.
*
* 0: Destination  1: Source   <<< Invert right end mask data >>>

lf_endmask:
  dc.w $FFFF

rt_endmask:
  dc.w $7FFF
  dc.w $3FFF
  dc.w $1FFF
  dc.w $0FFF
  dc.w $07FF
  dc.w $03FF
  dc.w $01FF
  dc.w $00FF
  dc.w $007F
  dc.w $003F
  dc.w $001F
  dc.w $000F
  dc.w $0007
  dc.w $0003
  dc.w $0001
  dc.w $0000

* TiTLE:  BLiT_iT
*
* PuRPoSE:
*    Transfer a rectangular block of pixels located at an
*    arbitrary X,Y position in the source memory form to
*    another arbitrary X,Y position in the destination memory
*    form using replace mode (boolean operator 3).
*    The source and destination rectangles should not overlap.
*
* iN:
*    a4   pointer to 34 byte input parameter block
*
* Note: This routine must be executed in supervisor mode as
*    access is made to hardware registers in the protected region
*    of the memory map.
*
*


BLiT_iT:

      lea  BLiTTER,a5          ; a5-> BLiTTER register block  

 *
 * Calculate Xmax coordinates from Xmin coordinates and width
 *
      move.w    WIDTH(a4),d6  
      subq.w    #1,d6               ; d6<- width-1  

      move.w    SRC_XMIN(a4),d0     ; d0<- src Xmin  
      move.w    d0,d1
      add.w     d6,d1               ; d1<- src Xmax=src Xmin+width-1

      move.w    DST_XMIN(a4),d2     ; d2<- dst Xmin  
      move.w    d2,d3
      add.w     d6,d3               ; d3<- dst Xmax=dstXmin+width-1

 *
 * Endmasks are derived from source Xmin mod 16 and source Xmax
 *    mod 16
 *  
      moveq.l   #$0F,d6   ; d6<- mod 16 mask

      move.w    d2,d4          ; d4<- DST_XMIN
      and.w     d6,d4          ; d4<- DST_XMIN mod 16
      add.w     d4,d4          ; d4<- offset into left end mask tbl

      move.w    lf_endmask(pc,d4.w),d4        ; d4<- left endmask

      move.w    d3,d5          ; d5<- DST_XMAX
      and.w     d6,d5          ; d5<- DST_XMAX mod 16
      add.w     d5,d5          ; d5<- offset into right end mask tbl

      move.w    rt_endmask(pc,d5.w),d5   ; d5<-inverted right end mask
      not.w     d5                       ; d5<- right end mask

 *  
 * Skew value is (destination Xmin mod 16 - source Xmin mod 16)
 * && 0x000F.  Three discriminators are used to determine the
 * states of FXSR and NFSR flags:
 *
 *    bit 0     0: Source Xmin mod 16 =< Destination Xmin mod 16
 *              1: Source Xmin mod 16 >  Destination Xmin mod 16
 *
 *    bit 1     0: SrcXmax/16-SrcXmin/16 <> DstXmax/16-DstXmin/16
 *                   Source span              Destination span
 *              1: SrcXmax/16-SrcXmin/16 == DstXmax/16-DstXmin/16
 *
 *    bit 2     0: multiple word Destination span
 *              1: single word Destination span
 *
 *    These flags form an offset into a skew flag table yielding
 *    correct FXSR and NFSR flag states for the given source and
 *    destination alignments
 *

      move.w    d2,d7     ; d7<- Dst Xmin
      and.w     d6,d7     ; d7<- Dst Xmin mod16
      and.w     d0,d6     ; d6<- Src Xmin mod16
      sub.w     d6,d7     ; d7<- Dst Xmin mod16-Src Xmin mod16
 *                        ; if Sx&F > Dx&F then cy:1 else cy:0
      clr.w     d6        ; d6<- initial skew flag table index
      addx.w    d6,d6     ; d6[bit0]<- intraword alignment flag

      lsr.w     #4,d0     ; d0<- word offset to src Xmin
      lsr.w     #4,d1     ; d1<- word offset to src Xmax
      sub.w     d0,d1     ; d1<- Src span - 1

      lsr.w     #4,d2     ; d2<- word offset to dst Xmin
      lsr.w     #4,d3     ; d3<- word offset to dst Xmax
      sub.w     d2,d3     ; d3<- Dst span - 1
      bne       set_endmasks   ; 2nd discriminator is one word dst  

 * When destination spans a single word, both end masks are merged
 * into Endmask1.  The other end masks will be ignored by the BLiTTER

      and.w     d5,d4          ; d4<- single word end mask
      addq.w    #4,d6          ; d6[bit2]:1 => single word dst

set_endmasks:

      move.w    d4,Endmask1(a5)     ; left end mask
      move.w    #$FFFF,Endmask2(a5) ; center end mask
      move.w    d5,Endmask3(a5)     ; right end mask

      cmp.w     d1,d3          ; the last discriminator is the
      bne       set_count      ; equality of src and dst spans

      addq.w    #2,d6          ; d6[bit1]:1 => equal spans

set_count:
      move.w    d3,d4
      addq.w    #1,d4          ; d4<- number of words in dst line
      move.w    d4,X_Count(a5) ; set value in BLiTTER

 * Calculate Source starting address:
 *
 *   Source Form address              +
 *  (Source Ymin * Source Form Width) +
 * ((Source Xmin/16) * Source Xinc)  

      move.l    SRC_FORM(a4),a0     ; a0-> start of Src form
      move.w    SRC_YMIN(a4),d4     ; d4<- offset in lines to Src Ymin
      move.w    SRC_NXLN(a4),d5     ; d5<- length of Src form line
      mulu      d5,d4               ; d4<- byte offset to (0, Ymin)
      add.l     d4,a0               ; a0-> (0, Ymin)

      move.w    SRC_NXWD(a4),d4;    d4<- offset between consecutive
      move.w    d4,Src_Xinc(a5)     ;      words in Src plane

      mulu      d4,d0          ; d0<- offset to word containing Xmin
      add.l     d0,a0          ; a0-> 1st src word (Xmin, Ymin)

 * Src_Yinc is the offset in bytes from the last word of one Source
 * line to the first word of the next Source line

      mulu      d4,d1               ; d1<- width of src line in bytes
      sub.w     d1,d5               ; d5<- value added to ptr at end
      move.w    d5,Src_Yinc(a5)     ; of line to reach start of next

 * Calculate Destination starting address

      move.l    DST_FORM(a4),a1     ; a1-> start of dst form
      move.w    DST_YMIN(a4),d4     ; d4<- offset in lines to dst Ymin
      move.w    DST_NXLN(a4),d5     ; d5<- width of dst form

      mulu      d5,d4     ; d4<- byte offset to (0, Ymin)
      add.l     d4,a1     ; a1-> dst (0, Ymin)

      move.w    DST_NXWD(a4),d4     ; d4<- offset between consecutive
      move.w    d4,Dst_Xinc(a5)     ;  words in dst plane

      mulu      d4,d2               ; d2<- DST_NXWD * (DST_XMIN/16)
      add.l     d2,a1               ; a1-> 1st dst word (Xmin, Ymin)

 * Calculate Destination Yinc

      mulu      d4,d3               ; d3<- width of dst line - DST_NXWD
      sub.w     d3,d5               ; d5<- value added to dst ptr at
      move.w    d5,Dst_Yinc(a5)     ;  end of line to reach next line

 * The low nibble of the difference in Source and Destination alignment
 * is the skew value.  Use the skew flag index to reference FXSR and
 * NFSR states in skew flag table.

      and.b     #$0F,d7                  ; d7<- isolated skew count
      or.b      skew_flags(pc,d6.w),d7 ; d7<- necessary flags and skew
      move.b    d7,Skew(a5)              ; load Skew register  

      move.b    #mHOP_Source,HOP(a5)     ; set HOP to source only
      move.b    #3,OP(a5)           ; set OP to "replace" mode

      lea       Line_Num(a5),a2     ; fast refer to Line_Num register
      move.b    #fLineBusy,d2       ; fast refer to LineBusy flag
      move.w    PLANES(a4),d7       ; d7 <- plane counter
      bra       begin

 *    T h e   s e t t i n g   o f   s k e w   f l a g s
 *
 *
 * QUALIFIERS   ACTIONS           BITBLT DIRECTION: LEFT -> RIGHT
 *
 * equal Sx&F>
 * spans Dx&F FXSR NFSR
 *
 * 0     0     0    1 |..ssssssssssssss|ssssssssssssss..|  
 *   |......dddddddddd|dddddddddddddddd|dd..............|  
 *  
 * 0     1     1    0 |..dddddddddddddd|dddddddddddddd..|
 *   |......ssssssssss|ssssssssssssssss|ss..............|  
 *  
 * 1     0     0    0 |..ssssssssssssss|ssssssssssssss..|  
 *                    |...ddddddddddddd|ddddddddddddddd.|  
 *  
 * 1     1     1    1 |...sssssssssssss|sssssssssssssss.|  
 *                    |..dddddddddddddd|dddddddddddddd..|  


skew_flags:

      dc.b mSkewNFSR           ; Source span < Destination span
      dc.b mSkewFXSR           ; Source span > Destination span
      dc.b 0                   ; Spans equal Shift Source right
      dc.b mSkewNFSR+mSkewFXSR ; Spans equal Shift Source left

 * When Destination span is but a single word ...

      dc.b 0         ; Implies a Source span of no words
      dc.b mSkewFXSR ; Source span of two words
      dc.b 0         ; Skew flags aren't set if Source and
      dc.b mSkewFXSR ; Spans equal Shift Source left
;      dc.b 0         ; Destination spans are both one word

next_plane:
      move.l    a0,Src_Addr(a5)     ; load Source pointer to this plane

      move.l    a1,Dst_Addr(a5)     ; load Dest ptr to this plane  
      move.w    HEIGHT(a4),Y_Count(a5)   ; load the line count  

      move.b    #mLineBusy,(a2)     ; <<< start the BLiTTER >>>  

      add.w     SRC_NXPL(a4),a0     ; a0-> start of next src plane  
      add.w     DST_NXPL(a4),a1     ; a1-> start of next dst plane  


 * The BLiTTER is usually operated with the HOG flag cleared.
 * In this mode the BLiTTER and the ST's cpu share the bus equally,
 * each taking 64 bus cycles while the other is halted.  This mode
 * allows interrupts to be fielded by the cpu while an extensive
 * BitBlt is being processed by the BLiTTER.  There is a drawback in
 * that BitBlts in this shared mode may take twice as long as BitBlts
 * executed in hog mode.  Ninety percent of hog mode performance is
 * achieved while retaining robust interrupt handling via a method
 * of prematurely restarting the BLiTTER.  When control is returned
 * to the cpu by the BLiTTER, the cpu immediately resets the BUSY
 * flag, restarting the BLiTTER after just 7 bus cycles rather than
 * after the usual 64 cycles.  Interrupts pending will be serviced
 * before the restart code regains control.  If the BUSY flag is
 * reset when the Y_Count is zero, the flag will remain clear
 * indicating BLiTTER completion and the BLiTTER won't be restarted.
 *
 * (Interrupt service routines may explicitly halt the BLiTTER
 * during execution time critical sections by clearing the BUSY flag.
 * The original BUSY flag state must be restored however, before
 * termination of the interrupt service routine.)

restart:
      bset.b    d2,(a2)        ; Restart BLiTTER and test the BUSY
      nop                      ; flag state.  The "nop" is executed
      bne  restart             ; prior to the BLiTTER restarting.
 *                             ; Quit if the BUSY flag was clear.  

begin:
      dbra d7,next_plane
      rts


**************************************************************************************
	DATA
**************************************************************************************
gAtariBlit:
	ds.b	BLIT_SIZEOF

gGraphic_4BP_LeftMasks:
	dc.w	$FFFF
	dc.w	$7FFF
	dc.w	$3FFF
	dc.w	$1FFF
	dc.w	$0FFF
	dc.w	$07FF
	dc.w	$03FF
	dc.w	$01FF
	dc.w	$00FF
	dc.w	$007F
	dc.w	$003F
	dc.w	$001F
	dc.w	$000F
	dc.w	$0007
	dc.w	$0003
	dc.w	$0001

gGraphic_4BP_DoubleLeftMasks:
	dc.w	$FFFF,$FFFF
	dc.w	$7FFF,$7FFF
	dc.w	$3FFF,$3FFF
	dc.w	$1FFF,$1FFF
	dc.w	$0FFF,$0FFF
	dc.w	$07FF,$07FF
	dc.w	$03FF,$03FF
	dc.w	$01FF,$01FF
	dc.w	$00FF,$00FF
	dc.w	$007F,$007F
	dc.w	$003F,$003F
	dc.w	$001F,$001F
	dc.w	$000F,$000F
	dc.w	$0007,$0007
	dc.w	$0003,$0003
	dc.w	$0001,$0001

gGraphic_4BP_RightMasks:
	dc.w	$8000
	dc.w	$C000
	dc.w	$E000
	dc.w	$F000
	dc.w	$F800
	dc.w	$FC00
	dc.w	$FE00
	dc.w	$FF00
	dc.w	$FF80
	dc.w	$FFC0
	dc.w	$FFE0
	dc.w	$FFF0
	dc.w	$FFF8
	dc.w	$FFFC
	dc.w	$FFFE
	dc.w	$FFFF

gGraphic_4BP_DoubleRightMasks:
	dc.w	$8000,$8000
	dc.w	$C000,$C000
	dc.w	$E000,$E000
	dc.w	$F000,$F000
	dc.w	$F800,$F800
	dc.w	$FC00,$FC00
	dc.w	$FE00,$FE00
	dc.w	$FF00,$FF00
	dc.w	$FF80,$FF80
	dc.w	$FFC0,$FFC0
	dc.w	$FFE0,$FFE0
	dc.w	$FFF0,$FFF0
	dc.w	$FFF8,$FFF8
	dc.w	$FFFC,$FFFC
	dc.w	$FFFE,$FFFE
	dc.w	$FFFF,$FFFF

gGraphic_4BP_LongMasks:
	dc.l	$FFFF0000
	dc.l	$7FFF8000
	dc.l	$3FFFC000
	dc.l	$1FFFE000
	dc.l	$0FFFF000
	dc.l	$07FFF800
	dc.l	$03FFFC00
	dc.l	$01FFFE00
	dc.l	$00FFFF00
	dc.l	$007FFF80
	dc.l	$003FFFC0
	dc.l	$001FFFE0
	dc.l	$000FFFF0
	dc.l	$0007FFF8
	dc.l	$0003FFFC
	dc.l	$0001FFFE
	dc.l	$0000FFFF


gGraphic_4BP_Points:
	dc.w	$8000
	dc.w	$4000
	dc.w	$2000
	dc.w	$1000
	dc.w	$0800
	dc.w	$0400
	dc.w	$0200
	dc.w	$0100
	dc.w	$0080
	dc.w	$0040
	dc.w	$0020
	dc.w	$0010
	dc.w	$0008
	dc.w	$0004
	dc.w	$0002
	dc.w	$0001

Graphic_4BP_LopColours:
	dc.l	$00000000
	dc.l	$0000000F
	dc.l	$00000F00
	dc.l	$00000F0F
	dc.l	$000F0000
	dc.l	$000F000F
	dc.l	$000F0F00
	dc.l	$000F0F0F
	dc.l	$0F000000
	dc.l	$0F00000F
	dc.l	$0F000F00
	dc.l	$0F000F0F
	dc.l	$0F0F0000
	dc.l	$0F0F000F
	dc.l	$0F0F0F00
	dc.l	$0F0F0F0F

Graphic_4BP_ColourChunks:
	dc.w	$0000,$0000,$0000,$0000
	dc.w	$FFFF,$0000,$0000,$0000
	dc.w	$0000,$FFFF,$0000,$0000
	dc.w	$FFFF,$FFFF,$0000,$0000
	dc.w	$0000,$0000,$FFFF,$0000
	dc.w	$FFFF,$0000,$FFFF,$0000
	dc.w	$0000,$FFFF,$FFFF,$0000
	dc.w	$FFFF,$FFFF,$FFFF,$0000
	dc.w	$0000,$0000,$0000,$FFFF
	dc.w	$FFFF,$0000,$0000,$FFFF
	dc.w	$0000,$FFFF,$0000,$FFFF
	dc.w	$FFFF,$FFFF,$0000,$FFFF
	dc.w	$0000,$0000,$FFFF,$FFFF
	dc.w	$FFFF,$0000,$FFFF,$FFFF
	dc.w	$0000,$FFFF,$FFFF,$FFFF
	dc.w	$FFFF,$FFFF,$FFFF,$FFFF


**************************************************************************************
