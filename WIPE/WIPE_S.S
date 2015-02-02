**************************************************************************************
*	WIPE_S.S
*
*	fast transition effects
*
* Disappear routine vesrion 2 coded in Turbo Assembler
* Coded By George Nakos (version 1 in GFA BASIC by the same author)
* a0.l should contain the source address
* a3.l should contain the destination address
*
*                            Diary of the program
*                            ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ
*   Date        Try #   Comments
* 08/06/95       01     First session, the code is uncompleted (needs supervisor
*                       on and off and VSYNC routines to be added). It is
*                       uncertain whether the code will run or not (after all,
*                       nothing works on the first go!). Just 1 effect is used.
* 09/06/95       02     Second session, and after a couple of hours of debugging
*                       and head scrtching, head bangning etc. it works! (in 1
*                       VBL!). The code is very messy now, next session I'll
*                       have to clean it up. Then I'll transfer all 39 effects
*                       and make it appear screens as well as disappearing 'em.
* 23/06/95       03     Have succesfully transferred all 39 effects across and
*                       coded a little routine to find them (The effects take
*                       no less space than 18862 fucking bytes!). The code is
*                       really messy now (even with the comments it's very
*                       unreadable).
* 15/10/95       04     Have written the appear patch, but it's disappointing
*                       (it runs in 2 VBLs). Cannot find a way to make it
*                       run faster. Have tidied up the code a bit.
* 02/10/02       05     Yikes! Almost 7 years after!!! Well, I've thought of
*                       a way of speeding it up a long time ago (about a year
*                       or so - using halftone masks) but I was too bored to
*                       do it. Until today :) Everything runs in 50 hz now!!
* 22/01/03	 06	Tried to make the source fit MrPink's specs!
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	Wipe_In_Init
	export	Wipe_Out_Init
	export	Wipe_IsFinished
	export	Wipe_Update
	export	Wipe_Render_STE

	export	gWipeFrameCounts


**************************************************************************************
;	EQUATES
**************************************************************************************

halftone        EQU $FFFF8A00
s_x_inc         EQU $FFFF8A20
s_y_inc         EQU $FFFF8A22
s_hw            EQU $FFFF8A24
s_lw            EQU $FFFF8A26
end_1           EQU $FFFF8A28
end_2           EQU $FFFF8A2A
end_3           EQU $FFFF8A2C
d_x_inc         EQU $FFFF8A2E
d_y_inc         EQU $FFFF8A30
d_hw            EQU $FFFF8A32
d_lw            EQU $FFFF8A34
x_cnt           EQU $FFFF8A36
y_cnt           EQU $FFFF8A38
hop             EQU $FFFF8A3A   ;Byte
op              EQU $FFFF8A3B   ; >>
skew            EQU $FFFF8A3D   ; >>
blit            EQU $FFFF8A3C

eWIPE_CHUNK_SIZE	EQU	(16*2)
eWIPE_FX_LIMIT		EQU	39
eWIPE_MODE_NONE		EQU	0
eWIPE_MODE_IN		EQU	1
eWIPE_MODE_OUT		EQU	2



**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION: void Wipe_In_Init( U16 aFxIndex, U16 * apGfx )
* ACTION:   initialises wipe effect
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_In_Init:
	cmp.w	#eWIPE_FX_LIMIT,d0
	bge		.ret

	move.w	d0,gWipeFxIndex
	move.l	a0,gpWipeGfx

	lea		gWipeFrameCounts,a1
	add.w	d0,d0
	add.w	d0,a1
	move.w	(a1),gWipeFramesLeft

	lea		gpWipeData,a1
	add.w	d0,d0
	add.w	d0,a1
	move.l	(a1),gpWipeMask

	move.w	#eWIPE_MODE_IN,gWipeFxMode
.ret:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: U8 Wipe_IsFinished( void )
* ACTION:   is wipe over?
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_IsFinished:
	move.w	gWipeFxMode,d0
	seq		d0
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: void Wipe_Out_Init( U16 aFxIndex )
* ACTION:   initialises wipe effect
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_Out_Init:
	cmp.w	#eWIPE_FX_LIMIT,d0
	bge		.ret

	move.w	d0,gWipeFxIndex

	lea		gWipeFrameCounts,a1
	add.w	d0,d0
	add.w	d0,a1
	move.w	(a1),gWipeFramesLeft

	lea		gpWipeData,a1
	add.w	d0,d0
	add.w	d0,a1
	move.l	(a1),gpWipeMask

	move.w	#eWIPE_MODE_OUT,gWipeFxMode
.ret:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: U16 Wipe_Update( U16 * apScreen )
* ACTION:   updates wipe
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_Update:
	movem.l	d0-d7/a0-a2,-(a7)

	move.w	gWipeFxMode,d0
	beq		.ret

	cmpi.w	#eWIPE_MODE_IN,d0
	bne.s	.checkout

	move.l	a0,a1
	move.l	gpWipeGfx,a0
	move.l	gpWipeMask,a2
	bsr		Wipe_In_Render

	add.l	#eWIPE_CHUNK_SIZE,gpWipeMask
	subq.w	#1,gWipeFramesLeft
	ble		.wipe_end
	bra		.ret

.checkout:
	cmpi.w	#eWIPE_MODE_OUT,d0
	bne.s	.ret

	move.l	gpWipeMask,a1
	bsr		Wipe_Out_Render

	add.l	#eWIPE_CHUNK_SIZE,gpWipeMask
	subq.w	#1,gWipeFramesLeft
	bgt		.ret

.wipe_end:
	moveq	#0,d0
	move.w	d0,gWipeFramesLeft
	move.w	#eWIPE_MODE_NONE,gWipeFxMode
	bra		.ret
.ret:
	movem.l	(a7)+,d0-d7/a0-a2
	rts


*------------------------------------------------------------------------------------*
* FUNCTION: U8 Wipe_Render( U16 * apSrc, U16 * apDst, U16 aFXindex, U16 aFrameIndex )
* ACTION:   renders wipe
* CREATION: 21.01.03 GGN
*------------------------------------------------------------------------------------*

Wipe_Render_STE:
	movem.l	d3-d7/a2,-(a7)

	move.l	(a2),a2
	lsl.w	#4,d1
	add.w	d1,a2

	move.w  #80,x_cnt.w     ;These values are set here because
	move.w  #2,s_x_inc.w    ;they need not be changed as long as
	move.w  #2,s_y_inc.w    ;the effect takes place.
	move.w  #2,d_x_inc.w
	move.w  #2,d_y_inc.w
	clr.b   skew.w       	;Don't shift anything
	move.b  #3,hop.w        ;Halftone mode :) source and halftone
	move.b  #3,op.w         ;0=erase, 3=show - source
	move.l  #-1,end_1.w	;no endmasks needed
	move.w  #-1,end_3.w

	movem.l (A2)+,D0-D7	;load & store our halftones
	movem.l D0-D7,halftone.w

	move.l  A0,s_hw.w       ;Source address
	move.l  A1,d_hw.w       ;Dest address
	move.w  #200,y_cnt.w    ;Copy everything in 1 go
	move.b  #192,blit.w     ;Blit it


	movem.l	(a7)+,a2/d3-d7
	rts                     ;We're outta here!


*------------------------------------------------------------------------------------*
* FUNCTION: U8 Wipe_In_Render( U16 * apSrc, U16 * apDst, U16 * apMask, U16 aFXindex, U16 aFrameIndex )
* ACTION:   renders wipe
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_In_Render:
	move.w	#199,d0

	moveq	#16,d1
.yloop:
	dbra	d1,.chunkloop
	lea		-32(a2),a2
	moveq	#15,d1
.chunkloop:
	move.w	(a2),d2
	swap	d2
	move.w	(a2)+,d2

	REPT	40

	move.l	(a0)+,d3
	and.l	d2,d3
	move.l	d3,(a1)+

	ENDM

	dbra	d0,.yloop

	rts


*------------------------------------------------------------------------------------*
* FUNCTION: U8 Wipe_Out_Render( U16 * apSrc, U16 * apMask, U16 aFXindex, U16 aFrameIndex )
* ACTION:   initialises wipe effect
* CREATION: 09.02.04 PNK
*------------------------------------------------------------------------------------*

Wipe_Out_Render:

	move.w	#199,d0

	moveq	#16,d1
.yloop:
	dbra	d1,.chunkloop
	lea		-32(a1),a1
	moveq	#15,d1
.chunkloop:
	move.w	(a1),d2
	swap	d2
	move.w	(a1)+,d2

	REPT	40

	and.l	d2,(a0)+

	ENDM

	dbra	d0,.yloop

	rts


