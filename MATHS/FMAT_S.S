**************************************************************************
*
*	Floating Point Matrix module
*
*	code Pink
*
*	[c] 2005 Reservoir Gods
*
**************************************************************************

**************************************************************************
;	STRUCTS
**************************************************************************



**************************************************************************
	TEXT
**************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildAxisAngle( sFMatrix * apMat,sFVector * apVec,FP32 aAngle )
* ACTION   : FMatrix_BuildAxisAngle
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildAxisAngle:
	fsincos	fp0,fp0:fp1		;	C:S
	fmove.s	#1.0,fp4		;	1.0
	fsub	fp0,fp4			;	T = 1.0 - C

	fmove.s	0(a1),fp7		;	x
	fmove.s	4(a1),fp2		;	y
	fmove.s	8(a1),fp3		;	z

	fmove	fp2,fp5			;	y
	fmove	fp3,fp6			;	z

	fsglmul	fp1,fp2			;	sY
	fsglmul	fp1,fp3			;	sZ
	fsglmul	fp7,fp1			;	sX

	fsglmul	fp4,fp5			;	tY
	fsglmul	fp4,fp6			;	tZ
	fsglmul	fp7,fp4			;	tX


	fmove.s	(a1),fp7		;	X
	fsglmul	fp4,fp7			;	x*tX
	fadd	fp0,fp7			;	C + x*tX
	fmove.s	fp7,(a0)+

	fmul.s	4(a1),fp4		;	tX*y
	fmove	fp4,fp7			;	tX*y
	fadd	fp3,fp7			;	sz + tX*Y
	fmove.s	fp7,(a0)+

	fmove.s	(a1),fp7		;	X
	fsglmul	fp6,fp7			;	x*tZ
	fsub	fp2,fp7			;	x*tZ - sY
	fmove.s	fp7,(a0)+

	clr.l	(a0)+			;	0

	fsub	fp3,fp4			;	tX*y - sZ
	fmove.s	fp4,(a0)+
	
	fmove.s	4(a1),fp7		;	y
	fsglmul	fp5,fp7			;	tY*y
	fadd	fp0,fp7			;	tY*y + c
	fmove.s	fp7,(a0)+

	fmul.s	8(a1),fp5		;	z*ty
	fadd	fp1,fp6			;	sx + z*ty
	fmove.s	fp6,(a0)+

	clr.l	(a0)+			;	0

	fmove.s	(a1),fp7		;	x
	fsglmul	fp6,fp7			;	tZ*x
	fadd	fp2,fp7			;	sY + tZ *x
	fmove.s	fp7,(a0)+

	fsub	fp1,fp5			;	z*ty - sX
	fmove.s	fp5,(a0)+

	fmove.s	8(a1),fp7		;	z
	fsglmul	fp6,fp7			;	z*tz
	fadd	fp0,fp7			;	z*tz + c
	fmove.s	fp7,(a0)+
	

;not finished
	rts




*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildIdentity( sFMatrix * apMat )
* ACTION   : FMatrix_BuildIdentity
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildIdentity:
	moveq	#0,d0
	move.l	#$3F800000,d1
	
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+

	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildRotate( sFMatrix * apMat,sFVector * apAngles )
