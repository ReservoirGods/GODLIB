;#####################################################################################
;	PROTOTYPES
;#####################################################################################

	xdef	Packer_DepackAtomic
	xdef	Packer_DepackAuto5
	xdef	Packer_DepackIce
	xdef	Packer_DepackSpeed3


	OFFSET

osp3_53:	DS.L	1
osp3_54:	DS.B	8
osp3_55:	DS.W	2*64
osp3_56:	DS.W	2
			DS.B	1
osp3_57:	DS.B	1
			DS.B	2*64
osp3_58:	DS.B	512


;#####################################################################################
	TEXT
;#####################################################################################

;-----------------------------------------------------------------------------------*
;
; Function:    void Packer_DepackIce( void * apData )
;
; Description: depacks ice packed apData to apData
;
; Created:     Axe/Delight
;
;-----------------------------------------------------------------------------------*

Packer_DepackIce:
	move.l	a0,-(a7)

	link	a3,#-120
	movem.l	d0-d7/a0-a6,-(sp)
	lea	120(a0),a4
	move.l	a4,a6
	bsr	.getinfo
	cmpi.l	#'ICE!',d0
	bne	.not_packed
	bsr	.getinfo
	lea.l	-8(a0,d0.l),a5
	bsr.s	.getinfo
	move.l	d0,(sp)
	adda.l	d0,a6
	move.l	a6,a1

	moveq	#119,d0
.save:	move.b	-(a1),-(a3)
	dbf	d0,.save
	move.l	a6,a3
	move.b	-(a5),d7
	bsr.s	.normal_bytes
	move.l	a3,a5


	bsr	.get_1_bit
	bcc.s	.no_picture
	move.w	#$0f9f,d7
	bsr	.get_1_bit
	bcc.s	.ice_00
	moveq	#15,d0
	bsr	.get_d0_bits
	move.w	d1,d7
.ice_00:	moveq	#3,d6
.ice_01:	move.w	-(a3),d4
	moveq	#3,d5
.ice_02:	add.w	d4,d4
	addx.w	d0,d0
	add.w	d4,d4
	addx.w	d1,d1
	add.w	d4,d4
	addx.w	d2,d2
	add.w	d4,d4
	addx.w	d3,d3
	dbra	d5,.ice_02
	dbra	d6,.ice_01
	movem.w	d0-d3,(a3)
	dbra	d7,.ice_00
.no_picture:
	movem.l	(sp),d0-d7/a0-a3

.move:
	move.b	(a4)+,(a0)+
	subq.l	#1,d0
	bne.s	.move
	moveq	#119,d0
.rest:
	move.b	-(a3),-(a5)
	dbf	d0,.rest
.not_packed:
	movem.l	(sp)+,d0-d7/a0-a6
	unlk	a3
	move.l	(a7)+,a0
	rts

.getinfo: moveq	#3,d1
.getbytes: lsl.l	#8,d0
	move.b	(a0)+,d0
	dbf	d1,.getbytes
	rts

.normal_bytes:
	bsr.s	.get_1_bit
	bcc.s	.test_if_end
	moveq.l	#0,d1
	bsr.s	.get_1_bit
	bcc.s	.copy_direkt
	lea.l	.direkt_tab+20(pc),a1
	moveq.l	#4,d3
.nextgb:	move.l	-(a1),d0
	bsr.s	.get_d0_bits
	swap	d0
	cmp.w	d0,d1
	dbne	d3,.nextgb
.no_more: add.l	20(a1),d1
.copy_direkt:
	move.b	-(a5),-(a6)
	dbf	d1,.copy_direkt
.test_if_end:
	cmpa.l	a4,a6
	bgt.s	.strings
	rts

.get_1_bit:
	add.b	d7,d7
	bne.s	.bitfound
	move.b	-(a5),d7
	addx.b	d7,d7
.bitfound:
	rts

.get_d0_bits:
	moveq.l	#0,d1
.hole_bit_loop:
	add.b	d7,d7
	bne.s	.on_d0
	move.b	-(a5),d7
	addx.b	d7,d7
.on_d0:	addx.w	d1,d1
	dbf	d0,.hole_bit_loop
	rts


.strings: lea.l	.length_tab(pc),a1
	moveq.l	#3,d2