**************************************************************************************
	DATA
**************************************************************************************

gWipeFxIndex:		dc.w	0
gWipeFxMode:		dc.w	0
gWipeFramesLeft:	dc.w	0
gpWipeGfx:			dc.l	0
gpWipeMask:			dc.l	0

gpWipeData:
	dc.l gWipeData1,gWipeData2,gWipeData3,gWipeData4,gWipeData5,gWipeData6,gWipeData7,gWipeData8,gWipeData9
	dc.l gWipeData10,gWipeData11,gWipeData12,gWipeData13,gWipeData14,gWipeData15,gWipeData16,gWipeData17,gWipeData18,gWipeData19
	dc.l gWipeData20,gWipeData21,gWipeData22,gWipeData23,gWipeData24,gWipeData25,gWipeData26,gWipeData27,gWipeData28,gWipeData29
	dc.l gWipeData30,gWipeData31,gWipeData32,gWipeData33,gWipeData34,gWipeData35,gWipeData36,gWipeData37,gWipeData38,gWipeData39


gWipeData1:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0100,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$0100,$0380,$0100,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$0100,$0380,$07C0,$0380,$0100,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0100,$07C0,$07C0,$0FE0,$07C0,$07C0,$0100,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$0100,$07C0,$0FE0,$0FE0,$1FF0,$0FE0,$0FE0,$07C0,$0100,$00,$00,$00,$00
	dc.w $00,$00,$0100,$07C0,$0FE0,$1FF0,$1FF0,$3FF8,$1FF0,$1FF0,$0FE0,$07C0,$0100,$00,$00,$00
	dc.w $00,$00,$0100,$0FE0,$1FF0,$1FF0,$3FF8,$7FFC,$3FF8,$1FF0,$1FF0,$0FE0,$0100,$00,$00,$00
	dc.w $00,$0100,$07C0,$1FF0,$3FF8,$7FFC,$7FFC,$FFFE,$7FFC,$7FFC,$3FF8,$1FF0,$07C0,$0100,$00,$00
	dc.w $0100,$0FE0,$1FF0,$3FF8,$7FFC,$FFFE,$FFFE,$FFFF,$FFFE,$FFFE,$7FFC,$3FF8,$1FF0,$0FE0,$0100,$00
	dc.w $1FE0,$3FF0,$7FFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$7FFC,$3FF8,$0FE0,$0100
	dc.w $7FF8,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$7FF8,$1FE0
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$7FFC
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData1_End:

gWipeData2:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0180,$0180,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$03C0,$03C0,$03C0,$03C0,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$00,$00,$00,$00
	dc.w $00,$00,$00,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$00,$00,$00
	dc.w $00,$00,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$00,$00
	dc.w $00,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData2_End:


gWipeData3:
	dc.w $FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData3_End:


gWipeData4:
	dc.w $8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000
	dc.w $C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000
	dc.w $E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000
	dc.w $F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000
	dc.w $F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800
	dc.w $FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00
	dc.w $FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00
	dc.w $FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80
	dc.w $FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0
	dc.w $FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0
	dc.w $FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0
	dc.w $FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8
	dc.w $FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC
	dc.w $FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData4_End:


gWipeData5:
	dc.w $8000,$4000,$2000,$1000,$0800,$0400,$0200,$0100,$80,$40,$20,$10,$08,$04,$02,$01
	dc.w $C000,$E000,$7000,$3800,$1C00,$0E00,$0700,$0380,$01C0,$E0,$70,$38,$1C,$0E,$07,$03
	dc.w $E000,$F000,$F800,$7C00,$3E00,$1F00,$0F80,$07C0,$03E0,$01F0,$F8,$7C,$3E,$1F,$0F,$07
	dc.w $F000,$F800,$FC00,$FE00,$7F00,$3F80,$1FC0,$0FE0,$07F0,$03F8,$01FC,$FE,$7F,$3F,$1F,$0F
	dc.w $F800,$FC00,$FE00,$FF00,$FF80,$7FC0,$3FE0,$1FF0,$0FF8,$07FC,$03FE,$01FF,$FF,$7F,$3F,$1F
	dc.w $FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$7FF0,$3FF8,$1FFC,$0FFE,$07FF,$03FF,$01FF,$FF,$7F,$3F
	dc.w $FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$7FFC,$3FFE,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$7F
	dc.w $FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF
	dc.w $FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF
	dc.w $FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF
	dc.w $FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF
	dc.w $FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF
	dc.w $FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF
	dc.w $FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData5_End:


gWipeData6:
	dc.w $0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180
	dc.w $03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0
	dc.w $07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0,$07E0
	dc.w $0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0,$0FF0
	dc.w $1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8,$1FF8
	dc.w $3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC,$3FFC
	dc.w $7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData6_End:


gWipeData7:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0180,$0180,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$03C0,$0180,$0180,$03C0,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$00,$00,$00,$00
	dc.w $00,$00,$00,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$00,$00,$00
	dc.w $00,$00,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$00,$00
	dc.w $00,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$00
	dc.w $FFFF,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$FFFF
	dc.w $FFFF,$FFFF,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$1FF8,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$1FF8,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$0FF0,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$0FF0,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$07E0,$03C0,$0180,$0180,$03C0,$07E0,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$03C0,$0180,$0180,$03C0,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$0180,$0180,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData7_End:


gWipeData8:
	dc.w $8000,$01,$8000,$01,$8000,$01,$8000,$01,$8000,$01,$8000,$01,$8000,$01,$8000,$01
	dc.w $C000,$03,$C000,$03,$C000,$03,$C000,$03,$C000,$03,$C000,$03,$C000,$03,$C000,$03
	dc.w $E000,$07,$E000,$07,$E000,$07,$E000,$07,$E000,$07,$E000,$07,$E000,$07,$E000,$07
	dc.w $F000,$0F,$F000,$0F,$F000,$0F,$F000,$0F,$F000,$0F,$F000,$0F,$F000,$0F,$F000,$0F
	dc.w $F800,$1F,$F800,$1F,$F800,$1F,$F800,$1F,$F800,$1F,$F800,$1F,$F800,$1F,$F800,$1F
	dc.w $FC00,$3F,$FC00,$3F,$FC00,$3F,$FC00,$3F,$FC00,$3F,$FC00,$3F,$FC00,$3F,$FC00,$3F
	dc.w $FE00,$7F,$FE00,$7F,$FE00,$7F,$FE00,$7F,$FE00,$7F,$FE00,$7F,$FE00,$7F,$FE00,$7F
	dc.w $FF00,$FF,$FF00,$FF,$FF00,$FF,$FF00,$FF,$FF00,$FF,$FF00,$FF,$FF00,$FF,$FF00,$FF
	dc.w $FF80,$01FF,$FF80,$01FF,$FF80,$01FF,$FF80,$01FF,$FF80,$01FF,$FF80,$01FF,$FF80,$01FF,$FF80,$01FF
	dc.w $FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF,$FFC0,$03FF
	dc.w $FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF,$FFE0,$07FF
	dc.w $FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF,$FFF0,$0FFF
	dc.w $FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF,$FFF8,$1FFF
	dc.w $FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF,$FFFC,$3FFF
	dc.w $FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF,$FFFE,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData8_End:


gWipeData9:
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00
	dc.w $00,$00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80
	dc.w $00,$00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0
	dc.w $00,$00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0
	dc.w $00,$00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0
	dc.w $00,$00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8
	dc.w $00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC
	dc.w $00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE
	dc.w $8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF
	dc.w $C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF
	dc.w $E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF
	dc.w $F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData9_End:


