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
	export	Bios_GetPipeOffset
	export	Bios_ClearPipeOffset

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
	move.l	a0,gBios_Buffer			; store buffer ptr
	move.l	d0,gBios_BufferSize		; store buffer size
	clr.l	gBios_BufferOff			; start at beginning of redirection buffer
	pea		Bios_PipeConsoleX		; routine to execute in supervisor mode
	bra		Bios_Supexec			; execute

Bios_PipeConsoleX:
	lea		Bios_Trap13,a0			; our bios handler
	cmp.l	$b4.w,a0				; already installed?
	beq.s	.nope					; don't install again

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
	move.w	#8,Bios_StackOffset+2	; extend stack frame offset
.install:
	move.l	$B4.w,Bios_Trap13End-4	; save old bios handler
	move.l	#Bios_Trap13,$B4.w		; install our handler
.nope:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Bios_UnPipeConsole()
* ACTION   : un-installs the bios redirection and uses original bios trap handler
* CREATION : 21.08.18 PNK
*------------------------------------------------------------------------------------*

Bios_UnPipeConsole:
	pea		Bios_TrapDeInitX		; routine to execute in supervisor mode
	bra		Bios_Supexec			; execute

Bios_TrapDeInitX:
	move.l	Bios_Trap13End-4,d0		; old bios handler
	beq.s	.nope					; if nullptr don't install
	move.l	d0,$B4.w				; install old bios handler
.nope:
	clr.l	gBios_BufferOff			; clear buffer offset
	rts


Bios_Trap13:
	tas		gBios_Mutex				; we don't want this handler to be rentrant
	bne		Bios_T13_noReentry		; skip handler if already in it

	move.w	(a7),d0					; saved status register
Bios_StackOffset:
	lea		6(a7),a0				; skip exception stack frame, get to trap arguments
	btst	#13,d0					; was trap called from supervisor mode?
	bne.s	.super					; yes, use ssp
	move.l	USP,a0					; no, use user mode stack pointer
.super:
	cmp.w	#$3,(a0)				; is this bconout() call?
	bne.s	.oldTrap13				; nope
	cmp.w	#2,2(a0)				; is the screen the output?
	bne.s	.oldTrap13				; nope

	move.l	gBios_Buffer,a1			; get our buffer
	move.l	gBios_BufferOff,d0		; get buffer offset
	cmp.l	gBios_BufferSize,d0		; have we overflowed?
	bge.s	.oldTrap13				; yes, don't try to write more
	move.b	5(a0),(a1,d0.l)			; store bconout character in buffer
	addq.l	#1,d0					; increase buffer offset
	move.l	d0,gBios_BufferOff		; store buffer offest
.oldTrap13:
	clr.b	gBios_Mutex				; clear rentrancy control
Bios_T13_noReentry:
	jmp		$12345678				; jump back to original bios handler
Bios_Trap13End:

Bios_Supexec:
	move.w	#$26,-(a7)				; xbios supexec
	trap	#14						; call xbios
	addq.l	#6,a7					; fix stack
	rts
	
Bios_GetPipeOffset:
	move.l	gBios_BufferOff,d0		; return buffer offset
	rts

Bios_ClearPipeOffset:
	clr.l	gBios_BufferOff			; clear buffer offset
	rts

gBios_Buffer:			dc.l	0
gBios_BufferSize:		dc.l	0
gBios_BufferOff:		dc.l	0
gBios_Mutex:			dc.w	0