.get_length_bit:
	bsr.s	.get_1_bit
	dbcc	d2,.get_length_bit
.no_length_bit:
	moveq.l	#0,d4
	moveq.l	#0,d1
	move.b	1(a1,d2.w),d0
	ext.w	d0
	bmi.s	.no_ober
.get_ober:
	bsr.s	.get_d0_bits
.no_ober:
	move.b	6(a1,d2.w),d4
	add.w	d1,d4
	beq.s	.get_offset_2


	lea.l	.more_offset(pc),a1
	moveq.l	#1,d2
.getoffs: bsr.s	.get_1_bit
	dbcc	d2,.getoffs
	moveq.l	#0,d1
	move.b	1(a1,d2.w),d0
	ext.w	d0
	bsr.s	.get_d0_bits
	add.w	d2,d2
	add.w	6(a1,d2.w),d1
	bpl.s	.depack_bytes
	sub.w	d4,d1
	bra.s	.depack_bytes


.get_offset_2:
	moveq.l	#0,d1
	moveq.l	#5,d0
	moveq.l	#-1,d2
	bsr.s	.get_1_bit
	bcc.s	.less_40
	moveq.l	#8,d0
	moveq.l	#$3f,d2
.less_40:
	bsr.s	.get_d0_bits
	add.w	d2,d1

.depack_bytes:
	lea.l	2(a6,d4.w),a1
	adda.w	d1,a1
	move.b	-(a1),-(a6)
.dep_b:	move.b	-(a1),-(a6)
	dbf	d4,.dep_b
	bra	.normal_bytes



.direkt_tab:
	dc.l $7fff000e,$00ff0007,$00070002,$00030001,$00030001
	dc.l 270-1,15-1,8-1,5-1,2-1

.length_tab:
	dc.b 9,1,0,-1,-1
	dc.b 8,4,2,1,0

.more_offset:
	dc.b 11,4,7,0		; Bits lesen
	dc.w $11f,-1,$1f	; Standard Offset


;-----------------------------------------------------------------------------------*
;
; Function:    void Packer_DepackAtomic( void * apData )
;
; Description: depacks atomic packed apData to apData
;
; Created:     Altair/Vmax
;
;-----------------------------------------------------------------------------------*

Packer_DepackAtomic:
  MOVEM.L   A0-A6/D0-D7,-(A7)
  CMPI.L    #1096043829,(A0)+
  BNE       AM029
  LINK      A2,#-28
  MOVE.L    (A0)+,D0
  LEA       4(A0,D0.L),A5
  MOVE.L    D0,-(A7)
  MOVEA.L   A5,A4
  LEA       128(A4),A5
  LEA       -12(A4),A4
  MOVE.L    (A0)+,D0
  MOVEA.L   A0,A6
  ADDA.L    D0,A6
  SUBQ.W    #1,A6
  MOVE.B    -(A6),D7
  BRA       AM01F
AM000:
  MOVE.W    D3,D5
AM001:
  ADD.B     D7,D7
AM002:
  DBCS      D5,AM001
  BEQ.S     AM005
  BCC.S     AM003
  SUB.W     D3,D5
  NEG.W     D5
  BRA.S     AM008
AM003:
  MOVEQ     #3,D6
  BSR.S     AM00E
  BEQ.S     AM004
  BRA.S     AM007
AM004:
  MOVEQ     #7,D6
  BSR.S     AM00E
  BEQ.S     AM006
  ADDI.W    #15,D5
  BRA.S     AM007
AM005:
  MOVE.B    -(A6),D7
  ADDX.B    D7,D7
  BRA.S     AM002
AM006:
  MOVEQ     #13,D6
  BSR.S     AM00E
  ADDI.W    #270,D5
AM007:
  ADD.W     D3,D5
AM008:
  LEA       AM02A(PC),A4
  MOVE.W    D5,D2
  BNE.S     AM012
  ADD.B     D7,D7
  BNE.S     AM009
  MOVE.B    -(A6),D7
  ADDX.B    D7,D7
AM009:
  BCS.S     AM00A
  MOVEQ     #1,D6
  BRA.S     AM013