gWipeData10:
	dc.w $E000,$1F00,$F8,$07,$3C,$03C0,$3C00,$C000,$3C00,$03C0,$3C,$03,$3C,$03C0,$3C00,$C000
	dc.w $F000,$1F00,$01F8,$0F,$7C,$03C0,$3E00,$E000,$3E00,$03C0,$7C,$07,$7C,$03C0,$3E00,$E000
	dc.w $F800,$1F80,$01F8,$1F,$7C,$07E0,$3E00,$F000,$3E00,$07E0,$7C,$0F,$7C,$07E0,$3E00,$F000
	dc.w $F800,$3F80,$03F8,$3F,$FC,$07E0,$3F00,$F800,$3F00,$07E0,$FC,$1F,$FC,$07E0,$3F00,$F800
	dc.w $FC00,$3FC0,$03FC,$3F,$01FC,$07E0,$3F80,$FC00,$3F80,$07E0,$01FC,$3F,$01FC,$07E0,$3F80,$FC00
	dc.w $FE00,$3FC0,$07FC,$7F,$01FC,$0FF0,$3F80,$FE00,$3F80,$0FF0,$01FC,$7F,$01FC,$0FF0,$3F80,$FE00
	dc.w $FF00,$3FE0,$07FC,$FF,$03FC,$0FF0,$3FC0,$FF00,$3FC0,$0FF0,$03FC,$FF,$03FC,$0FF0,$3FC0,$FF00
	dc.w $FF80,$3FE0,$0FFC,$01FF,$07FC,$1FF0,$7FC0,$FF00,$7FC0,$1FF0,$07FC,$01FF,$07FC,$1FF0,$7FC0,$FF00
	dc.w $FFC0,$3FF0,$0FFC,$03FF,$07FE,$1FF8,$7FE0,$FF80,$7FE0,$1FF8,$07FE,$01FF,$07FE,$1FF8,$7FE0,$FF80
	dc.w $FFC0,$7FF0,$1FFC,$07FF,$0FFE,$1FF8,$7FF0,$FFC0,$7FF0,$1FF8,$0FFE,$03FF,$0FFE,$1FF8,$7FF0,$FFC0
	dc.w $FFE0,$7FF8,$1FFE,$07FF,$0FFE,$3FFC,$7FF0,$FFE0,$7FF0,$3FFC,$0FFE,$07FF,$0FFE,$3FFC,$7FF0,$FFE0
	dc.w $FFF0,$7FF8,$3FFE,$0FFF,$1FFE,$3FFC,$7FF8,$FFF0,$7FF8,$3FFC,$1FFE,$0FFF,$1FFE,$3FFC,$7FF8,$FFF0
	dc.w $FFF8,$7FFC,$3FFE,$1FFF,$3FFE,$3FFE,$7FFC,$FFF8,$7FFC,$3FFE,$3FFE,$1FFF,$3FFE,$3FFE,$7FFC,$FFF8
	dc.w $FFFC,$7FFE,$7FFE,$3FFF,$3FFF,$7FFE,$7FFE,$FFFC,$7FFE,$7FFE,$3FFF,$3FFF,$3FFF,$7FFE,$7FFE,$FFFC
	dc.w $FFFE,$FFFE,$7FFF,$7FFF,$7FFF,$7FFF,$FFFE,$FFFE,$FFFE,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$FFFE,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData10_End:


gWipeData11:
	dc.w $FF00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF
	dc.w $FF00,$FF00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$00,$00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$00,$00,$00,$00,$00,$00,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$00,$00,$00,$00,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$00,$00,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FFFF,$FFFF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF,$FF,$FF
	dc.w $FF00,$FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF,$FF
	dc.w $FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData11_End:

gWipeData12:
	dc.w $8000,$8000,$8000,$8000,$8000,$8000,$8000,$8000,$01,$01,$01,$01,$01,$01,$01,$01
	dc.w $C000,$C000,$C000,$C000,$C000,$C000,$C000,$C000,$03,$03,$03,$03,$03,$03,$03,$03
	dc.w $E000,$E000,$E000,$E000,$E000,$E000,$E000,$E000,$07,$07,$07,$07,$07,$07,$07,$07
	dc.w $F000,$F000,$F000,$F000,$F000,$F000,$F000,$F000,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F
	dc.w $F800,$F800,$F800,$F800,$F800,$F800,$F800,$F800,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F
	dc.w $FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$3F,$3F,$3F,$3F,$3F,$3F,$3F,$3F
	dc.w $FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$7F,$7F,$7F,$7F,$7F,$7F,$7F,$7F
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	dc.w $FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$01FF,$01FF,$01FF,$01FF,$01FF,$01FF,$01FF,$01FF
	dc.w $FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$03FF,$03FF,$03FF,$03FF,$03FF,$03FF,$03FF,$03FF
	dc.w $FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$07FF,$07FF,$07FF,$07FF,$07FF,$07FF,$07FF,$07FF
	dc.w $FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$0FFF,$0FFF,$0FFF,$0FFF,$0FFF,$0FFF,$0FFF,$0FFF
	dc.w $FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$1FFF,$1FFF,$1FFF,$1FFF,$1FFF,$1FFF,$1FFF,$1FFF
	dc.w $FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF
	dc.w $FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData12_End:


gWipeData13:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0180,$0180,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$0180,$03C0,$03C0,$0180,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$0180,$03C0,$07E0,$07E0,$03C0,$0180,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0180,$03C0,$07E0,$0FF0,$0FF0,$07E0,$03C0,$0180,$00,$00,$00,$00
	dc.w $00,$00,$00,$0180,$03C0,$07E0,$0FF0,$1FF8,$1FF8,$0FF0,$07E0,$03C0,$0180,$00,$00,$00
	dc.w $00,$00,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$00,$00
	dc.w $00,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$00
	dc.w $0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180
	dc.w $03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0
	dc.w $07E0,$0FF0,$1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8,$0FF0,$07E0
	dc.w $0FF0,$1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8,$0FF0
	dc.w $1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8
	dc.w $3FFC,$7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE,$3FFC
	dc.w $7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData13_End:


gWipeData14:
	dc.w $80,$80,$80,$80,$80,$80,$80,$FF80,$01FF,$0100,$0100,$0100,$0100,$0100,$0100,$0100
	dc.w $C0,$C0,$C0,$C0,$C0,$C0,$FFC0,$FFC0,$03FF,$03FF,$0300,$0300,$0300,$0300,$0300,$0300
	dc.w $E0,$E0,$E0,$E0,$E0,$FFE0,$FFE0,$FFE0,$07FF,$07FF,$07FF,$0700,$0700,$0700,$0700,$0700
	dc.w $F0,$F0,$F0,$F0,$FFF0,$FFF0,$FFF0,$FFF0,$0FFF,$0FFF,$0FFF,$0FFF,$0F00,$0F00,$0F00,$0F00
	dc.w $F8,$F8,$F8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$1FFF,$1FFF,$1FFF,$1FFF,$1FFF,$1F00,$1F00,$1F00
	dc.w $FC,$FC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3F00,$3F00
	dc.w $FE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7F00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData14_End:


gWipeData15:
	dc.w $8101,$4102,$2104,$1108,$0910,$0520,$0340,$01FF,$FF80,$02C0,$04A0,$0890,$1088,$2084,$4082,$8081
	dc.w $8303,$C306,$630C,$3318,$1910,$0520,$034F,$01FF,$FF80,$F2C0,$04A0,$0898,$18CC,$30C6,$60C3,$C0C1
	dc.w $8707,$C70E,$E30C,$7318,$1B30,$0F63,$077F,$01FF,$FF80,$FEE0,$C6F0,$0CD8,$18CE,$30C7,$70E3,$E0E1
	dc.w $8F0F,$CF1E,$E71C,$F738,$7B33,$1F6F,$077F,$01FF,$FF80,$FEE0,$F6F8,$CCDE,$1CEF,$38E7,$78F3,$F0F1
	dc.w $9F1F,$CF1E,$EF3C,$F739,$FF77,$3F7F,$0FFF,$03FF,$FFC0,$FFF0,$FEFC,$EEFF,$9CEF,$3CF7,$78F3,$F8F9
	dc.w $BF3F,$DF3E,$FF7D,$FF7F,$FF7F,$FF7F,$3FFF,$03FF,$FFC0,$FFFC,$FEFF,$FEFF,$FEFF,$BEFF,$7CFB,$FCFD
	dc.w $FF7F,$FF7F,$FF7F,$FF7F,$FFFF,$FFFF,$FFFF,$0FFF,$FFF0,$FFFF,$FFFF,$FFFF,$FEFF,$FEFF,$FEFF,$FEFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData15_End:


