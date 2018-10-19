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

/*
	http://www.cs.cmu.edu/~music/icm-online/readings/panlaws/

	eMixer_PanType_Linear			[ cheap, but inaccurate. quietens in centre ]
		gain(left) + gain(right) = 1
		gain(right) = 1 - gain(left)

	eMixer_PanType_ConstantPower	[ better sound balance, but slightly boosts centre]
		gain(left) = cos(theta)
		gain(right = cos(theta)
		where theta = 0-90 degrees representing angle from listener to sound

	eMixer_PanType_PanLaw			[ best type, but more expensive]
		gain(left)  = sqrt( (pi/2 -theta) * (2/pi) * cos(theta)
		gain(right) = sqrt ( theta * (2/pi) * sin theta )

*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"AMIXER.H"

#include	"AM_MUL.H"
#include	"AM_SINE.H"
#include	"AM_COS.H"
#include	"AMPANLAW.H"

#include	<GODLIB/ASSERT/ASSERT.H>
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

#define	dAMIXER_SILENCE_SIZE	(4*1024)
#define	dAMIXER_CHANNEL_LIMIT	2


/* ###################################################################################
#  DATA
################################################################################### */

U16		gAudioMixerDMAHardWareFlag;
/*U8		gAudioMixerSineLaw[ 256 ];*/
U32		gAudioMixerChunkSize;
U32		gAudioMixerOffset;

sAmixerConfig	gAudioMixerConfig;
sAmixerSpl		gAudioMixerSilenceSpl;

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
#ifdef dGODLIB_PLATFORM_WIN
		gAudioMixerDMAHardWareFlag = 1;
#else
		gAudioMixerDMAHardWareFlag = 0;
#endif
		break;
	}

	gAudioMixerConfig.mBits			= eAUDIO_BITS_8;
	gAudioMixerConfig.mFrequency	= eAUDIO_FREQ_12;
	gAudioMixerConfig.mPanType		= eMixer_PanType_Linear;
	gAudioMixerConfig.mStereoFlag = 1;

	gpAudioMixerSilence        = (U8*)mMEMSCREENCALLOC( dAMIXER_SILENCE_SIZE );
	Memory_Clear( ( dAMIXER_SILENCE_SIZE ), gpAudioMixerSilence );
	gpAudioMixerBuffer         = (U8*)mMEMSCREENCALLOC( dAMIXER_BUFFER_SIZE+(2*1024) );
/*	gpAudioMixerMulTable       = (U8*)mMEMCALLOC( 0x10000L );*/
	gAudioMixerBufferOffset    = (4*1024);
	gAudioMixerLockFlag        = 0;

	gpAudioMixerMulTable = &AM_MUL_Array[0];


	gAudioMixerSilenceSpl.mActiveFlag = 0;
	gAudioMixerSilenceSpl.mpCurrent = gpAudioMixerSilence;
	gAudioMixerSilenceSpl.mpStart = gpAudioMixerSilence;
	gAudioMixerSilenceSpl.mLength = dAMIXER_SILENCE_SIZE;
	gAudioMixerSilenceSpl.mpEnd = ( (U8*)gAudioMixerSilenceSpl.mpStart ) + gAudioMixerSilenceSpl.mLength;

	for( i=0; i<dAMIXER_CHANNEL_LIMIT; i++ )
		gAudioMixerSamples[ i ] = gAudioMixerSilenceSpl;

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
	U32 lFreq = Audio_GetFrequency( gAudioMixerConfig.mFrequency );

	if( gAudioMixerConfig.mStereoFlag )
		lFreq <<= 1;
	lFreq /= 50;						/* amount to process per VBL*/
	gAudioMixerChunkSize = lFreq * 2;	/* process up to 2 VBLs each iteration*/
	gAudioMixerOffset = lFreq / 2;	/* offset from DMA ptr to begin mixing at*/

	if( (gAudioMixerDMAHardWareFlag) && (!gAudioMixerEnableFlag) )
	{
		lSound.mBits        = gAudioMixerConfig.mBits;
		lSound.mFreq        = gAudioMixerConfig.mFrequency;
		lSound.mLength      = dAMIXER_BUFFER_SIZE;
		lSound.mLoopingFlag = 1;
		lSound.mStereoFlag  = gAudioMixerConfig.mStereoFlag;
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

	gAudioMixerEnableFlag = 0;
	if( gAudioMixerEnableFlag )
	{
#ifdef dGODLIB_PLATFORM_ATARI
		U8 *	lpCR;
		U8		lOff;
		lpCR                  = (U8*)0xFFFF8901L;
		lOff                  = *lpCR;
		lOff                 &= 0xFE;
		*lpCR                 = lOff;
#endif
		Vbl_RemoveCall( AudioMixer_Vbl );
	}
}

