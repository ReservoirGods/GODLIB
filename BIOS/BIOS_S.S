**************************************************************************************
*	BIOS_S.S
*
*	TOS BIOS calling routines
*
*	[c] 2001 Reservoir Gods
**************************************************************************************


**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	Bios_Bconin
	export	Bios_Bconout
	export	Bios_Bconstat
	export	Bios_Drvmap
	export	Bios_Getmbpb
	export	Bios_Kbshift
	export	Bios_Mediach
	export	Bios_Rwabs
	export	Bios_Setexec

	export	Bios_PipeConsole
	export	Bios_UnPipeConsole

	export	brake

**************************************************************************************
;	EQUATES
**************************************************************************************

**************************************************************************************
;	MACROS
**************************************************************************************

	MACRO	mBIOS	aOP

	pea		(a2)
	move.w	#aOP,-(a7)
	trap	#13
	addq.l	#2,a7
	move.l	(a7)+,a2

	ENDM

*------------------------------------------------------------------------------------*

	MACRO	mBIOS_W	aOP

	pea		(a2)
	move.w	d0,-(a7)
	move.w	#aOP,-(a7)
	trap	#13
	addq.l	#4,a7
	move.l	(a7)+,a2

	ENDM

*------------------------------------------------------------------------------------*

	MACRO	mBIOS_WW	aOP

	pea		(a2)
	move.w	d1,-(a7)
	move.w	d0,-(a7)
	move.w	#aOP,-(a7)
	trap	#13
	addq.l	#6,a7
	move.l	(a7)+,a2

	ENDM

*------------------------------------------------------------------------------------*

	MACRO	mBIOS_P	aOP

	pea		(a2)
	pea		(a0)
	move.w	#aOP,-(a7)
	trap	#13
	addq.l	#6,a7
	move.l	(a7)+,a2

	ENDM

*------------------------------------------------------------------------------------*

	MACRO	mBIOS_WP	aOP

	pea		(a2)
	pea		(a0)
	move.w	d0,-(a7)
	move.w	#aOP,-(a7)
	trap	#13
	addq.l	#6,a7
	move.l	(a7)+,a2

	ENDM


**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Bconin
* ACTION   :
* CREATION : 07.06.09 PNK
*------------------------------------------------------------------------------------*

Bios_Bconin:
	mBIOS_W		2
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Bconout
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Bconout:
	mBIOS_WW	3
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Bconstat
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Bconstat:
	mBIOS_W	1
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Drvmap
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Drvmap:
	mBIOS	10
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Getbpb
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Getbpb:
	mBIOS_W	7
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Getmbpb
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Getmbpb:
	mBIOS_P	0
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Kbshift
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Kbshift:
	mBIOS_W	11
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Mediach
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Mediach:
	mBIOS_W	9
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Rwabs
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Rwabs:
	pea		(a2)

	lea		4(a7),a2
	move.l	(a2)+,-(a7)
	move.w	(a2)+,-(a7)
	move.w	d2,-(a7)
	move.w	d1,-(a7)
	pea		(a0)
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	trap	#13
	lea		18(a7),a7

	move.l	(a7)+,a2
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_Setexec
* ACTION   :
* CREATION : 11.11.01 PNK
*------------------------------------------------------------------------------------*

Bios_Setexec:
	mBIOS_WP	5
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_PipeConsole( void * buffer, u32 aSize )
* ACTION   : installs a redirection into bios to wrote conout to file
* CREATION : 21.08.18 PNK
*------------------------------------------------------------------------------------*
brake:
Bios_PipeConsole:
	move.l	a0,Bios_NewBconoutDst+4
	move.l	d0,gBios_NewBconoutSize
	pea		Bios_PipeConsoleX
	bra		Bios_Supexec

Bios_PipeConsoleX:
	lea		Bios_Trap13,a0
	cmp.l	$b4.w,a0
	beq.s	.nope
	move.l	$B4.w,Bios_OldTrap13+2
	move.l	#Bios_Trap13,$B4.w
.nope:
	rts

Bios_UnPipeConsole:
	pea		Bios_TrapDeInitX
	bra		Bios_Supexec

Bios_TrapDeInitX:
	move.l	Bios_OldTrap13+2,d0
	cmp.l	#$12345678,d0
	beq.s	.nope
	move.l	d0,$B4.w
.nope:
	rts

Bios_Trap13:
	tas		gBios_Mutex
	bne		Bios_OldTrap13

	move.w	(a7),d0
	lea		6(a7),a0
	btst	#13,d0
	bne.s	.super
	move.l	USP,a0
.super:
	cmp.w	#$3,(a0)
	bne		Bios_OldTrap130
	cmp.w	#2,2(a0)
	beq		Bios_NewBconout

Bios_OldTrap130:
	clr.b	gBios_Mutex
Bios_OldTrap13:
	jmp		$12345678


Bios_NewBconout:
	tst.l	gBios_NewBconoutSize
	beq		Bios_OldTrap13
	subq.l	#1,gBios_NewBconoutSize
Bios_NewBconoutDst:
	move.b	5(a0),$12345678
	bra		Bios_OldTrap13


Bios_Supexec:
	move.w	#$26,-(a7)
	trap	#14
	addq.l	#6,a7
	rts

gBios_NewBconoutSize:	dc.l	0
gBios_Mutex:			dc.w	0