gWipeData16:
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$00,$0F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$03,$3F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$01,$07,$1F,$7F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$01,$07,$0F,$1F,$7F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $01,$03,$07,$0F,$0F,$1F,$3F,$7F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $07,$0F,$1F,$1F,$3F,$3F,$7F,$7F,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $1F,$1F,$3F,$3F,$3F,$7F,$7F,$FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $7F,$7F,$7F,$7F,$7F,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $01FF,$01FF,$01FF,$01FF,$01FF,$FF,$FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $07FF,$07FF,$03FF,$03FF,$03FF,$01FF,$01FF,$FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $1FFF,$0FFF,$0FFF,$07FF,$03FF,$03FF,$01FF,$01FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $7FFF,$3FFF,$1FFF,$0FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$0FFF,$07FF,$03FF,$FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$1FFF,$07FF,$01FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$03FF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$0FFF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$F000,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FC00,$C000,$00,$00,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FE00,$F800,$E000,$8000,$00,$00,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF00,$FC00,$F800,$F000,$C000,$8000,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF00,$FE00,$FC00,$F800,$F800,$F000,$E000
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF00,$FF00,$FE00,$FE00,$FC00,$FC00,$F800
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF80,$FF00,$FF00,$FE00,$FE00,$FE00,$FC00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF80,$FF80,$FF80,$FF00,$FF00,$FF00,$FF00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF80,$FF80,$FF80,$FFC0,$FFC0,$FFC0,$FFC0
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FF80,$FFC0,$FFC0,$FFE0,$FFE0,$FFE0,$FFF0
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFC0,$FFC0,$FFE0,$FFE0,$FFF0,$FFF8,$FFF8
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFC,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFE0,$FFF0,$FFF8,$FFFE,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFE0,$FFF8,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData16_End:


gWipeData17:
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$FFFF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$00,$0F,$FFFF,$F000,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$03,$3F,$FFFF,$FC00,$C000,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$01,$07,$1F,$7F,$FFFF,$FE00,$F800,$E000,$8000,$00,$00,$00
	dc.w $00,$00,$00,$01,$07,$0F,$1F,$7F,$FFFF,$FF00,$FC00,$F800,$F000,$C000,$8000,$00
	dc.w $01,$03,$07,$0F,$0F,$1F,$3F,$7F,$FFFF,$FF00,$FE00,$FC00,$F800,$F800,$F000,$E000
	dc.w $07,$0F,$1F,$1F,$3F,$3F,$7F,$7F,$FFFF,$FF00,$FF00,$FE00,$FE00,$FC00,$FC00,$F800
	dc.w $1F,$1F,$3F,$3F,$3F,$7F,$7F,$FF,$FFFF,$FF80,$FF00,$FF00,$FE00,$FE00,$FE00,$FC00
	dc.w $7F,$7F,$7F,$7F,$7F,$FF,$FF,$FF,$FFFF,$FF80,$FF80,$FF80,$FF00,$FF00,$FF00,$FF00
	dc.w $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FFFF,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80
	dc.w $01FF,$01FF,$01FF,$01FF,$01FF,$FF,$FF,$FF,$FFFF,$FF80,$FF80,$FF80,$FFC0,$FFC0,$FFC0,$FFC0
	dc.w $07FF,$07FF,$03FF,$03FF,$03FF,$01FF,$01FF,$FF,$FFFF,$FF80,$FFC0,$FFC0,$FFE0,$FFE0,$FFE0,$FFF0
	dc.w $1FFF,$0FFF,$0FFF,$07FF,$03FF,$03FF,$01FF,$01FF,$FFFF,$FFC0,$FFC0,$FFE0,$FFE0,$FFF0,$FFF8,$FFF8
	dc.w $7FFF,$3FFF,$1FFF,$0FFF,$0FFF,$07FF,$03FF,$01FF,$FFFF,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFC,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$0FFF,$07FF,$03FF,$FFFF,$FFE0,$FFF0,$FFF8,$FFFE,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$1FFF,$07FF,$FFFF,$FFE0,$FFF8,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$FFFF,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData17_End:


gWipeData18:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0180,$0180,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$07E0,$0420,$05A0,$05A0,$0420,$07E0,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$1FF8,$1008,$17E8,$1428,$15A8,$15A8,$1428,$17E8,$1008,$1FF8,$00,$00,$00
	dc.w $00,$7FFE,$4002,$5FFA,$500A,$57EA,$542A,$55AA,$55AA,$542A,$57EA,$500A,$5FFA,$4002,$7FFE,$00
	dc.w $00,$7FFE,$4002,$5FFA,$500A,$57EA,$57EA,$57EA,$57EA,$57EA,$57EA,$500A,$5FFA,$4002,$7FFE,$00
	dc.w $00,$7FFE,$4002,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$5FFA,$4002,$7FFE,$00
	dc.w $00,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData18_End:


gWipeData19:
	dc.w $0180,$0180,$0180,$0180,$0180,$0180,$0180,$FFFF,$FFFF,$0180,$0180,$0180,$0180,$0180,$0180,$0180
	dc.w $03C0,$03C0,$03C0,$03C0,$03C0,$03C0,$FFFF,$FFFF,$FFFF,$FFFF,$03C0,$03C0,$03C0,$03C0,$03C0,$03C0
	dc.w $07E0,$07E0,$07E0,$07E0,$07E0,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$07E0,$07E0,$07E0,$07E0,$07E0
	dc.w $0FF0,$0FF0,$0FF0,$0FF0,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$0FF0,$0FF0,$0FF0,$0FF0
	dc.w $1FF8,$1FF8,$1FF8,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$1FF8,$1FF8,$1FF8
	dc.w $3FFC,$3FFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFC,$3FFC
	dc.w $7FFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData19_End:


gWipeData20:
	dc.w $03,$0C,$30,$C0,$0300,$0C00,$3000,$C000,$C000,$3000,$0C00,$0300,$C0,$30,$0C,$03
	dc.w $07,$1C,$70,$01C0,$0700,$1C00,$7000,$C000,$C000,$7000,$1C00,$0700,$01C0,$70,$1C,$07
	dc.w $07,$1E,$78,$01E0,$0780,$1E00,$7800,$E000,$E000,$7800,$1E00,$0780,$01E0,$78,$1E,$07
	dc.w $0F,$3E,$F8,$03E0,$0780,$1F00,$7C00,$F000,$F000,$7C00,$1F00,$0780,$03E0,$F8,$3E,$0F
	dc.w $1F,$7E,$F8,$03F0,$0FC0,$1F00,$7E00,$F800,$F800,$7E00,$1F00,$0FC0,$03F0,$F8,$7E,$1F
	dc.w $3F,$FE,$01F8,$07F0,$0FC0,$1F80,$7F00,$FC00,$FC00,$7F00,$1F80,$0FC0,$07F0,$01F8,$FE,$3F
	dc.w $7F,$FE,$03FC,$07F0,$0FE0,$3FC0,$7F00,$FE00,$FE00,$7F00,$3FC0,$0FE0,$07F0,$03FC,$FE,$7F
	dc.w $FF,$01FE,$03FC,$0FF8,$1FE0,$3FC0,$7F80,$FF00,$FF00,$7F80,$3FC0,$1FE0,$0FF8,$03FC,$01FE,$FF
	dc.w $01FF,$03FE,$07FC,$0FF8,$1FF0,$3FE0,$7FC0,$FF80,$FF80,$7FC0,$3FE0,$1FF0,$0FF8,$07FC,$03FE,$01FF
	dc.w $03FF,$07FE,$0FFC,$1FF8,$1FF8,$3FF0,$7FE0,$FFC0,$FFC0,$7FE0,$3FF0,$1FF8,$1FF8,$0FFC,$07FE,$03FF
	dc.w $07FF,$0FFE,$0FFE,$1FFC,$3FF8,$7FF0,$7FF0,$FFE0,$FFE0,$7FF0,$7FF0,$3FF8,$1FFC,$0FFE,$0FFE,$07FF
	dc.w $0FFF,$1FFE,$1FFE,$3FFC,$3FFC,$7FF8,$7FF8,$FFF0,$FFF0,$7FF8,$7FF8,$3FFC,$3FFC,$1FFE,$1FFE,$0FFF
	dc.w $1FFF,$1FFF,$3FFE,$3FFE,$7FFC,$7FFC,$FFF8,$FFF8,$FFF8,$FFF8,$7FFC,$7FFC,$3FFE,$3FFE,$1FFF,$1FFF
	dc.w $3FFF,$3FFF,$7FFE,$7FFE,$7FFE,$7FFE,$FFFC,$FFFC,$FFFC,$FFFC,$7FFE,$7FFE,$7FFE,$7FFE,$3FFF,$3FFF
	dc.w $7FFF,$7FFF,$7FFF,$7FFF,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$7FFF,$7FFF,$7FFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData20_End:


