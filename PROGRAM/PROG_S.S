**************************************************************************************
*	PROG_S.S
*
*	program execution routines
*
*	[c] 2018 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	Program_Execute_Internal

	export	brake
**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET
sProgramHeader_mMagic:				ds.w	1
sProgramHeader_mTextSize:			ds.l	1
sProgramHeader_mDataSize:			ds.l	1
sProgramHeader_mBSSSize:			ds.l	1
sProgramHeader_mSymbolTableSize:	ds.l	1
sProgramHeader_mReserved:			ds.l	1
sProgramHeader_mFlags:				ds.l	1
sProgramHeader_mRelocationFlag:		ds.w	1
sProgramHeader_mSizeof:				ds.b	1


	OFFSET
sBasePage_mpLowTPA:			ds.l	1
sBasePage_mpHiTPA:			ds.l	1
sBasePage_mpText:			ds.l	1
sBasePage_mTextLength:		ds.l	1
sBasePage_mpData:			ds.l	1
sBasePage_mDataLength:		ds.l	1
sBasePage_mpBSS:			ds.l	1
sBasePage_mBSSLength:		ds.l	1
sBasePage_mpDTA:			ds.l	1
sBasePage_mpParentBP:		ds.l	1
sBasePage_mReserved0:		ds.l	1
sBasePage_mpEnvironment:	ds.l	1
sBasePage_mReserved1:		ds.b	80
sBasePage_mCommandLine:		ds.b	128
sBasePage_sizeof:			ds.b	1


**************************************************************************************
;	EQUATES
**************************************************************************************



**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : Program_Execute_Internal( sProgramHeader * apProg, char * apCmdLine )
* ACTION   : starts a program
* CREATION : 12.08.18 PNK
*------------------------------------------------------------------------------------*
brake:
Program_Execute_Internal:
	movem.l	d0-a6,-(a7)
	
	move.l	a7,Program_Execute_Return+2
	
	move.l	a0,a6

	bsr		Program_TrapInit

	move.l	a6,-(a7)
	clr.l	-(a7)

	move.l	sBasePage_mpData(a6),a4
	move.l	sBasePage_mpBSS(a6),a5

	; set up heap for linear allocator at 4 byte aligned address after BSS segment
	move.l	a5,d0
	add.l	sBasePage_mBSSLength(a6),d0
	addq.l	#3,d0
	and.b	#$FC,d0
	move.l	d0,gProgram_MallocAdr

	moveq	#0,d0
	move.l	d0,d1
	move.l	d0,d2
	move.l	d0,d3
	move.l	d0,d4
	move.l	d0,d5
	move.l	d0,d6
	move.l	d0,d7

	move.l	d0,a0
	move.l	d0,a1
	move.l	d0,a2
	move.l	d0,a3

	lea		sBasePage_sizeof(a6),a6
	jmp		(a6)
Program_Execute_Return:
	move.l	#$12345678,a7

	bsr		Program_TrapDeInit

	movem.l	(a7)+,d0-a6
	rts

Program_Supexec:
	move.w	#$26,-(a7)
	trap	#14
	addq.l	#6,a7
	rts

Program_TrapInit:
	move.w	SR,Program_pterm+2
	pea	Program_TrapInitX
	bra	Program_Supexec

Program_TrapInitX:
	move.l	$84.w,Program_OldTrap1+2
	move.l	#Program_Trap1,$84.w
	rts

Program_TrapDeInit:
	pea	Program_TrapDeInitX
	bra	Program_Supexec

Program_TrapDeInitX:
	move.l	Program_OldTrap1+2,$84.w
	rts

*------------------------------------------------------------------------------------*
* FUNCTION : Program_Trap1
* ACTION   : Replacement for GEMDOS trap #1 handler. Replaces following functions:
*			 $48 - Malloc : uses simple linear allocator instead
*			 $4A - Mshrink : parent has already mshrink'd, so this just returns 0
*			 $4C/0 - PTERM : ensures control handed back to parent
* CREATION : 13.08.2018 PNK
*------------------------------------------------------------------------------------*

Program_Trap1:
	move.l	USP,a0
	cmp.w	#$48,(a0)
	beq		Program_malloc
	cmp.w	#$4A,(a0)
	beq		Program_mshrink
	cmp.w	#$4C,(a0)
	beq		Program_pterm
	tst.w	(a0)
	beq		Program_pterm
Program_OldTrap1:	
	jmp		$12345678
Program_mshrink:
	clr.l	d0
	rte


*------------------------------------------------------------------------------------*
* FUNCTION : Program_malloc
* ACTION   : Replacement for GEMDOS malloc that can fragment after multiple PEXECs
*			 This is a simple linear allocator
*			 Allocations are 4 byte aligned
*			 On a new pexec, the malloc base pointer is reset
* CREATION : 13.08.2018 PNK
*------------------------------------------------------------------------------------*

Program_malloc:
	move.l	2(a0),d1
	move.l	gProgram_MallocAdr,d0
	move.l	d0,a0
	add.l	d0,d1
	addq.l	#3,d1
	and.b	#$fc,d1
	move.l	d1,gProgram_MallocAdr
	rte


*------------------------------------------------------------------------------------*
* FUNCTION : Program_pterm
* ACTION   : Replacement for GEMDOS pterm
*			 Restores Status Register and PC back to calling program
* CREATION : 13.08.2018 PNK
*------------------------------------------------------------------------------------*

Program_pterm:
	move.w	#$1234,(a7)
	move.l	#Program_Execute_Return,2(a7)
	rte

gProgram_MallocAdr:	dc.l	0


**************************************************************************************
