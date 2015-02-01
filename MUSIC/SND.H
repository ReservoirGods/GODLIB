#ifndef	INCLUDED_SND_H
#define	INCLUDED_SND_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUM
################################################################################### */

enum
{
	eSND_TIMER_A,
	eSND_TIMER_B,
	eSND_TIMER_C,
	eSND_TIMER_D,
	eSND_TIMER_VBI,

	eSND_TIMER_LIMIT
};


/* ###################################################################################
#  TYPEDEFS
################################################################################### */

typedef	void	(*fSnd)( void );


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct
{
	U16	mInstruction;
	U16	mOffset;
} sSndJump;

typedef	struct
{
	U16	mType;
	U16	mFreq;
	U8	mDiv;
	U8	mData;
} sSndTimer;

typedef	struct
{
	sSndJump	mStart;
	sSndJump	mStop;
	sSndJump	mPlay;
	U32			mID;
	char		mData[ 1 ];
} sSndHeader;

typedef	struct
{
	sSndHeader *	mpHeader;
	char *			mpComposerTxt;
	char *			mpTitleTxt;
	char *			mpRipperTxt;
	char *			mpConversionTxt;
	fSnd			mStart;
	fSnd			mStop;
	fSnd			mPlay;
	sSndTimer		mTimer;
} sSndInfo;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Snd_Init( void );
void	Snd_DeInit( void );

void	Snd_GetInfo( sSndHeader * apHeader, sSndInfo * apInfo );

void	Snd_TuneInit( sSndInfo * apInfo );
void	Snd_TuneDeInit( sSndInfo * apInfo );

void	Snd_SetpPlayFunc( void (*aPlayFunc)(void) );
void	Snd_SetpChaserFunc( void (*aChaserFunc)(void) );


/* ################################################################################ */

#endif	/*	INCLUDED_SND_H */