gWipeData21:
	dc.w $00,$4444,$00,$1111,$00,$4444,$00,$1111,$00,$4444,$00,$1111,$00,$4444,$00,$1111
	dc.w $00,$5555,$00,$5555,$00,$5555,$00,$5555,$00,$5555,$00,$5555,$00,$5555,$00,$5555
	dc.w $8888,$5555,$2222,$5555,$8888,$5555,$2222,$5555,$8888,$5555,$2222,$5555,$8888,$5555,$2222,$5555
	dc.w $AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555
	dc.w $AAAA,$DDDD,$AAAA,$7777,$AAAA,$DDDD,$AAAA,$7777,$AAAA,$DDDD,$AAAA,$7777,$AAAA,$DDDD,$AAAA,$7777
	dc.w $AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF,$AAAA,$FFFF
	dc.w $EEEE,$FFFF,$BBBB,$FFFF,$EEEE,$FFFF,$BBBB,$FFFF,$EEEE,$FFFF,$BBBB,$FFFF,$EEEE,$FFFF,$BBBB,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData21_End:


gWipeData22:
	dc.w $3C,$66,$42,$C3,$81,$81,$81,$8100,$8100,$8100,$4200,$4200,$6600,$2400,$3C00,$1000
	dc.w $7C,$EE,$C6,$01C7,$0183,$0183,$0183,$8301,$8301,$8301,$C600,$C600,$EE00,$6C00,$7C00,$3000
	dc.w $FC,$01FE,$01CE,$03CF,$0387,$0387,$0387,$8703,$8703,$8703,$CE01,$CE01,$FE01,$FC00,$FC00,$7000
	dc.w $01FC,$03FE,$03DE,$07DF,$078F,$078F,$078F,$8F07,$8F07,$8F07,$DE03,$DE03,$FE03,$FC01,$FC01,$F000
	dc.w $03FC,$07FE,$07FE,$0FFF,$0F9F,$0F9F,$0F9F,$9F0F,$9F0F,$9F0F,$FE07,$FE07,$FE07,$FC03,$FC03,$F000
	dc.w $07FC,$0FFE,$0FFE,$0FFF,$1FBF,$1FBF,$1FBF,$BF1F,$BF1F,$BF1F,$FE0F,$FE0F,$FE0F,$FC07,$FC07,$F001
	dc.w $0FFC,$1FFE,$1FFE,$1FFF,$3FFF,$3FFF,$3FFF,$FF3F,$FF3F,$FF3F,$FE1F,$FE1F,$FE1F,$FC0F,$FC0F,$F003
	dc.w $1FFC,$3FFE,$3FFE,$3FFF,$7FFF,$7FFF,$7FFF,$FF7F,$FF7F,$FF7F,$FE3F,$FE3F,$FE3F,$FC1F,$FC1F,$F007
	dc.w $3FFC,$7FFE,$7FFE,$7FFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FE7F,$FE7F,$FE7F,$FC3F,$FC3F,$F00F
	dc.w $7FFC,$FFFE,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FEFF,$FEFF,$FEFF,$FC7F,$FC7F,$F01F
	dc.w $FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FCFF,$FCFF,$F03F
	dc.w $FFFD,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FDFF,$FDFF,$F07F
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$F0FF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$F1FF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$F3FF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$F7FF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData22_End:


gWipeData23:
	dc.w $00,$00,$00,$00,$00,$00,$00,$FFFF,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$7E7E,$FFFF,$FFFF,$7EFE,$1018,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$3C3C,$7E7E,$FFFF,$FFFF,$FFFF,$FFFF,$7E7E,$3C3C,$1008,$00,$00,$00,$00
	dc.w $00,$3C3C,$7E7E,$7E7E,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7E7E,$7E7E,$3C3C,$1008,$00,$00
	dc.w $3C3C,$7E7E,$7E7E,$7EFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFF,$7E7E,$7E7E,$3C3C,$3C3C,$1008
	dc.w $7E7E,$7E7E,$7EFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFE,$7E7E,$7E7E,$3C7E,$3C3C
	dc.w $7E7E,$7E7E,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFE,$7E7E,$7E7E,$7E7E
	dc.w $7EFE,$7EFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFF,$7EFE,$7E7E,$7E7E
	dc.w $7EFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFF,$7EFE,$7E7E
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFE,$7EFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFF,$7EFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7EFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData23_End:


gWipeData24:
	dc.w $00,$00,$00,$00,$00,$00,$00,$FF0F,$FFFF,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$F003,$FF3F,$FFFF,$F00F,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$C003,$FC0F,$FF3F,$FFFF,$FC3F,$C003,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$C001,$F007,$FC1F,$FF7F,$FFFF,$FC3F,$F00F,$C003,$00,$00,$00,$00
	dc.w $00,$00,$00,$8001,$E007,$F80F,$FE1F,$FF7F,$FFFF,$FE7F,$F81F,$E007,$8001,$00,$00,$00
	dc.w $00,$00,$8001,$E003,$F007,$F81F,$FE3F,$FF7F,$FFFF,$FE7F,$F81F,$F00F,$E007,$8001,$00,$00
	dc.w $00,$8001,$C003,$E007,$F80F,$FC1F,$FE3F,$FF7F,$FFFF,$FE7F,$FC3F,$F81F,$E007,$C003,$8001,$00
	dc.w $8001,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F,$FFFF,$FFFF,$FE7F,$FC3F,$F81F,$F00F,$E007,$C003,$8001
	dc.w $C003,$E007,$F00F,$F81F,$F81F,$FC3F,$FE7F,$FFFF,$FFFF,$FE7F,$FC3F,$F81F,$F81F,$F00F,$E007,$C003
	dc.w $E007,$F00F,$F00F,$F81F,$FC3F,$FE7F,$FE7F,$FFFF,$FFFF,$FE7F,$FE7F,$FC3F,$F81F,$F00F,$F00F,$E007
	dc.w $F00F,$F81F,$F81F,$FC3F,$FC3F,$FE7F,$FE7F,$FFFF,$FFFF,$FE7F,$FE7F,$FC3F,$FC3F,$F81F,$F81F,$F00F
	dc.w $F81F,$F81F,$FC3F,$FC3F,$FE7F,$FE7F,$FFFF,$FFFF,$FFFF,$FFFF,$FE7F,$FE7F,$FC3F,$FC3F,$F81F,$F81F
	dc.w $FC3F,$FC3F,$FE7F,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF,$FFFF,$FFFF,$FE7F,$FE7F,$FE7F,$FE7F,$FC3F,$FC3F
	dc.w $FE7F,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FE7F,$FE7F,$FE7F,$FE7F
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData24_End:


gWipeData25:
	dc.w $8000,$4000,$2000,$1000,$0800,$0400,$0200,$0100,$80,$40,$20,$10,$08,$04,$02,$01
	dc.w $8000,$4000,$2000,$1000,$0800,$0400,$0200,$0180,$01C0,$E0,$70,$38,$1C,$0E,$07,$03
	dc.w $8000,$4000,$2000,$1800,$1C00,$0E00,$0700,$0380,$01C0,$E0,$78,$3C,$3E,$1F,$0F,$07
	dc.w $8000,$4000,$3000,$3800,$1C00,$0E00,$0780,$03C0,$03E0,$01F0,$FC,$7E,$3F,$3F,$1F,$0F
	dc.w $8000,$6000,$7000,$3800,$1E00,$0F00,$0F80,$07E0,$03F0,$01F8,$01FE,$FF,$7F,$3F,$3F,$1F
	dc.w $8000,$6000,$7000,$3C00,$1E00,$1F80,$0FC0,$07F0,$07F8,$03FE,$01FF,$01FF,$FF,$7F,$7F,$3F
	dc.w $8000,$6000,$7800,$3C00,$3F00,$1FC0,$0FE0,$0FF8,$07FE,$07FF,$03FF,$01FF,$01FF,$FF,$FF,$7F
	dc.w $8000,$6000,$7800,$3E00,$3F80,$1FE0,$1FF8,$0FFE,$0FFF,$07FF,$07FF,$03FF,$03FF,$01FF,$01FF,$FF
	dc.w $C000,$F000,$7C00,$7F00,$3FC0,$3FF0,$1FFC,$1FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$03FF,$01FF,$01FF
	dc.w $C000,$F000,$7E00,$7F80,$3FF0,$3FFC,$3FFF,$1FFF,$1FFF,$0FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$03FF
	dc.w $C000,$F800,$7F00,$7FE0,$7FFC,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF,$0FFF,$0FFF,$0FFF,$07FF,$07FF
	dc.w $C000,$FC00,$7FC0,$7FFC,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF,$1FFF,$0FFF,$0FFF
	dc.w $E000,$FF00,$FFF8,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF
	dc.w $F000,$FFF0,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF
	dc.w $FF00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData25_End:


