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

Program_Execute_Internal:
	movem.l	d0-a6,-(a7)
	
	move.l	a7,Program_Execute_Return+2		; save SP
	
	move.l	a0,a6							; save ptr to program header

	bsr		Program_TrapInit				; install new GEMDOS handler

	move.l	a6,-(a7)						; basepage pointer on stack
	clr.l	-(a7)							; clear stack

	move.l	sBasePage_mpData(a6),a4			; ptr to DATA segment
	move.l	sBasePage_mpBSS(a6),a5			; ptr to BSS segment

	
	move.l	a5,d0							; start of BSS
	add.l	sBasePage_mBSSLength(a6),d0		; end of BSS segment
	addq.l	#3,d0							; for alignment
	and.b	#$FC,d0							; 4 byte align
	move.l	d0,gProgram_MallocAdr			; store initial malloc address

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
	move.l	d0,a3							; clear registers

	lea		sBasePage_sizeof(a6),a6			; program starts at end of basepage
	jmp		(a6)
Program_Execute_Return:
	move.l	#$12345678,a7					; restore old SP

	bsr		Program_TrapDeInit				; reinstall old GEMDOS handler

	movem.l	(a7)+,d0-a6						; restore registers
	rts

Program_Supexec:
	move.w	#$26,-(a7)				; superexec()
	trap	#14						; call XBIOS
	addq.l	#6,a7					; fix stack
	rts

Program_TrapInit:
	move.w	SR,Program_pterm+2		; original status register
	pea	Program_TrapInitX			; routine to execute in supervisor mode
	bra	Program_Supexec				; execute it

Program_TrapInitX:

	move.l	$5A0,d0					; cookie jar
	tst.l	d0						; valid pointer?
	beq.s	.install				; no, must be oldschool ST
	move.l	d0,a0					; ptr to first cookie
.nextCookie:
	move.l	(a0),d0					; read cookie key
	beq.s	.install				; if zero we are at end of jar
	cmp.l	#$5F435055,d0			; does key == _CPU ?
	beq.s	.gotCPU					; yep
	addq.l	#8,a0					; move to next cookie
	bra.s	.nextCookie				; loop over all cookies
.gotCPU:
	move.l	4(a0),d0				; read _CPU value
	cmp.l	#20,d0					; is it at least a 68020?
	blt.s	.install				; no, stack frame is just 6 bytes
	move.w	#8,Program_StackOff+2	; extend stack frame offset
.install:

	move.l	$84.w,Program_OldTrap1+2	; old GEMDOS handler
	move.l	#Program_Trap1,$84.w		; install new handler
	rts

Program_TrapDeInit:
	pea	Program_TrapDeInitX			; routine to execute in supervisor mode
	bra	Program_Supexec				; execute it

Program_TrapDeInitX:
	move.l	Program_OldTrap1+2,d0	; original GEMDOS handler
	beq.s	.nope					; bail if nullptr
	move.l	d0,$84.w				; reinstall old handler
.nope:
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
	move.w	(a7),d0					; saved status register
Program_StackOff:
	lea		6(a7),a0				; skip exception stack frame, get to trap arguments
	btst	#13,d0					; was trap called from supervisor mode?
	bne.s	.super					; yes, use ssp
	move.l	USP,a0					; no, use user mode stack pointer
.super:
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
