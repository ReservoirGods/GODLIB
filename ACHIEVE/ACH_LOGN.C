/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_LOGN.H"

#include	"ACH_GFX.H"
#include	"ACH_MAIN.H"
#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/THREAD/THREAD.H>
#include	<GODLIB/XBIOS/XBIOS.H>
#ifndef	dGODLIB_PLATFORM_ATARI
#include	<GODLIB/VIDEO/VIDEO.H>
#endif


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_LOGIN_LINE_Y0		10
#define	dACH_LOGIN_LINE_Y1		190
#define	dACH_LOGIN_LINE_SPEED	4
#define	dACH_LOGIN_SLOT_LIMIT	7
#define	dACH_LOGIN_SLOT_CENTRE_INDEX	3
#define	dACH_LOGIN_BOX_LIMIT	14
#define dACH_LOGIN_MORPH_SPEED	16

#define	dACH_LOGIN_BOX_MINY		12
#define	dACH_LOGIN_BOX_MAXY		189

#define dACH_LOGIN_SPAWN_MORPH_SPEED	16
#define dACH_LOGIN_SCROLL_MORPH_SPEED	16

#define	dACH_LOGIN_SMALL_BOX_WIDTH	136
#define	dACH_LOGIN_SMALL_BOX_HEIGHT	16
#define	dACH_LOGIN_SMALL_BOX_X0		160
#define	dACH_LOGIN_BIG_BOX_WIDTH	224
#define	dACH_LOGIN_BIG_BOX_HEIGHT	32
#define	dACH_LOGIN_BIG_BOX_X0		160

#define	dACH_LOGIN_SEL_BOX_WIDTH	96
#define	dACH_LOGIN_SEL_BOX_HEIGHT	24
#define	dACH_LOGIN_UNSEL_BOX_WIDTH	80
#define	dACH_LOGIN_UNSEL_BOX_HEIGHT	16

#define	dACH_LOGIN_OK_BOX_X			80
#define	dACH_LOGIN_CANCEL_BOX_X		240

#define dACH_LOGIN_CURSOR_RATE		8


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_LOGIN_VIEW_USERS,
	eACH_LOGIN_VIEW_SIGNIN_CONFIRM,
	eACH_LOGIN_VIEW_DELETE_CONFIRM,
};

enum
{
	eACH_LOGIN_THREAD_BOXES,
	eACH_LOGIN_THREAD_TITLES,
	eACH_LOGIN_THREAD_MAIN,
	eACH_LOGIN_THREAD_LIMIT
};

enum
{
	eACH_LOGIN_BOXTYPE_NONE,
	eACH_LOGIN_BOXTYPE_CANCEL_SEL,
	eACH_LOGIN_BOXTYPE_CANCEL_UNSEL,
	eACH_LOGIN_BOXTYPE_DELETE,
	eACH_LOGIN_BOXTYPE_OK_SEL,
	eACH_LOGIN_BOXTYPE_OK_UNSEL,
	eACH_LOGIN_BOXTYPE_SIGNIN,
	eACH_LOGIN_BOXTYPE_SLOT0,
	eACH_LOGIN_BOXTYPE_SLOT1,
	eACH_LOGIN_BOXTYPE_SLOT2,
	eACH_LOGIN_BOXTYPE_SLOT3,
	eACH_LOGIN_BOXTYPE_SLOT4,
	eACH_LOGIN_BOXTYPE_SLOT5,
	eACH_LOGIN_BOXTYPE_SLOT6,

	eACH_LOGIN_BOXTYPE_LIMIT
};

enum
{
	eACH_LOGIN_DIALOG_OK,
	eACH_LOGIN_DIALOG_CANCEL,
};