gWipeData26:
	dc.w $8000,$4000,$2000,$1000,$0800,$0400,$0200,$0100,$80,$40,$20,$10,$08,$04,$02,$01
	dc.w $C000,$E000,$7000,$3800,$1C00,$0E00,$0700,$0380,$01C0,$E0,$70,$38,$1C,$0E,$07,$03
	dc.w $E000,$F000,$F800,$7C00,$3C00,$1E00,$0700,$0380,$01C0,$E0,$78,$3C,$3E,$1F,$0F,$07
	dc.w $F000,$F800,$FC00,$FC00,$7E00,$3F00,$0F80,$07C0,$03E0,$01F0,$FC,$7E,$3F,$3F,$1F,$0F
	dc.w $F800,$FC00,$FC00,$FE00,$FF00,$7F80,$1F80,$0FE0,$07F0,$01F8,$01FE,$FF,$7F,$3F,$3F,$1F
	dc.w $FC00,$FE00,$FE00,$FF00,$FF80,$FF80,$7FC0,$1FF0,$0FF8,$03FE,$01FF,$01FF,$FF,$7F,$7F,$3F
	dc.w $FE00,$FF00,$FF00,$FF80,$FF80,$FFC0,$FFE0,$7FF8,$1FFE,$07FF,$03FF,$01FF,$01FF,$FF,$FF,$7F
	dc.w $FF00,$FF80,$FF80,$FFC0,$FFC0,$FFE0,$FFF8,$FFFE,$7FFF,$1FFF,$07FF,$03FF,$03FF,$01FF,$01FF,$FF
	dc.w $FF80,$FF80,$FFC0,$FFC0,$FFE0,$FFF0,$FFFC,$FFFF,$FFFF,$3FFF,$0FFF,$07FF,$03FF,$03FF,$01FF,$01FF
	dc.w $FFC0,$FFC0,$FFE0,$FFE0,$FFF0,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$0FFF,$07FF,$07FF,$03FF,$03FF
	dc.w $FFE0,$FFE0,$FFF0,$FFF0,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$0FFF,$0FFF,$07FF,$07FF
	dc.w $FFF0,$FFF0,$FFF8,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$1FFF,$0FFF,$0FFF
	dc.w $FFF8,$FFF8,$FFF8,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$1FFF,$1FFF,$1FFF
	dc.w $FFFC,$FFFC,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$3FFF,$3FFF
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData26_End:


gWipeData27:
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$F00F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$C003,$FC3F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$C003,$F00F,$FC3F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8001,$E007,$F81F,$FE7F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$8001,$E007,$F00F,$F81F,$FE7F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$8001,$C003,$E007,$F81F,$FC3F,$FE7F,$FFFF
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$8001,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F,$FFFF
	dc.w $8001,$8001,$8001,$8001,$8001,$8001,$8001,$8001,$8001,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F,$FFFF
	dc.w $C003,$C003,$C003,$C003,$C003,$C003,$C003,$C003,$C003,$E007,$F00F,$F81F,$F81F,$FC3F,$FE7F,$FFFF
	dc.w $E007,$E007,$E007,$E007,$E007,$E007,$E007,$E007,$E007,$F00F,$F00F,$F81F,$FC3F,$FE7F,$FE7F,$FFFF
	dc.w $F00F,$F00F,$F00F,$F00F,$F00F,$F00F,$F00F,$F00F,$F00F,$F81F,$F81F,$FC3F,$FC3F,$FE7F,$FE7F,$FFFF
	dc.w $F81F,$F81F,$F81F,$F81F,$F81F,$F81F,$F81F,$F81F,$F81F,$F81F,$FC3F,$FC3F,$FE7F,$FE7F,$FFFF,$FFFF
	dc.w $FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FC3F,$FE7F,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF
	dc.w $FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData27_End:


gWipeData28:
	dc.w $FFFF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$FF,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$0FFF,$0F,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$1FFF,$FF,$07,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$3FFF,$03FF,$3F,$03,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$3FFF,$07FF,$FF,$1F,$03,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$3FFF,$0FFF,$01FF,$7F,$0F,$03,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$3FFF,$0FFF,$03FF,$FF,$3F,$0F,$03,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$7FFF,$1FFF,$07FF,$01FF,$7F,$1F,$07,$01,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$7FFF,$1FFF,$07FF,$03FF,$FF,$3F,$1F,$07,$01,$00,$00,$00,$00,$00,$00
	dc.w $FFFF,$7FFF,$1FFF,$0FFF,$03FF,$01FF,$7F,$3F,$0F,$07,$01,$00,$00,$00,$00,$00
	dc.w $FFFF,$7FFF,$1FFF,$0FFF,$07FF,$01FF,$FF,$7F,$1F,$0F,$07,$01,$00,$00,$00,$00
	dc.w $FFFF,$7FFF,$3FFF,$0FFF,$07FF,$03FF,$01FF,$7F,$3F,$1F,$0F,$03,$01,$00,$00,$00
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$07FF,$03FF,$01FF,$FF,$7F,$3F,$1F,$07,$03,$01,$00,$00
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$7F,$3F,$1F,$0F,$07,$03,$01,$00
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$3F,$1F,$0F,$07,$03,$01,$00
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$7F,$3F,$1F,$0F,$07,$03,$01
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$01FF,$FF,$7F,$3F,$1F,$0F,$07,$03
	dc.w $FFFF,$7FFF,$3FFF,$1FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$7F,$3F,$3F,$1F,$0F,$07
	dc.w $FFFF,$7FFF,$3FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$03FF,$01FF,$FF,$7F,$3F,$3F,$1F,$0F
	dc.w $FFFF,$7FFF,$7FFF,$3FFF,$1FFF,$0FFF,$0FFF,$07FF,$03FF,$01FF,$01FF,$FF,$7F,$3F,$3F,$1F
	dc.w $FFFF,$7FFF,$7FFF,$3FFF,$1FFF,$1FFF,$0FFF,$07FF,$07FF,$03FF,$01FF,$01FF,$FF,$7F,$7F,$3F
	dc.w $FFFF,$7FFF,$7FFF,$3FFF,$3FFF,$1FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$01FF,$01FF,$FF,$FF,$7F
	dc.w $FFFF,$7FFF,$7FFF,$3FFF,$3FFF,$1FFF,$1FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$03FF,$01FF,$01FF,$FF
	dc.w $FFFF,$FFFF,$7FFF,$7FFF,$3FFF,$3FFF,$1FFF,$1FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$03FF,$01FF,$01FF
	dc.w $FFFF,$FFFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$0FFF,$0FFF,$0FFF,$07FF,$07FF,$03FF,$03FF
	dc.w $FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF,$0FFF,$0FFF,$0FFF,$07FF,$07FF
	dc.w $FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF,$1FFF,$0FFF,$0FFF
	dc.w $FFFF,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF,$3FFF,$1FFF,$1FFF,$1FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FFF,$3FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData28_End:


gWipeData29:
	dc.w $00,$00,$00,$00,$00,$00,$00,$8000,$01,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$8000,$C000,$03,$01,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$8000,$C000,$E000,$07,$03,$01,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$8000,$C000,$E000,$F000,$0F,$07,$03,$01,$00,$00,$00,$00
	dc.w $00,$00,$00,$8000,$C000,$E000,$F000,$F800,$1F,$0F,$07,$03,$01,$00,$00,$00
	dc.w $00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$3F,$1F,$0F,$07,$03,$01,$00,$00
	dc.w $00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$7F,$3F,$1F,$0F,$07,$03,$01,$00
	dc.w $8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF,$7F,$3F,$1F,$0F,$07,$03,$01
	dc.w $C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$01FF,$FF,$7F,$3F,$1F,$0F,$07,$03
	dc.w $E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$03FF,$01FF,$FF,$7F,$3F,$1F,$0F,$07
	dc.w $F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$07FF,$03FF,$01FF,$FF,$7F,$3F,$1F,$0F
	dc.w $F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$0FFF,$07FF,$03FF,$01FF,$FF,$7F,$3F,$1F
	dc.w $FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$7F,$3F
	dc.w $FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF,$7F
	dc.w $FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF,$FF
	dc.w $FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF,$01FF
	dc.w $FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF,$03FF
	dc.w $FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF,$07FF
	dc.w $FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$0FFF
	dc.w $FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF
	dc.w $FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF,$3FFF
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$7FFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData29_End:


