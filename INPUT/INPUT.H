#ifndef	INCLUDED_INPUT_H
#define	INCLUDED_INPUT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eINPUTTYPE_IKBD,
	eINPUTTYPE_MOUSE,
	eINPUTTYPE_JOY0,
	eINPUTTYPE_JOY1,
	eINPUTTYPE_PADA,
	eINPUTTYPE_PADB,

	eINPUTTYPE_TAP0_PADA,
	eINPUTTYPE_TAP0_PADB,
	eINPUTTYPE_TAP0_PADC,
	eINPUTTYPE_TAP0_PADD,

	eINPUTTYPE_TAP1_PADA,
	eINPUTTYPE_TAP1_PADB,
	eINPUTTYPE_TAP1_PADC,
	eINPUTTYPE_TAP1_PADD,

	eINPUTTYPE_AI,

	eINPUTTYPE_NONE,

	eINPUTTYPE_LIMIT
};

enum
{
	eINPUTKEYSTATUS_NONE	=	0,
	eINPUTKEYSTATUS_HELD	=	(1<<0),
	eINPUTKEYSTATUS_HIT		=	(1<<1),
	eINPUTKEYSTATUS_UNHIT	=	(1<<2)
};

enum
{
	eINPUTKEY_LEFT,
	eINPUTKEY_RIGHT,
	eINPUTKEY_DOWN,
	eINPUTKEY_UP,
	
	eINPUTKEY_FIREA,
	eINPUTKEY_FIREB,
	eINPUTKEY_PAUSE,
	eINPUTKEY_OPTION,
	eINPUTKEY_QUIT,

	eINPUTKEY_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef struct sInput
{
	U16	mInputType;
	S16	mX;
	S16	mY;
	U16	mMovedFlag;
	U8	mKeyStatus[ eINPUTKEY_LIMIT ];
	U8	mScanCodes[ eINPUTKEY_LIMIT ];
	U8	mRepeatCounts[ eINPUTKEY_LIMIT ];
} sInput;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Input_Init( sInput * apInput );
void	Input_Update( sInput * apLayout );

void	Input_CombinedInit( sInput * apDst, sInput * apSrc, const U16 aCount );
void	Input_CombinedUpdate( sInput * apDst, sInput * apSrc, const U16 aCount );
void	Input_Combine( sInput * apDst, const sInput * apSrc0, const sInput * apSrc1 );
void	Input_CombineFire( sInput * apDst, const sInput * apSrc0, const sInput * apSrc1 );
void	Input_DirClear( sInput * apInput );
char *	Input_GetTypeName( const sInput * apInput );
void	Input_BuildHeldString( const sInput * apInput, char * apOutput );
void	Input_BuildHitString( const sInput * apInput, char * apOutput );
void	Input_BuildUnHitString( const sInput * apInput, char * apOutput );


/* ################################################################################ */

#endif	/*	INCLUDED_INPUT_H	*/