sAchieveGfxSizePos	gAchieveLoginBoxSizePos[ eACH_LOGIN_BOXTYPE_LIMIT ] =
{
	{	0,							 0,								 0,							  0	},	/*	eACH_LOGIN_BOXTYPE_NONE,	     */
	{	dACH_LOGIN_CANCEL_BOX_X,	150,	dACH_LOGIN_SEL_BOX_WIDTH,	dACH_LOGIN_SEL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_CANCEL_SEL, 	 */
	{	dACH_LOGIN_CANCEL_BOX_X,	150,	dACH_LOGIN_UNSEL_BOX_WIDTH,	dACH_LOGIN_UNSEL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_CANCEL_UNSEL, */
	{	dACH_LOGIN_SMALL_BOX_X0,	 50,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_DELETE,	     */
	{	dACH_LOGIN_OK_BOX_X,		150,	dACH_LOGIN_SEL_BOX_WIDTH,	dACH_LOGIN_SEL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_OK_SEL,		 */
	{	dACH_LOGIN_OK_BOX_X,		150,	dACH_LOGIN_UNSEL_BOX_WIDTH,	dACH_LOGIN_UNSEL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_OK_UNSEL,	 */
	{	dACH_LOGIN_BIG_BOX_X0,		 50,	dACH_LOGIN_BIG_BOX_WIDTH,	dACH_LOGIN_BIG_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SIGNIN,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	  0,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT0,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	 28,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT1,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	 56,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT2,	*/
	{	dACH_LOGIN_BIG_BOX_X0,		100,	dACH_LOGIN_BIG_BOX_WIDTH,	dACH_LOGIN_BIG_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT3,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	144,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT4,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	172,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT5,	*/
	{	dACH_LOGIN_SMALL_BOX_X0,	200,	dACH_LOGIN_SMALL_BOX_WIDTH,	dACH_LOGIN_SMALL_BOX_HEIGHT	},	/*	eACH_LOGIN_BOXTYPE_SLOT6,	*/
};

sAchieveGfxQuad	gAchieveLoginClipScreen	= { 0, 319,  0, 199 };
sAchieveGfxQuad	gAchieveLoginClipBoxes	= { 0, 319, 11, 189 };


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef struct	sAchieveLoginBox
{
	const char *		mpTitle;
	sAchieveGfxQuad		mDrawnCoords[2];
	sAchieveGfxQuad		mNextCoords;
	sAchieveGfxSizePos	mPosOld;
	sAchieveGfxSizePos	mPosTarget;
	S8					mSlotIndex;
	U8					mBoxType;
	U8					mRedrawFlag;
	U8					mActiveFlag;
	U8					mHiddenFlag;
} sAchieveLoginBox;

typedef struct sAchieveLoginClass
{
	U8 *	mpUnshiftTable;
	U8 *	mpShiftTable;
	U8 *	mpCapsTable;
	U16		mFinishedFlag;
	U16		mLinesActiveFlag;
	U16		mScreenIndex;
	S16		mMorphPos;
	U16		mMorphDoneFlag;
	sAchieveGfxQuad		mVerticalLines[2];
	sAchieveLoginBox	mBoxes[ dACH_LOGIN_BOX_LIMIT ];
	sAchieveLoginBox *	mpOkBox;
	sAchieveLoginBox *	mpCancelBox;
	sAchieveLoginBox *	mpInfoBox;
	sAchieveLoginBox *	mpNameBox;
	sThreadObject		mThreads[ eACH_LOGIN_THREAD_LIMIT ];
	sAchieveUserName	mUserName;
	U16					mPad;
	S8					mTextCursorX;
	S8					mUserCursorY;
	S8					mUserCount;
	S8					mCursorBlinkCounter;
	U8					mCursorBlinkOnFlag;
	U8					mCursorEnabledFlag;
	U16 *	mpLogic;
	U16 *	mpPhysic;
	U8		mDialogSelection;
}sAchieveLoginClass;


/* ###################################################################################
#  DATA
################################################################################### */

U16		gAchieveLoginPal[ 16 ] =
{
	0x0000,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
	0xFFFF,
};

sAchieveLoginClass	gAchieveLoginClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		Achieve_Login_Main_Appear_Update( void );
void	Achieve_Login_Box_Morph( sAchieveLoginBox * apBox, const U16 aMorphPos );

sAchieveLoginBox *	Achieve_Login_Box_GetFree( void  );
sAchieveLoginBox *	Achieve_Login_Box_Create( const U8 aBoxType, const char * apTitle, const S8 aSlotIndex );
void				Achieve_Login_Box_SetNewType( sAchieveLoginBox * apBox, const U8 aBoxType, const S8 aSlotIndex );

void	Achieve_Login_Boxes_MoveUp( void );
void	Achieve_Login_Boxes_MoveDown( void );

void	Achieve_Login_NameBox_Update( sAchieveLoginBox * apBox, const U8 aKey );

U8		Achieve_Login_Thread_Boxes( sThread * apThread );
U8		Achieve_Login_Thread_Main( sThread * apThread );
U8		Achieve_Login_Thread_Lines( sThread * apThread );
U8		Achieve_Login_Thread_Titles( sThread * apThread );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_AppInit( void )
* ACTION   : Achieve_Login_AppInit
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_AppInit( void )
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_AppDeInit( void )
* ACTION   : Achieve_Login_AppDeInit
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Init( void )
* ACTION   : Achieve_Login_Init
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_Init( void )
{
	U16	i;

	memset( &gAchieveLoginClass, 0, sizeof(sAchieveLoginClass) );

	gAchieveLoginClass.mThreads[ eACH_LOGIN_THREAD_BOXES  ].mfFunc = Achieve_Login_Thread_Boxes;
	gAchieveLoginClass.mThreads[ eACH_LOGIN_THREAD_MAIN   ].mfFunc = Achieve_Login_Thread_Main;
	gAchieveLoginClass.mThreads[ eACH_LOGIN_THREAD_TITLES ].mfFunc = Achieve_Login_Thread_Titles;


	gAchieveLoginClass.mpOkBox     = Achieve_Login_Box_Create( eACH_LOGIN_BOXTYPE_OK_SEL,		"OK", 0     );
	gAchieveLoginClass.mpCancelBox = Achieve_Login_Box_Create( eACH_LOGIN_BOXTYPE_CANCEL_UNSEL, "CANCEL", 0 );
	gAchieveLoginClass.mpInfoBox   = Achieve_Login_Box_Create( eACH_LOGIN_BOXTYPE_SIGNIN,		"SIGN IN AS", 0   );
	gAchieveLoginClass.mpNameBox   = Achieve_Login_Box_Create( eACH_LOGIN_BOXTYPE_SLOT0,		&gAchieveLoginClass.mUserName.mName[0], dACH_LOGIN_SLOT_CENTRE_INDEX );

	gAchieveLoginClass.mpOkBox->mPosTarget.mWidth = 0;
	gAchieveLoginClass.mpOkBox->mPosTarget.mHeight = 0;
	gAchieveLoginClass.mpCancelBox->mPosTarget.mWidth = 0;
	gAchieveLoginClass.mpCancelBox->mPosTarget.mHeight = 0;
	gAchieveLoginClass.mpInfoBox->mPosTarget.mWidth = 0;
	gAchieveLoginClass.mpInfoBox->mPosTarget.mHeight = 0;

#ifdef	dGODLIB_PLATFORM_ATARI
	{
		sXbiosKeyTab * lpKeyTab = Xbios_Keytbl( (const char *)0xFFFFFFFFL, (const char *)0xFFFFFFFFL, (const char *)0xFFFFFFFFL );
		gAchieveLoginClass.mpUnshiftTable = (U8 *)lpKeyTab->mpNormal;
		gAchieveLoginClass.mpShiftTable   = (U8 *)lpKeyTab->mpShift;
		gAchieveLoginClass.mpCapsTable    = (U8 *)lpKeyTab->mpCaps;
	}
#else
	{
		extern sIKBD gIKBD;
		gAchieveLoginClass.mpUnshiftTable = (U8 *)gIKBD.mpUnshiftTable;
		gAchieveLoginClass.mpShiftTable   = (U8 *)gIKBD.mpShiftTable;
		gAchieveLoginClass.mpCapsTable    = (U8 *)gIKBD.mpCapsTable;
	}
#endif

	for( i=0; i<eACH_LOGIN_THREAD_LIMIT; i++ )
	{
		mTHREAD_INIT( &gAchieveLoginClass.mThreads[i].mThread );
	}
	gAchieveLoginClass.mFinishedFlag = 0;

	gAchieveLoginClass.mUserCount = 0;
	for( i=0; i<Achieve_GetUserCount(); i++ )
	{
		Achieve_Login_Box_Create( eACH_LOGIN_BOXTYPE_SLOT0, Achieve_GetUserName(i), (S8)(dACH_LOGIN_SLOT_CENTRE_INDEX+i) );
	}
	Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpNameBox, gAchieveLoginClass.mpNameBox->mBoxType, (S8)(dACH_LOGIN_SLOT_CENTRE_INDEX+i) );
	gAchieveLoginClass.mUserCount = (S8)(Achieve_GetUserCount() + 1);
	gAchieveLoginClass.mUserCursorY = 0;

	Achieve_Gfx_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_DeInit( void )
* ACTION   : Achieve_Login_DeInit
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_DeInit( void )
{
	U16	i;
	for( i=0; i<eACH_LOGIN_THREAD_LIMIT; i++ )
	{
		mTHREAD_DEINIT( &gAchieveLoginClass.mThreads[i].mThread );
	}
	Achieve_Gfx_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Login_Update
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Login_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey )
{
	U16	i;

	gAchieveLoginClass.mpPhysic = (U16*)apPhysic;
	gAchieveLoginClass.mpLogic  = (U16*)apLogic;
	gAchieveLoginClass.mScreenIndex ^= 1;

	Achieve_Gfx_Update( apPhysic, apLogic );
	Achieve_Input_Update( aPacket, aKey );

	for( i=0; i<eACH_LOGIN_THREAD_LIMIT; i++ )
	{
		gAchieveLoginClass.mThreads[ i ].mfFunc( &gAchieveLoginClass.mThreads[ i ].mThread );
	}

	return( gAchieveLoginClass.mFinishedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Thread_Main( sThread * apPT )
* ACTION   : Achieve_Login_Thread_Main
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Login_Thread_Main( sThread * apPT )
{
	U16	i;

	mTHREAD_BEGIN( apPT );

	/*	fade to white */
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 16 );
	mTHREAD_YIELD_UNTIL( apPT, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveLoginClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apPT );
	memset( gAchieveLoginClass.mpLogic, 0, 160L*200L );

	gAchieveLoginClass.mMorphPos = 0;

	/* fade in */
	Achieve_Gfx_Fade_Begin( gAchieveLoginPal, 16 );
	gAchieveLoginClass.mLinesActiveFlag = 1;
	Achieve_Gfx_Beam_Begin();
	mTHREAD_YIELD_UNTIL( apPT, Achieve_Gfx_Fade_IsFinished() );

	do
	{
		gAchieveLoginClass.mCursorEnabledFlag = 1;
		/* process input */
		do
		{
			mTHREAD_YIELD_UNTIL( apPT, gAchieveLoginClass.mMorphDoneFlag );
			if( Achieve_Input_GetHit() & eACH_INPUT_PACKET_UP )
			{
				Achieve_Login_Boxes_MoveDown();
			}
			if( Achieve_Input_GetHit() & eACH_INPUT_PACKET_DOWN )
			{
				Achieve_Login_Boxes_MoveUp();
			}

			if( gAchieveLoginClass.mpNameBox->mSlotIndex == dACH_LOGIN_SLOT_CENTRE_INDEX )
			{
				Achieve_Login_NameBox_Update( gAchieveLoginClass.mpNameBox, Achieve_Input_GetKey() );
			}
		} while( !(Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE) );


		for( i=0; i<eACH_LOGIN_BOXTYPE_LIMIT; i++ )
		{
			if( gAchieveLoginClass.mBoxes[ i ].mSlotIndex != dACH_LOGIN_SLOT_CENTRE_INDEX )
			{
				gAchieveLoginClass.mBoxes[ i ].mPosTarget.mHeight = 0;
				gAchieveLoginClass.mBoxes[ i ].mPosTarget.mWidth = 0;
			}
		}

		Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpOkBox,     eACH_LOGIN_BOXTYPE_OK_SEL,       0 );
		Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpCancelBox, eACH_LOGIN_BOXTYPE_CANCEL_UNSEL, 0 );
		Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpInfoBox,   eACH_LOGIN_BOXTYPE_SIGNIN ,      0 );
		gAchieveLoginClass.mDialogSelection = eACH_LOGIN_DIALOG_OK;
		gAchieveLoginClass.mMorphPos = 0;

		/* process confirm dialog */
		gAchieveLoginClass.mCursorEnabledFlag = 0;
		do
		{
			mTHREAD_YIELD_UNTIL( apPT, gAchieveLoginClass.mMorphDoneFlag );
			if( Achieve_Input_GetHit() & eACH_INPUT_PACKET_LEFT )
			{
				if( gAchieveLoginClass.mDialogSelection != eACH_LOGIN_DIALOG_OK )
				{
					gAchieveLoginClass.mDialogSelection = eACH_LOGIN_DIALOG_OK;

					Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpOkBox,     eACH_LOGIN_BOXTYPE_OK_SEL,       0 );
					Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpCancelBox, eACH_LOGIN_BOXTYPE_CANCEL_UNSEL, 0 );
					gAchieveLoginClass.mMorphPos = 0;
				}
			}
			if( Achieve_Input_GetHit() & eACH_INPUT_PACKET_RIGHT )
			{
				if( gAchieveLoginClass.mDialogSelection != eACH_LOGIN_DIALOG_CANCEL )
				{
					gAchieveLoginClass.mDialogSelection = eACH_LOGIN_DIALOG_CANCEL;

					Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpOkBox,     eACH_LOGIN_BOXTYPE_OK_UNSEL,   0 );
					Achieve_Login_Box_SetNewType( gAchieveLoginClass.mpCancelBox, eACH_LOGIN_BOXTYPE_CANCEL_SEL, 0 );
					gAchieveLoginClass.mMorphPos = 0;
				}
			}
		} while( !(Achieve_Input_GetHit() & eACH_INPUT_PACKET_FIRE) );

		if( gAchieveLoginClass.mDialogSelection == eACH_LOGIN_DIALOG_CANCEL )
		{
			for( i=0; i<eACH_LOGIN_BOXTYPE_LIMIT; i++ )
			{
				Achieve_Login_Box_SetNewType( &gAchieveLoginClass.mBoxes[ i ], gAchieveLoginClass.mBoxes[ i ].mBoxType, gAchieveLoginClass.mBoxes[ i ].mSlotIndex );
			}
		}

		gAchieveLoginClass.mpOkBox->mPosTarget.mWidth = 0;
		gAchieveLoginClass.mpOkBox->mPosTarget.mHeight = 0;
		gAchieveLoginClass.mpCancelBox->mPosTarget.mWidth = 0;
		gAchieveLoginClass.mpCancelBox->mPosTarget.mHeight = 0;
		gAchieveLoginClass.mpInfoBox->mPosTarget.mWidth = 0;
		gAchieveLoginClass.mpInfoBox->mPosTarget.mHeight = 0;
		gAchieveLoginClass.mMorphPos = 0;
		
	} while(gAchieveLoginClass.mDialogSelection != eACH_LOGIN_DIALOG_OK);

	Achieve_SetUserInfo( gAchieveLoginClass.mpNameBox->mpTitle );

	/* fade out */
	gAchieveLoginClass.mLinesActiveFlag = 1;
	Achieve_Gfx_Beam_Begin();
	mTHREAD_SLEEP( apPT, 20 );
	Achieve_Gfx_Fade_Begin( Fade_GetpWhitePal(), 32 );
	mTHREAD_YIELD_UNTIL( apPT, Achieve_Gfx_Fade_IsFinished() );

	/* clear screens */
	memset( gAchieveLoginClass.mpLogic, 0, 160L*200L );
	mTHREAD_YIELD( apPT );
	memset( gAchieveLoginClass.mpLogic, 0, 160L*200L );

	gAchieveLoginClass.mFinishedFlag = 1;
	mTHREAD_SUSPEND( apPT );

	mTHREAD_END( apPT );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Thread_Boxes( sThread * apThread )
* ACTION   : Achieve_Login_Thread_Boxes
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Login_Thread_Boxes( sThread * apThread )
{
	U8	lDrawnFlag = 0;
	U16	i;

	/* clear old boxes */

	for( i=0; i<dACH_LOGIN_BOX_LIMIT; i++ )
	{
		sAchieveLoginBox * lpBox = &gAchieveLoginClass.mBoxes[ i ];

		if( lpBox->mActiveFlag && lpBox->mRedrawFlag )
		{
			sAchieveGfxPos		lPos;
			sAchieveGfxQuad *	lpQuad = &lpBox->mDrawnCoords[gAchieveLoginClass.mScreenIndex ];
			sAchieveGfxQuad		lFontClipQuad;


			lPos.mX =lpQuad->mX0 + ((lpQuad->mX1-lpQuad->mX0)>>1);
			lPos.mY =lpQuad->mY0 + ((lpQuad->mY1-lpQuad->mY0)>>1) -4;
			if( Achieve_Gfx_QuadClip( &lFontClipQuad, lpQuad, &gAchieveLoginClipBoxes ) )
			{
				Achieve_Gfx_FontDraw( lpBox->mpTitle, gAchieveLoginClass.mpLogic, 0, eACH_GFX_ALIGN_CENTRE, &lPos, &lFontClipQuad );
			}
			Achieve_Gfx_Box_Render( &lpBox->mDrawnCoords[ gAchieveLoginClass.mScreenIndex ], 0, &gAchieveLoginClipBoxes );
		}
	}

	/* draw new boxes*/

	for( i=0; i<dACH_LOGIN_BOX_LIMIT; i++ )
	{
		sAchieveLoginBox * lpBox = &gAchieveLoginClass.mBoxes[ i ];

		lpBox->mDrawnCoords[ gAchieveLoginClass.mScreenIndex ] = lpBox->mNextCoords;
		if( lpBox->mActiveFlag && lpBox->mRedrawFlag )
		{
			sAchieveGfxPos		lPos;
			sAchieveGfxQuad *	lpQuad = &lpBox->mDrawnCoords[gAchieveLoginClass.mScreenIndex ];
			sAchieveGfxQuad		lFontClipQuad;

			lPos.mX =lpQuad->mX0 + ((lpQuad->mX1-lpQuad->mX0)>>1);
			lPos.mY =lpQuad->mY0 + ((lpQuad->mY1-lpQuad->mY0)>>1) -4;
			Achieve_Gfx_QuadClip( &lFontClipQuad, lpQuad, &gAchieveLoginClipBoxes );
			if( Achieve_Gfx_QuadClip( &lFontClipQuad, lpQuad, &gAchieveLoginClipBoxes ) )
			{
				Achieve_Gfx_FontDraw( lpBox->mpTitle, gAchieveLoginClass.mpLogic, 1, eACH_GFX_ALIGN_CENTRE, &lPos, &lFontClipQuad );
			}
			Achieve_Gfx_Box_Render( &lpBox->mDrawnCoords[ gAchieveLoginClass.mScreenIndex ], 1, &gAchieveLoginClipBoxes );

			lpBox->mRedrawFlag--;
			lDrawnFlag = 1;

			if( gAchieveLoginClass.mCursorEnabledFlag && (gAchieveLoginClass.mMorphPos >= 0x100) )
			{
				if( (lpBox == gAchieveLoginClass.mpNameBox)  && (lpBox->mSlotIndex == dACH_LOGIN_SLOT_CENTRE_INDEX) )
				{
					gAchieveLoginClass.mCursorBlinkCounter--;
					if( gAchieveLoginClass.mCursorBlinkCounter <= 0 )
					{
						gAchieveLoginClass.mCursorBlinkCounter = dACH_LOGIN_CURSOR_RATE;
						gAchieveLoginClass.mCursorBlinkOnFlag ^= 1;
					}
					if( gAchieveLoginClass.mCursorBlinkOnFlag )
					{
						Achieve_Gfx_CursorDraw( lpBox->mpTitle, gAchieveLoginClass.mpLogic, 1, eACH_GFX_ALIGN_CENTRE, &lPos, &lFontClipQuad );
					}
				}
			}
		}
	}

	mTHREAD_BEGIN( apThread );

	/* move+morph boxes */

	gAchieveLoginClass.mMorphDoneFlag = 0;
	if( gAchieveLoginClass.mMorphPos < 0x100 )
	{
		gAchieveLoginClass.mMorphPos += dACH_LOGIN_MORPH_SPEED;
		if( gAchieveLoginClass.mMorphPos > 0x100 )
		{
			gAchieveLoginClass.mMorphPos = 0x100;
		}
		for( i=0; i<dACH_LOGIN_BOX_LIMIT; i++ )
		{
			sAchieveLoginBox * lpBox = &gAchieveLoginClass.mBoxes[ i ];
			if( lpBox->mActiveFlag )
			{
				Achieve_Login_Box_Morph( lpBox, gAchieveLoginClass.mMorphPos );
			}
		}
	}
	else
	{
		gAchieveLoginClass.mMorphDoneFlag = 1;
	}

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Thread_Lines( sThread * apThread )
* ACTION   : Achieve_Login_Thread_Lines
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Login_Thread_Lines( sThread * apThread )
{
	mTHREAD_BEGIN( apThread );

	gAchieveLoginClass.mLinesActiveFlag = 0;
	mTHREAD_WAIT_UNTIL( apThread, gAchieveLoginClass.mLinesActiveFlag );

	gAchieveLoginClass.mVerticalLines[ 0 ].mX0 = 0;
	gAchieveLoginClass.mVerticalLines[ 0 ].mX1 = 0;
	gAchieveLoginClass.mVerticalLines[ 0 ].mY0 = dACH_LOGIN_LINE_Y0;
	gAchieveLoginClass.mVerticalLines[ 0 ].mY1 = dACH_LOGIN_LINE_Y0;

	gAchieveLoginClass.mVerticalLines[ 1 ].mX0 = 319;
	gAchieveLoginClass.mVerticalLines[ 1 ].mX1 = 319;
	gAchieveLoginClass.mVerticalLines[ 1 ].mY0 = dACH_LOGIN_LINE_Y1;
	gAchieveLoginClass.mVerticalLines[ 1 ].mY1 = dACH_LOGIN_LINE_Y1;

	do
	{
		gAchieveLoginClass.mVerticalLines[ 0 ].mX1 += dACH_LOGIN_LINE_SPEED;
		gAchieveLoginClass.mVerticalLines[ 1 ].mX0 -= dACH_LOGIN_LINE_SPEED;

		Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 0 ], 1 );
		Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 1 ], 1 );

		mTHREAD_YIELD( apThread );
	} while( gAchieveLoginClass.mVerticalLines[ 1 ].mX0 > 0 );

	Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 0 ], 1 );
	Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 1 ], 1 );

	gAchieveLoginClass.mLinesActiveFlag = 0;
	mTHREAD_WAIT_UNTIL( apThread, gAchieveLoginClass.mLinesActiveFlag );

	gAchieveLoginClass.mVerticalLines[ 0 ].mX0 = 319;
	gAchieveLoginClass.mVerticalLines[ 0 ].mX1 = 319;
	gAchieveLoginClass.mVerticalLines[ 1 ].mX0 = 0;
	gAchieveLoginClass.mVerticalLines[ 1 ].mX1 = 0;

	do
	{
		gAchieveLoginClass.mVerticalLines[ 0 ].mX0 -= dACH_LOGIN_LINE_SPEED;
		gAchieveLoginClass.mVerticalLines[ 1 ].mX0 += dACH_LOGIN_LINE_SPEED;

		Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 0 ], 0 );
		Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 1 ], 0 );

		mTHREAD_YIELD( apThread );
	} while( gAchieveLoginClass.mVerticalLines[ 0 ].mX0 > 0 );
	Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 0 ], 0 );
	Achieve_Gfx_HLine_Render( gAchieveLoginClass.mpLogic, &gAchieveLoginClass.mVerticalLines[ 1 ], 0 );


	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Box_Morph( sAchieveLoginBox * apBox )
