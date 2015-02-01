/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: AUDIO.C
::
:: Low level audio routines
::
:: This file contains all the platform specific audio routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"AMIXER.H"

#include	"AM_MUL.H"
#include	"AM_SINE.H"

#include	<GODLIB/AUDIO/AUDIO.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/MFP/MFP.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>

#ifndef dGODLIB_COMPILER_AHCC
#include	<MATH.H>
#endif

/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dAMIXER_BUFFER_SIZE		(8*1024)
#define	dAMIXER_CHANNEL_LIMIT	2


/* ###################################################################################
#  DATA
################################################################################### */

U16		gAudioMixerDMAHardWareFlag;
/*U8		gAudioMixerSineLaw[ 256 ];*/

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	U8 *		gpAudioMixerSilence;
extern	U8 *		gpAudioMixerBuffer;
extern	U8 *		gpAudioMixerMulTable;
extern	U32			gAudioMixerBufferOffset;
extern	U8			gAudioMixerLockFlag;
extern	U8			gAudioMixerEnableFlag;
extern	sAmixerSpl	gAudioMixerSamples[ dAMIXER_CHANNEL_LIMIT ];

extern	void		AudioMixer_Vbl( void );

void	AudioMixer_TablesBuild( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_Init( void )
* ACTION   : inits mixer
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	AudioMixer_Init( void )
{
	S16		i;

	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
	case	MCH_FALCON:
		gAudioMixerDMAHardWareFlag = 1;
		break;
	default:
		gAudioMixerDMAHardWareFlag = 0;
		break;
	}

	gpAudioMixerSilence        = (U8*)mMEMSCREENCALLOC( 4*1024 );
	Memory_Clear( (4*1024), gpAudioMixerSilence );
	gpAudioMixerBuffer         = (U8*)mMEMSCREENCALLOC( dAMIXER_BUFFER_SIZE+(2*1024) );
/*	gpAudioMixerMulTable       = (U8*)mMEMCALLOC( 0x10000L );*/
	gAudioMixerBufferOffset    = (4*1024);
	gAudioMixerLockFlag        = 0;

	gpAudioMixerMulTable = &AM_MUL_Array[0];


	for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
	{
		gAudioMixerSamples[ i ].mActiveFlag = 0;
		gAudioMixerSamples[ i ].mpCurrent   = gpAudioMixerSilence;
		gAudioMixerSamples[ i ].mpStart     = gpAudioMixerSilence;
		gAudioMixerSamples[ i ].mLength     = 1024;
	}

/*
	AudioMixer_TablesBuild();
*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_DeInit( void )
* ACTION   : deinits mixer
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	AudioMixer_DeInit( void )
{
	U16	i;

	AudioMixer_Disable();
	for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
	{
		gAudioMixerSamples[ i ].mActiveFlag = 0;
		gAudioMixerSamples[ i ].mpCurrent   = gpAudioMixerSilence;
	}
	mMEMSCREENFREE( gpAudioMixerSilence );
	gpAudioMixerSilence = 0;
	mMEMSCREENFREE( gpAudioMixerBuffer  );
	gpAudioMixerBuffer = 0;
/*	mMEMFREE( gpAudioMixerMulTable );
	gpAudioMixerMulTable = 0;*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_Enable( void )
* ACTION   : enables mixer
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	AudioMixer_Enable( void )
{
	sAudioDmaSound	lSound;
	U16				i;

	if( (gAudioMixerDMAHardWareFlag) && (!gAudioMixerEnableFlag) )
	{
		lSound.mBits        = eAUDIO_BITS_8;
		lSound.mFreq        = eAUDIO_FREQ_50;
		lSound.mLength      = dAMIXER_BUFFER_SIZE;
		lSound.mLoopingFlag = 1;
		lSound.mStereoFlag  = 1;
		lSound.mpSound      = gpAudioMixerBuffer;


		Vbl_AddCall( AudioMixer_Vbl );
		gAudioMixerBufferOffset    = (4*1024);

		for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
		{
			gAudioMixerSamples[ i ].mActiveFlag = 0;
			gAudioMixerSamples[ i ].mpCurrent   = gpAudioMixerSilence;
		}

		gAudioMixerEnableFlag      = 1;
		Vbl_WaitVbl();
		Audio_DmaPlaySound( &lSound );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_Disable( void )
* ACTION   : disables mixer
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	AudioMixer_Disable( void )
{
	U8 *	lpCR;
	U8		lOff;

	if( gAudioMixerEnableFlag )
	{
		gAudioMixerEnableFlag = 0;
		lpCR                  = (U8*)0xFFFF8901L;
		lOff                  = *lpCR;
		lOff                 &= 0xFE;
		*lpCR                 = lOff;

		Vbl_RemoveCall( AudioMixer_Vbl );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_PlaySample( sAudioDmaSound * apSpl, const U8 aPan )
* ACTION   : adds sample to queue
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8		AudioMixer_PlaySample( sAudioDmaSound * apSpl, const U8 aPan )
{
	sAmixerSpl *	lpMix;
	U16				i;

	if( apSpl )
	{
		for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
		{
			lpMix = &gAudioMixerSamples[ i ];

			if( !lpMix->mActiveFlag )
			{
				lpMix->mActiveFlag = 0;
				lpMix->mLength     = apSpl->mLength;
				lpMix->mpStart     = apSpl->mpSound;
				lpMix->mpCurrent   = apSpl->mpSound;
				lpMix->mGainLeft   = gAudioMixerSineLaw[ aPan ];
				lpMix->mGainRight  = aPan;
				lpMix->mVolume     = 0xFF;
				lpMix->mActiveFlag = 1;
				return( 1 );
			}
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_PlaySampleDirect( sAudioDmaSound * apSpl, const U8 aPan )
* ACTION   : plays sample without queueing
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8		AudioMixer_PlaySampleDirect( sAudioDmaSound * apSpl, const U8 aPan )
{
	sAmixerSpl *	lpMix;
	U32				lLeft;
	U32				lBest;
	U16				i;

	lpMix = (sAmixerSpl*)0;
	lBest = 0x7FFFFFFFL;
	
	for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
	{
		if( !gAudioMixerSamples[ i ].mActiveFlag )
		{
			lpMix = &gAudioMixerSamples[ i ];
			lBest = 0;
		}
		else
		{
			lLeft  = (U32)gAudioMixerSamples[i].mpStart;
			lLeft += gAudioMixerSamples[i].mLength;
			lLeft -= (U32)gAudioMixerSamples[i].mpCurrent;

			if( lLeft < lBest )
			{
				lBest = lLeft;
				lpMix = &gAudioMixerSamples[ i ];
			}
		}
	}

	if( lpMix )
	{
		lpMix->mActiveFlag = 0;
		lpMix->mLength     = apSpl->mLength;
		lpMix->mpStart     = apSpl->mpSound;
		lpMix->mpCurrent   = apSpl->mpSound;
		lpMix->mGainLeft   = gAudioMixerSineLaw[ aPan ];
		lpMix->mGainRight  = aPan;
		lpMix->mVolume     = 0xFF;
		lpMix->mActiveFlag = 1;
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_CanPlayNewSpl( void )
* ACTION   : returns true if channels are available to play new sample on
* CREATION : 15.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8		AudioMixer_CanPlayNewSpl( void )
{
	U16	i;

	for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
	{
		if( !gAudioMixerSamples[ i ].mActiveFlag )
		{
			return( 1 );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AudioMixer_GetpBuffer( void )
* ACTION   : AudioMixer_GetpBuffer
* CREATION : 23.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U8 *	AudioMixer_GetpBuffer( void )
{
	return(	gpAudioMixerBuffer );
}


void	AudioMixer_TablesBuild( void )
{
	S8 * lpTable;
	U8	lTable[ 256 ];
	S16		i,j;
	S16		lX,lY;
#ifndef dGODLIB_COMPILER_AHCC
	FP32	lA,lB,lC;
#endif

#ifndef dGODLIB_COMPILER_AHCC

	for( i=0; i<256; i++ )
	{
		switch( i )
		{
		case	0:
			lTable[ i ] = 255;
			break;
		case	1:
			lTable[ i ] = 254;
			break;
		case	254:
			lTable[ i ] = 1;
			break;
		case	255:
			lTable[ i ] = 0;
			break;
		default:
			lA  = (FP32)i;
			lA /= 255.f;
			lB  = 1.f - ( lA * lA );
			lB  = (FP32)sqrt( lB);
			lC  = lA * lA;
			lC += lB * lB;
			lB *= 255.f;
			lTable[ i ] = (U8)lB;

			break;
		}
	}

	File_Save( "AM_SINE.DAT", &lTable[ 0 ], 256 );

#endif

	lpTable = (S8*)mMEMCALLOC( 0x10000L );

	if( lpTable )
	{
		S8 * lpEntry = lpTable;
		for( i=0; i<256; i++ )
		{
			lX   = i;
			for( j=0; j<256; j++ )
			{
				lY   = j;
				if( j>= 128 )
				{
					lY = (S16)(-128 +(j-128));
				}
				lY   = (S16)(lY *lX);
				lY >>= 8;
				*lpEntry++ = (S8)(lY);
			}
		}

		File_Save( "AM_MUL.DAT", lpTable, 0x10000L );

		mMEMFREE( lpTable );

	}

}

/* ################################################################################ */
