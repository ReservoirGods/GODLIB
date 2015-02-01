**************************************************************************************
*	RSPRIT_S.S
*
*	rle true colour sprite routs
*
*	[c] 2005 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	RleSprite_Draw
	export	RleSprite_DrawClipY
	export	RleSprite_DrawClip


**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET

sRleSprite_mWidth:		ds.w	1
sRleSprite_mHeight:		ds.w	1
sRleSprite_mpLines:		ds.w	1

	OFFSET

sSpriteRect_mX:			ds.w	1
sSpriteRect_mY:			ds.w	1
sSpriteRect_mWidth:		ds.w	1
sSpriteRect_mHeight:	ds.w	1


**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Draw( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
* ACTION   : RleSprite_Draw
* CREATION : 10.04.2005 PNK
*------------------------------------------------------------------------------------*

RleSprite_Draw:
	sub.w	sRleSprite_mWidth(a0),d0		; offset to next line
	ext.l	d0								; as long
	add.l	d0,d0							; * pixel size
	move.w	sRleSprite_mHeight(a0),d1		; sprite height
	move.l	sRleSprite_mpLines(a0),a0		; ptr to first line of data
	bra.s	.rleFirstY

.rleLoopX:
	move.w	(a0)+,(a1)+						; draw pixel
	dbra	d2,.rleLoopX					; xloop

.rleLoopY:
	move.w	(a0)+,d2						; skip count
	add.w	d2,d2							; *pixel size
	add.w	d2,a1							; skip transparent pixels

	move.w	(a0)+,d2						; draw count
	dbra	d2,.rleLoopX					; if 0, end of line

.rleNextY:
	add.l	d0,a1							; next line of dest

.rleFirstY:
	dbra	d1,.rleLoopY					; loop for height

	rts


*------------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Draw( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth, U32 aSprHeight, U32 aSprY0 )
* ACTION   : RleSprite_Draw
* CREATION : 10.04.2005 PNK
*------------------------------------------------------------------------------------*

RleSprite_DrawClipY:
	sub.w	sRleSprite_mWidth(a0),d0		; offset to next line
	ext.l	d0								; as long
	add.l	d0,d0							; * pixel size
	move.l	sRleSprite_mpLines(a0),a0		; ptr to first line of data

	ext.l	d2
	lsl.l	#2,d2							; y*4
	add.l	#sRleSprite_mpLines,d2
	move.l	(a0,d2.l),a0
	bra.s	.rleFirstY

.rleLoopX:
	move.w	(a0)+,(a1)+						; draw pixel
	dbra	d2,.rleLoopX					; xloop

.rleLoopY:
	move.w	(a0)+,d2						; skip count
	add.w	d2,d2							; *pixel size
	add.w	d2,a1							; skip transparent pixels

	move.w	(a0)+,d2						; draw count
	dbra	d2,.rleLoopX					; if 0, end of line

.rleNextY:
	add.l	d0,a1							; next line of dest

.rleFirstY:
	dbra	d1,.rleLoopY					; loop for height

	rts
		

*------------------------------------------------------------------------------------*
* FUNCTION : RleSprite_DrawClip( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth,sSpriteRect * apRect )
* ACTION   : RleSprite_DrawClip
* CREATION : 10.04.2005 PNK
*------------------------------------------------------------------------------------*

RleSprite_DrawClip:
	movem.l	d3-d7/a2-a6,-(a7)				; save registers

	move.l	11*4(a7),a2						; apRect

	ext.l	d0
	add.l	d0,d0							; offset to next line

	move.w	sSpriteRect_mHeight(a2),d1		; height
	move.w	sSpriteRect_mY(a2),d2			; first y
	ext.l	d2
	lsl.l	#2,d2							; y*4
	add.l	#sRleSprite_mpLines,d2			; offset into line pointers
	lea		(a0,d2.l),a4					; address of first line pointer

	move.w	sSpriteRect_mX(a2),d4			; x0
	move.w	sSpriteRect_mWidth(a2),d5		; width
	bra		.rleClipFirstY

.rleClipLoopY:
	move.l	a1,a3							; dest

	move.w	d4,d2							; x

.rleClipLoopX0:
	move.w	(a0)+,d3						; skip count
	sub.w	d3,d2							; x - skip count
	bmi		.rleClipSkip0					; entered non clip space
	move.w	(a0)+,d3						; draw count
	beq		.rleClipNextY					; end of line
	sub.w	d3,d2							; x - draw count
	bmi		.rleClipDraw0					; entered non clip space
	add.w	d3,d3							; *pixel size
	add.w	d3,a0							; skip draw words
	bra.s	.rleClipLoopX0					; loop through x clip space

.rleClipDraw0:
	add.w	d3,d2							; draw + (-clipx)
	sub.w	d2,d3							; -clipx
	ble		.rleClipNextY
	add.w	d2,d2							; * pixel size		
	add.w	d2,a0							; skip clipped pixels
	move.w	d5,d2							; width
	bra.s	.rleClipDrawStart1

.rleClipSkip0:
	sub.w	d2,a3							; skip dst
	sub.w	d2,a3							; skip dst
	add.w	d5,d2							; width - skip
	ble		.rleClipNextY

.rleClipDraw1:
	move.w	(a0)+,d3						; draw count
.rleClipDrawStart1:
	sub.w	d3,d2							; width - draw_width
	ble.s	.rleClipDraw2
	dbra	d3,.rleClipDrawLoop1			; dec for dbra
	bra		.rleClipNextY					; end of line
.rleClipDrawLoop1:
	move.w	(a0)+,(a3)+						; draw pixel
	dbra	d3,.rleClipDrawLoop1

.rleClipSkip1:
	move.w	(a0)+,d3						; skip count;
	sub.w	d3,d2							; adjust width
	ble		.rleClipNextY
	add.w	d3,d3							; * pixel size
	add.w	d3,a3							; adjust dest pointer
	bra.s	.rleClipDraw1

.rleClipDraw2:
	add.w	d2,d3							; adjust draw
.rleClipDrawLoop2:
	move.w	(a0)+,(a3)+						; draw pixel
	dbra	d3,.rleClipDrawLoop2

.rleClipNextY:
	add.l	d0,a1							; get to next line of dest
.rleClipFirstY:
	move.l	(a4)+,a0						; ptr to line of sprite
	dbra	d1,.rleClipLoopY				; loop for all height

	movem.l	(sp)+,d3-d7/a2-a6				; restore registers
	rts


