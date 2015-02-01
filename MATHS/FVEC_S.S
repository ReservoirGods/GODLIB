**************************************************************************
*
*	Floating Point Vector module
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
* FUNCTION : FVector_Add( sFVector * apSrc0,sFVector * apSrc1,sFVector * apDst )
* ACTION   : FVector_Add
* CREATION : 28.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Add:
	fmove.s	(a0)+,fp0
	fadd.s	(a1)+,fp0
	fmove.s	fp0,(a2)+

	fmove.s	(a0)+,fp0
	fadd.s	(a1)+,fp0
	fmove.s	fp0,(a2)+

	fmove.s	(a0),fp0
	fadd.s	(a1),fp0
	fmove.s	fp0,(a2)
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Sub( sFVector * apSrc0,sFVector * apSrc1,sFVector * apDst )
* ACTION   : FVector_Sub
* CREATION : 28.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Sub:
	fmove.s	(a0)+,fp0
	fsub.s	(a1)+,fp0
	fmove.s	fp0,(a2)+

	fmove.s	(a0)+,fp0
	fsub.s	(a1)+,fp0
	fmove.s	fp0,(a2)+

	fmove.s	(a0),fp0
	fsub.s	(a1),fp0
	fmove.s	fp0,(a2)
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Mul( sFVector * apSrc,FP32 aMul,sFVector * apDst )
* ACTION   : FVector_Mul
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Mul:
	fmove.s	(a0)+,fp1
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0)+,fp1
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0),fp1
	fmul	fp1,fp0
	fmove.s	fp0,(a1)
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Div( sFVector * apSrc,FP32 aDiv,sFVector * apDst )
* ACTION   : FVector_Div
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Div:
	fmove.s	#1.0,fp1
	fdiv	fp0,fp1

	fmove.s	(a0)+,fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0)+,fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0),fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Normalise( sFVector * apSrc,sFVector * apDst )
* ACTION   : FVector_Normalise
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Normalise:
	fmove.s	(a0),fp0
	fmul	fp0,fp0
	fmove.s	4(a0),fp1
	fmul	fp1,fp1
	fadd	fp1,fp0
	fmove.s	8(a0),fp1
	fmul	fp1,fp1
	fadd	fp1,fp0

	fsqrt	fp0
	fmove.s	#1.0,fp1
	fdiv	fp0,fp1

	fmove.s	(a0)+,fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0)+,fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)+

	fmove.s	(a0),fp0
	fmul	fp1,fp0
	fmove.s	fp0,(a1)
		
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Length( sFVector * apSrc )
* ACTION   : FVector_Length
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Length:
	fmove.s	(a0)+,fp0
	fmul	fp0,fp0
	fmove.s	(a0)+,fp1
	fmul	fp1,fp1
	fadd	fp1,fp0
	fmove.s	(a0),fp1
	fmul	fp1,fp1
	fadd	fp1,fp0

	fsqrt	fp0
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Dot( sFVector * apSrc )
* ACTION   : FVector_Dot
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Dot:
	fmove.s	(a0)+,fp0
	fmul	fp0,fp0
	fmove.s	(a0)+,fp1
	fmul	fp1,fp1
	fadd	fp1,fp0
	fmove.s	(a0),fp1
	fmul	fp1,fp1
	fadd	fp1,fp0
	
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Cross( sFVector * apSrc0, sFVector * apSrc1, sFVector * apDst )
* ACTION   : FVector_Cross
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Cross:
	fmove.s	4(a0),fp0	
	fmove.s	8(a1),fp1
	fmul	fp1,fp0

	fmove.s	8(a0),fp1
	fmove.s	4(a1),fp2
	fmul	fp2,fp1
	fadd	fp1,fp0
	fmove.s	fp0,(a2)+


	fmove.s	8(a0),fp0	
	fmove.s	(a1),fp1
	fmul	fp1,fp0

	fmove.s	(a0),fp1
	fmove.s	8(a1),fp2
	fmul	fp2,fp1
	fadd	fp1,fp0
	fmove.s	fp0,(a2)+


	fmove.s	(a0),fp0	
	fmove.s	4(a1),fp1
	fmul	fp1,fp0

	fmove.s	4(a0),fp1
	fmove.s	(a1),fp2
	fmul	fp2,fp1
	fadd	fp1,fp0
	fmove.s	fp0,(a2)+

	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Min( sFVector * apSrc0,sFVector * apSrc1,sFVector * apDst )
* ACTION   : FVector_Min
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Min:
	fmove.s	(a0)+,fp0
	fmove.s	(a1)+,fp1
	fcmp	fp0,fp1
	fbge	.minx
	fmove	fp1,fp0
.minx:
	fmove.s	fp0,(a2)+	

	fmove.s	(a0)+,fp0
	fmove.s	(a1)+,fp1
	fcmp	fp0,fp1
	fbge	.miny
	fmove	fp1,fp0
.miny:
	fmove.s	fp0,(a2)+	

	fmove.s	(a0),fp0
	fmove.s	(a1),fp1
	fcmp	fp0,fp1
	fbge	.minz
	fmove	fp1,fp0
.minz:
	fmove.s	fp0,(a2)
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Max( sFVector * apSrc0,sFVector * apSrc1,sFVector * apDst )
* ACTION   : FVector_Max
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Max:
	fmove.s	(a0)+,fp0
	fmove.s	(a1)+,fp1
	fcmp	fp0,fp1
	fble	.maxx
	fmove	fp1,fp0
.maxx:
	fmove.s	fp0,(a2)+	

	fmove.s	(a0)+,fp0
	fmove.s	(a1)+,fp1
	fcmp	fp0,fp1
	fble	.maxy
	fmove	fp1,fp0
.maxy:
	fmove.s	fp0,(a2)+	

	fmove.s	(a0),fp0
	fmove.s	(a1),fp1
	fcmp	fp0,fp1
	fble	.maxz
	fmove	fp1,fp0
.maxz:
	fmove.s	fp0,(a2)
	rts


*------------------------------------------------------------------------------------*
* FUNCTION : FVector_Lerp( sFVector * apSrc0,sFVector * apSrc1,FP32 aWeight sFVector * apDst )
* ACTION   : FVector_Lerp
* CREATION : 29.08.2005 PNK
*------------------------------------------------------------------------------------*

FVector_Lerp:
	fmove.s	#1.0,fp1
	fsub	fp0,fp1

	fmove.s	(a0)+,fp2
	fmove.s	(a1)+,fp3
	fmul	fp0,fp2
	fmul	fp1,fp3
	fadd	fp3,fp2
	fmove.s	fp2,(a2)+

	fmove.s	(a0)+,fp2
	fmove.s	(a1)+,fp3
	fmul	fp0,fp2
	fmul	fp1,fp3
	fadd	fp3,fp2
	fmove.s	fp2,(a2)+

	fmove.s	(a0),fp2
	fmove.s	(a1),fp3
	fmul	fp0,fp2
	fmul	fp1,fp3
	fadd	fp3,fp2
	fmove.s	fp2,(a2)
	
	rts