* ACTION   : FMatrix_BuildRotate
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildRotate:

	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp2
	fmove.s	(a1)+,fp4
	fsincos	fp0,fp0:fp1		;	cosx:sinx
	fsincos	fp2,fp2:fp3		;	cosy:siny
	fsincos	fp4,fp4:fp5		;	cosz:sinz
	
	fmove	fp2,fp6			;	cosy
	fsglmul	fp4,fp6			;	cosz*cosy
	fmove.s	fp6,(a0)+		

	fmove	fp5,fp6			;	sinz
	fneg	fp6				;	-sinz
	fsglmul	fp2,fp6			;	cosy*-sinz
	fmove.s	fp6,(a0)+

	fmove.s	fp3,(a0)+		;	siny

	clr.l	(a0)+			;	0.f

	fneg	fp1				;	-sinx
	fneg	fp3				;	-siny
	fmove	fp1,fp6			;	-sinx
	fsglmul	fp1,fp6			;	-sinx*-siny
	fmove	fp4,fp7			;	cosz
	fsglmul	fp6,fp7			;	-sinx*-siny*cosz
	fmove.s	fp6,d0			;	-sinx*-siny
	fmove	fp0,fp6			;	cosx
	fsglmul	fp5,fp6			;	sinz*cosx
	fadd.s	d7,fp6			;	-sinx*-siny*cosz + sinz*cosx
	fmove.s	fp6,(a0)+

	fmove	fp5,fp6			;	sinz
	fneg	fp6				;	-sinz
	fmul.s	d0,fp6			;	-sinx*-siny * -sinz
	fmove	fp0,fp7			;	cosx
	fsglmul	fp4,fp7			;	cosz * cosx
	fadd	fp7,fp6			;	cosz*cosx + -sinx*-siny*-sinz
	fmove.s	fp6,(a0)+

	fmove	fp2,fp6			;	cosy
	fsglmul	fp1,fp6			;	-sinx*cosy
	fmove.s	fp6,(a0)+

	clr.l	(a0)+			;	0.f

	fsglmul	fp0,fp3			;	cosx*-siny
	fmove	fp4,fp6			;	cosz
	fmul	fp3,fp6			;	cosx*-siny * cosz

	fneg	fp1				;	+sinx
	fmove	fp1,fp7			;	sinx
	fsglmul	fp5,fp7			;	sinz*sinx
	fadd	fp7,fp6
	fmove.s	fp6,(a0)+

	fneg	fp5				;  -sinz
	fsglmul	fp3,fp5			;	cosx*-siny * -sinz
	fsglmul	fp4,fp1			;	cosz*sinx
	fadd	fp1,fp5			;	cosz*sinz + cosx*-siny*-sinz
	fmove.s	fp5,(a0)+


	clr.l	(a0)+			;	0.f
	clr.l	(a0)+			;	0.f
	clr.l	(a0)+			;	0.f
	move.l	#$3F800000,(a0)	;	1.f

	rts



*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildRotateX( sFMatrix * apMat,FP32 aAngleX )
* ACTION   : FMatrix_BuildRotateX
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildRotateX:
	
	fsincos	fp0,fp0:fp1
	moveq	#0,d0
	move.l	#$3F800000,d1

	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	fneg	fp1
	move.l	d0,(a0)+
	fmove.s	fp0,(a0)+
	fmove.s	fp1,(a0)+
	move.l	d0,(a0)+

	fneg	fp1
	move.l	d0,(a0)+
	fmove.s	fp1,(a0)+
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+

	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildRotateY( sFMatrix * apMat,FP32 aAngleX )
* ACTION   : FMatrix_BuildRotateY
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildRotateY:
	fsincos	fp0,fp0:fp1
	moveq	#0,d0
	move.l	#$3F800000,d1

	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	fmove.s	fp1,(a0)+
	move.l	d0,(a0)+

	fneg	fp1
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	fmove.s	fp1,(a0)+
	move.l	d0,(a0)+
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildRotateZ( sFMatrix * apMat,FP32 aAngleX )
* ACTION   : FMatrix_BuildRotateZ
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildRotateZ:
	fsincos	fp0,fp0:fp1
	moveq	#0,d0
	move.l	#$3F800000,d1

	fneg	fp1
	fmove.s	fp0,(a0)+
	fmove.s	fp1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	fneg	fp1
	fmove.s	fp1,(a0)+
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	
	rts



*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildScale( sFMatrix * apMat,FP32 aScale )
* ACTION   : FMatrix_BuildScale
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildScale:
	moveq	#0,d0
	
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d0,(a0)+
	fmove.s	fp0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	#$3f80000,(a0)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_BuildTranslation( sFMatrix * apMat,sFVector * apTrans )
* ACTION   : FMatrix_BuildTranslation
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_BuildTranslation:
	moveq	#0,d0
	move.l	#$3F800000,d1
	
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+

	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d1,(a0)+
	move.l	d0,(a0)+

	move.l	(a1)+,(a0)+
	move.l	(a1)+,(a0)+
	move.l	(a1)+,(a0)+
	move.l	d1,(a0)+

	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_Mul( sFMatrix * apSrc0,sFMatrix * apSrc1,sFMatrix * apDst )
