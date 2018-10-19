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
##############DMA Sound System                                     ###########
------- +----- +-------------------------------------------------------+----------
$FF8900 | byte | Buffer interrupts                        BIT 3 2 1 0  |R/W (F030)
        |      | TimerA-Int at end of record buffer-----------' | | |  |
        |      | TimerA-Int at end of replay buffer-------------' | |  |
        |      | MFP-15-Int (I7) at end of record buffer----------' |  |
        |      | MFP-15-Int (I7) at end of replay buffer------------'  |
------- +----- +-------------------------------------------------------+----------
$FF8901 | byte | DMA Control Register              BIT 7 . 5 4 . . 1 0 |R/W
        |      | 1 - select record register -----------+   | |     | | |    (F030)
        |      | 0 - select replay register -----------'   | |     | | |    (F030)
        |      | Loop record buffer -----------------------' |     | | |    (F030)
        |      | DMA Record on ------------------------------'     | | |    (F030)
        |      | Loop replay buffer -------------------------------' | |     (STe)
        |      | DMA Replay on --------------------------------------' |     (STe)
--------+------+-------------------------------------------------------+----------
$FF8903 | byte | Frame start address( high byte )                      |R/W  (STe)
$FF8905 | byte | Frame start address( mid byte )                       |R/W  (STe)
$FF8907 | byte | Frame start address( low byte )                       |R/W  (STe)
$FF8909 | byte | Frame address counter( high byte )                    |R    (STe)
$FF890B | byte | Frame address counter( mid byte )                     |R    (STe)
$FF890D | byte | Frame address counter( low byte )                     |R    (STe)
$FF890F | byte | Frame end address( high byte )                        |R/W  (STe)
$FF8911 | byte | Frame end address( mid byte )                         |R/W  (STe)
$FF8913 | byte | Frame end address( low byte )                         |R/W  (STe)
------- +----- +---------------------------------------------------- - +----------
$FF8920 | byte | DMA Track Control                     BIT 5 4 . . 1 0 |R/W (F030)
        |      | 00 - Set DAC to Track 0 ------------------+-+     | | |
        |      | 01 - Set DAC to Track 1 ------------------+ +     | | |
        |      | 10 - Set DAC to Track 2 ------------------+-+     | | |
        |      | 11 - Set DAC to Track 3 ------------------+-'     | | |
        |      | 00 - Play 1 Track---------------------------------+-+ |
        |      | 01 - Play 2 Tracks--------------------------------+-+ |
        |      | 10 - Play 3 Tracks--------------------------------+-+ |
        |      | 11 - Play 4 Tracks--------------------------------+-' |
--------+------+-------------------------------------------------------+----------
$FF8921 | byte | Sound mode control                BIT 7 6 . . . . 1 0 |R/W  (STe)
        |      | 0 - Stereo, 1 - Mono -----------------' |         | | |
        |      | 0 - 8bit------------------------------- +         | | |
        |      | 1 - 16bit( F030 only )------------------'         | | |    (F030)
        |      | Frequency control bits                            | | |
        |      | 00 - Off( F030 only ) ----------------------------+-+ |    (F030)
        |      | 00 - 6258hz frequency( STe only ) --------------- +-+ |
        |      | 01 - 12517hz frequency -------------------------- +-+ |
        |      | 10 - 25033hz frequency -------------------------- +-+ |
        |      | 11 - 50066hz frequency -------------------------- +-' |
        |      | Samples are always signed.In stereo mode, data is     |
        |      | arranged in pairs with high pair the left channel, low|
        |      | pair right channel.Sample length MUST be even in      |
        |      | either mono or stereo mode.                           |
        |      | Example: 8 bit Stereo : LRLRLRLRLRLRLRLR              |
        |      | 16 bit Stereo : LLRRLLRRLLRRLLRR( F030 )              |
        |      | 2 track 16 bit stereo : LLRRllrrLLRRllrr( F030 )      |
--------+------+---------------------------------------------------- - +----------
*/
/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"AUDIO.H"

#include	<GODLIB/MFP/MFP.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

#define	dAUDIO_MW_MASK			0x7FF
#define	dAUDIO_MW_CMD_LEFTVOL	0x540
#define	dAUDIO_MW_CMD_RIGHTVOL	0x500
#define	dAUDIO_MW_CMD_TREBLE	0x480
#define	dAUDIO_MW_CMD_BASS		0x440
#define	dAUDIO_MW_CMD_MIXER		0x400


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8	mKeyClick;
	U8	mInternalSpeaker;
} sAudioState;


/* ###################################################################################
#  DATA
################################################################################### */

U16				gAudioDMAHardWareFlag;
sAudioState		gAudioOldState;
sAudioDmaSound	gAudioCurrentSound;
U8				gAudioSavedYM[ 16 ];
extern	U8		gAudioDmaPlayingFlag;

