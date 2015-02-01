#ifndef	INCLUDED_AMIXER_H
#define	INCLUDED_AMIXER_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/AUDIO/AUDIO.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eAMIXER_PAN_LEFT   = 0x00,
	eAMIXER_PAN_CENTRE = 0x80,
	eAMIXER_PAN_RIGHT  = 0xFF,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sAmixerSpl
{
	void *	mpStart;
	void *	mpCurrent;
	U32		mLength;
	U8		mGainLeft;
	U8		mGainRight;
	U8		mVolume;
	U8		mActiveFlag;
} sAmixerSpl;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	AudioMixer_Init( void );
void	AudioMixer_DeInit( void );

void	AudioMixer_Enable( void );
void	AudioMixer_Disable( void );

U8		AudioMixer_CanPlayNewSpl( void );
U8		AudioMixer_PlaySample( sAudioDmaSound * apSpl, const U8 aPan );
U8		AudioMixer_PlaySampleDirect( sAudioDmaSound * apSpl, const U8 aPan );
U8 *	AudioMixer_GetpBuffer( void );

/* ################################################################################ */

#endif	/*	INCLUDED_AMIXER_H	*/