* ACTION   : FMatrix_Mul
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_Mul:

	moveq	#3,d1
.matmuly:
	moveq	#3,d0
.matmulx
	fmove.s	(a0),fp0
	fmove.s	(a1),fp1
	fmul	fp1,fp0

	fmove.s	4(a0),fp1
	fmove.s	16(a1),fp2
	fsglmul	fp2,fp1
	fadd	fp2,fp0

	fmove.s	8(a0),fp1
	fmove.s	32(a1),fp2
	fsglmul	fp2,fp1
	fadd	fp2,fp0

	fmove.s	12(a0),fp1
	fmove.s	48(a1),fp2
	fsglmul	fp2,fp1
	fadd	fp2,fp0
	
	fmove.s	fp0,(a2)+

	addq.l	#4,a1
	dbra	d0,.matmulx

	lea		16(a0),a0
	lea		-16(a1),a1
	dbra	d1,.matmuly

	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_Transpose( sFMatrix * apSrc,sFMatrix * apDst )
* ACTION   : FMatrix_Transpose
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_Transpose:

	move.l	(a0),(a1)

	move.l	4(a0),d0
	move.l	16(a0),4(a1)
	move.l	d0,16(a1)

	move.l	8(a0),d0
	move.l	32(a0),8(a1)
	move.l	d0,32(a1)

	move.l	20(a0),20(a1)

	move.l	24(a0),d0
	move.l	36(a0),24(a1)
	move.l	d0,36(a1)

	move.l	40(a0),40(a1)
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_Apply( sFMatrix * apSrc,sFVector * apSrc,sFVector * apDst )
* ACTION   : FMatrix_Apply
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_Apply:
	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp1
	fmove.s	(a1)+,fp2
	

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_ApplyAxes( sFMatrix * apSrc,sFVector * apSrc,sFVector * apDst )
* ACTION   : FMatrix_ApplyAxes
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_ApplyAxes:
	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp1
	fmove.s	(a1)+,fp2
	

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_ApplyInv( sFMatrix * apSrc,sFVector * apSrc,sFVector * apDst )
* ACTION   : FMatrix_ApplyInv
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_ApplyInv:
	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp1
	fmove.s	(a1)+,fp2

	fmove.s	32(a0),fp3
	fsub	fp3,fp0
	fmove.s	36(a0),fp3
	fsub	fp3,fp1
	fmove.s	40(a0),fp3
	fsub	fp3,fp2
	
	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	addq.l	#4,a0

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	addq.l	#4,a0

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_ApplyInvAxes( sFMatrix * apSrc,sFVector * apSrc,sFVector * apDst )
* ACTION   : FMatrix_ApplyInvAxes
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_ApplyInvAxes:
	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp1
	fmove.s	(a1)+,fp2

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	addq.l	#4,a0

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	addq.l	#4,a0

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	(a0)+,fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FMatrix_ApplyPers( sFMatrix * apSrc,sFVector * apSrc,sFVector * apDst )
* ACTION   : FMatrix_ApplyPers
* CREATION : 30.08.2005 PNK
*------------------------------------------------------------------------------------*

FMatrix_ApplyPers:
	fmove.s	(a1)+,fp0
	fmove.s	(a1)+,fp1
	fmove.s	(a1)+,fp2
	

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+
	
	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3
	fmove.s	fp3,(a2)+

	fmove.s	(a0)+,fp3
	fsglmul	fp0,fp3
	fmove.s	12(a0),fp4
	fsglmul	fp1,fp4
	fadd	fp4,fp3
	fmove.s	28(a0),fp4
	fsglmul	fp2,fp4
	fadd	fp4,fp3
	fmove.s	44(a0),fp4
	fadd	fp4,fp3

	fmove.s	#1.0,fp4
	fdiv	fp3,fp4

	fmove.s	-(a2),fp0
	fsglmul	fp4,fp0
	fmove.s	fp0,(a2)

	fmove.s	-(a2),fp0
	fsglmul	fp4,fp0
	fmove.s	fp0,(a2)

	fmove.s	-(a2),fp0
	fsglmul	fp4,fp0
	fmove.s	fp0,(a2)
	
	rts