AM00A:
  MOVEQ     #3,D6
  BSR.S     AM00E
  TST.B     -28(A2)
  BEQ.S     AM00B
  MOVE.B    -18(A2,D5.W),-(A5)
  BRA       AM01E
AM00B:
  MOVE.B    (A5),D0
  BTST      #3,D5
  BNE.S     AM00C
  BRA.S     AM00D
AM00C:
  ADDI.B    #-16,D5
AM00D:
  SUB.B     D5,D0
  MOVE.B    D0,-(A5)
  BRA       AM01E
AM00E:
  CLR.W     D5
AM00F:
  ADD.B     D7,D7
  BEQ.S     AM011
AM010:
  ADDX.W    D5,D5
  DBF       D6,AM00F
  TST.W     D5
  RTS
AM011:
  MOVE.B    -(A6),D7
  ADDX.B    D7,D7
  BRA.S     AM010
AM012:
  MOVEQ     #2,D6
AM013:
  BSR.S     AM00E
  MOVE.W    D5,D4
  MOVE.B    14(A4,D4.W),D6
  EXT.W     D6
  TST.B     -27(A2)
  BNE.S     AM014
  ADDQ.W    #4,D6
  BRA.S     AM018
AM014:
  BSR.S     AM00E
  MOVE.W    D5,D1
  LSL.W     #4,D1
  MOVEQ     #2,D6
  BSR.S     AM00E
  CMP.B     #7,D5
  BLT.S     AM016
  MOVEQ     #0,D6
  BSR.S     AM00E
  BEQ.S     AM015
  MOVEQ     #2,D6
  BSR.S     AM00E
  ADD.W     D5,D5
  OR.W      D1,D5
  BRA.S     AM019
AM015:
  OR.B      -26(A2),D1
  BRA.S     AM017
AM016:
  OR.B      -25(A2,D5.W),D1
AM017:
  MOVE.W    D1,D5
  BRA.S     AM019
AM018:
  BSR.S     AM00E
AM019:
  ADD.W     D4,D4
  BEQ.S     AM01A
  ADD.W     -2(A4,D4.W),D5
AM01A:
  LEA       1(A5,D5.W),A4
  MOVE.B    -(A4),-(A5)
AM01B:
  MOVE.B    -(A4),-(A5)
  DBF       D2,AM01B
  BRA.S     AM01E
AM01C:
  ADD.B     D7,D7
  BNE.S     AM01D
  MOVE.B    -(A6),D7
  ADDX.B    D7,D7
AM01D:
  BCS.S     AM027
  MOVE.B    -(A6),-(A5)
AM01E:
  CMPA.L    A5,A3
  BNE.S     AM01C
  CMPA.L    A6,A0
  BEQ.S     AM028
AM01F:
  MOVEQ     #0,D6
  BSR       AM00E
  BEQ.S     AM022
  MOVE.B    -(A6),D0
  LEA       -26(A2),A1
  MOVE.B    D0,(A1)+
  MOVEQ     #1,D1
  MOVEQ     #6,D2
AM020:
  CMP.B     D0,D1
  BNE.S     AM021
  ADDQ.W    #2,D1
AM021:
  MOVE.B    D1,(A1)+
  ADDQ.W    #2,D1
  DBF       D2,AM020
  ST        -27(A2)
  BRA.S     AM023
AM022:
  SF        -27(A2)
AM023:
  MOVEQ     #0,D6
  BSR       AM00E
  BEQ.S     AM025
  LEA       -18(A2),A1
  MOVEQ     #15,D0
AM024:
  MOVE.B    -(A6),(A1)+
  DBF       D0,AM024
  ST        -28(A2)
  BRA.S     AM026
AM025:
  SF        -28(A2)
AM026:
  CLR.W     D3
  MOVE.B    -(A6),D3
  MOVE.B    -(A6),D0
  LSL.W     #8,D0
  MOVE.B    -(A6),D0
  MOVEA.L   A5,A3
  SUBA.W    D0,A3
  BRA.S     AM01C
AM027:
  BRA       AM000
AM028:
  MOVE.L    (A7)+,D0
  BSR       AM02B
  UNLK      A2
AM029:
  MOVEM.L   (A7)+,A0-A6/D0-D7
  RTS
