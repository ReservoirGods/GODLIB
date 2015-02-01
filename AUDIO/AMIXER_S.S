**************************************************************************************
*	AMIXER_S.S
*
*	AUDIO mixer functions
*
*	[c] 2001 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	AudioMixer_Vbl

	export	gpAudioMixerBuffer
	export	gpAudioMixerSilence
	export	gpAudioMixerMulTable
	export	gAudioMixerLockFlag
	export	gAudioMixerBufferOffset
	export	gAudioMixerSamples
	export	gAudioMixerEnableFlag

	import	gAudioMixerSineLaw

**************************************************************************************
;	EQUATES
**************************************************************************************

eAMIXER_BUFFER_SIZE		EQU	(8*1024)
eAMIXER_PLAY_OFFSET		EQU	(6*1024)
eAMIXER_CHANNEL_LIMIT	EQU	2


**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET

sAmixerSpl_mpStart:		ds.l	1
sAmixerSpl_mpCurrent:	ds.l	1
sAmixerSpl_mLength:		ds.l	1
sAmixerSpl_mGainLeft:	ds.b	1
sAmixerSpl_mGainRight:	ds.b	1
sAmixerSpl_mVolume:		ds.b	1
sAmixerSpl_mActiveFlag:	ds.b	1
sAmixerSpl_msizeof:		ds.b	1


**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : Audio_MixerVbl
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_Vbl:
;	not.w	$ffff8240.w
	tas		gAudioMixerLockFlag			; already in a mixer interrupt?
	bne		.locked						; yes, don't try further mixing

	movem.l	d0-a6,-(a7)					; save registers

	lea		gAudioMixerSamples,a0
	lea		sAmixerSpl_msizeof(a0),a1

	move.l	gpAudioMixerMulTable,a2
	moveq	#0,d0
	move.b	sAmixerSpl_mGainLeft(a0),d0
	lsl.w	#8,d0
	lea		(a2,d0.l),a3				; gain left
	moveq	#0,d0
	move.b	sAmixerSpl_mGainRight(a0),d0
	lsl.w	#8,d0
	lea		(a2,d0.l),a4				; gain right

	moveq	#0,d0
	move.b	sAmixerSpl_mGainLeft(a1),d0
	lsl.w	#8,d0
	lea		(a2,d0.l),a5				; gain left
	moveq	#0,d0
	move.b	sAmixerSpl_mGainRight(a1),d0
	lsl.w	#8,d0
	lea		(a2,d0.l),a6				; gain right

	moveq	#0,d3
	move.w	d3,d4

	moveq	#0,d0
	moveq	#0,d1
	move.b	sAmixerSpl_mGainLeft(a0),d0
	move.b	sAmixerSpl_mGainRight(a0),d1
	cmp.w	#96,d0
	blt		.noL0
	or.w	#$FF00,d3
.noL0:
	cmp.w	#96,d1
	blt		.noR0
	or.w	#$00FF,d3
.noR0:

	moveq	#0,d0
	moveq	#0,d1
	move.b	sAmixerSpl_mGainLeft(a1),d0
	move.b	sAmixerSpl_mGainRight(a1),d1
	cmp.w	#96,d0
	blt		.noL1
	or.w	#$FF00,d4
.noL1:
	cmp.w	#96,d1
	blt		.noR1
	or.w	#$00FF,d4
.noR1:

	lea		AudioMixer_DoMixingO,a3
	move.w	d3,d0
	eor.w	d4,d0
	bne.s	.mixO
	lea		AudioMixer_DoMixingI,a3
.mixO:


	move.l	sAmixerSpl_mpCurrent(a0),a0
	move.l	sAmixerSpl_mpCurrent(a1),a1

	movea.w	#$8909,a2					; dma sound frame ptr
	movep.l	(a2),d7						; read address
	lsr.l	#8,d7						; ignore frame end address high byte

	move.l	gpAudioMixerBuffer,a2			; start of mixing buffer
	sub.l	a2,d7							; current h/w offset into buffer
	move.l	gAudioMixerBufferOffset,d6		; end of last s/w mix
	and.l	#(eAMIXER_BUFFER_SIZE-1),d6		; clip to buffer size
	lea		(a2,d6.l),a2					; get to place in buffer

	and.l	#(eAMIXER_BUFFER_SIZE-1)&$FFFFFFF8,d7	; offset moves in steps of 8
	move.l	d7,gAudioMixerBufferOffset				; end of new s/w mix
	sub.l	d6,d7									; mix length
	bpl.s	.lpls									; do straight linear mix

	move.l	#(eAMIXER_BUFFER_SIZE),d0
	add.l	d0,d7
	sub.l	d6,d0
