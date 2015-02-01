**************************************************************************************
*	LZ77_S.S
*
*	GodPack LZ77 decompression routs
*
*	[c] 2005 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	GodPack_Lz77_Decode_Asm

**************************************************************************************
	TEXT
**************************************************************************************


*------------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77_Decode_Asm
* ACTION   : lz77 decode
* CREATION : 23.02.05 PNK
*------------------------------------------------------------------------------------*

GodPack_Lz77_Decode_Asm:
	movem.l	d0-d7/a0-a6,-(a7)

	moveq	#0,d6

	move.l	d0,d5
	bra		.next
.loop:
	bsr		GodPack_Lz77_BitRead
	tst.w	d0
	beq		.literal_read

.symbol_read:
	moveq	#4-1,d2
	moveq	#8-1,d3
	bsr		GodPack_Lz77_BitRead
	tst.w	d0
	beq		.symbol_small
	moveq	#8-1,d2
	moveq	#14-1,d3

.symbol_small:
	move.l	d3,d1
	bsr		GodPack_Lz77_BitsRead
	move.l	a1,a2
	sub.l	d0,a2

	move.l	d2,d1
	bsr		GodPack_Lz77_BitsRead
	addq.l	#2,d0
.symbol_loop:
	move.b	(a2)+,(a1)+
	dbra	d0,.symbol_loop
	bra		.next

.literal_read:
	moveq	#3-1,d1
	bsr		GodPack_Lz77_BitsRead
	move.w	d0,d4
.literal_loop:
	moveq	#8-1,d1
	bsr		GodPack_Lz77_BitsRead
	move.b	d0,(a1)+
	dbra	d4,.literal_loop

.next:
	dbra	d5,.loop

	movem.l	(a7)+,d0-d7/a0-a6
	rts

GodPack_Lz77_BitRead:
	dbra	d6,.bit
	move.b	(a0)+,d7
	moveq	#7,d6
.bit:
	moveq	#1,d0
	and.w	d7,d0
	lsr.b	#1,d7
	rts

GodPack_Lz77_BitsRead:
	moveq	#0,d0
.bitloop:
	dbra	d6,.bit
	move.b	(a0)+,d7
	moveq	#7,d6
.bit:
	lsr.b	#1,d7
	addx.l	d0,d0
	dbra	d1,.bitloop
	rts
