**************************************************************************************
*	CHUNKY_S.S
*
*	Chunky <-> Bitplane routs
*
*	[c] 2004 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
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
sGraphicRect_mX:				ds.w	1
sGraphicRect_mY:				ds.w	1
sGraphicRect_mWidth:			ds.w	1
sGraphicRect_mHeight:			ds.w	1
sGraphicRect_msizeof:			ds.w	1

	OFFSET
sGraphicCanvas_mpVRAM:			ds.l	1
sGraphicCanvas_mColourMode:		ds.w	1
sGraphicCanvas_mWidth:			ds.w	1
sGraphicCanvas_mHeight:			ds.w	1
sGraphicCanvas_mPad:			ds.w	1
sGraphicCanvas_mmClipBox:		ds.b	sGraphicBox_sizeof
sGraphicCanvas_mpFuncs:			ds.l	1
sGraphicCanvas_mpClipFuncs:		ds.l	1
sGraphicCanvas_mLineOffsets:	ds.l	485
sGraphicCanvas_sizeof:			ds.w	1

**************************************************************************************
	TEXT
**************************************************************************************

; a0 -> src
; a1 -> dst
; a2 = width
; a3 = width
; a4 = height
; a5 = srclinelen
; a6 = dstlineadd

*------------------------------------------------------------------*
* FUNCTION : C2P_To4P( sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc )
* ACTION   : converts byte chunky buffer to 4bp screen.
* CREATION : 04.04.04 Kalms/PNK
*------------------------------------------------------------------*

C2P_To4P:
	movem.l	d3-d7/a2-a6,-(a7)

	move.l	11*4(a7),a2							; apRect
	move.l	12*4(a7),a3							; apSrc

	moveq	#0,d0								; clear d0
	moveq	#0,d1								; clear d1
	move.w	sGraphicPos_mX(a1),d0				; X
	move.w	sGraphicPos_mY(a1),d1				; Y
	mulu.w	sGraphicCanvas_mLineOffsets+4(a0),d1	; y offset
	lsr.w	#4,d0								; x/16
	lsl.w	#3,d0								; x*8
	add.l	d1,d0								; pixel offset
	move.l	sGraphicCanvas_mpVRAM(a0),a1		; pdest
	add.l	d0,a1								; get to screen pos

	moveq	#0,d0
	move.w	sGraphicCanvas_mWidth(a0),d0		; dst witdh
	sub.w	sGraphicRect_mWidth(a2),d0			; src width
	lsr.w	#4,d0								; off/16
	lsl.w	#3,d0								; off*8
	move.w	d0,a6								; dst line add

	moveq	#0,d0								; clear d0
	moveq	#0,d1								; clear d1
	move.w	sGraphicRect_mX(a2),d0				; X
	move.w	sGraphicRect_mY(a2),d1				; Y
	mulu.w	sGraphicCanvas_mWidth(a2),d1		; y offset
	add.l	d1,d0								; buffer offset
	move.l	sGraphicCanvas_mpVRAM(a3),a0		; psrc
	add.l	d0,a0								; get to pos in chunky buffer

	move.w	sGraphicCanvas_mWidth(a2),d0		; width of src
	sub.w	sGraphicRect_mWidth(a2),d0			; srclineadd
	move.w	sGraphicRect_mWidth(a2),a3			; width
	move.w	sGraphicRect_mHeight(a2),a4			; height

	move.l	#$0f0f0f0f,d4						; mask
	move.l	#$00ff00ff,d5						; mask
	move.l	#$55555555,d6						; mask

.loopY:
	move.l	a3,a2								; width
.loopX:	
	move.l	(a0)+,d0							; read pixels 0-3
	move.l	(a0)+,d2							; read pixels 4-7
	move.l	(a0)+,d1							; read pixels 8-11
	move.l	(a0)+,d3							; read pixels 12-15
	and.l	d4,d0								; mask pixels 0-3 to 4bit
	and.l	d4,d2								; mask pixels 4-7 to 4bit
	and.l	d4,d1								; mask pixels 8-11 to 4bit
	and.l	d4,d3								; mask pixels 12-15 to 4bit
	lsl.l	#4,d0								; shift into high nibble
	lsl.l	#4,d1								; shift into high nibble
	or.l	d2,d0								; combine nibbles of pixels 0-7
	or.l	d3,d1								; combine nibbles of pixles 8-15

	; a3a2a1a0e3e2e1e0 b3b2b1b0f3f2f1f0 c3c2c1c0g3g2g1g0 d3d2d1d0h3h2h1h0
	; i3i2i1i0m3m2m1m0 j3j2j1j0n3n2n1n0 k3k2k1k0o3o2o1o0 l3l2l1l0p3p2p1p0

	move.l	d1,d7								; pixels 8-15
	lsr.l	#8,d7								; pixels 8-13
	eor.l	d0,d7								
	and.l	d5,d7
	eor.l	d7,d0
	lsl.l	#8,d7
	eor.l	d7,d1

	; a3a2a1a0e3e2e1e0 i3i2i1i0m3m2m1m0 c3c2c1c0g3g2g1g0 k3k2k1k0o3o2o1o0
	; b3b2b1b0f3f2f1f0 j3j2j1j0n3n2n1n0 d3d2d1d0h3h2h1h0 l3l2l1l0p3p2p1p0

	move.l	d1,d7
	lsr.l	#1,d7
	eor.l	d0,d7
	and.l	d6,d7
	eor.l	d7,d0
	add.l	d7,d7
	eor.l	d7,d1

	; a3b3a1b1e3f3e1f1 i3j3i1j1m3n3m1n1 c3d3c1d1g3h3g1h1 k3l3k1l1o3p3o1p1
	; a2b2a0b0e2f2e0f0 i2j2i0j0m2n2m0n0 c2d2c0d0g2h2g0h0 k2l2k0l0o2p2o0p0

	move.w	d1,d7
	move.w	d0,d1
	swap	d1
	move.w	d1,d0
	move.w	d7,d1

	; a3b3a1b1e3f3e1f1 i3j3i1j1m3n3m1n1 a2b2a0b0e2f2e0f0 i2j2i0j0m2n2m0n0
	; c3d3c1d1g3h3g1h1 k3l3k1l1o3p3o1p1 c2d2c0d0g2h2g0h0 k2l2k0l0o2p2o0p0

	move.l	d1,d7
	lsr.l	#2,d7
	eor.l	d0,d7
	and.l	#$33333333,d7
	eor.l	d7,d0
	lsl.l	#2,d7
	eor.l	d7,d1

	; a3b3c3d3e3f3g3h3 i3j3k3l3m3n3o3p3 a2b2c2d2e2f2g2h2 i2j2k2l2m2n2o2p2
	; a1b1c1d1e1f1g1h1 i1j1k1l1m1n1o1p1 a0b0c0d0e0f0g0h0 i0j0k0l0m0n0o0p0

	swap	d0
	swap	d1

	move.l	d1,(a1)+							; store planes 0-1
	move.l	d0,(a1)+							; store planes 2-3

	subq.l	#1,a2								; dec width
	cmpa.l	#0,a2								; reached end of line?
	bne		.loopX								; loop for all x

	add.l	a5,a0								; pSrc += srclineoffset
	add.l	a6,a1								; pDst += dstlineoffset

	subq.l	#1,a4								; dec height
	cmpa.l	#0,a4								; reached end of box?
	bne		.loopY								; loop for all y

	
	movem.l	(sp)+,d3-d7/a2-a6
	rts