U32				gAudioFrequencies[ eAUDIO_FREQ_LIMIT  ] =
{
	6258,
	12517,
	25033,
	50066
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	void	Audio_MicroWireWrite( const U16 aValue );

extern	void	Audio_ReplayEndInt( void );
extern	void	Audio_SaveYM( void );
extern	void	Audio_RestoreYM( void );
extern	void	Audio_SaveDmaSte( void );
extern	void	Audio_RestoreDmaSte( void );
extern	void	Audio_SaveDmaFalcon( void );
extern	void	Audio_RestoreDmaFalcon( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_Init( void )
* ACTION   : inits audio
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_Init( void )
{
	gAudioDMAHardWareFlag = 0;

	Audio_SaveYM();
	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		Audio_SaveDmaSte();
		gAudioDMAHardWareFlag = 1;
		Audio_MicroWireWrite( 0 );
		break;
	case	MCH_FALCON:
		Audio_SaveDmaFalcon();
		gAudioDMAHardWareFlag = 1;
		break;
	default:
		break;
	}

	gAudioOldState.mKeyClick        = (U8)Audio_GetKeyClickState();
	gAudioOldState.mInternalSpeaker = (U8)Audio_GetInternalSpeakerState();

	Audio_SetKeyClickState( 0 );
	Audio_SetInternalSpeakerState( 1 );

	gAudioDmaPlayingFlag   = 0;
	Audio_SoundChipOff();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DeInit( void )
* ACTION   : deinits audio
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_DeInit( void )
{
	Audio_SetKeyClickState( gAudioOldState.mKeyClick );
	Audio_SetInternalSpeakerState( gAudioOldState.mInternalSpeaker );
	Audio_SoundChipOff();


	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		Audio_RestoreDmaSte();
		break;
	case	MCH_FALCON:
		Audio_RestoreDmaFalcon();
		break;
	default:
		break;
	}
	Audio_RestoreYM();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetKeyClickState( void )
* ACTION   : returns keyclick state
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

U16		Audio_GetKeyClickState( void )
{
#ifndef	dGODLIB_PLATFORM_ATARI
	return( 0 );
#else
	U8 *	lpKC;

	lpKC = (U8*)0x484L;

	return( *lpKC & 1 );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_SetKeyClickState( const U16 aState )
* ACTION   : set keyclick state
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_SetKeyClickState( const U16 aState )
{
#ifndef	dGODLIB_PLATFORM_ATARI
	(void)aState;
#else
	U8 *	lpKC;

	lpKC = (U8*)0x484L;

	*lpKC &= 0xFE;
	*lpKC |= (aState&1);
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetInternalSpeakerState( void )
* ACTION   : returns internal speaker state
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

U16		Audio_GetInternalSpeakerState( void )
{
#ifndef	dGODLIB_PLATFORM_ATARI
	return( 0 );
#else
	U8 *	lpPSG;

	lpPSG      = (U8*)0xFFFF8800L;
	lpPSG[ 0 ] = 14;

	return( (lpPSG[ 0 ]>>6) & 1 );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_SetInternalSpeakerState( const U16 aState )
* ACTION   : set internal speaker state
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_SetInternalSpeakerState( const U16 aState )
{
#ifndef	dGODLIB_PLATFORM_ATARI
	(void)aState;
#else
	U8 *	lpPSG;
	U8		lVal;

	lpPSG      = (U8*)0xFFFF8800L;
	lpPSG[ 0 ] = 14;

	lVal  = lpPSG[ 0 ];
	lVal &= 0xBF;
	lVal |= (aState&1)<<6;

	lpPSG[ 2 ] = lVal;
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaPlaySound( sAudioDmaSound * apSound )
* ACTION   : plays a sound defined by apSound
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_DmaPlaySound( sAudioDmaSound * apSound )
{
	U8 *	lpDMAc;
	U8 *	lpMode;
	U8 *	lpStart;
	U8 *	lpEnd;
	U32		lAdr;
	U8		lMode;
	U8		lDMAc;

	if( gAudioDMAHardWareFlag )
	{
		lpDMAc  = (U8*)0xFFFF8901L;
		lpMode  = (U8*)0xFFFF8921L;
		lpStart = (U8*)0xFFFF8903L;
		lpEnd   = (U8*)0xFFFF890FL;

		*lpDMAc &= 0xFE;

		lMode  = (U8)(((apSound->mStereoFlag^1)&1)<<7);
		lMode |= (apSound->mBits&1)<<6;
		lMode |= (apSound->mFreq&3);

		lDMAc  = (U8)(1 | ((apSound->mLoopingFlag&1)<<1));

		lAdr   = (U32)apSound->mpSound;

		lpStart[ 0 ] = (U8)((lAdr >> 16L)&0xFF);
		lpStart[ 2 ] = (U8)((lAdr >>  8L)&0xFF);
		lpStart[ 4 ] = (U8)((lAdr       )&0xFF);

		lAdr += apSound->mLength;

		lpEnd[ 0 ] = (U8)((lAdr >> 16L)&0xFF);
		lpEnd[ 2 ] = (U8)((lAdr >>  8L)&0xFF);
		lpEnd[ 4 ] = (U8)((lAdr       )&0xFF);

		gAudioDmaPlayingFlag   = 1;

		*lpMode  = lMode;
		*lpDMAc  = lDMAc;
	}

	gAudioCurrentSound     = *apSound;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaIsSoundPlaying( void )
* ACTION   : returns true if sound is playing
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

U8		Audio_DmaIsSoundPlaying( void )
{
	U8	lDMAc;

	if( gAudioDMAHardWareFlag )
	{
		lDMAc  = *(U8*)0xFFFF8901L;

		if( !(lDMAc & 1) )
		{
			return( 0 );
		}

		lDMAc  = (U8)(*(U8*)0xFFFF890FL - *(U8*)0xFFFF8909L);
		lDMAc |= *(U8*)0xFFFF8911L - *(U8*)0xFFFF890BL;
		lDMAc |= *(U8*)0xFFFF8913L - *(U8*)0xFFFF890DL;
	}
	else
	{
		lDMAc =0;
	}

	return( lDMAc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_ToggleSign( sAudioDmaSound * apSound )
* ACTION   : toggles audio between signed and unsigned
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_ToggleSign( sAudioDmaSound * apSound )
{
	U32	i;
	U8 *	lpSrc;
	U8		lSam;

	lpSrc = apSound->mpSound;

	if( lpSrc )
	{
		for( i=0; i<apSound->mLength; i++ )
		{
			lSam     = *lpSrc;
			lSam    += 0x80;
			*lpSrc++ = lSam;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_MaximiseVolume( sAudioDmaSound * apSound, U8 aAdd )
* ACTION   : maximises a sample volume. pass 0x80 to work with signed samples, 0 for unsigned
* CREATION : 13.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_MaximiseVolume( sAudioDmaSound * apSound, U8 aAdd )
{
	U8	lMax = 0;
	U8	lMin = 0xFF;
	U32 i;
	U16	scale;
	U8	* lpS = apSound->mpSound;

	for( i = 0; i < apSound->mLength; i++ )
	{
		U8 b = *lpS++;
		b += aAdd;
		if( b < lMin )
			lMin = b;
		if( b > lMax )
			lMax = b;
	}

	scale = 0xFFFF;
	scale /= lMax;

	lpS = apSound->mpSound;
	for( i = 0; i < apSound->mLength; i++ )
	{
		U16	lVal;
		U8 b = *lpS;
		b += aAdd;
		lVal = b;
		lVal *= scale;
		lVal >>= 8;
		b = (U8)lVal;
		b -= aAdd;
		*lpS++ = b;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_MaximiseVolumeSigned( sAudioDmaSound * apSound )
* ACTION   : boosts volume of a sample to maximum level
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_MaximiseVolumeSigned( sAudioDmaSound * apSound )
{
	Audio_MaximiseVolume( apSound, 0x80 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_MaximiseVolumeUnSigned( sAudioDmaSound * apSound )
* ACTION   : boosts volume of a sample to maximum level
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_MaximiseVolumeUnSigned( sAudioDmaSound * apSound )
{
	Audio_MaximiseVolume( apSound, 0x80 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaScaleSignedVol( sAudioDmaSound * apSound,const U16 aScale )
* ACTION   : Audio_DmaScaleSignedVol
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_ScaleVolumeSigned( sAudioDmaSound * apSound,const U16 aScale )
{
	S16		lVol;
	S8 *	lpByte;
	U32		i;

	lpByte = (S8*)apSound->mpSound;
	for( i=0; i<apSound->mLength; i++ )
	{
		lVol   = *lpByte;
		lVol <<= 8;
		lVol >>= 8;
		lVol   = (S16)(lVol * aScale);

		lVol >>= 8;

		*lpByte = (S8)lVol;

		lpByte++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaScaleUnsignedVol( sAudioDmaSound * apSound,const U16 aScale )
* ACTION   : Audio_DmaScaleUnsignedVol
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_ScaleVolumeUnsigned( sAudioDmaSound * apSound,const U16 aScale )
{
	(void)apSound;
	(void)aScale;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetpDmaCurrentSound( void )
* ACTION   : returns pointer to current sound
* CREATION : 23.03.01 PNK
*-----------------------------------------------------------------------------------*/

sAudioDmaSound *	Audio_GetpDmaCurrentSound( void )
{
	return( &gAudioCurrentSound );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetLeftVolume( void )
* ACTION   : Audio_GetLeftVolume
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U8	Audio_GetLeftVolume( void )
{
	U8	lVol;

	lVol = 0;

	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		lVol   = 0xF;
		lVol  *= (255/15);
		break;

	case	MCH_FALCON:
		lVol   = *(U8*)0xFFFF8939L;
		lVol >>= 4;
		lVol  &= 0xF;
		lVol  *= (255/15);
		break;
	default:
		break;
	}

	return( lVol );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetRightVolume( void )
* ACTION   : Audio_GetRightVolume
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U8	Audio_GetRightVolume( void )
{
	U8	lVol;

	lVol = 0;

	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		lVol = 0xF;
		lVol  *= (255/15);
		break;

	case	MCH_FALCON:
		lVol  = *(U8*)0xFFFF8939L;
		lVol &= 0xF;
		lVol  *= (255/15);
		break;
	default:
		break;
	}

	return( lVol );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_SetLeftVolume( const U8 aVol )
* ACTION   : Audio_SetLeftVolume
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_SetLeftVolume( const U8 aVol )
{
	U16	lVal;
	U16	lNewVol;
	U8	lVol;


	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		lNewVol   = aVol;
		lNewVol  *= 21;
		lNewVol >>= 8;

		lVal  = dAUDIO_MW_CMD_LEFTVOL;
		lVal |= lNewVol;
		Audio_MicroWireWrite( lVal );
		break;

	case	MCH_FALCON:
		lNewVol   = (U16)(255-aVol);
		lNewVol >>= 4;

		lVol  = *(U8*)0xFFFF893AL;
		lVol &= 0xF;
		lVol |= ((lNewVol&0xF)<<4);
		*(U8*)0xFFFF893AL = lVol;
		*(U8*)0xFFFF893BL = lVol;
		break;
	default:
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_SetRightVolume( const U8 aVol )
* ACTION   : Audio_SetRightVolume
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_SetRightVolume( const U8 aVol )
{
	U16	lVal;
	U16	lNewVol;
	U8	lVol;

	switch( System_GetMCH() )
	{
	case	MCH_STE:
	case	MCH_MEGASTE:
	case	MCH_TT:
		lNewVol   = aVol;
		lNewVol  *= 21;
		lNewVol >>= 8;


		lVal  = dAUDIO_MW_CMD_RIGHTVOL;
		lVal |= lNewVol;
		Audio_MicroWireWrite( lVal );
		break;

	case	MCH_FALCON:
		lNewVol   = (U16)(255-aVol);
		lNewVol >>= 4;

		lVol  = *(U8*)0xFFFF893AL;
		lVol &= 0xF0;
		lVol |= (lNewVol&0xF);
		*(U8*)0xFFFF893AL = lVol;
		*(U8*)0xFFFF893BL = lVol;
		break;
	default:
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetVolume( void )
* ACTION   : Audio_GetVolume
* CREATION : 30.07.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	Audio_GetVolume( void )
{
	U16	lVol;

	lVol  = (U16)( Audio_GetLeftVolume() + Audio_GetRightVolume() );
	lVol >>= 1;

	return( (U8)lVol );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_SetVolume( const U8 aVol )
* ACTION   : Audio_SetVolume
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_SetVolume( const U8 aVol )
{
	Audio_SetLeftVolume( aVol );
	Audio_SetRightVolume( aVol );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_GetpSavedYM( void )
* ACTION   : Audio_GetpSavedYM
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

U8 *	Audio_GetpSavedYM( void )
{
	return( gAudioSavedYM );
}

U32		Audio_GetFrequency( U8 aFreq )
{
	return( gAudioFrequencies[ aFreq ] );
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaSound_Delocate( sAudioDmaSound * apSnd )
* ACTION   : Audio_DmaSound_Delocate
* CREATION : 23.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_DmaSound_Delocate( sAudioDmaSound * apSnd )
{
	*(U32*)&apSnd->mpSound -= (U32)apSnd;
	Endian_FromBigU32( &apSnd->mpSound );
	Endian_FromBigU32( &apSnd->mLength );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Audio_DmaSound_Relocate( sAudioDmaSound * apSnd )
* ACTION   : Audio_DmaSound_Relocate
* CREATION : 23.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Audio_DmaSound_Relocate( sAudioDmaSound * apSnd )
{
	Endian_FromBigU32( &apSnd->mpSound );
	Endian_FromBigU32( &apSnd->mLength );
	*(U32*)&apSnd->mpSound += (U32)apSnd;
}


/* ################################################################################ */
