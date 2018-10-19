**************************************************************************************
*	AMIXER_S.S
*
*	AUDIO mixer functions
*
*	[c] 2001 Reservoir Gods
**************************************************************************************

;	Mixer frequencies:
;   Frequency : Bytes Per Frame @ 50hz (mono/stereo) 
;	 6258	  :  125 / 250
;	12517	  :  250 / 500
;	25033     :  500 / 1000
;	50066     : 1001 / 2002

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

	; a chunk is our unit of mixing, should always be mixing up to 2 VBLs ahead

	import	gAudioMixerChunkSize
	import	gAudioMixerOffset

	import	AudioMixer_Slow

**************************************************************************************
;	EQUATES
**************************************************************************************

eAMIXER_BUFFER_SIZE		EQU	(8*1024)
eAMIXER_SILENCE_SIZE	EQU	(4*1024)
eAMIXER_PLAY_OFFSET		EQU	(6*1024)
eAMIXER_CHANNEL_LIMIT	EQU	2


**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET

sAmixerSpl_mpStart:		ds.l	1
sAmixerSpl_mpCurrent:	ds.l	1
sAmixerSpl_mpEnd:		ds.l	1
sAmixerSpl_mLength:		ds.l	1
sAmixerSpl_mGainLeft:	ds.b	1
sAmixerSpl_mGainRight:	ds.b	1
sAmixerSpl_mVolume:		ds.b	1
sAmixerSpl_mActiveFlag:	ds.b	1
sAmixerSpl_msizeof:		ds.b	1


	OFFSET

sAmixerOp_mpSpl0:			ds.l	1
sAmixerOp_mpSpl1:			ds.l	1
sAmixerOp_mBufferOffset:	ds.l	1
sAmixerOp_mByteCount:		ds.l	1
sAmixerOp_sizeof:			ds.b	1

**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : Audio_MixerVbl
* ACTION   : interrupt based mixer
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

export poop	
poop:

AudioMixer_Vbl:
	tas		gAudioMixerLockFlag			; already in a mixer interrupt?
	bne		.locked						; yes, don't try further mixing
	movem.l	d0-a6,-(a7)					; save registers
	jsr		AudioMixer_Slow
	movem.l	(a7)+,d0-a6
	clr.b	gAudioMixerLockFlag			; signal end of mixing
.locked:
	rts

AudioMixer_Vbl1:
;	not.w	$ffff8240.w
	tas		gAudioMixerLockFlag			; already in a mixer interrupt?
	bne		.locked						; yes, don't try further mixing

	movem.l	d0-a6,-(a7)					; save registers

	lea		gAudioMixerSamples,a0				; sample0
	lea		sAmixerSpl_msizeof(a0),a1			; sample1

	tst.b	sAmixerSpl_mActiveFlag(a0)
	bne.s	.new
	tst.b	sAmixerSpl_mActiveFlag(a1)
	beq.s	.silence
.new:
	moveq	#0,d0
.silence:

	; build set of mix operations
	; this takes into account wrapping of ring buffer
	; and fact we could hit end of current samples during mixing

	move.l	gAudioMixerBufferOffset,d4			; size of ring buffer
	move.l	gAudioMixerChunkSize,d3				; amount to mix per VBL

	lea		gAudioMixerOperations,a2			; array of operations to perform
	move.l	sAmixerSpl_mpCurrent(a0),a3			; data for spl0
	move.l	sAmixerSpl_mpCurrent(a1),a4			; data for spl1

	move.l	a3,sAmixerOp_mpSpl0(a2)				; ptr to start of spl0
	move.l	a4,sAmixerOp_mpSpl1(a2)				; ptr to start of spl0
	move.l	d4,sAmixerOp_mBufferOffset(a2)		; dst (offset into mixing buffer)
	move.l	d3,sAmixerOp_mByteCount(a2)			; bytes to mix

	move.l	sAmixerSpl_mpEnd(a0),d0				; end of spl0
	sub.l	sAmixerSpl_mpCurrent(a0),d0			; bytes left in sample 0
	move.l	sAmixerSpl_mpEnd(a1),d1				; end of spl1
	sub.l	sAmixerSpl_mpCurrent(a1),d1			; bytes left in sample 1

	move.l	#eAMIXER_BUFFER_SIZE,d2				; ring buffer size
	sub.l	d4,d2								; mixable bytes left 

	sub.l	d2,d3								; how much left over?
	bge.s	.overflow							; some overflow, wrapping needed
	moveq	#0,d3								; no need to wrap