AM02A:
  DC.B      $00,' ',$00,'`',$01,'`',$03,'`'
  DC.B      $07,'`',$0F,'`',$1F,'`',$00,$01
  DC.B      $03,$04,$05,$06,$07,$08
AM02B:
  LSR.L     #4,D0
  LEA       -12(A6),A6
AM02C:
  MOVE.L    (A5)+,(A6)+
  MOVE.L    (A5)+,(A6)+
  MOVE.L    (A5)+,(A6)+
  MOVE.L    (A5)+,(A6)+
  DBF       D0,AM02C
  RTS


*------------------------------------------------------------------------------------*
* Function:    Packer_DepackAuto5
* Action:      depacks files packed with automation packer 5.01
* Created:
*------------------------------------------------------------------------------------*

; A0 -> A0 depacker
Packer_DepackAuto5:
AU5_decrunch_2:
	link	a3,#-120
	movem.l	d0-a6,-(sp)
	lea	120(a0),a4
	move.l	a4,a6
	bsr.s	.getinfo
	cmpi.l	#'AU5!',d0
	bne	.not_packed
	bsr.s	.getinfo
	lea.l	-8(a0,d0.l),a5
	bsr.s	.getinfo
	move.l	d0,(sp)
	adda.l	d0,a6
	move.l	a6,a1

	moveq	#119,d0
.save:	move.b	-(a1),-(a3)
	dbf	d0,.save
	move.l	a6,a3
	move.b	-(a5),d7
	bsr.s	.normal_bytes
	move.l	a3,a5

	bsr.s	.get_1_bit
	bcc.s	.no_picture
	move.w	#$0f9f,d7
.AU5_00:
	moveq	#3,d6
.AU5_01:
	move.w	-(a3),d4
	moveq	#3,d5
.AU5_02:
	add.w	d4,d4
	addx.w	d0,d0
	add.w	d4,d4
	addx.w	d1,d1
	add.w	d4,d4
	addx.w	d2,d2
	add.w	d4,d4
	addx.w	d3,d3
	dbra	d5,.AU5_02
	dbra	d6,.AU5_01
	movem.w	d0-d3,(a3)
	dbra	d7,.AU5_00
.no_picture:
	movem.l	(sp),d0-a3

.move:
	move.b	(a4)+,(a0)+
	subq.l	#1,d0
	bne.s	.move
	moveq	#119,d0
.rest:
	move.b	-(a3),-(a5)
	dbf	d0,.rest
.not_packed:
	movem.l	(sp)+,d0-a6
	unlk	a3
	rts
.getinfo:
	moveq	#3,d1
.getbytes:
	lsl.l	#8,d0
	move.b	(a0)+,d0
	dbf	d1,.getbytes
	rts
.normal_bytes:
	bsr.s	.get_1_bit
	bcc.s	.test_if_end
	moveq.l	#0,d1
	bsr.s	.get_1_bit
	bcc.s	.copy_direkt
	lea.l	.direkt_tab+20(pc),a1
	moveq.l	#4,d3
.nextgb:
	move.l	-(a1),d0
	bsr.s	.get_d0_bits
	swap	d0
	cmp.w	d0,d1
	dbne	d3,.nextgb
.no_more:
	add.l	20(a1),d1
.copy_direkt:
	move.b	-(a5),-(a6)
	dbf	d1,.copy_direkt
.test_if_end:
	cmpa.l	a4,a6
	bgt.s	.strings
	rts

.get_1_bit:
	add.b	d7,d7
	bne.s	.bitfound
	move.b	-(a5),d7
	addx.b	d7,d7
.bitfound:
	rts

.get_d0_bits:
	moveq.l	#0,d1
.hole_bit_loop:
	add.b	d7,d7
	bne.s	.on_d0
	move.b	-(a5),d7
	addx.b	d7,d7
.on_d0:	addx.w	d1,d1
	dbf	d0,.hole_bit_loop
	rts

.strings:
	lea.l	.length_tab(pc),a1
	moveq.l	#3,d2
.get_length_bit:
	bsr.s	.get_1_bit
	dbcc	d2,.get_length_bit
.no_length_bit:
	moveq.l	#0,d4
	moveq.l	#0,d1
	move.b	1(a1,d2.w),d0
	ext.w	d0
	bmi.s	.no_ober