gWipeData30:
	dc.w $00,$00,$00,$00,$00,$00,$00,$8000,$8000,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$8000,$C000,$C000,$8000,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$8000,$C000,$E000,$E000,$C000,$8000,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$8000,$C000,$E000,$F000,$F000,$E000,$C000,$8000,$00,$00,$00,$00
	dc.w $00,$00,$00,$8000,$C000,$E000,$F000,$F800,$F800,$F000,$E000,$C000,$8000,$00,$00,$00
	dc.w $00,$00,$8000,$C000,$E000,$F000,$F800,$FC00,$FC00,$F800,$F000,$E000,$C000,$8000,$00,$00
	dc.w $00,$8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FE00,$FC00,$F800,$F000,$E000,$C000,$8000,$00
	dc.w $8000,$C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF00,$FE00,$FC00,$F800,$F000,$E000,$C000,$8000
	dc.w $C000,$E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FF80,$FF00,$FE00,$FC00,$F800,$F000,$E000,$C000
	dc.w $E000,$F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFC0,$FF80,$FF00,$FE00,$FC00,$F800,$F000,$E000
	dc.w $F000,$F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFE0,$FFC0,$FF80,$FF00,$FE00,$FC00,$F800,$F000
	dc.w $F800,$FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF0,$FFE0,$FFC0,$FF80,$FF00,$FE00,$FC00,$F800
	dc.w $FC00,$FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFF8,$FFF0,$FFE0,$FFC0,$FF80,$FF00,$FE00,$FC00
	dc.w $FE00,$FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFC,$FFF8,$FFF0,$FFE0,$FFC0,$FF80,$FF00,$FE00
	dc.w $FF00,$FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFE,$FFFC,$FFF8,$FFF0,$FFE0,$FFC0,$FF80,$FF00
	dc.w $FF80,$FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFE,$FFFC,$FFF8,$FFF0,$FFE0,$FFC0,$FF80
	dc.w $FFC0,$FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$FFFC,$FFF8,$FFF0,$FFE0,$FFC0
	dc.w $FFE0,$FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$FFFC,$FFF8,$FFF0,$FFE0
	dc.w $FFF0,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$FFFC,$FFF8,$FFF0
	dc.w $FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$FFFC,$FFF8
	dc.w $FFFC,$FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE,$FFFC
	dc.w $FFFE,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFE
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData30_End:


gWipeData31:
	dc.w $8001,$4002,$2004,$1008,$0810,$0420,$0240,$FFFF,$0180,$03C0,$05A0,$0990,$1188,$2184,$4182,$8181
	dc.w $C003,$6006,$300C,$1818,$0810,$0420,$F24F,$FFFF,$0180,$03C0,$05A0,$1998,$33CC,$63C6,$C3C3,$83C1
	dc.w $E007,$700E,$300C,$1818,$0C30,$C663,$FE7F,$FFFF,$0180,$07E0,$0FF0,$1BD8,$73CE,$E3C7,$C7E3,$87E1
	dc.w $F00F,$781E,$381C,$1C38,$CC33,$F66F,$FE7F,$FFFF,$0180,$07E0,$1FF8,$7BDE,$F7EF,$E7E7,$CFF3,$8FF1
	dc.w $F81F,$781E,$3C3C,$9C39,$EE77,$FE7F,$FFFF,$FFFF,$03C0,$0FF0,$3FFC,$FFFF,$F7EF,$EFF7,$CFF3,$9FF9
	dc.w $FC3F,$7C3E,$BE7D,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF,$03C0,$3FFC,$FFFF,$FFFF,$FFFF,$FFFF,$DFFB,$BFFD
	dc.w $FE7F,$FE7F,$FE7F,$FE7F,$FFFF,$FFFF,$FFFF,$FFFF,$0FF0,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData31_End:


gWipeData32:
	dc.w $00,$00,$00,$00,$00,$00,$00,$0180,$0180,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$0180,$03C0,$03C0,$0180,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$0180,$03C0,$07E0,$07E0,$03C0,$0180,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0180,$03C0,$07E0,$0FF0,$0FF0,$07E0,$03C0,$0180,$00,$00,$00,$00
	dc.w $00,$00,$00,$0180,$03C0,$07E0,$0FF0,$1FF8,$1FF8,$0FF0,$07E0,$03C0,$0180,$00,$00,$00
	dc.w $00,$00,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$00,$00
	dc.w $00,$0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180,$00
	dc.w $0180,$03C0,$07E0,$0FF0,$1FF8,$3FFC,$7FFE,$FFFF,$FFFF,$7FFE,$3FFC,$1FF8,$0FF0,$07E0,$03C0,$0180
	dc.w $01C0,$03E0,$07E0,$0FF0,$1FF8,$7FFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFE,$1FF8,$0FF0,$07E0,$07C0,$0380
	dc.w $01E0,$03F0,$07F0,$1FF8,$7FF8,$FFFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFF,$1FFC,$0FF0,$0FE0,$07C0,$0780
	dc.w $01F0,$03F0,$07F8,$3FF8,$FFF8,$FFFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFF,$1FFF,$1FFC,$1FE0,$0FC0,$0F80
	dc.w $01F8,$03F8,$3FFC,$FFFC,$FFFC,$FFFC,$FFFE,$FFFF,$FFFF,$7FFF,$3FFF,$3FFF,$3FFF,$1FF8,$1FC0,$1F80
	dc.w $01FC,$0FFC,$FFFC,$FFFC,$FFFE,$FFFE,$FFFE,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$3FFF,$3FFF,$3FF0,$3F80
	dc.w $01FE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFF,$FFFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7FFF,$7F80
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData32_End:


gWipeData33:
	dc.w $FFFF,$8001,$8001,$8001,$8001,$8001,$8001,$8181,$8181,$8001,$8001,$8001,$8001,$8001,$8001,$FFFF
	dc.w $FFFF,$FFFF,$C003,$C003,$C003,$C003,$C3C3,$C3C3,$C3C3,$C3C3,$C003,$C003,$C003,$C003,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$E007,$E007,$E7E7,$E7E7,$E7E7,$E7E7,$E7E7,$E7E7,$E007,$E007,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData33_End:


gWipeData34:
	dc.w $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$00,$03C0,$07C0,$0780,$00,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$00,$0380,$07E0,$07C0,$0FC0,$0380,$00,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$00,$0780,$07C0,$0FF0,$0FE0,$1FE0,$07C0,$03C0,$00,$00,$00,$00,$00
	dc.w $00,$00,$00,$0700,$0FE0,$0FF8,$1FF8,$1FF0,$3FF0,$3FE0,$0FE0,$01C0,$00,$00,$00,$00
	dc.w $00,$00,$0F00,$1FE0,$1FFC,$1FFC,$3FF8,$3FF8,$3FF8,$7FF0,$7FF0,$0FF0,$01E0,$00,$00,$00
	dc.w $00,$1F00,$3FE0,$3FFE,$3FFE,$3FFC,$7FFC,$7FFC,$7FFC,$7FF8,$FFF8,$FFF8,$0FF8,$01F0,$00,$00
	dc.w $00,$3F80,$3FFE,$3FFE,$7FFE,$7FFC,$7FFC,$7FFC,$7FFC,$7FFC,$FFFC,$FFF8,$FFF8,$03F8,$00,$00
	dc.w $00,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$FFFE,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$00,$00
	dc.w $00,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData34_End:


gWipeData35:
	dc.w $8001,$4000,$2000,$1000,$0800,$0400,$0200,$0100,$80,$40,$20,$10,$08,$04,$02,$8001
	dc.w $C003,$E001,$7000,$3800,$1C00,$0E00,$0700,$0380,$01C0,$E0,$70,$38,$1C,$0E,$8007,$C003
	dc.w $E007,$F003,$F801,$7C00,$3E00,$1F00,$0F80,$07C0,$03E0,$01F0,$F8,$7C,$3E,$801F,$C00F,$E007
	dc.w $F00F,$F807,$FC03,$FE01,$7F00,$3F80,$1FC0,$0FE0,$07F0,$03F8,$01FC,$FE,$807F,$C03F,$E01F,$F00F
	dc.w $F81F,$FC0F,$FE07,$FF03,$FF81,$7FC0,$3FE0,$1FF0,$0FF8,$07FC,$03FE,$81FF,$C0FF,$E07F,$F03F,$F81F
	dc.w $FC3F,$FE1F,$FF0F,$FF87,$FFC3,$FFE1,$7FF0,$3FF8,$1FFC,$0FFE,$87FF,$C3FF,$E1FF,$F0FF,$F87F,$FC3F
	dc.w $FE7F,$FF3F,$FF9F,$FFCF,$FFE7,$FFF3,$FFF9,$7FFC,$3FFE,$9FFF,$CFFF,$E7FF,$F3FF,$F9FF,$FCFF,$FE7F
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData35_End:


gWipeData36:
	dc.w $8001,$4002,$2004,$1008,$0810,$0420,$0240,$0180,$0180,$0240,$0420,$0810,$1008,$2004,$4002,$8001
	dc.w $8001,$4002,$2004,$1818,$1C38,$0E70,$07E0,$03C0,$03C0,$07E0,$0E70,$1C38,$1818,$2004,$4002,$8001
	dc.w $8001,$6006,$700E,$381C,$1E78,$0FF0,$0FF0,$07E0,$07E0,$0FF0,$0FF0,$1E78,$381C,$700E,$6006,$8001
	dc.w $8001,$6006,$781E,$3E7C,$3FFC,$1FF8,$1FF8,$0FF0,$0FF0,$1FF8,$1FF8,$3FFC,$3E7C,$781E,$6006,$8001
	dc.w $C003,$F00F,$7C3E,$7FFE,$3FFC,$3FFC,$1FF8,$1FF8,$1FF8,$1FF8,$3FFC,$3FFC,$7FFE,$7C3E,$F00F,$C003
	dc.w $C003,$FC3F,$7FFE,$7FFE,$7FFE,$7FFE,$3FFC,$3FFC,$3FFC,$3FFC,$7FFE,$7FFE,$7FFE,$7FFE,$FC3F,$C003
	dc.w $F00F,$FFFF,$FFFF,$FFFF,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$7FFE,$FFFF,$FFFF,$FFFF,$F00F
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData36_End:

gWipeData37:
	dc.w $8000,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $C000,$C000,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $E000,$E000,$E000,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $F000,$F000,$F000,$F000,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $F800,$F800,$F800,$F800,$F800,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FC00,$FC00,$FC00,$FC00,$FC00,$FC00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$FE00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$FF00,$00,$00,$00,$00,$00,$00,$00,$00
	dc.w $FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$FF80,$00,$00,$00,$00,$00,$00,$00
	dc.w $FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$FFC0,$00,$00,$00,$00,$00,$00
	dc.w $FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$FFE0,$00,$00,$00,$00,$00
	dc.w $FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$FFF0,$00,$00,$00,$00
	dc.w $FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$FFF8,$00,$00,$00
	dc.w $FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$FFFC,$00,$00
	dc.w $FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$FFFE,$00
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData37_End:


gWipeData38:
	dc.w $FE7F,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$FE7F
	dc.w $FE7F,$1C38,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$1C38,$FE7F
	dc.w $FE7F,$7C3E,$1818,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$1818,$7C3E,$FE7F
	dc.w $FE7F,$7C3E,$381C,$1008,$00,$00,$00,$00,$00,$00,$00,$00,$1008,$381C,$7C3E,$FE7F
	dc.w $FE7F,$7C3E,$781E,$300C,$2004,$00,$00,$00,$00,$00,$00,$2004,$300C,$781E,$7C3E,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$700E,$6006,$4002,$00,$00,$00,$00,$4002,$6006,$700E,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F00F,$E007,$C003,$8001,$00,$00,$8001,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F00F,$E007,$C003,$8001,$0180,$0180,$8001,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F00F,$E007,$C003,$8181,$03C0,$03C0,$8181,$C003,$E007,$F00F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F00F,$E007,$C183,$83C1,$07E0,$07E0,$83C1,$C183,$E007,$F00F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F00F,$E187,$C3C3,$87E1,$0FF0,$0FF0,$87E1,$C3C3,$E187,$F00F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F81F,$F18F,$E3C7,$C7E3,$8FF1,$1FF8,$1FF8,$8FF1,$C7E3,$E3C7,$F18F,$F81F,$FC3F,$FE7F
	dc.w $FE7F,$FC3F,$F99F,$F3CF,$E7E7,$CFF3,$9FF9,$3FFC,$3FFC,$9FF9,$CFF3,$E7E7,$F3CF,$F99F,$FC3F,$FE7F
	dc.w $FE7F,$FDBF,$FBDF,$F7EF,$EFF7,$DFFB,$BFFD,$7FFE,$7FFE,$BFFD,$DFFB,$EFF7,$F7EF,$FBDF,$FDBF,$FE7F
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData38_End:


gWipeData39:
	dc.w $8001,$4002,$2004,$1008,$0810,$0420,$0240,$0180,$0180,$0240,$0420,$0810,$1008,$2004,$4002,$8001
	dc.w $8003,$C006,$600C,$3018,$1810,$0420,$0240,$0180,$0180,$0240,$0420,$0818,$180C,$3006,$6003,$C001
	dc.w $8007,$C00E,$E00C,$7018,$1830,$0C60,$0640,$0180,$0180,$0260,$0630,$0C18,$180E,$3007,$7003,$E001
	dc.w $800F,$C01E,$E01C,$F038,$7830,$1C60,$06C0,$0380,$01C0,$0360,$0638,$0C1E,$1C0F,$3807,$7803,$F001
	dc.w $801F,$C01E,$E03C,$F038,$F870,$3C60,$0EC0,$0380,$01C0,$0370,$063C,$0E1F,$1C0F,$3C07,$7803,$F801
	dc.w $803F,$C03E,$E07C,$F078,$F870,$FCE0,$3EC0,$0780,$01E0,$037C,$073F,$0E1F,$1E0F,$3E07,$7C03,$FC01
	dc.w $807F,$C07E,$E07C,$F0F8,$F8F0,$FCE0,$FEC0,$1F80,$01F8,$037F,$073F,$0F1F,$1F0F,$3E07,$7E03,$FE01
	dc.w $80FF,$C0FE,$E0FC,$F0F8,$F8F0,$FCE0,$FEC0,$FF80,$01FF,$037F,$073F,$0F1F,$1F0F,$3F07,$7F03,$FF01
	dc.w $81FF,$C1FE,$E1FC,$F1F8,$F9F0,$FDE0,$FFC0,$FFFF,$FFFF,$03FF,$07BF,$0F9F,$1F8F,$3F87,$7F83,$FF81
	dc.w $83FF,$C3FE,$E3FC,$F1F8,$F9F0,$FDE0,$FFC7,$FFFF,$FFFF,$E3FF,$07BF,$0F9F,$1F8F,$3FC7,$7FC3,$FFC1
	dc.w $87FF,$C7FE,$E3FC,$F3F8,$FBF0,$FDE3,$FFDF,$FFFF,$FFFF,$FBFF,$C7BF,$0FDF,$1FCF,$3FC7,$7FE3,$FFE1
	dc.w $8FFF,$CFFE,$E7FC,$F7F8,$FBF3,$FFEF,$FFFF,$FFFF,$FFFF,$FFFF,$F7FF,$CFDF,$1FEF,$3FE7,$7FF3,$FFF1
	dc.w $9FFF,$CFFE,$EFFC,$F7F9,$FFF7,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$EFFF,$9FEF,$3FF7,$7FF3,$FFF9
	dc.w $BFFF,$DFFE,$FFFD,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$BFFF,$7FFB,$FFFD
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
	dc.w $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
gWipeData39_End:


gWipeFrameCounts:
	dc.w	(gWipeData1_End-gWipeData1)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData2_End-gWipeData2)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData3_End-gWipeData3)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData4_End-gWipeData4)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData5_End-gWipeData5)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData6_End-gWipeData6)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData7_End-gWipeData7)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData8_End-gWipeData8)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData9_End-gWipeData9)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData10_End-gWipeData10)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData11_End-gWipeData11)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData12_End-gWipeData12)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData13_End-gWipeData13)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData14_End-gWipeData14)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData15_End-gWipeData15)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData16_End-gWipeData16)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData17_End-gWipeData17)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData18_End-gWipeData18)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData19_End-gWipeData19)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData20_End-gWipeData20)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData21_End-gWipeData21)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData22_End-gWipeData22)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData23_End-gWipeData23)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData24_End-gWipeData24)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData25_End-gWipeData25)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData26_End-gWipeData26)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData27_End-gWipeData27)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData28_End-gWipeData28)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData29_End-gWipeData29)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData30_End-gWipeData30)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData31_End-gWipeData31)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData32_End-gWipeData32)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData33_End-gWipeData33)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData34_End-gWipeData34)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData35_End-gWipeData35)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData36_End-gWipeData36)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData37_End-gWipeData37)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData38_End-gWipeData38)/eWIPE_CHUNK_SIZE
	dc.w	(gWipeData39_End-gWipeData39)/eWIPE_CHUNK_SIZE