.overflow:

	cmp.l	d0,d1								; is more left in s0 or s1?
	bge		.s1bigger

	move.l	d2,d4								; ring buffer chunk0 size
	sub.l	d0,d4								; can we fill it completely?
	bmi		.s0checkr1							; yes

	move.l	d0,sAmixerOp_mByteCount(a2)			; first mix chunk just size of spl0
	lea		sAmixerOp_sizeof(a2),a2

	move.l	gpAudioMixerSilence,a3				; spl0 now silence
	move.l	#eAMIXER_SILENCE_SIZE,d0			; size of silence

	add.l	d0,a4								; move start ptr of spl1
	sub.l	d0,d1								; remaining bytes in spl1


.s0checkr1:

.s1bigger:

	move.l	d2,d4								; ring chunk 0
	sub.l	d0,d4								; -spl0

	;
	; Calculate gain scalars for sample0 and sample1
	;

	move.l	gpAudioMixerMulTable,a2				; fixed point multiply table
	
	moveq	#0,d0								;
	move.b	sAmixerSpl_mGainLeft(a0),d0			; sample0.gain.left
	lsl.w	#8,d0								; *256
	lea		(a2,d0.l),a3						; gain left
	moveq	#0,d0
	move.b	sAmixerSpl_mGainRight(a0),d0		; sample0.gain.right
	lsl.w	#8,d0								; *256
	lea		(a2,d0.l),a4						; gain right

	moveq	#0,d0
	move.b	sAmixerSpl_mGainLeft(a1),d0			; sample1.gain.left
	lsl.w	#8,d0								; *256
	lea		(a2,d0.l),a5						; gain left
	moveq	#0,d0								;
	move.b	sAmixerSpl_mGainRight(a1),d0		; sample1.gain.right
	lsl.w	#8,d0								; *256
	lea		(a2,d0.l),a6						; gain right





	move.l	sAmixerSpl_mpCurrent(a0),a0
	move.l	sAmixerSpl_mpCurrent(a1),a1


	movea.w	#$8909,a2							; current DMA sound ptr
	moveq	#0,d7
	move.b	(a2),d7								; hi byte of ptr
	swap	d7
	move.b	2(a2),d7							; mid byte of ptr
	lsl.w	#8,d7
	move.b	4(a2),d7							; low byte of ptr

	; valid start ptr

	add.l	gAudioMixerOffset,d7				; start of mixing
	move.l	gpAudioMixerBuffer,a2				; start of mixing buffer
	sub.l	a2,d7								; current h/w offset into buffer

	add.l	gAudioMixerOffset,d7				; get to our mixing start point
	and.l	#(eAMIXER_BUFFER_SIZE-1),d7			; clip to buffer size
	add.l	d7,a2								; start of buffer to mix into
	move.l	gAudioMixerChunkSize,d0				; size to mix per vbl
	move.l	d7,d2								; start offset
	add.l	d0,d2								; end offset
	sub.l	#eAMIXER_BUFFER_SIZE,d2				; have we wrapped?
	bmi.s	.singlepass							; no, do single pass
	move.l	d2,-(a7)
	sub.l	d2,d0								; size from current to end of ring buffer
	bsr		AudioMixer_Mix1						; mix to end of ring buffer
	move.l	gpAudioMixerBuffer,a2				; wrap buffer
	move.l	(a7)+,d2
.singlepass:
	bsr		AudioMixer_Mix1
.nowrap:

	; update sample positions, and reset to silence when finished

	lea		gAudioMixerSamples,a2				; sample0
	tst.b	sAmixerSpl_mActiveFlag(a2)			; is this sample active?
	beq.s	.noSpl0
	move.l	a0,sAmixerSpl_mpCurrent(a2)
	cmp.l	sAmixerSpl_mpEnd(a2),a0
	blt.s	.noSpl0
	bsr		AudioMixer_SampleReset	