.get_ober:
	bsr.s	.get_d0_bits
.no_ober:
	move.b	6(a1,d2.w),d4
	add.w	d1,d4
	beq.s	.get_offset_2

	lea.l	.more_offset(pc),a1
	moveq.l	#1,d2
.getoffs:
	bsr.s	.get_1_bit
	dbcc	d2,.getoffs
	moveq.l	#0,d1
	move.b	1(a1,d2.w),d0
	ext.w	d0
	bsr.s	.get_d0_bits
	add.w	d2,d2
	add.w	6(a1,d2.w),d1
	bpl.s	.depack_bytes
	sub.w	d4,d1
	bra.s	.depack_bytes


.get_offset_2:
	moveq.l	#0,d1
	moveq.l	#5,d0
	moveq.l	#-1,d2
	bsr.s	.get_1_bit
	bcc.s	.less_40
	moveq.l	#8,d0
	moveq.l	#$3f,d2
.less_40:
	bsr.s	.get_d0_bits
	add.w	d2,d1

.depack_bytes:
	lea.l	2(a6,d4.w),a1
	adda.w	d1,a1
	move.b	-(a1),-(a6)
.dep_b:	move.b	-(a1),-(a6)
	dbf	d4,.dep_b
	bra	.normal_bytes


.direkt_tab:
	dc.l $7fff000e,$00ff0007,$00070002,$00030001,$00030001
	dc.l 270-1,15-1,8-1,5-1,2-1

.length_tab:
	dc.b 9,1,0,-1,-1
	dc.b 8,4,2,1,0

.more_offset:
	dc.b	  11,   4,   7,  0
	dc.w	$11f,  -1, $1f

ende_AU5_decrunch_2:
;*************************************************** Ende der Unpackroutine



*------------------------------------------------------------------------------------*
* Function:    Packer_DepackSpeed3
* Action:      depacks files packed with speed packer v3
* Created:
*------------------------------------------------------------------------------------*

* UNPACK source for SPACKERv3	(C)THE FIREHAWKS'92
* -------------------------------------------------
* in	a0: even address start packed block
* out	d0: original length or 0 if not SPv3 packed
* =================================================
* Use AUTO_SP3.PRG for multiblk packed files

Packer_DepackSpeed3:
unpack:
	moveq	#0,d0
	movem.l	d0-a6,-(sp)
	lea	sp3_53(pc),a6
	movea.l	a0,a1
	cmpi.l	#'SPv3',(a1)+
	bne.s	sp3_02
	tst.w	(a1)
	bne.s	sp3_02
	move.l	(a1)+,d5
	move.l	(a1)+,d0
	move.l	(a1)+,(sp)
	movea.l	a0,a2
	adda.l	d0,a0
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	adda.l	(sp),a1
;	lea	sp3_58-sp3_53(a6),a3
	moveq	#128-1,d0
sp3_01:
	move.l	(a2)+,(a3)+
	dbf	d0,sp3_01
	suba.l	a2,a3
	move.l	a3,-(sp)
	bsr.s	sp3_03
	bsr	sp3_21
	move.b	-(a0),d0
	adda.l	(sp)+,a0
	move.b	d0,(a0)+
;	lea	sp3_58-sp3_53(a6),a2
	lea	osp3_58(a6),a2
	bsr	sp3_22
	bsr	sp3_15
sp3_02:
	movem.l	(sp)+,d0-a6
	rts
sp3_03:
;	move.w	SR,d1
;	andi.w	#$2000,d1
;	beq.s	sp3_04
;	move.w	$FFFF8240.W,2(a6)
;	btst	#1,$FFFF8260.W
;	bne.s	sp3_04
;	swap	d5
sp3_04:	clr.w	d5
	move.w	-(a0),d6
;	lea	sp3_54-sp3_53(a6),a3
	lea	osp3_54(a6),a3
	move.b	d6,(a3)+
	moveq	#1,d3
	moveq	#6,d4
sp3_05:	cmp.b	d6,d3
	bne.s	sp3_06
	addq.w	#2,d3
