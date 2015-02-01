/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: SND.C
::
:: SND chip routines
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PINKNOTE.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/MUSIC/SND.H>
#include	<GODLIB/MFP/MFP.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dPINKNOTE_QUEUE_LIMIT	16


/* ###################################################################################
#  DATA
################################################################################### */


sPinkNoteQueueEntry	gPinkNoteQueue[ dPINKNOTE_CHANNEL_LIMIT][ dPINKNOTE_QUEUE_LIMIT ];
U16					gPinkNoteQueueIndex[ dPINKNOTE_CHANNEL_LIMIT ];
sMfpTimer			gPinkNoteInterruptTimer;

sPinkNote			gPinkNoteSilence[2] =
{
	mPINKNOTE_VOLUME( 0, 0 ), 
	mPINKNOTE_END 
};

extern	sPinkNoteChannel	gPinkNoteChannel[ dPINKNOTE_CHANNEL_LIMIT ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	PinkNote_ChannelInit( U16 aIndex );
void	PinkNote_ChannelDeInit( U16 aIndex );
void	PinkNote_QueueInit( void );

extern	void	PinkNote_Player( void );
extern	void	PinkNote_NotePlayChannelA( sPinkNoteChannel * apChannel );
extern	void	PinkNote_NotePlayChannelB( sPinkNoteChannel * apChannel );
extern	void	PinkNote_NotePlayChannelC( sPinkNoteChannel * apChannel );


/* ###################################################################################
#	CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_Init( void )
* ACTION   : inits pinknote stuffs
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_Init()
{
	U16	i;

	for( i=0; i<dPINKNOTE_CHANNEL_LIMIT; i++ )
	{
		PinkNote_ChannelInit( i );
	}
	PinkNote_QueueInit();


	gPinkNoteInterruptTimer.mData       = 0;
	gPinkNoteInterruptTimer.mFreq       = 0;
	gPinkNoteInterruptTimer.mMode       = 0;
	gPinkNoteInterruptTimer.mfTimerFunc = PinkNote_Player;

	Snd_SetpChaserFunc( PinkNote_Player );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_DeInit( void )
* ACTION   : deinits all pinknote channels
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_DeInit()
{
	U16					i;

	for( i=0; i<dPINKNOTE_CHANNEL_LIMIT; i++ )
	{
		PinkNote_ChannelDeInit( i );
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_ReInit( void )
* ACTION   : reinits all pinknote channels
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_ReInit()
{
	U16	i;

	for( i=0; i<dPINKNOTE_CHANNEL_LIMIT; i++ )
	{
		PinkNote_ChannelDeInit( i );
		PinkNote_ChannelInit( i );
	}
	PinkNote_QueueInit();

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_Update( void )
* ACTION   : plays queued sounds
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_Update( void )
{
	U16	c,q,i,j;

	for( c=0; c<dPINKNOTE_CHANNEL_LIMIT; c++ )
	{
		q = gPinkNoteQueueIndex[ c ];
		i = 0;
		while( (i < dPINKNOTE_QUEUE_LIMIT) && (!gPinkNoteQueue[c][q].mActiveFlag) )
		{
			i++;
			q++;
			if( q >= dPINKNOTE_QUEUE_LIMIT )
				q=0;
		}
		if( i<dPINKNOTE_QUEUE_LIMIT )
		{
			if( (!gPinkNoteChannel[ c ].mActiveFlag) || (gPinkNoteChannel[ c ].mLoopingFlag) )
			{
				gPinkNoteChannel[ c ].mActiveFlag  = 0;
				gPinkNoteChannel[ c ].mpNotes      = gPinkNoteQueue[ c ][ q ].mpNotes;
				gPinkNoteChannel[ c ].mIndex       = 0;
				gPinkNoteChannel[ c ].mLoopingFlag = 0;
				gPinkNoteChannel[ c ].mActiveFlag  = 1;

				gPinkNoteQueue[ c ][ q ].mActiveFlag = 0;
				q++;
				if( q >= dPINKNOTE_QUEUE_LIMIT )
				{
					q = 0;
				}
				gPinkNoteQueueIndex[ c ] = q;
			}
		}
	}

	for( i=0; i<dPINKNOTE_CHANNEL_LIMIT; i++ )
	{
		for( j=0; j<dPINKNOTE_QUEUE_LIMIT; j++ )
		{
			if( gPinkNoteQueue[ i ][ j ].mActiveFlag )
			{
				gPinkNoteQueue[ i ][ j ].mActiveFlag--;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_QueueInit( void )
* ACTION   : inits note queues on all channels
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_QueueInit( void )
{
	U16	i,j;

	for( i=0; i<dPINKNOTE_CHANNEL_LIMIT; i++ )
	{
		for( j=0; j<dPINKNOTE_CHANNEL_LIMIT; j++ )
		{
			gPinkNoteQueue[ i ][ j ].mActiveFlag = 0;
			gPinkNoteQueue[ i ][ j ].mpNotes     = 0;
			gPinkNoteQueue[ i ][ j ].mPriority   = 0;
		}
		gPinkNoteQueueIndex[ i ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_ChannelInit( U16 aIndex )
* ACTION   : inits a channel
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_ChannelInit( U16 aIndex )
{
	sPinkNoteChannel *	lpChannel;

	lpChannel = &gPinkNoteChannel[ aIndex ];

	lpChannel->mActiveFlag  = 0;
	lpChannel->mEnvFreq     = 0;
	lpChannel->mEnvType     = 0;
	lpChannel->mIndex       = 0;
	lpChannel->mLoopingFlag = 0;
	lpChannel->mNoiseFreq   = 0;
	lpChannel->mNoiseTone   = (1<<3)|1;
	lpChannel->mPause       = 0;
	lpChannel->mpNotes      = 0;
	lpChannel->mToneFreq    = 0;
	lpChannel->mVolume      = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_ChannelDeInit( U16 aIndex )
* ACTION   : deinits a channel
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_ChannelDeInit( U16 aIndex )
{
	sPinkNoteChannel *	lpChannel;

	lpChannel = &gPinkNoteChannel[ aIndex ];
	if( lpChannel->mActiveFlag )
	{
		PinkNote_ChannelInit( aIndex );

		switch( aIndex )
		{
		case	0:
			PinkNote_NotePlayChannelA( lpChannel );
			break;
		case	1:
			PinkNote_NotePlayChannelB( lpChannel );
			break;
		case	2:
			PinkNote_NotePlayChannelC( lpChannel );
			break;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_ChannelEnable( U16 aIndex )
* ACTION   : enables PinkNote processing on channel aIndex
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_ChannelEnable( U16 aIndex )
{
	gPinkNoteChannel[ aIndex ].mActiveFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_Channel_Disable( U16 aIndex)
* ACTION   : disables PinkNote processing on channel aIndex
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_Channel_Disable( U16 aIndex)
{
	gPinkNoteChannel[ aIndex ].mActiveFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PinkNote_PlayNote( sPinkNote * apNote, U16 aChannel, U16 aPriority )
* ACTION   : adds note to the queue on channel aChannel
* CREATION : 04.02.01 PNK
*-----------------------------------------------------------------------------------*/

void	PinkNote_PlayNote( sPinkNote * apNote, U16 aChannel, U16 aPriority )
{
	U16	i,j;

	j = gPinkNoteQueueIndex[ aChannel ];
	for( i=0; i<dPINKNOTE_QUEUE_LIMIT; i++ )
	{
		if( (!gPinkNoteQueue[ aChannel ][ j ].mActiveFlag) || (aPriority>gPinkNoteQueue[ aChannel ][ j ].mPriority) )
		{
			gPinkNoteQueue[ aChannel ][ j ].mActiveFlag = 1;
			gPinkNoteQueue[ aChannel ][ j ].mpNotes     = apNote;
			gPinkNoteQueue[ aChannel ][ j ].mPriority   = aPriority;
			return;
		}
		j++;
		if( j >= dPINKNOTE_QUEUE_LIMIT )
		{
			j=0;
		}
	}

}


/* ################################################################################ */