.noSpl0:
	lea		sAmixerSpl_msizeof(a2),a2
	tst.b	sAmixerSpl_mActiveFlag(a2)				; is this sample active?
	beq.s	.noSpl1
	move.l	a1,sAmixerSpl_mpCurrent(a2)
	cmp.l	sAmixerSpl_mpEnd(a2),a0
	blt.s	.noSpl1
	bsr		AudioMixer_SampleReset	
.noSpl1:

	clr.b	gAudioMixerLockFlag			; signal end of mixing
	movem.l	(a7)+,d0-a6					; restore registers
.locked:
	rts

AudioMixer_SampleReset:
	move.l	gpAudioMixerSilence,d0
	move.l	d0,sAmixerSpl_mpStart(a2)				; switch sample to silence
	move.l	d0,sAmixerSpl_mpCurrent(a2)				; point current pointer to silence
	add.l	#1024,d0
	move.l	d0,sAmixerSpl_mpEnd(a2)				; point current pointer to silence
	move.l	#1024,sAmixerSpl_mLength(a2)
	clr.b	sAmixerSpl_mActiveFlag(a2)				; mark sample as free
	clr.b	sAmixerSpl_mVolume(a2)					; volume = 0
	clr.b	sAmixerSpl_mGainLeft(a2)				; gain.left = 0
	clr.b	sAmixerSpl_mGainRight(a2)				; gain.right = 0
	rts


	movea.w	#$8909,a2								; dma sound frame ptr
	movep.l	(a2),d7									; read address
	lsr.l	#8,d7									; ignore frame end address high byte

	move.l	gpAudioMixerBuffer,a2					; start of mixing buffer
	sub.l	a2,d7									; current h/w offset into buffer
	move.l	gAudioMixerBufferOffset,d6				; end of last s/w mix
	and.l	#(eAMIXER_BUFFER_SIZE-1),d6				; clip to buffer size
	lea		(a2,d6.l),a2							; get to place in buffer

	and.l	#(eAMIXER_BUFFER_SIZE-1)&$FFFFFFF8,d7	; offset moves in steps of 8
	move.l	d7,gAudioMixerBufferOffset				; end of new s/w mix
	sub.l	d6,d7									; mix length
	bpl.s	.lpls									; do straight linear mix

	move.l	#(eAMIXER_BUFFER_SIZE),d0
	add.l	d0,d7
	sub.l	d6,d0
	bsr		AudioMixer_Mix1

	move.l	gpAudioMixerBuffer,a2
	move.l	gAudioMixerBufferOffset,d0
	bsr		AudioMixer_Mix1
	bra.s	.update
.lpls:
	move.l	d7,d0
	bsr		AudioMixer_Mix1

.update:
	move.l	d7,d0
	bsr		AudioMixer_UpdateSamples

	clr.b	gAudioMixerLockFlag			; signal end of mixing
	movem.l	(a7)+,d0-a6					; restore registers

.locked:
;	not.w	$ffff8240.w
	rts





OldShit:

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

	movea.w	#$8909,a2								; dma sound frame ptr
	movep.l	(a2),d7									; read address
	lsr.l	#8,d7									; ignore frame end address high byte

	move.l	gpAudioMixerBuffer,a2					; start of mixing buffer
	sub.l	a2,d7									; current h/w offset into buffer
	move.l	gAudioMixerBufferOffset,d6				; end of last s/w mix
	and.l	#(eAMIXER_BUFFER_SIZE-1),d6				; clip to buffer size
	lea		(a2,d6.l),a2							; get to place in buffer

	and.l	#(eAMIXER_BUFFER_SIZE-1)&$FFFFFFF8,d7	; offset moves in steps of 8
	move.l	d7,gAudioMixerBufferOffset				; end of new s/w mix
	sub.l	d6,d7									; mix length
	bpl.s	.lpls									; do straight linear mix

	move.l	#(eAMIXER_BUFFER_SIZE),d0
	add.l	d0,d7
	sub.l	d6,d0
	jsr		(a3)

	move.l	gpAudioMixerBuffer,a2
	move.l	gAudioMixerBufferOffset,d0
	jsr		(a3)
	bra.s	.update
.lpls:
	move.l	d7,d0
	jsr		(a3)

