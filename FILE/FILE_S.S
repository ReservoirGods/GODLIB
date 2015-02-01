**************************************************************************************
*
* File Library
*
* [c] 2008 Reservoir Gods
*
**************************************************************************************

**************************************************************************************
;	EXPORT / IMPORT
**************************************************************************************

	export	File_FileSelectorAES

	import	gpGemDosStackUser


**************************************************************************************
;	EQUATES
**************************************************************************************

	OFFSET

sAESControl_OpCode:			ds.w	1
sAESControl_IntInCount:		ds.w	1
sAESControl_IntOutCount:	ds.w	1
sAESControl_AddrInCount:	ds.w	1
sAESControl_AddrOutCount:	ds.w	1
sAESControl_sizeof:			ds.w	1


**************************************************************************************
	TEXT
**************************************************************************************


*------------------------------------------------------------------------------------*
* FUNCTION : File_FileSelectorAES( const char * apTitle, const char * apPath, const char * apFile )
* ACTION   : calls file selector
* CREATION : 11.01.08 PNK
*------------------------------------------------------------------------------------*

File_FileSelectorAES:
	move.l	a0,gFileSelectorArgs+0
	move.l	a1,gFileSelectorArgs+4
	move.l	4(a7),gFileSelectorArgs+8

	movem.l	d1-d7/a0-a6,-(a7)

	move.l	#1,-(a7)
	move.w	#$20,-(a7)
	trap	#1
	addq.l	#6,a7

	move.w	d0,gFileSelectorSuperFlag

	tst.w	d0
	beq		.usermode

	move	usp,a0
	move.l	a0,gFileSelectorSP
	lea		-32(a7),a0
	move	a0,usp

	move.l	gpGemDosStackUser,-(a7)
	move.w	#$20,-(a7)
	trap	#1
	addq.l	#6,a7

.usermode:

	lea		gFileControlAES,a3
	move.w	#10,sAESControl_OpCode(a3)			; appl_init
	move.w	#0,sAESControl_IntInCount(a3)
	move.w	#1,sAESControl_IntOutCount(a3)
	move.w	#0,sAESControl_AddrInCount(a3)
	move.w	#0,sAESControl_AddrOutCount(a3)
	move.l	#gFileParameterBlockAES,d1
	move.w	#200,d0
	trap	#2

	dc.w	$a009

	lea		gFileControlAES,a3
	move.l	gFileSelectorArgs+0,gFileAddrInAES+8	;title
	move.l	gFileSelectorArgs+4,gFileAddrInAES+0	;path
	move.l	gFileSelectorArgs+8,gFileAddrInAES+4	;filename

	moveq	#90,d0								; fsel_input
	move.w	#2,sAESControl_AddrInCount(a3)
	cmpi.w	#$0140,gFileGlobalAES				; check AES verion
	blt.s	.aeslo
	moveq	#91,d0								; fsel_exinput
	move.w	#3,sAESControl_AddrInCount(a3)
.aeslo:


	move.w	d0,sAESControl_OpCode(a3)
	move.w	#0,sAESControl_IntInCount(a3)
	move.w	#2,sAESControl_IntOutCount(a3)
	move.w	#0,sAESControl_AddrOutCount(a3)
	move.l	#gFileParameterBlockAES,d1
	move.w	#200,d0
	trap	#2
	move.w	gFileIntOutAES+2,d7

	dc.w	$a00a

	lea		gFileControlAES,a3
	move.w	#19,sAESControl_OpCode(a3)			; appl_exit
	move.w	#0,sAESControl_IntInCount(a3)
	move.w	#1,sAESControl_IntOutCount(a3)
	move.w	#0,sAESControl_AddrInCount(a3)
	move.w	#0,sAESControl_AddrOutCount(a3)
	move.l	#gFileParameterBlockAES,d1
	move.w	#200,d0
	trap	#2

	tst.w	gFileSelectorSuperFlag
	beq		.noswapback

	clr.l	-(a7)
	move.w	#$20,-(a7)
	trap	#1

	move.l	gFileSelectorSP,a0
	move	a0,usp

	lea		36(a7),a7

.noswapback:

	moveq	#0,d0
	move.w	d7,d0
	movem.l	(a7)+,d1-d7/a0-a6
	rts


**************************************************************************************
	DATA
**************************************************************************************
gFileSelectorSuperFlag:
	ds.w	1

gFileSelectorSP:
	ds.l	4
gFileSelectorArgs:
	ds.l	4

gFileControlAES:
	ds.w	sAESControl_sizeof

gFileGlobalAES:
	ds.w	15

gFileIntInAES:
	ds.w	16

gFileIntOutAES:
	ds.w	7

gFileAddrInAES:
	ds.l	2

gFileAddrOutAES:
	ds.l	1

gFileParameterBlockAES:
	dc.l	gFileControlAES
	dc.l	gFileGlobalAES
	dc.l	gFileIntInAES
	dc.l	gFileIntOutAES
	dc.l	gFileAddrInAES
	dc.l	gFileAddrOutAES


**************************************************************************************
	BSS
**************************************************************************************