* ACTION   : Achieve_Login_Box_Morph
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

#define	mACH_LOGIN_MORPH_VAL( _a )	{lVal = (lpTarget->_a-lpSrc->_a); lVal *= aMorphPos; lVal >>= 8L; lpDst->_a = lpSrc->_a + (S16)lVal;}

void	Achieve_Login_Box_Morph( sAchieveLoginBox * apBox, const U16 aMorphPos )
{
	S32	lVal;
	sAchieveGfxSizePos *	lpTarget;
	sAchieveGfxSizePos *	lpSrc;
	sAchieveGfxSizePos *	lpDst;
	sAchieveGfxSizePos		lTmp;

	lpDst = &lTmp;
	lpSrc = &apBox->mPosOld;
	lpTarget = &apBox->mPosTarget;

	mACH_LOGIN_MORPH_VAL( mX );
	mACH_LOGIN_MORPH_VAL( mY );
	mACH_LOGIN_MORPH_VAL( mWidth );
	mACH_LOGIN_MORPH_VAL( mHeight );

	Achieve_Gfx_QuadFromSizePos( &apBox->mNextCoords, &lTmp );

	if( aMorphPos >= 0x100 )
	{
		apBox->mPosOld = apBox->mPosTarget;
	}

	apBox->mRedrawFlag = 2;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Thread_Titles( sThread * apThread )
* ACTION   : Achieve_Login_Thread_Titles
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Login_Thread_Titles( sThread * apThread )
{
	static	U8	i;

	mTHREAD_BEGIN( apThread );

	mTHREAD_WAIT_UNTIL( apThread, gAchieveLoginClass.mLinesActiveFlag );
	mTHREAD_WAIT_UNTIL( apThread, Achieve_Gfx_Fade_IsFinished() );
	
	for( i=0; i<2; i++ )
	{
		sAchieveGfxQuad	lQuad;
		sAchieveGfxPos	lPos;

		lQuad.mX0 = 0;
		lQuad.mY0 = 0;
		lQuad.mX1 = 319;
		lQuad.mY1 = 199;

		lPos.mX = 0;
		lPos.mY = 0;
		Achieve_Gfx_FontDraw( "RESERVOIR LOGIN", gAchieveLoginClass.mpLogic, 1, eACH_GFX_ALIGN_LEFT, &lPos, &lQuad );

		mTHREAD_YIELD( apThread );
	}

	mTHREAD_SUSPEND( apThread );
	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Box_GetFree( void )
* ACTION   : Achieve_Login_Box_GetFree
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

sAchieveLoginBox *	Achieve_Login_Box_GetFree( void )
{
	U16	i;

	for( i=eACH_LOGIN_BOXTYPE_SLOT0; i<dACH_LOGIN_BOX_LIMIT; i++ )
	{
		if( !gAchieveLoginClass.mBoxes[ i ].mActiveFlag )
		{
			return( &gAchieveLoginClass.mBoxes[ i ] );
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Box_Create( const U8 aBoxType, , const char * apTitle, const S8 aSlotIndex )
* ACTION   : Achieve_Login_Box_Create
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

sAchieveLoginBox *	Achieve_Login_Box_Create( const U8 aBoxType, const char * apTitle, const S8 aSlotIndex )
{
	sAchieveLoginBox * lpBox;

	lpBox = Achieve_Login_Box_GetFree();
	if( lpBox )
	{
		lpBox->mPosTarget = gAchieveLoginBoxSizePos[ aBoxType + aSlotIndex ];
		lpBox->mPosOld = lpBox->mPosTarget;
		lpBox->mPosOld.mWidth = 0;
		lpBox->mPosOld.mHeight = 0;
		lpBox->mActiveFlag = 1;
		lpBox->mRedrawFlag = 2;
		lpBox->mpTitle = apTitle;
		lpBox->mBoxType = aBoxType;
		lpBox->mSlotIndex = aSlotIndex;
	}
	return( lpBox );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Box_SetNewType( sAchieveLoginBox * apBox, const U8 aBoxType, const S8 aSlotIndex )
* ACTION   : Achieve_Login_Box_SetNewType
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_Box_SetNewType( sAchieveLoginBox * apBox, const U8 aBoxType, const S8 aSlotIndex  )
{
	apBox->mBoxType = aBoxType;
	apBox->mPosOld = apBox->mPosTarget;
	apBox->mSlotIndex = aSlotIndex;
	if( aSlotIndex >= 0 && aSlotIndex < dACH_LOGIN_SLOT_LIMIT )
	{
		apBox->mPosTarget = gAchieveLoginBoxSizePos[ apBox->mBoxType+aSlotIndex ];
	}

	apBox->mRedrawFlag = 2;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Boxes_MoveUp( void )
* ACTION   : Achieve_Login_Boxes_MoveUp
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_Boxes_MoveUp( void )
{
	U16	i;

	if( gAchieveLoginClass.mUserCursorY+1 < gAchieveLoginClass.mUserCount )
	{
		gAchieveLoginClass.mUserCursorY++;
		for( i=0; i<dACH_LOGIN_BOX_LIMIT; i++ )
		{
			sAchieveLoginBox * lpBox = &gAchieveLoginClass.mBoxes[i];

			if( lpBox->mActiveFlag )
			{
				if( ( lpBox->mBoxType >= eACH_LOGIN_BOXTYPE_SLOT0) && ( lpBox->mBoxType <= eACH_LOGIN_BOXTYPE_SLOT6) )
				{
					Achieve_Login_Box_SetNewType( lpBox, lpBox->mBoxType, lpBox->mSlotIndex-1 );
				}
			}
		}
		gAchieveLoginClass.mMorphPos = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_Boxes_MoveDown( void )
* ACTION   : Achieve_Login_Boxes_MoveDown
* CREATION : 8.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_Boxes_MoveDown( void )
{
	U16	i;

	if( gAchieveLoginClass.mUserCursorY )
	{
		gAchieveLoginClass.mUserCursorY--;
		for( i=0; i<dACH_LOGIN_BOX_LIMIT; i++ )
		{
			sAchieveLoginBox * lpBox = &gAchieveLoginClass.mBoxes[i];

			if( lpBox->mActiveFlag )
			{
				if( ( lpBox->mBoxType >= eACH_LOGIN_BOXTYPE_SLOT0) && ( lpBox->mBoxType <= eACH_LOGIN_BOXTYPE_SLOT5) )
				{
					Achieve_Login_Box_SetNewType( lpBox, lpBox->mBoxType, lpBox->mSlotIndex+1 );
				}
			}
		}
		gAchieveLoginClass.mMorphPos = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Login_NameBox_Update( sAchieveLoginBox * apBox, const U8 aKey )
* ACTION   : Achieve_Login_NameBox_Update
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Login_NameBox_Update( sAchieveLoginBox * apBox, const U8 aKey )
{
	U8	lAscii;

	apBox->mpTitle = &gAchieveLoginClass.mUserName.mName[ 0 ];
	apBox->mRedrawFlag = 2;

	if( aKey )
	{
		switch( aKey )
		{
		case	eIKBDSCAN_BACKSPACE:
			gAchieveLoginClass.mUserName.mName[ gAchieveLoginClass.mTextCursorX ] = 0;
			if( gAchieveLoginClass.mTextCursorX )
			{
				gAchieveLoginClass.mTextCursorX--;
				gAchieveLoginClass.mUserName.mName[ gAchieveLoginClass.mTextCursorX ] = 0;
			}
			break;
		default:
			lAscii = gAchieveLoginClass.mpCapsTable[ aKey ];
			if( lAscii >= ' ' )
			{
				gAchieveLoginClass.mUserName.mName[ gAchieveLoginClass.mTextCursorX ] = lAscii;
				if( gAchieveLoginClass.mTextCursorX < 15 )
				{
					gAchieveLoginClass.mTextCursorX++;
				}
			}
		}
	}
}


/* ################################################################################ */
