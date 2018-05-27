/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_COM.H"

#include	"ACH_GFX.H"
#include	"ACH_MAIN.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/THREAD/THREAD.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_DISP_STAT_LINE_GAP		10
#define	dACH_DISP_SCORE_LINE_GAP	10
#define	dACH_DISP_TASK_LINE_GAP		10


/* ###################################################################################
#  ENUMS
################################################################################### */



/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveDisplay
{
	sThread	mStatsThread;
	sThread	mScoreTableThread;
	sThread	mTasksThread;
	sThread	mUnlockedThread;
	U16		mStatsTopIndex;
	U16		mPalOld[ 16 ];
	U16 *	mpLogic;
	U16 *	mpPhysic;
	U16		mScoreTableIndex;
	U16		mTaskTopIndex;
	U16		mUnlockedIndex;
	U8		mInputPacket;
	U8		mInputKey;
	U8		mInputHit;
	U8		mInputUnHit;
	U8		mFinishedFlag;
}sAchieveDisplay;


/* ###################################################################################
#  DATA
################################################################################### */

sAchieveDisplay	gAchieveDisplayClass;

U16		gAchieveDisplayPal[ 16 ] =
{
	0x000,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U8		Achieve_Display_Stats_Thread( sThread * apThread );
U8		Achieve_Display_ScoreTable_Thread( sThread * apThread );
U8		Achieve_Display_Tasks_Thread( sThread * apThread );
U8		Achieve_Display_Unlocked_Thread( sThread * apThread );
void	Achieve_Display_Stats_Render( void );
void	Achieve_Display_ScoreTable_Render( void );
void	Achieve_Display_Tasks_Render( void );
void	Achieve_Display_Unlocked_Render( void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_AppInit( void )
* ACTION   : Achieve_Display_AppInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_AppInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_AppDeInit( void )
* ACTION   : Achieve_Display_AppDeInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_ScoreTable_Init( const U16 aTableIndex )
* ACTION   : Achieve_Display_ScoreTable_Init
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_ScoreTable_Init( const U16 aTableIndex )
{
	mTHREAD_INIT( &gAchieveDisplayClass.mScoreTableThread );
	gAchieveDisplayClass.mScoreTableIndex = aTableIndex;
	Achieve_Gfx_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_ScoreTable_DeInit( void )
* ACTION   : Achieve_Display_ScoreTable_DeInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_ScoreTable_DeInit( void )
{
	mTHREAD_DEINIT( &gAchieveDisplayClass.mScoreTableThread );
	Achieve_Gfx_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_ScoreTable_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Display_ScoreTable_Update
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Display_ScoreTable_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
{
	gAchieveDisplayClass.mpPhysic = (U16*)apPhysic;
	gAchieveDisplayClass.mpLogic  = (U16*)apLogic;

	Achieve_Gfx_Update( apPhysic, apLogic );
	Achieve_Input_Update( aPacket, aKey );
	Achieve_Display_ScoreTable_Thread( &gAchieveDisplayClass.mScoreTableThread );

	return( gAchieveDisplayClass.mFinishedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_ScoreTable_Thread( sThread * apThread )
* ACTION   : Achieve_Display_ScoreTable_Thread
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Display_ScoreTable_Thread( sThread * apThread )
{
	sAchieveGfxQuad	lQuad;
	sAchieveGfxPos	lPos;

	mTHREAD_BEGIN( apThread );

	/*	fade to white */
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 16 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( Achieve_ScoreTable_GetpTableName(gAchieveDisplayClass.mScoreTableIndex), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( Achieve_ScoreTable_GetpTableName(gAchieveDisplayClass.mScoreTableIndex), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );

	Achieve_Display_ScoreTable_Render();
	mTHREAD_YIELD( apThread );
	Achieve_Display_ScoreTable_Render();

	/*	fade in */
	Achieve_Gfx_Fade_Begin( gAchieveDisplayPal, 16 );
	Achieve_Gfx_Beam_Begin();
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* wait for key */
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE );

	/* fade out */
	Achieve_Gfx_Beam_Begin();
	mTHREAD_SLEEP( apThread, 20 );
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 32 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );

	gAchieveDisplayClass.mFinishedFlag = 1;

	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_ScoreTable_Render( void )
* ACTION   : Achieve_Display_ScoreTable_Render
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_ScoreTable_Render( void )
{
	U16				lIndex;
	U16				lLineCount;
	U32				lCount;
	U16				lLenMax;
	U16				lLen;
	sAchieveGfxPos	lPos;
	sAchieveGfxQuad		lFontClipQuad;
	char			lString[ 128 ];

	lCount  = Achieve_ScoreTable_GetEntryCount( gAchieveDisplayClass.mScoreTableIndex );

	lPos.mX = 160;
	lPos.mY = (S16)(100 - ((lCount*dACH_DISP_SCORE_LINE_GAP)>>1));

	if( lPos.mY < 12 )
	{
		lPos.mY = 12;
	}

	lLineCount = (186 / dACH_DISP_SCORE_LINE_GAP);

	lIndex  = gAchieveDisplayClass.mStatsTopIndex;


	lLenMax = 0;

	while( (lIndex<lCount) && (lLineCount) )
	{
		sprintf( lString, "%ld", Achieve_ScoreTable_GetScore( gAchieveDisplayClass.mScoreTableIndex, lIndex ) );
		lLen = 0;
		while( lString[ lLen ] )	lLen++;
		if( lLen > lLenMax ) lLenMax = lLen;
		lIndex++;
		lLineCount--;
	}

	lFontClipQuad.mX0 = 0;
	lFontClipQuad.mX1 = 319;
	lFontClipQuad.mY0 = 10;
	lFontClipQuad.mY1 = 190;
	lIndex  = gAchieveDisplayClass.mStatsTopIndex;
	lLineCount = (186 / dACH_DISP_SCORE_LINE_GAP);
	while( (lIndex<lCount) && (lLineCount) )
	{
		U32	lValue;
		lPos.mX = 160 - 8;
		Achieve_Gfx_FontDraw( Achieve_ScoreTable_GetpName( gAchieveDisplayClass.mScoreTableIndex, lIndex ), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_RIGHT, &lPos, &lFontClipQuad );

		lPos.mX = (U16)(160 + 8 + (lLenMax<<3));
		lValue = Achieve_ScoreTable_GetScore( gAchieveDisplayClass.mScoreTableIndex, lIndex );
		sprintf( lString, "%ld", lValue );
		Achieve_Gfx_FontDraw( &lString[0], gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_RIGHT, &lPos, &lFontClipQuad );

		lPos.mY += dACH_DISP_SCORE_LINE_GAP;

		lIndex++;
		lLineCount--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Stats_Init( void )
* ACTION   : Achieve_Display_Stats_Init
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Stats_Init( void )
{
	mTHREAD_INIT( &gAchieveDisplayClass.mStatsThread );
	gAchieveDisplayClass.mFinishedFlag = 0;
	Achieve_Gfx_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Stats_DeInit( void )
* ACTION   : Achieve_Display_Stats_DeInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Stats_DeInit( void )
{
	mTHREAD_DEINIT( &gAchieveDisplayClass.mStatsThread );
	Achieve_Gfx_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Stats_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Display_Stats_Update
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Display_Stats_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
{
	gAchieveDisplayClass.mpPhysic = (U16*)apPhysic;
	gAchieveDisplayClass.mpLogic  = (U16*)apLogic;

	Achieve_Gfx_Update( apPhysic, apLogic );
	Achieve_Input_Update( aPacket, aKey );
	Achieve_Display_Stats_Thread( &gAchieveDisplayClass.mStatsThread );

	return( gAchieveDisplayClass.mFinishedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Stats_Thread( sThread * apThread )
* ACTION   : Achieve_Display_Stats_Thread
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Display_Stats_Thread( sThread * apThread )
{
	sAchieveGfxQuad	lQuad;
	sAchieveGfxPos	lPos;

	mTHREAD_BEGIN( apThread );

	/*	fade to white */
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 16 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "STATS", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "STATS", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );

	Achieve_Display_Stats_Render();
	mTHREAD_YIELD( apThread );
	Achieve_Display_Stats_Render();

	/*	fade in */
	Achieve_Gfx_Fade_Begin( gAchieveDisplayPal, 16 );
	Achieve_Gfx_Beam_Begin();
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* wait for key */
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE );

	/* fade out */
	Achieve_Gfx_Beam_Begin();
	mTHREAD_SLEEP( apThread, 20 );
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 32 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );

	gAchieveDisplayClass.mFinishedFlag = 1;

	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Stats_Render( void )
* ACTION   : Achieve_Display_Stats_Render
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Stats_Render( void )
{
	U16				lIndex;
	U16				lLineCount;
	U32				lCount;
	sAchieveGfxPos	lPos;
	sAchieveGfxQuad		lFontClipQuad;
	char			lString[ 128 ];

	lCount  = Achieve_Stat_GetCount();

	lPos.mX = 160;
	lPos.mY = (S16)(100 - ((lCount*dACH_DISP_STAT_LINE_GAP)>>1));

	if( lPos.mY < 12 )
	{
		lPos.mY = 12;
	}

	lLineCount = (186 / dACH_DISP_STAT_LINE_GAP);


	lIndex  = gAchieveDisplayClass.mStatsTopIndex;
	lFontClipQuad.mX0 = 0;
	lFontClipQuad.mX1 = 319;
	lFontClipQuad.mY0 = 10;
	lFontClipQuad.mY1 = 190;

	while( (lIndex<lCount) && (lLineCount) )
	{
		U32	lValue;
		lPos.mX = 160 - 8;
		Achieve_Gfx_FontDraw( Achieve_Stat_GetName( lIndex ), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_RIGHT, &lPos, &lFontClipQuad );

		lPos.mX = 160 + 8;
		lValue = Achieve_Stat_Get( lIndex );
		sprintf( lString, "%ld", lValue );
		Achieve_Gfx_FontDraw( &lString[0], gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lFontClipQuad );

		lPos.mY += dACH_DISP_STAT_LINE_GAP;

		lIndex++;
		lLineCount--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Tasks_Init( void )
* ACTION   : Achieve_Display_Tasks_Init
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Tasks_Init( void )
{
	mTHREAD_INIT( &gAchieveDisplayClass.mTasksThread );
	gAchieveDisplayClass.mFinishedFlag = 0;
	Achieve_Gfx_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Tasks_DeInit( void )
* ACTION   : Achieve_Display_Tasks_DeInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Tasks_DeInit( void )
{
	mTHREAD_DEINIT( &gAchieveDisplayClass.mTasksThread );
	Achieve_Gfx_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Tasks_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Display_Tasks_Update
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Display_Tasks_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
{
	gAchieveDisplayClass.mpPhysic = (U16*)apPhysic;
	gAchieveDisplayClass.mpLogic  = (U16*)apLogic;

	Achieve_Gfx_Update( apPhysic, apLogic );
	Achieve_Input_Update( aPacket, aKey );
	Achieve_Display_Tasks_Thread( &gAchieveDisplayClass.mTasksThread );

	return( gAchieveDisplayClass.mFinishedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Tasks_Thread( sThread * apThread )
* ACTION   : Achieve_Display_Tasks_Thread
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Display_Tasks_Thread( sThread * apThread )
{
	sAchieveGfxQuad	lQuad;
	sAchieveGfxPos	lPos;

	mTHREAD_BEGIN( apThread );

	/*	fade to white */
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 16 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "TASKS", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "TASKS", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );

	Achieve_Display_Tasks_Render();
	mTHREAD_YIELD( apThread );
	Achieve_Display_Tasks_Render();

	/*	fade in */
	Achieve_Gfx_Fade_Begin( gAchieveDisplayPal, 16 );
	Achieve_Gfx_Beam_Begin();
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* wait for key */
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE );

	/* fade out */
	Achieve_Gfx_Beam_Begin();
	mTHREAD_SLEEP( apThread, 20 );
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 32 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );

	gAchieveDisplayClass.mFinishedFlag = 1;

	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Tasks_Render( void )
* ACTION   : Achieve_Display_Tasks_Render
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Tasks_Render( void )
{
	U16				lIndex;
	U16				lLineCount;
	U32				lCount;
	sAchieveGfxPos	lPos;
	sAchieveGfxQuad		lFontClipQuad;

	lCount  = Achieve_Tasks_GetCount();

	lPos.mX = 160;
	lPos.mY = (S16)(100 - ((lCount*dACH_DISP_TASK_LINE_GAP)>>1));

	if( lPos.mY < 12 )
	{
		lPos.mY = 12;
	}

	lLineCount = (186 / dACH_DISP_TASK_LINE_GAP);


	lIndex  = gAchieveDisplayClass.mStatsTopIndex;
	lFontClipQuad.mX0 = 0;
	lFontClipQuad.mX1 = 319;
	lFontClipQuad.mY0 = 10;
	lFontClipQuad.mY1 = 190;

	while( (lIndex<lCount) && (lLineCount) )
	{
		lPos.mX = 8;
		Achieve_Gfx_FontDraw( Achieve_Tasks_GetpTitle( lIndex ), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lFontClipQuad );
		lPos.mY += dACH_DISP_TASK_LINE_GAP;

		lIndex++;
		lLineCount--;
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Unlocked_Init( const U16 aTaskIndex )
* ACTION   : Achieve_Display_Unlocked_Init
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Unlocked_Init( const U16 aTaskIndex )
{
	mTHREAD_INIT( &gAchieveDisplayClass.mUnlockedThread );
	gAchieveDisplayClass.mUnlockedIndex = aTaskIndex;
	gAchieveDisplayClass.mFinishedFlag = 0;
	Achieve_Gfx_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Unlocked_DeInit( void )
* ACTION   : Achieve_Display_Unlocked_DeInit
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Unlocked_DeInit( void )
{
	mTHREAD_DEINIT( &gAchieveDisplayClass.mUnlockedThread );
	Achieve_Gfx_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Unlocked_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Display_Unlocked_Update
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Display_Unlocked_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
{
	gAchieveDisplayClass.mpPhysic = (U16*)apPhysic;
	gAchieveDisplayClass.mpLogic  = (U16*)apLogic;

	Achieve_Gfx_Update( apPhysic, apLogic );
	Achieve_Input_Update( aPacket, aKey );
	Achieve_Display_Unlocked_Thread( &gAchieveDisplayClass.mUnlockedThread );

	return( gAchieveDisplayClass.mFinishedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Unlocked_Thread( sThread * apThread )
* ACTION   : Achieve_Display_Unlocked_Thread
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Display_Unlocked_Thread( sThread * apThread )
{
	sAchieveGfxQuad	lQuad;
	sAchieveGfxPos	lPos;

	mTHREAD_BEGIN( apThread );

	/*	fade to white */
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 16 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "TASK COMPLETE", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	lPos.mX = 0;
	lPos.mY = 0;
	lQuad.mX0 = 0;
	lQuad.mX1 = 319;
	lQuad.mY0 = 0;
	lQuad.mY1 = 199;
	Achieve_Gfx_FontDraw( "TASK COMPLETE", gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );

	Achieve_Display_Unlocked_Render();
	mTHREAD_YIELD( apThread );
	Achieve_Display_Unlocked_Render();

	/*	fade in */
	Achieve_Gfx_Fade_Begin( gAchieveDisplayPal, 16 );
	Achieve_Gfx_Beam_Begin();
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* wait for key */
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE );

	/* fade out */
	Achieve_Gfx_Beam_Begin();
	mTHREAD_SLEEP( apThread, 20 );
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 32 );
	mTHREAD_YIELD_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apThread );
	memset( gAchieveDisplayClass.mpLogic, 0, 160L*200L );

	gAchieveDisplayClass.mFinishedFlag = 1;

	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Display_Unlocked_Render( void )
* ACTION   : Achieve_Display_Unlocked_Render
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Display_Unlocked_Render( void )
{
	sAchieveGfxPos	lPos;
	sAchieveGfxQuad		lFontClipQuad;

	lPos.mX = 8;
	lPos.mY = 100-4;

	lFontClipQuad.mX0 = 0;
	lFontClipQuad.mX1 = 319;
	lFontClipQuad.mY0 = 10;
	lFontClipQuad.mY1 = 190;

	Achieve_Gfx_FontDraw( Achieve_Tasks_GetpTitle( gAchieveDisplayClass.mUnlockedIndex ), gAchieveDisplayClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lFontClipQuad );
}


/* ################################################################################ */