;	bsr		AudioMixer_DoMixingW
	jsr		(a3)

	move.l	gpAudioMixerBuffer,a2
	move.l	gAudioMixerBufferOffset,d0
;	bsr		AudioMixer_DoMixingW
	jsr		(a3)
	bra.s	.update
.lpls:
	move.l	d7,d0
;	bsr		AudioMixer_DoMixingW
	jsr		(a3)

.update:
	move.l	d7,d0
	bsr		AudioMixer_UpdateSamples

	clr.b	gAudioMixerLockFlag			; signal end of mixing
	movem.l	(a7)+,d0-a6					; restore registers

.locked:
;	not.w	$ffff8240.w
	rts

*------------------------------------------------------------------------------------*
* FUNCTION : Audio_DoMixingI( U8 * apSpl0, U8 * apSpl1, U8 * apBuffer, U32 aBytes )
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_DoMixingI:

	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:

	move.w	(a0)+,d1					; byte of sample0
	move.w	(a1)+,d2					; byte of sample1
	and.w	d3,d1
	and.w	d4,d2

	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d2,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d2,(a2)+					; write L.R into buffer

	dbra	d0,.loop

.nomix:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : Audio_DoMixingI( U8 * apSpl0, U8 * apSpl1, U8 * apBuffer, U32 aBytes )
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_DoMixingO:

	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:

	move.w	(a0)+,d1					; byte of sample0
	move.w	(a1)+,d2					; byte of sample1
	and.w	d3,d1
	and.w	d4,d2
	or.w	d2,d1

	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer

	dbra	d0,.loop

.nomix:
	rts

*------------------------------------------------------------------------------------*
* FUNCTION : Audio_DoMixing( U8 * apSpl0, U8 * apSpl1, U8 * apBuffer, U32 aBytes )
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_DoMixing:

	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:

	move.b	(a0)+,d1					; byte of sample0
	lsl.w	#8,d1						; into top byte
	move.b	(a1)+,d1					; byte of sample1

	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer

	dbra	d0,.loop

.nomix:
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_DoMixing2:( U8 * apSpl0, U8 * apSpl1, U8 * apBuffer, U32 aBytes )
* ACTION   : interrupt based mixer with stereo positioning
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_DoMixing2:

	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:
	moveq	#0,d1
	move.b	(a0)+,d1					; byte of sample

	move.b	(a3,d1.w),d2				; * left pos
	move.b	(a4,d1.w),d1				; * right pos
;	move.b	d1,d2

	moveq	#0,d3
	move.b	(a1)+,d3					; byte of sample

	move.b	(a5,d3.w),d4				; * left pos
	move.b	(a6,d3.w),d3				; * right pos
;	move.b	d3,d4

	lsl.w	#8,d1						; into top byte
	move.b	d2,d1						; byte of sample1

	lsl.w	#8,d3						; into top byte
	move.b	d4,d3						; byte of sample1

	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d3,(a2)+					; write L.R into buffer
	move.w	d1,(a2)+					; write L.R into buffer
	move.w	d3,(a2)+					; write L.R into buffer

	dbra	d0,.loop

.nomix:
	rts


AudioMixer_DoMixing3:

	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d1
	move.b	(a0)+,d1					; byte of sample
	move.b	(a3,d1.w),d2				; *stereo pos
	sub.b	d2,d1

	moveq	#0,d3
	move.b	(a1)+,d3					; byte of sample
	move.b	(a5,d3.w),d4				; *stereo pos
	sub.b	d4,d3

	add.b	d3,d1
	add.b	d4,d2
.loop:

	moveq	#0,d3
	move.b	(a0)+,d3					; byte of sample
	move.b	(a3,d3.w),d4				; *stereo pos
	sub.b	d4,d3

	moveq	#0,d5
	move.b	(a1)+,d5					; byte of sample
	move.b	(a5,d5.w),d6				; *stereo pos
	sub.b	d6,d5

	add.b	d5,d3
	add.b	d6,d4

	move.b	d3,d5
	move.b	d4,d6
	sub.b	d1,d5
	sub.b	d4,d6
	asr.w	#2,d5
	asr.w	#2,d6

	move.b	d1,(a2)+
	move.b	d2,(a2)+
	add.b	d5,d1
	add.b	d6,d2
	move.b	d1,(a2)+
	move.b	d2,(a2)+
	add.b	d5,d1
	add.b	d6,d2
	move.b	d1,(a2)+
	move.b	d2,(a2)+
	add.b	d5,d1
	add.b	d6,d2
	move.b	d1,(a2)+
	move.b	d2,(a2)+

	move.w	d3,d1
	move.w	d4,d2

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingCC:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d1,(a2)+					; write R into buffer

	move.w	d2,(a2)+					; write L into buffer
	move.w	d2,(a2)+					; write R into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingCR:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d2
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d1,(a2)+					; write R into buffer

	move.w	d2,(a2)+					; write L.R into buffer

	move.b	d1,(a2)+					; write L into buffer
	move.b	d1,(a2)+					; write R into buffer

	move.w	d2,(a2)+					; write L.R into buffer


	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingCL:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d3
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d1,(a2)+					; write R into buffer

	move.b	d2,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0 into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingLC:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d3
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0 into buffer

	move.b	d2,(a2)+					; write L into buffer
	move.b	d2,(a2)+					; write R into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingLL:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d3
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0 into buffer

	move.b	d2,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0 into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingLR:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d3
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.b	d1,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0 into buffer

	move.b	d3,(a2)+					; write 0 into buffer
	move.b	d2,(a2)+					; write R into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts


Audio_DoMixingRC:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d1
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.w	d1,(a2)+					; write 0.R into buffer

	move.b	d2,(a2)+					; write L into buffer
	move.b	d2,(a2)+					; write R into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts

Audio_DoMixingRL:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d1
	moveq	#0,d3
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.w	d1,(a2)+					; write 0.R into buffer

	move.b	d2,(a2)+					; write L into buffer
	move.b	d3,(a2)+					; write 0.R into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts

Audio_DoMixingRR:
	lsr.l	#3,d0
	subq.w	#1,d0
	bmi.s	.nomix

	moveq	#0,d1
	moveq	#0,d2
.loop:

	move.b	(a0)+,d1					; byte of sample0
	move.b	(a1)+,d2					; byte of sample1

	move.w	d1,(a2)+					; write 0.R into buffer
	move.w	d2,(a2)+					; write L into buffer

	move.l	-4(a2),(a2)+

	dbra	d0,.loop

.nomix:
	rts

*------------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_UpdateSamples( U32 aBytes )
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

AudioMixer_UpdateSamples:

	lsr.l	#3,d0
	add.l	d0,d0
	moveq	#eAMIXER_CHANNEL_LIMIT-1,d1
	move.l	gpAudioMixerSilence,a1
	lea		gAudioMixerSamples,a0

.loop:

	tst.b	sAmixerSpl_mActiveFlag(a0)
	beq.s	.next

.active:
	move.l	sAmixerSpl_mpCurrent(a0),d2
	add.l	d0,d2
	move.l	d2,sAmixerSpl_mpCurrent(a0)
	sub.l	sAmixerSpl_mpStart(a0),d2
	sub.l	sAmixerSpl_mLength(a0),d2
	bmi.s	.next

	move.l	a1,sAmixerSpl_mpStart(a0)
	move.l	a1,sAmixerSpl_mpCurrent(a0)
	clr.b	sAmixerSpl_mActiveFlag(a0)
	clr.b	sAmixerSpl_mVolume(a0)
	clr.b	sAmixerSpl_mGainLeft(a0)
	clr.b	sAmixerSpl_mGainRight(a0)

.next:
	lea		sAmixerSpl_msizeof(a0),a0
	dbra	d1,.loop

	rts


**************************************************************************************
	DATA
**************************************************************************************

gpAudioMixerRouts:
	dc.l	Audio_DoMixingCC	;0000
	dc.l	Audio_DoMixingCL	;0001
	dc.l	Audio_DoMixingCR	;0010
	dc.l	Audio_DoMixingCC	;0011

	dc.l	Audio_DoMixingLC	;0100
	dc.l	Audio_DoMixingLL	;0101
	dc.l	Audio_DoMixingLR	;0110
	dc.l	Audio_DoMixingLC	;0111

	dc.l	Audio_DoMixingRC	;1000
	dc.l	Audio_DoMixingRL	;1001
	dc.l	Audio_DoMixingRR	;1010
	dc.l	Audio_DoMixingRC	;1011

	dc.l	Audio_DoMixingCC	;1100
	dc.l	Audio_DoMixingCC	;1101
	dc.l	Audio_DoMixingCC	;1110
	dc.l	Audio_DoMixingCC	;1101

gpAudioMixerBuffer:			dc.l	0
gpAudioMixerSilence:		dc.l	0
gpAudioMixerMulTable:		dc.l	0
gAudioMixerBufferOffset:	dc.l	0
gAudioMixerLockFlag:		dc.b	0
gAudioMixerEnableFlag:		dc.b	0


**************************************************************************************
	BSS
**************************************************************************************

gAudioMixerSamples:			ds.l	(sAmixerSpl_msizeof*eAMIXER_CHANNEL_LIMIT)

