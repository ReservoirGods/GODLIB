**************************************************************************************
*	MEMORY_S.S
*
*	Fast Memory Routs
*
*	[c] 2002 Reservoir Gods
**************************************************************************************

	xdef	Memory_ClearSimple_1
	export	Memory_ClearSimple_16
	export	Memory_ClearMajor

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

Memory_ClearSimple_1:

	bra.s	.first
.loop:
	clr.b	(a0)+
.first:
	dbra	d0,.loop

	rts

Memory_ClearMajor:
	tst.l	d0
	beq		.noClear

	moveq	#0,d2
	add.l	d0,a0

	move.l	a0,d1
	andi.w	#1,d1
	beq.s	.aligned
	move.b	d2,-(a0)
	subq.l	#1,d0
.aligned:


	move.l	d0,d1
	lsr.l	#6,d1
	beq		.nobiggie

	movem.l	d3-d7/a2,-(a7)

	move.l	d2,d3
	move.l	d2,d4
	
	move.l	d2,d5
	move.l	d2,d6
	move.l	d2,d7
	move.l	d2,a1
	move.l	d2,a2

	bra.s	.first
.loop:
	movem.l	d2-d7/a1-a2,-(a0)
.first:
	dbra	d1,.loop

	movem.l	(a7)+,d3-d7/a2

	andi.w	#63,d0
.nobiggie:

	move.l	d0,d1
	lsr.l	#2,d1
	andi.w	#3,d0
	bra.s	.longFirst
.longLoop:
	move.l	d2,-(a0)
.longFirst:
	dbra	d1,.longLoop

	bra.s	.byteFirst
.byteLoop:
	move.b	d2,-(a0)
.byteFirst:
	dbra	d0,.byteLoop

.noClear:
	rts



Memory_ClearSimple_16:
	tst.l	d0
	beq		.noClear

	moveq	#0,d1

	move.l	a0,d2
	andi.w	#1,d2
	beq.s	.aligned
	move.b	d1,(a0)+
	subq.l	#1,d0
.aligned:

	move.l	d0,d2
	lsr.l	#4,d2
	andi.w	#15,d0
	bra.s	.bigFirst
.bigLoop:
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
.bigFirst:
	dbra	d2,.bigLoop

	move.l	d0,d2
	lsr.l	#2,d2
	andi.w	#3,d0
	bra.s	.longFirst
.longLoop:
	move.l	d1,(a0)+
.longFirst:
	dbra	d2,.longLoop

	bra.s	.byteFirst
.byteLoop:
	move.b	d1,(a0)+
.byteFirst:
	dbra	d0,.byteLoop

.noClear:
	rts


	moveq	#3,d1
	and.l	d0,d1
	beq.s	.noSkip
	moveq	#4,d2
	sub.w	d1,d2
	moveq	#0,d1
	bra.s	.byteSkipLoopStart
.byteSkipLoop:
	move.b	d1,(a0)+
.byteSkipLoopStart:
	dbra	d2,.byteSkipLoop
.noSkip:

	rts

	moveq	#3,d2
	and.l	d0,d2
	beq.s	.noHead
	bra.s	.noHead
	move.b	d1,(a0)+
	move.b	d1,(a0)+
	move.b	d1,(a0)+
.noHead:

	move.b	d1,(a0)+
	move.b	d1,(a0)+
	move.b	d1,(a0)+


Memory_Clear0:
	movem.l	d3-d7/a2-a6,-(a7)				; save registers

	moveq	#0,d2

	moveq	#0,d3
	moveq	#0,d4

	moveq	#0,d5
	moveq	#0,d6
	moveq	#0,d7
	suba.l	a1,a1
	suba.l	a1,a2


	adda.l	d0,a0

	moveq	#32,d1
	bra.s	.startloop1
.loop1:
	movem.l	d1-d7/a1,-(a0)
.startloop1:
	sub.l	d1,d0
	bpl.s	.loop1

	add.l	d1,d0
	beq		.done

.loop2:


.done:
	movem.l	(a7)+,d3-d7
	rts