.update:
	move.l	d7,d0
	bsr		AudioMixer_UpdateSamples

	clr.b	gAudioMixerLockFlag			; signal end of mixing
	movem.l	(a7)+,d0-a6					; restore registers

.locked:
;	not.w	$ffff8240.w
	rts



AudioMixer_Mix1:
;	lsr.l	#3,d0
	lsr.l	#1,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:

	move.b	(a0)+,d1					; sample0 [s0]
	move.b	(a1)+,d2					; sample1 [s1]
	move.b d1,d2

	move.b	d1,(a2)+
	move.b	d2,(a2)+

	dbra	d0,.loop

.nomix:
	rts

; equation:
;	left = (sample0 * leftgain) + (sample1 *leftgain) - ((sample0 * leftgain) * (sample1 *leftgain)/256)
;	left = (sample0 * rightgain) + (sample1 *rightgain) - ((sample0 * rightgain) * (sample1 *rightgain)/256)

AudioMixer_Mix1b:
	lsr.l	#1,d0
	subq.w	#1,d0
	bmi.s	.nomix

.loop:
	move.l	a2,d6
	move.l	gpAudioMixerMulTable,a2
	moveq	#0,d1
	moveq	#0,d2

	move.b	(a0)+,d1					; sample0 [s0]
	move.b	(a1)+,d3					; sample1 [s1]

	move.b	(a4,d1),d2					; s0 * r0
	move.b	(a3,d1),d1					; s0 * l0

	move.b	(a6,d3),d4					; s1 * r1
	move.b	(a5,d3),d3					; s1 * l1

	move.w	d1,d5						; s0 * l0
	lsl.w	#8,d5						; * 256
	add.w	d3,d5						; s1 * l1
	move.b	(a2,d3),d5					;  (s0*l1) * (s1*l1) / 256

	add.w	d3,d1						; (s0*l0) + (s1*l1)
	sub.b	d5,d1						; (s0*l0) + (s1*l1) - ( (s0*l1) * (s1*l1) / 256)


	move.w	d2,d5						; s0 * r0
	lsl.w	#8,d5						; * 256
	add.w	d4,d5						; s1 * r1
	move.b	(a2,d3),d5					;  (s0*r0) * (s1*r1) / 256

	add.w	d4,d2
	sub.b	d5,d2

	add.b	#128,d1
	add.b	#128,d2

	move.l	d6,a2
	move.b	d1,(a2)+
	move.b	d2,(a2)+

	dbra	d0,.loop

.nomix:
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

;	lsr.l	#3,d0
	lsr.l	#1,d0
	add.l	d0,d0
	moveq	#eAMIXER_CHANNEL_LIMIT-1,d1				; we have one sample per channel
	move.l	gpAudioMixerSilence,a1					; go back to silence sample if queue is empty
	lea		gAudioMixerSamples,a0					; start of samples

.loop:

	tst.b	sAmixerSpl_mActiveFlag(a0)				; is this sample active?
	beq.s	.next									; no, goto next

.active:
	move.l	sAmixerSpl_mpCurrent(a0),d2				; current sample pointer
	add.l	d0,d2									; add offset
	move.l	d2,sAmixerSpl_mpCurrent(a0)				; store updated sample pointer
	sub.l	sAmixerSpl_mpStart(a0),d2				; diff from start
	sub.l	sAmixerSpl_mLength(a0),d2				; minus length
	bmi.s	.next									; still bytes to play

	move.l	a1,sAmixerSpl_mpStart(a0)				; switch sample to silence
	move.l	a1,sAmixerSpl_mpCurrent(a0)				; point current pointer to silence
	clr.b	sAmixerSpl_mActiveFlag(a0)				; mark sample as free
	clr.b	sAmixerSpl_mVolume(a0)					; volume = 0
	clr.b	sAmixerSpl_mGainLeft(a0)				; gain.left = 0
	clr.b	sAmixerSpl_mGainRight(a0)				; gain.right = 0

.next:
	lea		sAmixerSpl_msizeof(a0),a0				; next sample in the array
	dbra	d1,.loop								; loop for all samples

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

gAudioMixerOperations:		ds.b	(sAmixerOp_sizeof*4)
gAudioMixerSamples:			ds.l	(sAmixerSpl_msizeof*eAMIXER_CHANNEL_LIMIT)

