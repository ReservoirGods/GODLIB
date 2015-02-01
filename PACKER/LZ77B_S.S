**************************************************************************************
*	LZ77_S.S
*
*	GodPack LZ77b decompression routs
*
*	[c] 2005 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	GodPack_Lz77b_Decode_Asm

**************************************************************************************
	TEXT
**************************************************************************************


eFLAG_MASK				EQU	$E0

eRUN_BITS				EQU 2

eSYMBOL0_COST			EQU 2
eSYMBOL1_COST			EQU 3
eSYMBOL2_COST			EQU 4

eSYMBOL0_OFF_BITS		EQU	9
eSYMBOL0_COUNT_BITS		EQU	4
eSYMBOL1_OFF_BITS		EQU	14
eSYMBOL1_COUNT_BITS		EQU	6
eSYMBOL2_OFF_BITS		EQU	14
eSYMBOL2_COUNT_BITS		EQU	13

eSYMBOL0_OFF_MASK		EQU	((1<<eSYMBOL0_OFF_BITS)-1)
eSYMBOL0_COUNT_MASK		EQU	((1<<eSYMBOL0_COUNT_BITS)-1)
eSYMBOL1_OFF_MASK		EQU	((1<<eSYMBOL1_OFF_BITS)-1)
eSYMBOL1_COUNT_MASK		EQU	((1<<eSYMBOL1_COUNT_BITS)-1)
eSYMBOL2_OFF_MASK		EQU	((1<<eSYMBOL2_OFF_BITS)-1)
eSYMBOL2_COUNT_MASK		EQU	((1<<eSYMBOL2_COUNT_BITS)-1)

eSYMBOL0_OFF_SHIFT		EQU	((eSYMBOL0_COUNT_BITS+eRUN_BITS)&7)
eSYMBOL1_OFF_SHIFT		EQU	((eSYMBOL1_COUNT_BITS+eRUN_BITS)&7)
eSYMBOL2_OFF_SHIFT		EQU	((eSYMBOL2_COUNT_BITS+eRUN_BITS)&7)


*------------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77b_Decode_Asm( const void * apSrc, void * apDst, const U32 aSize )
* ACTION   : lz77 decode
* CREATION : 23.02.05 PNK
*------------------------------------------------------------------------------------*

GodPack_Lz77b_Decode_Asm:
	movem.l	d0-d7/a0-a6,-(a7)

.lzb_loop:
	moveq	#0,d1
	move.b	(a0)+,d1
	move.b	d1,d2
	add.b	d2,d2
	bcc		.lzb_symbol0
	add.b	d2,d2
	bcc		.lzb_symbol1
	add.b	d2,d2
	bcc		.lzb_symbol2

.lzb_literal:
	moveq	#31,d2
	and.b	d2,d1
	addq.w	#1,d1
	add.w	d1,d1
	add.w	d1,d1
	
	sub.l	d1,d0
	bge.s	.lzb_lit4_first
	move.l	d0,d1
	neg.l	d1
	bra.s	.lzb_lit4_first
.lzb_lit4:
	move.b	(a0)+,(a1)+
.lzb_lit4_first:
	dbra	d1,.lzb_lit4
	bra		.lzb_next

.lzb_symbol0:
	moveq	#0,d2
	lsl.w	#8,d1
	move.b	(a0)+,d3
	move.b	d3,d1
	lsr.w	#eSYMBOL0_OFF_SHIFT,d1
	andi.w	#eSYMBOL0_OFF_MASK,d1

	move.b	d3,d2
	lsr.b	#eRUN_BITS,d2
	andi.w	#eSYMBOL0_COUNT_MASK,d2
	addq.w	#eSYMBOL0_COST,d2
	bra		.lzb_symbol_run

.lzb_symbol1:
	moveq	#0,d2
	lsl.w	#8,d1
	move.b	(a0)+,d2
	move.b	d2,d1
*	lsr.w	#eSYMBOL1_OFF_SHIFT,d1
	andi.w	#eSYMBOL1_OFF_MASK,d1

	lsl.w	#8,d2
	move.b	(a0)+,d3
	move.b	d3,d2
	lsr.w	#eRUN_BITS,d2
	andi.w	#eSYMBOL1_COUNT_MASK,d2
	addq.w	#eSYMBOL1_COST,d2
	bra		.lzb_symbol_run

.lzb_symbol2:
	moveq	#0,d2
	lsl.w	#8,d1
	move.b	(a0)+,d1
	lsl.l	#8,d1
	move.b	(a0)+,d2
	move.b	d2,d1
	lsr.l	#eSYMBOL2_OFF_SHIFT,d1
	andi.l	#eSYMBOL2_OFF_MASK,d1

	lsl.w	#8,d2
	move.b	(a0)+,d3
	move.b	d3,d2
	lsr.w	#eRUN_BITS,d2
	andi.w	#eSYMBOL2_COUNT_MASK,d2
	addq.w	#eSYMBOL2_COST,d2

.lzb_symbol_run:

	and.w	#3,d3
	bra.s	.lzb_lit_first
.lzb_lit_loop:
	move.b	(a0)+,(a1)+
	subq.l	#1,d0
.lzb_lit_first:
	dbra	d3,.lzb_lit_loop

	move.l	a1,a2
	sub.l	d1,a2

	sub.l	d2,d0
.lzb_symbol_loop:
	move.b	(a2)+,(a1)+
	dbra	d2,.lzb_symbol_loop

	subq.l	#1,d0
.lzb_next:
	tst.l	d0
	bgt		.lzb_loop

	movem.l	(a7)+,d0-d7/a0-a6
	rts
