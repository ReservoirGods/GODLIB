#ifndef	INCLUDED_VECTOR_H
#define	INCLUDED_VECTOR_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Vector_Add( const sVector * apSrcVec0, const sVector * apSrcVec1, sVector * apDstVec );
void	Vector_Sub( const sVector * apSrcVec0, const sVector * apSrcVec1, sVector * apDstVec );
void	Vector_Mul( const sVector * apSrcVec, const S16 * aScaler, sVector * apDstVec );

void	Vector_Normal( const sVector * apSrcVec, sVector * apDstVec );
S32		Vector_Length( const sVector * apSrcVec );
S32		Vector_SquaredLength( const sVector * apSrcVec );

S32		Vector_Dot( const sVector * apSrcVec0, const sVector * apSrcVec1 );
void	Vector_Cross( const sVector * apSrcVec0, const sVector * apSrcVec1, sVector * apDstVec );

void	FMatrix_BuildIdentity( sFMatrix * apMat );

void	FMatrix_BuildRotate( sFMatrix * apMat, sFVector * apAngles );

void	FMatrix_BuildRotateX( sFMatrix * apMat, FP32 aAngleX );
void	FMatrix_BuildRotateY( sFMatrix * apMat, FP32 aAngleX );
void	FMatrix_BuildRotateZ( sFMatrix * apMat, FP32 aAngleX );

void	FMatrix_BuildScaleX( sFMatrix * apMat, FP32 aAngleX );
void	FMatrix_BuildScaleY( sFMatrix * apMat, FP32 aAngleX );
void	FMatrix_BuildScaleZ( sFMatrix * apMat, FP32 aAngleX );

void	FMatrix_BuildAxisAngle( sFMatrix * apMat, sFVector * apVec, FP32 aAngle );
void	FMatrix_Mul( sFMatrix * apSrc0, sFMatrix * apSrc1, sFMatrix * apDst );

void	FMatrix_Normalise( sFMatrix * apSrc, sFMatrix * apDst );
void	FMatrix_Transpose( sFMatrix * apSrc, sFMatrix * apDst );

void	FMatrix_Apply( sFMatrix * apSrc, sFVector * apSrc, sFVector * apDst );
void	FMatrix_ApplyAxes( sFMatrix * apSrc, sFVector * apSrc, sFVector * apDst );
void	FMatrix_ApplyInv( sFMatrix * apSrc, sFVector * apSrc, sFVector * apDst );
void	FMatrix_ApplyInvAxes( sFMatrix * apSrc, sFVector * apSrc, sFVector * apDst );
void	FMatrix_ApplyPers( sFMatrix * apSrc, sFVector * apSrc, sFVector * apDst );


/* ################################################################################ */

#endif	/* INCLUDED_VECTOR_H */