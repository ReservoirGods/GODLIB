	export	DebugLog_IsSTEEM
	export fart
fart:
DebugLog_IsSTEEM:
	movem.l	d1-d7/a0-a6,-(a7)

	move.l	#'Emu?',d6
	move.l	d6,d7
	move.w	#$25,-(a7)
	trap	#14
	addq.l	#2,a7

	moveq	#0,d0

	cmp.l	#'STEe',d6
	bne.s	.not
	cmp.l	#'mEng',d7
	bne.s	.not
	moveq	#1,d0
.not:
	movem.l	(a7)+,d1-d7/a0-a6
	rts