sp3_06:	move.b	d3,(a3)+
	addq.w	#2,d3
	dbf	d4,sp3_05
	moveq	#$10,d4
	move.b	-(a0),(a3)+
	move.b	d4,(a3)+
	move.b	-(a0),(a3)+
	move.b	d4,(a3)+
	move.b	-(a0),d4
	move.w	d4,(a6)
;	lea	sp3_57-sp3_53(a6),a5
	lea	osp3_57(a6),a5
	move.b	-(a0),d4
	lea	1(a5,d4.w),a3
sp3_07:
	move.b	-(a0),-(a3)
	dbf	d4,sp3_07
	move.b	-(a0),-(a3)
	beq.s	sp3_08
	suba.w	d4,a0
sp3_08:
	moveq	#0,d2
	move.b	-(a0),d2
	move.w	d2,d3
	move.b	-(a0),d7
sp3_09:
	bsr.s	sp3_10
	bsr.s	sp3_10
	dbf	d2,sp3_09
	rts
sp3_10:
	not.w	d4
	add.b	d7,d7
	bne.s	sp3_11
	move.b	-(a0),d7
	addx.b	d7,d7
sp3_11:
	bcs.s	sp3_12
	move.w	d2,d0
	subq.w	#1,d3
	sub.w	d3,d0
	add.w	d0,d0
	add.w	d4,d0
	add.w	d0,d0
	neg.w	d0
	move.w	d0,-(a3)
	rts
sp3_12:
	moveq	#2,d1
	bsr	sp3_44
	add.w	d0,d0
	beq.s	sp3_13
	move.b	d0,-(a3)
	moveq	#2,d1
	bsr	sp3_44
	add.w	d0,d0
	move.b	d0,-(a3)
	rts
sp3_13:
	moveq	#2,d1
	bsr	sp3_44
;	move.w	sp3_55-sp3_53(a6),d1
	move.w	osp3_55(a6),d1
	add.w	d0,d0
	beq.s	sp3_14
;	move.w	sp3_55+2-sp3_53(a6),d1
	move.w	osp3_55+2(a6),d1
sp3_14:
	or.w	d1,d0
	move.w	d0,-(a3)
	rts
sp3_15:
;	move.w	SR,d1
;	andi.w	#$2000,d1
;	beq.s	sp3_16
;	move.w	2(a6),$FFFF8240.W
sp3_16:	tst.w	d6
	bpl.s	sp3_20
	movea.l	a1,a2
	movea.l	a1,a3
	adda.l	4(sp),a3
sp3_17:
	moveq	#3,d6
sp3_18:
	move.w	(a2)+,d0
	moveq	#3,d5
sp3_19:
	add.w	d0,d0
	addx.w	d1,d1
	add.w	d0,d0
	addx.w	d2,d2
	add.w	d0,d0
	addx.w	d3,d3
	add.w	d0,d0
	addx.w	d4,d4
	dbf	d5,sp3_19
	dbf	d6,sp3_18
	cmpa.l	a2,a3
	blt.s	sp3_20
	movem.w	d1-d4,-8(a2)
	cmpa.l	a2,a3
	bne.s	sp3_17
sp3_20:
	rts
sp3_21:
	move.b	-(a0),-(a1)
sp3_22:
	swap	d5
;	beq.s	sp3_23
;	move.w	d5,$FFFF8240.W
sp3_23:
;	lea	sp3_56+2-sp3_53(a6),a3
	lea	osp3_56+2(a6),a3
	cmpa.l	a0,a2
	blt.s	sp3_25
	rts
sp3_24:
	adda.w	d3,a3
sp3_25:
	add.b	d7,d7
	bcc.s	sp3_28
	beq.s	sp3_27
sp3_26:
	move.w	(a3),d3
	bmi.s	sp3_24
	bra.s	sp3_29
sp3_27:
	move.b	-(a0),d7
	addx.b	d7,d7
	bcs.s	sp3_26
sp3_28:
	move.w	-(a3),d3
	bmi.s	sp3_24
sp3_29:
	ext.w	d3
	jmp	sp3_30(pc,d3.w)
sp3_30:
	bra.s	sp3_30
	bra.s	sp3_41
	bra.s	sp3_41
	bra.s	sp3_41
	bra.s	sp3_41
	bra.s	sp3_41
	bra.s	sp3_37
	bra.s	sp3_36
	bra.s	sp3_32
	bra.s	sp3_33
	bra.s	sp3_31
	bra.s	sp3_34
	bra.s	sp3_21
