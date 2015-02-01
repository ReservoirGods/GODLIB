**************************************************************************************
*	RLE_S.S
*
*	GodPack RLE decompression routs
*
*	[c] 2005 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	GodPack_Rle_Decode_Asm

**************************************************************************************
	TEXT
**************************************************************************************


*------------------------------------------------------------------------------------*
* FUNCTION : GodPack_Rle_Decode_Asm
* ACTION   : RLE decode
* CREATION : 23.02.05 PNK
*------------------------------------------------------------------------------------*

GodPack_Rle_Decode_Asm:
	movem.l	d0-d7/a0-a6,-(a7)

	move.b	(a0)+,d1
	bra		.next
.rle_loop:
	move.b	(a0)+,d2
	cmp.b	d1,d2
	bne		.literal
	moveq	#0,d3
	move.b	(a0)+,d3
	subq.w	#1,d3
	bmi		.literal_esc
	sub.l	d3,d0
	move.b	(a0)+,d4
.loop:
	move.b	d4,(a1)+
.first:
	dbra	d3,.loop
	bra		.next

.literal_esc:
	move.b	d1,(a1)+
	bra		.next

.literal:
	move.b	d2,(a1)+
.next:
	subq.l	#1,d0
	bge		.rle_loop

	movem.l	(a7)+,d0-d7/a0-a6
	rts