sAmixerConfig *	AudioMixer_GetpConfig( void )
{
	return( &gAudioMixerConfig );
}

void			AudioMixer_SetConfig( const sAmixerConfig * apConfig )
{
	AudioMixer_Disable();
	gAudioMixerConfig = *apConfig;
	AudioMixer_Enable();
}


void	AudioMixer_MixerSampleActivate( sAmixerSpl * apMix, const sAudioDmaSound * apSpl, U8 aPan )
{
	apMix->mActiveFlag = 0;
	apMix->mLength = apSpl->mLength;
	apMix->mpStart = apSpl->mpSound;
	apMix->mpEnd   = ((U8*)apSpl->mpSound) + apSpl->mLength;
	switch( gAudioMixerConfig.mPanType )
	{
	case eMixer_PanType_Linear:
		apMix->mGainLeft = 0xFF - aPan;
		apMix->mGainRight = aPan;
		break;
	case eMixer_PanType_ConstantPower:
		apMix->mGainLeft = gAudioMixerSineLaw[ aPan ];
		apMix->mGainRight = aPan;
		break;
	case eMixer_PanType_PanLaw:
		apMix->mGainLeft = gAudioMixerPanLawLeftTable[ aPan ];
		apMix->mGainRight = gAudioMixerPanLawRightTable[ aPan ];
		break;
	}

	apMix->mpCurrent = apSpl->mpSound;
	apMix->mGainLeft = gAudioMixerSineLaw[ aPan ];
	apMix->mGainRight = aPan;
	apMix->mVolume = 0xFF;
	apMix->mActiveFlag = 1;

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
				AudioMixer_MixerSampleActivate( lpMix, apSpl, aPan );
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
	sAmixerSpl *	lpMix = 0;
	U32				lLeft;
	U32				lBest;
	U16				i;

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
		AudioMixer_MixerSampleActivate( lpMix, apSpl, aPan );
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

void	AudioMixer_MixIt( sAmixerSpl * apSpl0, sAmixerSpl * apSpl1, U8 * apDst, U32 aByteCount )
{
	(void)apSpl0;
	(void)apSpl1;
	(void)apDst;
	(void)aByteCount;
#if 0
	U32 i;
	U32	lCount = aByteCount >> 1;
	U8 * lpSrc0 = apSpl0->mpCurrent;
	U8 * lpSrc1 = apSpl1->mpCurrent;

	GODLIB_ASSERT( apSpl0->mpCurrent + lCount <= apSpl0->mpEnd );
	GODLIB_ASSERT( apSpl1->mpCurrent + lCount <= apSpl1->mpEnd );

	for( i = 0; i < lCount; i++ )
	{
/*
		*apDst++ = *lpSrc0++;
		*apDst++ = *lpSrc1++;
*/
		(void)lpSrc0;
		(void)lpSrc1;
		*apDst++ = 0x80;
		*apDst++ = 0x40;
	}
#endif
}

typedef struct sAmixerOp
{
	sAmixerSpl *	mpSample;
	U32				mRemainingByteCount;
} sAmixerOp;

void	AudioMixer_Slow()
{
	sAmixerSpl * spl0 = &gAudioMixerSamples[ 0 ];
	sAmixerSpl * spl1 = &gAudioMixerSamples[ 1 ];
	sAmixerOp	src[ 2 ];
	U32			ringSize;

	if( !gAudioMixerEnableFlag )
		return;

	src[ 0 ].mpSample = spl0;
	src[ 0 ].mRemainingByteCount = (U32)spl0->mpEnd - (U32)spl0->mpCurrent;

	src[ 1 ].mpSample = spl1;
	src[ 1 ].mRemainingByteCount = (U32)spl1->mpEnd - (U32)spl1->mpCurrent;

#ifdef dGODLIB_PLATFORM_ATARI
	{
		U32 lDMA = 0;
		U8 * lpDMA = (U8*)0xFFFF8909L;
		lDMA = lpDMA[ 0 ];
		lDMA <<= 8;
		lDMA |= lpDMA[ 2 ];
		lDMA <<= 8;
		lDMA |= lpDMA[ 4 ];

		lDMA -= (U32)gpAudioMixerBuffer;
		lDMA &= ( dAMIXER_BUFFER_SIZE - 1 );
		lDMA += gAudioMixerChunkSize;
		if( lDMA < gAudioMixerOffset )
			ringSize = ( dAMIXER_BUFFER_SIZE - gAudioMixerOffset ) + lDMA;
		else
			ringSize = lDMA - gAudioMixerOffset;
	}
#else
	ringSize = gAudioMixerChunkSize;

#endif // dGODLIB_PLATFORM_ATARI
	ringSize = gAudioMixerChunkSize;


	while( ringSize )
	{
		U32 lMixBytes = ringSize;
		U32	lLeft = dAMIXER_BUFFER_SIZE - gAudioMixerOffset;

		if( !lLeft )
		{
			lLeft = dAMIXER_BUFFER_SIZE;
			gAudioMixerOffset = 0;
		}

		if( lLeft < lMixBytes )
			lMixBytes = lLeft;

		if( src[ 0 ].mRemainingByteCount < lMixBytes )
			lMixBytes = src[ 0 ].mRemainingByteCount;
		if( src[ 1 ].mRemainingByteCount < lMixBytes )
			lMixBytes = src[ 1 ].mRemainingByteCount;

		AudioMixer_MixIt( &gAudioMixerSamples[ 0 ], &gAudioMixerSamples[ 1 ], gpAudioMixerBuffer + gAudioMixerOffset, lMixBytes );

		gAudioMixerOffset += lMixBytes;
		src[ 0 ].mRemainingByteCount -= lMixBytes;
		src[ 0 ].mpSample->mpCurrent += lMixBytes;

		src[ 1 ].mRemainingByteCount -= lMixBytes;
		src[ 1 ].mpSample->mpCurrent += lMixBytes;

		if( !src[ 0 ].mRemainingByteCount )
		{
			*src[ 0 ].mpSample = gAudioMixerSilenceSpl;
			src[ 0 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;
		}
		if( !src[ 1 ].mRemainingByteCount )
		{
			*src[ 1 ].mpSample = gAudioMixerSilenceSpl;
			src[ 1 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;
		}

		ringSize -= lMixBytes;
	}


#if 0

	S32	ringBufferBytesLeft = dAMIXER_BUFFER_SIZE - gAudioMixerOffset;
	sAmixerSpl * spl0 = &gAudioMixerSamples[ 0 ];
	sAmixerSpl * spl1 = &gAudioMixerSamples[ 1 ];
	sAmixerOp	src[ 3 ];
	S32			ringSizes[ 2 ];
	U16			i;


	src[ 0 ].mpSample = spl0;
	src[ 0 ].mRemainingByteCount = (U32)spl0->mpEnd - (U32)spl0->mpCurrent;

	src[ 1 ].mpSample = spl1;
	src[ 1 ].mRemainingByteCount = (U32)spl1->mpEnd - (U32)spl1->mpCurrent;

	if( src[ 1 ].mRemainingByteCount < src[ 0 ].mRemainingByteCount )
	{
		src[ 2 ] = src[ 0 ];
		src[ 0 ] = src[ 1 ];
		src[ 1 ] = src[ 2 ];
	}

	ringSizes[ 0 ] = gAudioMixerChunkSize;

	if( ringBufferBytesLeft < (S32)gAudioMixerChunkSize )
		ringSizes[ 0 ] = ringBufferBytesLeft;

	ringSizes[ 1 ] = gAudioMixerChunkSize - ringSizes[0];

	for( i = 0; i < 2; i++ )
	{
		S32 ringSize = ringSizes[ i ];
		S32	left = ringSize - (S32)src[ 0 ].mRemainingByteCount;
		S32 left2;
		if( !ringSize )
			continue;

		if( i )
			gAudioMixerOffset = 0;

		while( ringSize )
		{
			U32 lMixBytes = ringSize;

			if( src[ 0 ].mRemainingByteCount < lMixBytes )
				lMixBytes = src[ 0 ].mRemainingByteCount;
			if( src[ 1 ].mRemainingByteCount < lMixBytes )
				lMixBytes = src[ 1 ].mRemainingByteCount;

			AudioMixer_MixIt( src[ 0 ].mpSample, src[ 0 ].mpSample, gpAudioMixerBuffer + gAudioMixerOffset, lMixBytes );

			gAudioMixerOffset += lMixBytes;
			src[ 0 ].mRemainingByteCount -= lMixBytes;
			src[ 0 ].mpSample->mpCurrent += lMixBytes;

			src[ 1 ].mRemainingByteCount -= lMixBytes;
			src[ 1 ].mpSample->mpCurrent += lMixBytes;

			if( !src[ 0 ].mRemainingByteCount )
			{
				*src[ 0 ].mpSample = gAudioMixerSilenceSpl;
				src[ 0 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;
			}
			if( !src[ 1 ].mRemainingByteCount )
			{
				*src[ 1 ].mpSample = gAudioMixerSilenceSpl;
				src[ 1 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;
			}

			ringSize -= lMixBytes;
		}
		if( left > 0 )
		{
			AudioMixer_MixIt( src[ 0 ].mpSample, src[ 0 ].mpSample, gpAudioMixerBuffer + gAudioMixerOffset, src[ 0 ].mRemainingByteCount );

			ringSize -= src[ 0 ].mRemainingByteCount;

			if( src[ 1 ].mpSample->mActiveFlag )
			{
				src[ 1 ].mpSample->mpCurrent += src[ 0 ].mRemainingByteCount;
				src[ 1 ].mRemainingByteCount -= src[ 0 ].mRemainingByteCount;
			}

			*src[ 0 ].mpSample = gAudioMixerSilenceSpl;
			src[ 0 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;

			gAudioMixerOffset += src[ 0 ].mRemainingByteCount;
			GODLIB_ASSERT( src[ 0 ].mpSample->mpCurrent <= src[ 0 ].mpSample->mpEnd );
			GODLIB_ASSERT( src[ 1 ].mpSample->mpCurrent <= src[ 1 ].mpSample->mpEnd );
		}

		left2 = ringSize - (S32)src[ 1 ].mRemainingByteCount;
		if( left2 > 0 )
		{
			AudioMixer_MixIt( src[ 0 ].mpSample, src[ 0 ].mpSample, gpAudioMixerBuffer + gAudioMixerOffset, src[ 1 ].mRemainingByteCount );

			ringSize -= src[ 0 ].mRemainingByteCount;

			if( src[ 0 ].mpSample->mActiveFlag )
			{
				src[ 0 ].mpSample->mpCurrent += src[ 1 ].mRemainingByteCount;
				src[ 0 ].mRemainingByteCount -= src[ 1 ].mRemainingByteCount;
			}

			*src[ 1 ].mpSample = gAudioMixerSilenceSpl;
			src[ 1 ].mRemainingByteCount = gAudioMixerSilenceSpl.mLength;

			gAudioMixerOffset += src[ 1 ].mRemainingByteCount;
			GODLIB_ASSERT( src[ 0 ].mpSample->mpCurrent <= src[ 0 ].mpSample->mpEnd );
			GODLIB_ASSERT( src[ 1 ].mpSample->mpCurrent <= src[ 1 ].mpSample->mpEnd );
		}

		if( ringSize > 0 )
		{
			AudioMixer_MixIt( src[ 0 ].mpSample, src[ 0 ].mpSample, gpAudioMixerBuffer + gAudioMixerOffset, ringSize );

			GODLIB_ASSERT( src[ 0 ].mpSample->mpCurrent <= src[ 0 ].mpSample->mpEnd );
			GODLIB_ASSERT( src[ 1 ].mpSample->mpCurrent <= src[ 1 ].mpSample->mpEnd );

			if( src[ 0 ].mpSample->mActiveFlag )
			{
				src[ 0 ].mpSample->mpCurrent += ringSize;
				if( src[ 0 ].mpSample->mpCurrent > src[ 0 ].mpSample->mpEnd )
					*src[ 0 ].mpSample = gAudioMixerSilenceSpl;
			}
			if( src[ 1 ].mpSample->mActiveFlag )
			{
				src[ 1 ].mpSample->mpCurrent += ringSize;
				if( src[ 1 ].mpSample->mpCurrent > src[ 1 ].mpSample->mpEnd )
					*src[ 1 ].mpSample = gAudioMixerSilenceSpl;
			}
			GODLIB_ASSERT( src[ 0 ].mpSample->mpCurrent <= src[ 0 ].mpSample->mpEnd );
			GODLIB_ASSERT( src[ 1 ].mpSample->mpCurrent <= src[ 1 ].mpSample->mpEnd );

			gAudioMixerOffset += ringSize;
		}
	}
#endif
}

#ifndef dGODLIB_PLATFORM_ATARI

void			AudioMixer_Vbl( void )
{
	AudioMixer_Slow();
}

#endif // !dGODLIB_PLATFORM_ATARI




#if 0

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

#endif

/* ################################################################################ */