sp3_31:
	move.b	(a5),-(a1)
	bra.s	sp3_22
sp3_32:
	bsr.s	sp3_43
	move.b	1(a5,d0.w),-(a1)
	bra.s	sp3_22
sp3_33:
	bsr.s	sp3_43
	add.w	(a6),d0
	move.b	1(a5,d0.w),-(a1)
	bra.s	sp3_22
sp3_34:
	moveq	#3,d1
	bsr.s	sp3_44
	lsr.w	#1,d0
	bcc.s	sp3_35
	not.w	d0
sp3_35:
	move.b	(a1),d1
	add.w	d0,d1
	move.b	d1,-(a1)
	bra.s	sp3_22
sp3_36:
;	lea	sp3_52-2-sp3_53(a6),a4
	lea	-(18*2)(a6),a4
	bsr.s	sp3_48
	addi.w	#16,d0
	lea	1(a1,d0.w),a3
	move.b	-(a3),-(a1)
	move.b	-(a3),-(a1)
	bra	sp3_22
sp3_37:
	moveq	#3,d1
	bsr.s	sp3_44
	tst.w	d0
	beq.s	sp3_38
	addq.w	#5,d0
	bra.s	sp3_40
sp3_38:
	move.b	-(a0),d0
	beq.s	sp3_39
	addi.w	#20,d0
	bra.s	sp3_40
sp3_39:
	moveq	#13,d1
	bsr.s	sp3_44
	addi.w	#276,d0
sp3_40:
	move.w	d0,d3
	add.w	d3,d3
sp3_41:
;	lea	sp3_52-sp3_53(a6),a4
	lea	-(17*2)(a6),a4
	bsr.s	sp3_48
	lsr.w	#1,d3
	lea	1(a1,d0.w),a3
	move.b	-(a3),-(a1)
sp3_42:
	move.b	-(a3),-(a1)
	dbf	d3,sp3_42
	bra	sp3_22
sp3_43:
	moveq	#0,d1
	move.b	(a3),d1
sp3_44:
	moveq	#0,d0
	cmpi.w	#7,d1
	bpl.s	sp3_47
sp3_45:
	add.b	d7,d7
	beq.s	sp3_46
	addx.w	d0,d0
	dbf	d1,sp3_45
	rts
sp3_46:
	move.b	-(a0),d7
	addx.b	d7,d7
	addx.w	d0,d0
	dbf	d1,sp3_45
	rts
sp3_47:
	move.b	-(a0),d0
	subq.w	#8,d1
	bpl.s	sp3_45
	rts
sp3_48:
	moveq	#0,d1
	move.b	(a3),d1
	adda.w	d1,a4
	move.w	(a4),d1
	bsr.s	sp3_44
	tst.b	d6
	beq.s	sp3_51
	move.w	d0,d4
	andi.w	#$FFF0,d4
	andi.w	#$000F,d0
	beq.s	sp3_50
	lsr.w	#1,d0
	beq.s	sp3_49
	roxr.b	#1,d7
	bcc.s	sp3_50
	move.b	d7,(a0)+
	moveq	#-128,d7
	bra.s	sp3_50
sp3_49:
	moveq	#2,d1
	bsr.s	sp3_44
	add.w	d0,d0
	or.w	d4,d0
	bra.s	sp3_51
sp3_50:
;	lea	sp3_54-sp3_53(a6),a3
	lea	osp3_54(a6),a3
	or.b	(a3,d0.w),d4
	move.w	d4,d0
sp3_51:
	add.w	18(a4),d0
	rts

	DC.W	3
sp3_52:
	DC.W	4,5,7,8,9,10,11,12
	DC.W	-16
	DC.W	0,32,96,352,864,1888,3936,8032

sp3_53:	DS.L	1				;   0  4
sp3_54:	DS.B	8				;   4  8
sp3_55:	DS.W	2*64			;  12  256
sp3_56:	DS.W	2				; 268  3
	DS.B	1
sp3_57:	DS.B	1				; 271 129
	DS.B	2*64
sp3_58:	DS.B	512				;
