#ifndef	INCLUDED_AMIXER_H
#define	INCLUDED_AMIXER_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/AUDIO/AUDIO.H>

/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dAMIXER_BUFFER_SIZE		(8*1024)

/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eAMIXER_PAN_LEFT   = 0x00,
	eAMIXER_PAN_CENTRE = 0x80,
	eAMIXER_PAN_RIGHT  = 0xFF,
};

enum
{
	eMixer_PanType_Linear,
	eMixer_PanType_ConstantPower,
	eMixer_PanType_PanLaw,

	eMixer_PanType_Limit
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sAmixerSpl
{
	U8 *	mpStart;
	U8 *	mpCurrent;
	U8 *	mpEnd;
	U32		mLength;
	U8		mGainLeft;
	U8		mGainRight;
	U8		mVolume;
	U8		mActiveFlag;
} sAmixerSpl;


typedef struct sAmixerConfig
{
	U8	mBits;
	U8	mFrequency;
	U8	mPanType;
	U8	mStereoFlag;
} sAmixerConfig;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	AudioMixer_Init( void );
void	AudioMixer_DeInit( void );

void	AudioMixer_Enable( void );
void	AudioMixer_Disable( void );

sAmixerConfig *	AudioMixer_GetpConfig( void );
void			AudioMixer_SetConfig( const sAmixerConfig * apConfig );


U8		AudioMixer_CanPlayNewSpl( void );
U8		AudioMixer_PlaySample( sAudioDmaSound * apSpl, const U8 aPan );
U8		AudioMixer_PlaySampleDirect( sAudioDmaSound * apSpl, const U8 aPan );

U8 *	AudioMixer_GetpBuffer( void );
#define	AudioMixer_GetBufferSize()	dAMIXER_BUFFER_SIZE

/* ################################################################################ */

#endif	/*	INCLUDED_AMIXER_H	*/