; a0->p4bp
; a1->pchunky
; a2->srclineoff
; a3->dstlineoff
; a4->width
; a5->height

; d6 - width

*------------------------------------------------------------------*
* FUNCTION : C2P_To4P( sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc )
* ACTION   : converts 4biplane image to chunky buffer
* CREATION : 04.04.04 Kalms/PNK
*------------------------------------------------------------------*

C2P_From4P:
	movem.l	d3-d7/a2-a6,-(a7)

	move.l	11*4(a7),a2							; apRect
	move.l	12*4(a7),a3							; apSrc

	move.w	sGraphicRect_mWidth(a2),a4			; width
	subq.l	#1,a4								; -1 for dbra
	move.w	sGraphicRect_mHeight(a2),a5			; height

	moveq	#15,d4								; 15
	and.w	sGraphicPos_mX(a1),d4				; x & 15

	moveq	#0,d2
	move.w	sGraphicCanvas_mWidth(a3),d2		; src witdh
	sub.w	sGraphicRect_mWidth(a2),d2			; -block width
	lsr.w	#4,d2								; off/16
	lsl.w	#3,d2								; off*8

	moveq	#0,d3
	move.w	sGraphicCanvas_mWidth(a0),d3		; dst width
	sub.w	sGraphicRect_mWidth(a0),d3			; -block width

	moveq	#0,d0								; clear d0
	moveq	#0,d1								; clear d1
	move.w	sGraphicPos_mX(a1),d0				; X
	move.w	sGraphicPos_mY(a1),d1				; Y
	mulu.w	sGraphicCanvas_mWidth(a0),d1		; y offset
	add.l	d1,d0								; buffer offset
	move.l	sGraphicCanvas_mpVRAM(a0),a1		; psrc
	add.l	d0,a1								; get to pos in chunky buffer

	moveq	#0,d0								; clear d0
	moveq	#0,d1								; clear d1
	move.w	sGraphicRect_mX(a2),d0				; X
	move.w	sGraphicRect_mY(a2),d1				; Y
	mulu.w	sGraphicCanvas_mLineOffsets+4(a3),d1	; y offset
	lsr.w	#4,d0								; x/16
	lsl.w	#3,d1								; x*8
	add.l	d1,d0								; pixel offset
	move.l	sGraphicCanvas_mpVRAM(a3),a0		; pdest
	add.l	d0,a0								; get to screen pos

	move.l	d2,a2								; src line off
	move.l	d3,a3								; dst line off

.loopY:

	move.w	(a0)+,d0
	move.w	(a0)+,d1
	move.w	(a0)+,d2
	move.w	(a0)+,d3

	lsl.w	d4,d0
	lsl.w	d4,d1
	lsl.w	d4,d2
	lsl.w	d4,d3

	moveq	#15,d5
	sub.w	d4,d5
	move.l	a4,d6
.loopX:
	moveq	#0,d7
	add.w	d0,d0
	addx.w	d7,d7
	add.w	d1,d1
	addx.w	d7,d7
	add.w	d2,d2
	addx.w	d7,d7
	add.w	d3,d3
	addx.w	d7,d7

	move.b	d4,(a1)+

	dbra	d5,.chunkLoop

	move.w	(a0)+,d0
	move.w	(a0)+,d1
	move.w	(a0)+,d2
	move.w	(a0)+,d3

	moveq	#15,d5

.chunkLoop:
	dbra	d6,.loopX

	add.l	a2,a0
	add.l	a3,a1

	subq.l	#1,a5
	cmpa.l	#0,a5
	bne		.loopY

	movem.l	(sp)+,d3-d7/a2-a6
	rts
	

