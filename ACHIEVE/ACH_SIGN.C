/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_SIGN.H"

#include	"ACH_MAIN.H"
#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SCREEN/SCREEN.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACHIEVE_SIGNIN_UN_X		20
#define	dACHIEVE_SIGNIN_UN_Y		20
#define	dACHIEVE_SIGNIN_UN_W		300
#define	dACHIEVE_SIGNIN_UN_H		10

#define	dACHIEVE_SI_V_BUTTON_W		16
#define	dACHIEVE_SI_V_BUTTON_H		16
#define	dACHIEVE_SI_V_BACK_COLOUR	8
#define	dACHIEVE_SI_V_HI_COLOUR		0

#define dACHIEVE_SI_SOFTKEY_X		48
#define dACHIEVE_SI_SOFTKEY_Y		136

#define	dACHIEVE_SI_PROFILE_COUNT	4
#define	dACHIEVE_SI_PROFILE_Y		70
#define	dACHIEVE_SI_PROFILE_H		16

#define	dACHIEVE_SI_CURSOR_Y		15

#define	dACHIEVE_SI_CURSOR_SPEED	0x1000


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_SI_MODE_ENTER,
	eACH_SI_MODE_VERIFY,
	eACH_SI_MODE_DONE,
};

enum
{
	eACH_SI_VERIFY_BUTTON_OK,
	eACH_SI_VERIFY_BUTTON_CANCEL,
	eACH_SI_VERIFY_BUTTON_LIMIT
};

enum
{
	eACH_SI_AREA_NAME,
	eACH_SI_AREA_PROFILE,
	eACH_SI_AREA_KEYBOARD,
	eACH_SI_AREA_LIMIT
};

enum
{
	eACH_SI_KEY_1,
	eACH_SI_KEY_2,
	eACH_SI_KEY_3,
	eACH_SI_KEY_4,
	eACH_SI_KEY_5,
	eACH_SI_KEY_6,
	eACH_SI_KEY_7,
	eACH_SI_KEY_8,
	eACH_SI_KEY_9,
	eACH_SI_KEY_0,
	eACH_SI_KEY_MINUS,
	eACH_SI_KEY_EQUALS,
	eACH_SI_KEY_DELETE0,
	eACH_SI_KEY_DELETE1,

	eACH_SI_KEY_Q,
	eACH_SI_KEY_W,
	eACH_SI_KEY_E,
	eACH_SI_KEY_R,
	eACH_SI_KEY_T,
	eACH_SI_KEY_Y,
	eACH_SI_KEY_U,
	eACH_SI_KEY_I,
	eACH_SI_KEY_O,
	eACH_SI_KEY_P,
	eACH_SI_KEY_BRACKETOPEN,
	eACH_SI_KEY_BRACKETCLOSE,
	eACH_SI_KEY_CAPS0,
	eACH_SI_KEY_CAPS1,

	eACH_SI_KEY_A,
	eACH_SI_KEY_S,
	eACH_SI_KEY_D,
	eACH_SI_KEY_F,
	eACH_SI_KEY_G,
	eACH_SI_KEY_H,
	eACH_SI_KEY_J,
	eACH_SI_KEY_K,
	eACH_SI_KEY_L,
	eACH_SI_KEY_COLON,
	eACH_SI_KEY_HASH,
	eACH_SI_KEY_TILDE,
	eACH_SI_KEY_LEFT,
	eACH_SI_KEY_RIGHT,

	eACH_SI_KEY_Z,
	eACH_SI_KEY_X,
	eACH_SI_KEY_C,
	eACH_SI_KEY_V,
	eACH_SI_KEY_B,
	eACH_SI_KEY_N,
	eACH_SI_KEY_M,
	eACH_SI_KEY_ARROWLEFT,
	eACH_SI_KEY_ARROWRIGHT,
	eACH_SI_KEY_PERIOD,
	eACH_SI_KEY_EXCLAMATION,
	eACH_SI_KEY_QUESTION,
	eACH_SI_KEY_ENTER0,
	eACH_SI_KEY_ENTER1,

	eACH_SI_KEY_LIMIT,
};

char	gAchieveSignInKeyboardCharacters[ eACH_SI_KEY_LIMIT ] =
{
	'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','(',')',0,0,
	'A','S','D','F','G','H','J','K','L',':','#','~',0,0,
	'Z','X','C','V','B','N','M','<','>','.','!','?',0,0,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveSignInClass
{
	sAssetClient *	mpFontAss;
	sAssetClient *	mpSmallFontAss;
	sAssetClient *	mpBGAss;
	sAssetClient *	mpPalAss;
	sAssetClient *	mpSignInAss;
	sAssetClient *	mpBigSausageAss;
	sAssetClient *	mpSmallSausageAss;
	sAssetClient *	mpCursorAss;
	sAssetClient *	mpSoftKeyAsses[ 2 ];
	sFont *			mpFont;
	sFont *			mpSmallFont;
	sSpriteBlock *	mpBigSausageSprites;
	sSpriteBlock *	mpSmallSausageSprites;
	sSpriteBlock *	mpSignInSpr;
	sSpriteBlock *	mpCursorSprites;
	sSpriteBlock *	mpSoftKeySprites[ 2 ];
	U16 *			mpPal;
	sGemDosDTA		mDTA;
	U16				mBGRedrawFlag;
	U16				mMode;
	U16				mButtonIndex;
	U16				mButtonOldIndex;
	U16				mKeyIndex;
	U16				mKeyOldIndex;
	U16				mKeyRedrawFlag;
	U16				mCurrentArea;
	U16				mNameRedrawFlag;
	U16				mProfileRedrawFlag;
	S16				mProfileTopIndex;
	U16				mCursorAnimFrame;
	sGraphicPos		mCursorPositions[ eACH_SI_AREA_LIMIT ];
	char			mUserName[ 16 ];
	U16				mFadePal[ 16 ];
}sAchieveSignInClass;


/* ###################################################################################
#  DATA
################################################################################### */

char *	gAchieveSignInVerifyButtonTexts[ eACH_SI_VERIFY_BUTTON_LIMIT ] =
{
	"OK",
	"CANCEL"
};

sGraphicRect	gAchieveSignInVerifyButtonRects[ eACH_SI_VERIFY_BUTTON_LIMIT ] =
{
	{	10,		124,	140,18	},
	{	170,	124,	140,18	},
};

sGraphicRect	gAchieveSignInVerifyRect =
{
	0,50,320,100
};

sGraphicRect	gAchieveSignInVerifyTitleRect =
{
	0,60,320,16
};

sGraphicRect	gAchieveSignInVerifyUserNameRect =
{
	0,92,320,16
};

sGraphicRect	gAchieveSignInTitleRect =
{
	0,10,320,16
};

sGraphicRect	gAchieveSignInUserNameRect =
{
	0,40,320,16
};

sGraphicPos	gAchieveSignInBigSausagePos =
{
	160-128,36
};

sAchieveSignInClass	gAchieveSignInClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		Achieve_SignIn_Enter_Update( sInput * apInput );
U16		Achieve_SignIn_Verify_Update( sInput * apInput );
void	Achieve_SignIn_SetMode( U16 aMode );
void	Achieve_SignIn_PrintCentred( const char * apString, sFont * apFont, sGraphicRect * apRect, sGraphicCanvas * apCanvas );
void	Achieve_SignIn_ProfilesRender( sGraphicCanvas * apCanvas );
void	Achieve_SignIn_NameFromProfile( void );
void	Achieve_SignIn_KeyRender( sGraphicCanvas * apCanvas, const U16 aKeyIndex, const U16 aSelectedFlag );
U16		Achieve_SignIn_AreaName_Update( sInput * apInput );
U16		Achieve_SignIn_AreaProfile_Update( sInput * apInput );
U16		Achieve_SignIn_AreaKeyboard_Update( sInput * apInput );
void	Achieve_SignIn_SetArea( const U16 aAreaIndex );
void	Achieve_SignIn_CursorRender( void );
void	Achieve_SignIn_BorderDraw( sGraphicCanvas * apCanvas, const sGraphicRect * apBorder, const U16 aColour );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_AppInit( void )
* ACTION   : Achieve_SignIn_AppInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_AppInit( void )
{
	Memory_Clear( sizeof(sAchieveSignInClass), &gAchieveSignInClass );
	gAchieveSignInClass.mpFontAss           = AssetClient_Register( "FONTX.BFB",    "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpFont                );
	gAchieveSignInClass.mpSmallFontAss      = AssetClient_Register( "FONT6X6.BFB",  "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpSmallFont           );
	gAchieveSignInClass.mpPalAss            = AssetClient_Register( "ACH.PAL",      "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpPal                 );
	gAchieveSignInClass.mpSignInAss         = AssetClient_Register( "SIGNIN.BSB",   "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpSignInSpr           );
	gAchieveSignInClass.mpCursorAss         = AssetClient_Register( "ACH_CURS.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpCursorSprites       );
	gAchieveSignInClass.mpBigSausageAss     = AssetClient_Register( "ACH_NSAS.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpBigSausageSprites   );
	gAchieveSignInClass.mpSmallSausageAss   = AssetClient_Register( "ACH_USAS.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpSmallSausageSprites );
	gAchieveSignInClass.mpSoftKeyAsses[ 0 ] = AssetClient_Register( "SOFTKEY0.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpSoftKeySprites[ 0 ] );
	gAchieveSignInClass.mpSoftKeyAsses[ 1 ] = AssetClient_Register( "SOFTKEY1.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveSignInClass.mpSoftKeySprites[ 1 ] );
	gAchieveSignInClass.mMode               = eACH_SI_MODE_ENTER;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_AppDeInit( void )
* ACTION   : Achieve_SignIn_AppDeInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_AppDeInit( void )
{
	AssetClient_UnRegister( gAchieveSignInClass.mpFontAss         );
	AssetClient_UnRegister( gAchieveSignInClass.mpSmallFontAss    );
	AssetClient_UnRegister( gAchieveSignInClass.mpPalAss          );
	AssetClient_UnRegister( gAchieveSignInClass.mpSignInAss       );
	AssetClient_UnRegister( gAchieveSignInClass.mpCursorAss       );
	AssetClient_UnRegister( gAchieveSignInClass.mpBigSausageAss   );
	AssetClient_UnRegister( gAchieveSignInClass.mpSmallSausageAss );
	AssetClient_UnRegister( gAchieveSignInClass.mpSoftKeyAsses[0] );
	AssetClient_UnRegister( gAchieveSignInClass.mpSoftKeyAsses[1] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_Init( void )
* ACTION   : Achieve_SignIn_Init
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_Init( void )
{
	U16	i;
	U16	lUserIndex;

	Achieve_Load( "AWARDS.ACH", 0, 0 );

	gAchieveSignInClass.mBGRedrawFlag = 2;
	gAchieveSignInClass.mKeyRedrawFlag = 2;
	gAchieveSignInClass.mProfileRedrawFlag = 2;
	gAchieveSignInClass.mNameRedrawFlag = 2;
	gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX = 0;

	gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX = 0;
	gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY = 0;
	gAchieveSignInClass.mCursorAnimFrame = 0;

	lUserIndex = Achieve_GetUserIndex( gAchieveSignInClass.mUserName );
	if( lUserIndex >= dACHIEVE_SI_PROFILE_COUNT )
	{
		gAchieveSignInClass.mProfileTopIndex                            = lUserIndex - (dACHIEVE_SI_PROFILE_COUNT - 1);
		gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY = dACHIEVE_SI_PROFILE_COUNT - 1;
	}
	else
	{
		gAchieveSignInClass.mProfileTopIndex                            = 0;
		gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY = lUserIndex;
	}

	for( i=0; i<16; i++ )
	{
		gAchieveSignInClass.mUserName[i] = 0;
	}

	if( gAchieveSignInClass.mpPal )
	{
		for( i=0; i<16; i++ )
		{
			gAchieveSignInClass.mFadePal[ i ] = gAchieveSignInClass.mpPal[ 0 ];
		}
	}

	Fade_StartVblFade( &gAchieveSignInClass.mFadePal[ 0 ], 16 );

	Achieve_SignIn_SetMode( eACH_SI_MODE_ENTER );

	if( Achieve_GetUserCount() )
	{
		Achieve_SignIn_SetArea( eACH_SI_AREA_PROFILE );
	}
	else
	{
		Achieve_SignIn_SetArea( eACH_SI_AREA_KEYBOARD );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_DeInit( void )
* ACTION   : Achieve_SignIn_DeInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_DeInit( void )
{
	Achieve_SetUserInfo( gAchieveSignInClass.mUserName );
	Achieve_Save( "AWARDS.ACH" );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16		Achieve_SignIn_Update( sInput * apInput )
{
	U16	lRes;

	lRes = 0;

	if( Fade_IsVblFadeFinished() )
	{
		switch( gAchieveSignInClass.mMode )
		{
		case	eACH_SI_MODE_ENTER:
			if( Achieve_SignIn_Enter_Update( apInput ) )
			{
				gAchieveSignInClass.mButtonIndex = eACH_SI_VERIFY_BUTTON_OK;
				Achieve_SignIn_SetMode( eACH_SI_MODE_VERIFY );
				Fade_StartVblFade( &gAchieveSignInClass.mFadePal[ 0 ], 16 );
			}
			break;
		case	eACH_SI_MODE_VERIFY:
			if( Achieve_SignIn_Verify_Update( apInput ) )
			{
				if( gAchieveSignInClass.mButtonIndex == eACH_SI_VERIFY_BUTTON_CANCEL )
				{
					Achieve_SignIn_SetMode( eACH_SI_MODE_ENTER );
					gAchieveSignInClass.mBGRedrawFlag = 2;
					Fade_StartVblFade( &gAchieveSignInClass.mFadePal[ 0 ], 16 );
				}
				else
				{
					Achieve_SignIn_SetMode( eACH_SI_MODE_DONE );
					Fade_StartVblFade( &gAchieveSignInClass.mFadePal[ 0 ], 32 );
					lRes = 1;
				}
			}
			break;
		case	eACH_SI_MODE_DONE:
			lRes = 1;
			break;
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_Enter_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_Enter_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_SignIn_Enter_Update( sInput * apInput )
{
	U16					lRes;
	U16					i,j,k;
	sGraphicCanvas *	lpLogic;
	sGraphicCanvas *	lpBack;
	sGraphicPos			lPos;

	lpBack  = Screen_GetpBackGraphic();
	lpLogic = Screen_GetpLogicGraphic();
	lRes    = 0;

	if( gAchieveSignInClass.mBGRedrawFlag )
	{
		if( 2 == gAchieveSignInClass.mBGRedrawFlag )
		{
			lpBack->mpFuncs->ClearScreen( lpBack );
			if( gAchieveSignInClass.mpSignInSpr )
			{
				lPos.mX = 0;
				lPos.mY = 0;
				lpBack->mpFuncs->DrawSprite( lpBack, &lPos, gAchieveSignInClass.mpSignInSpr->mpSprite[0] );
			}

			lPos.mY = dACHIEVE_SI_SOFTKEY_Y;
			k = 0;
			for( i=0; i<4; i++ )
			{
				lPos.mX = dACHIEVE_SI_SOFTKEY_X;
				for( j=0; j<14; j++ )
				{
					lpBack->mpFuncs->DrawSprite( lpBack, &lPos, gAchieveSignInClass.mpSoftKeySprites[0]->mpSprite[k] );
					lPos.mX += 16;
					k++;
				}
				lPos.mY += 16;
			}

			Achieve_SignIn_ProfilesRender( lpBack );
			lpBack->mpFuncs->DrawSprite( lpBack, &gAchieveSignInBigSausagePos, gAchieveSignInClass.mpBigSausageSprites->mpSprite[ 0 ] );
		}

/*		lpLogic->mpFuncs->CopyScreen( lpLogic, lpBack->mpVRAM );*/
		Memory_Copy( 32000, lpBack->mpVRAM, lpLogic->mpVRAM );

		gAchieveSignInClass.mBGRedrawFlag--;
		if( !gAchieveSignInClass.mBGRedrawFlag )
		{
			Fade_StartVblFade( gAchieveSignInClass.mpPal, 16 );
		}
	}

	if( gAchieveSignInClass.mProfileRedrawFlag )
	{
		gAchieveSignInClass.mProfileRedrawFlag--;
		Achieve_SignIn_ProfilesRender( lpLogic );
	}
	if( gAchieveSignInClass.mKeyRedrawFlag )
	{
		gAchieveSignInClass.mKeyRedrawFlag--;
		Achieve_SignIn_KeyRender( lpLogic, gAchieveSignInClass.mKeyOldIndex, 0 );
		if( eACH_SI_AREA_KEYBOARD == gAchieveSignInClass.mCurrentArea )
		{
			Achieve_SignIn_KeyRender( lpLogic, gAchieveSignInClass.mKeyIndex,    1 );
		}
	}

	if( gAchieveSignInClass.mNameRedrawFlag )
	{
		char lString[ 18 ];
		lpLogic->mpFuncs->DrawSprite( lpLogic, &gAchieveSignInBigSausagePos, gAchieveSignInClass.mpBigSausageSprites->mpSprite[ 0 ] );
		Achieve_SignIn_PrintCentred( &gAchieveSignInClass.mUserName[0], gAchieveSignInClass.mpFont, &gAchieveSignInUserNameRect, lpLogic );
		gAchieveSignInClass.mNameRedrawFlag--;
		lPos.mX = 0;
		lPos.mY = gAchieveSignInUserNameRect.mY + dACHIEVE_SI_CURSOR_Y;
		for( i=0; (S16)i<gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX; i++ )
		{
			lString[ i ] = gAchieveSignInClass.mUserName[ i ];
		}
		lString[ i ] = 0;
		lPos.mX = 160 + (Font_GetStringWidth(gAchieveSignInClass.mpFont,lString)>>1);

		lpLogic->mpFuncs->DrawSprite( lpLogic, &lPos, gAchieveSignInClass.mpCursorSprites->mpSprite[ 0 ] );
	}

	Achieve_SignIn_CursorRender();


	if( IKBD_GetKeyPressedFlag() )
	{
		U8	lKey;

		lKey = IKBD_GetLastKeyPress();
		IKBD_ClearKeyPressedFlag();

		switch( lKey )
		{
		case	eIKBDSCAN_BACKSPACE:
			gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
			if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX > 0 )
			{
				gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX--;
				gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
				gAchieveSignInClass.mNameRedrawFlag = 2;
			}
			break;
		case	eIKBDSCAN_NUMPADENTER:
		case	eIKBDSCAN_RETURN:
			lRes = 1;
			break;
		default:
			lKey = IKBD_ConvertScancodeAsciiCaps( lKey );
			if( ((lKey > ' ') && (lKey < 128)) ||
				((lKey == ' ') && gAchieveSignInClass.mCurrentArea == eACH_SI_AREA_NAME) )
			{
				gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = lKey;
				if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX < 14 )
				{
					gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX++;
					gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
					gAchieveSignInClass.mNameRedrawFlag = 2;
				}
			}
			break;
		}
	}

	switch( gAchieveSignInClass.mCurrentArea )
	{
	case	eACH_SI_AREA_NAME:
		lRes  |= Achieve_SignIn_AreaName_Update( apInput );
		break;
	case	eACH_SI_AREA_PROFILE:
		lRes  |= Achieve_SignIn_AreaProfile_Update( apInput );
		break;
	case	eACH_SI_AREA_KEYBOARD:
		lRes  |= Achieve_SignIn_AreaKeyboard_Update( apInput );
		break;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_Verify_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_Verify_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_SignIn_Verify_Update( sInput * apInput )
{
	U16					lRes;
	sGraphicCanvas *	lpLogic;
	sGraphicCanvas *	lpBack;

	lpBack          = Screen_GetpBackGraphic();
	lpLogic         = Screen_GetpLogicGraphic();
	lRes = 0;

	if( gAchieveSignInClass.mBGRedrawFlag )
	{
		if( 2 == gAchieveSignInClass.mBGRedrawFlag )
		{
			lpBack->mpFuncs->ClearScreen( lpBack );
			lpBack->mpFuncs->DrawBox( lpBack, &gAchieveSignInVerifyRect, dACHIEVE_SI_V_BACK_COLOUR );
			if( gAchieveSignInClass.mpFont )
			{
				Achieve_SignIn_PrintCentred(
					"SIGNING IN AS",
					gAchieveSignInClass.mpFont,
					&gAchieveSignInVerifyTitleRect,
					lpBack );

				Achieve_SignIn_PrintCentred(
					gAchieveSignInClass.mUserName,
					gAchieveSignInClass.mpFont,
					&gAchieveSignInVerifyUserNameRect,
					lpBack );

				Achieve_SignIn_PrintCentred(
					gAchieveSignInVerifyButtonTexts[ 0 ],
					gAchieveSignInClass.mpFont,
					&gAchieveSignInVerifyButtonRects[ 0 ],
					lpBack );

				Achieve_SignIn_PrintCentred(
					gAchieveSignInVerifyButtonTexts[ 1 ],
					gAchieveSignInClass.mpFont,
					&gAchieveSignInVerifyButtonRects[ 1 ],
					lpBack );
			}
		}

/*		lpLogic->mpFuncs->CopyScreen( lpLogic, lpBack->mpVRAM ); */
		Memory_Copy( 32000, lpBack->mpVRAM, lpLogic->mpVRAM );
		gAchieveSignInClass.mBGRedrawFlag--;
		if( !gAchieveSignInClass.mBGRedrawFlag )
		{
			Fade_StartVblFade( gAchieveSignInClass.mpPal, 16 );
		}
	}

/*
	lpLogic->mpFuncs->DrawBox( lpLogic, &gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonOldIndex ], dACHIEVE_SI_V_BACK_COLOUR );
	lpLogic->mpFuncs->DrawBox( lpLogic, &gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonIndex ], dACHIEVE_SI_V_HI_COLOUR );
*/

	Achieve_SignIn_BorderDraw( lpLogic, &gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonOldIndex ], dACHIEVE_SI_V_BACK_COLOUR );
	Achieve_SignIn_BorderDraw( lpLogic, &gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonIndex ], dACHIEVE_SI_V_HI_COLOUR );

	Achieve_SignIn_PrintCentred(
		gAchieveSignInVerifyButtonTexts[ gAchieveSignInClass.mButtonIndex ],
		gAchieveSignInClass.mpFont,
		&gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonIndex ],
		lpLogic );

	Achieve_SignIn_PrintCentred(
		gAchieveSignInVerifyButtonTexts[ gAchieveSignInClass.mButtonOldIndex ],
		gAchieveSignInClass.mpFont,
		&gAchieveSignInVerifyButtonRects[ gAchieveSignInClass.mButtonOldIndex ],
		lpLogic );

	if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mButtonIndex )
		{
			gAchieveSignInClass.mButtonOldIndex = gAchieveSignInClass.mButtonIndex;
			gAchieveSignInClass.mButtonIndex--;
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mButtonIndex < (eACH_SI_VERIFY_BUTTON_LIMIT-1) )
		{
			gAchieveSignInClass.mButtonOldIndex = gAchieveSignInClass.mButtonIndex;
			gAchieveSignInClass.mButtonIndex++;
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_FIREA ] & eINPUTKEYSTATUS_HIT )
	{
		lRes = 1;
	}

	if( IKBD_GetKeyPressedFlag() )
	{
		U8	lKey;

		lKey = IKBD_GetLastKeyPress();

		switch( lKey )
		{
		case	eIKBDSCAN_ESC:
		case	eIKBDSCAN_BACKSPACE:
			gAchieveSignInClass.mButtonIndex = eACH_SI_VERIFY_BUTTON_CANCEL;
		case	eIKBDSCAN_NUMPADENTER:
		case	eIKBDSCAN_RETURN:
			IKBD_ClearKeyPressedFlag();
			lRes = 1;
			break;
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_SetMode( U16 aMode )
* ACTION   : Achieve_SignIn_SetMode
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_SetMode( U16 aMode )
{
	gAchieveSignInClass.mMode         = aMode;
	gAchieveSignInClass.mBGRedrawFlag = 2;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_PrintCentred( const char * apString, sFont * apFont, sGraphicRect * apRect, sGraphicCanvas * apCanvas )
* ACTION   : Achieve_SignIn_PrintCentred
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_PrintCentred( const char * apString, sFont * apFont, sGraphicRect * apRect, sGraphicCanvas * apCanvas )
{
	S16			lStringW;
	sGraphicPos	lPos;

	lStringW = Font_GetStringWidth( apFont, apString );
	lPos.mX  = apRect->mX + (apRect->mWidth>>1) - (lStringW>>1);
	lPos.mY  = apRect->mY + (apRect->mHeight>>1) - (apFont->mHeightMax>>1);
	apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, apFont, apString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_AreaName_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_AreaName_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_SignIn_AreaName_Update( sInput * apInput )
{
	if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX )
		{
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX--;
		}
	}
	if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX < 14 )
		{
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX++;
		}
	}
	if( apInput->mKeyStatus[ eINPUTKEY_DOWN ] & eINPUTKEYSTATUS_HIT )
	{
		if( Achieve_GetUserCount() )
		{
			Achieve_SignIn_SetArea( eACH_SI_AREA_PROFILE );
		}
		else
		{
			Achieve_SignIn_SetArea( eACH_SI_AREA_KEYBOARD );
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_AreaProfile_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_AreaProfile_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_SignIn_AreaProfile_Update( sInput * apInput )
{
	if( apInput->mKeyStatus[ eINPUTKEY_DOWN ] & eINPUTKEYSTATUS_HIT )
	{
		gAchieveSignInClass.mProfileRedrawFlag = 2;

		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY + gAchieveSignInClass.mProfileTopIndex + 1 >= (S16)Achieve_GetUserCount() )
		{
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY = 0;
			Achieve_SignIn_SetArea( eACH_SI_AREA_KEYBOARD );
		}
		else
		{
			if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY < (dACHIEVE_SI_PROFILE_COUNT-1) )
			{
				gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY++;
			}
			else
			{
				gAchieveSignInClass.mProfileTopIndex++;
			}
			Achieve_SignIn_NameFromProfile();
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_UP ] & eINPUTKEYSTATUS_HIT )
	{
		gAchieveSignInClass.mProfileRedrawFlag = 2;
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY )
		{
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY--;
		}
		else
		{
			if( gAchieveSignInClass.mProfileTopIndex )
			{
				gAchieveSignInClass.mProfileTopIndex--;
			}
		}
		Achieve_SignIn_NameFromProfile();
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_AreaKeyboard_Update( sInput * apInput )
* ACTION   : Achieve_SignIn_AreaKeyboard_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_SignIn_AreaKeyboard_Update( sInput * apInput )
{
	U16					lRes;

	lRes    = 0;

	if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX )
		{
			gAchieveSignInClass.mKeyOldIndex = gAchieveSignInClass.mKeyIndex;
			gAchieveSignInClass.mKeyRedrawFlag = 2;
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX--;
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX < 13 )
		{
			gAchieveSignInClass.mKeyOldIndex = gAchieveSignInClass.mKeyIndex;
			gAchieveSignInClass.mKeyRedrawFlag = 2;
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX++;
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_DOWN ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY < 3 )
		{
			gAchieveSignInClass.mKeyOldIndex = gAchieveSignInClass.mKeyIndex;
			gAchieveSignInClass.mKeyRedrawFlag = 2;
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY++;
		}
	}
	if( apInput->mKeyStatus[ eINPUTKEY_UP ] & eINPUTKEYSTATUS_HIT )
	{
		gAchieveSignInClass.mKeyOldIndex = gAchieveSignInClass.mKeyIndex;
		gAchieveSignInClass.mKeyRedrawFlag = 2;
		if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY )
		{
			gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY--;
		}
		else
		{
			if( Achieve_GetUserCount() )
			{
				gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY = Achieve_GetUserCount() - 1;
				if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY >= dACHIEVE_SI_PROFILE_COUNT )
				{
					gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY = dACHIEVE_SI_PROFILE_COUNT-1;
				}
				Achieve_SignIn_SetArea( eACH_SI_AREA_PROFILE );
			}
		}
	}

	gAchieveSignInClass.mKeyIndex  = gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mY * 14;
	gAchieveSignInClass.mKeyIndex += gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_KEYBOARD ].mX;

	switch( gAchieveSignInClass.mKeyIndex )
	{
	case	eACH_SI_KEY_CAPS1:
	case	eACH_SI_KEY_DELETE1:
	case	eACH_SI_KEY_ENTER1:
		gAchieveSignInClass.mKeyIndex--;
		break;
	}

	if( apInput->mKeyStatus[ eINPUTKEY_FIREA ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveSignInKeyboardCharacters[ gAchieveSignInClass.mKeyIndex ] )
		{
			gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = gAchieveSignInKeyboardCharacters[ gAchieveSignInClass.mKeyIndex ];
			if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX < 14 )
			{
				gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX++;
			}
			gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
			gAchieveSignInClass.mNameRedrawFlag = 2;
		}
		else
		{
			switch( gAchieveSignInClass.mKeyIndex )
			{
			case	eACH_SI_KEY_DELETE0:
			case	eACH_SI_KEY_DELETE1:
				gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
				if( gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX > 0 )
				{
					gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX--;
					gAchieveSignInClass.mUserName[ gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX ] = 0;
					gAchieveSignInClass.mNameRedrawFlag = 2;
				}
				break;
			case	eACH_SI_KEY_ENTER0:
			case	eACH_SI_KEY_ENTER1:
				lRes = 1;
				break;
			}
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_ProfilesRender( sGraphicCanvas * apCanvas )
* ACTION   : Achieve_SignIn_ProfilesRender
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_ProfilesRender( sGraphicCanvas * apCanvas )
{
	U16				lColour;
	U16				lCount;
	U16				lIndex;
	U16				i;
	S16				lY;
	sGraphicRect	lRect;
	sGraphicPos		lPos;

	lRect.mHeight = dACHIEVE_SI_PROFILE_H;
	lRect.mWidth  = 320;
	lRect.mX      = 0;
	lRect.mY      = 0;
	lY     = dACHIEVE_SI_PROFILE_Y;
	lIndex = gAchieveSignInClass.mProfileTopIndex;
	lCount = Achieve_GetUserCount();
	for( i=0; i<dACHIEVE_SI_PROFILE_COUNT; i++ )
	{
		lRect.mY = lY;

		lPos.mX = (160-(108/2));
		lPos.mY = lY - 4;

		if( ((S16)i == gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY) &&
			( eACH_SI_AREA_PROFILE == gAchieveSignInClass.mCurrentArea ) )
		{
			lPos.mY += 2;
			lRect.mY++;
			lColour = 0;
		}
		else
		{
			lColour = 1;
		}


		if( lIndex < lCount )
		{
			apCanvas->mpFuncs->DrawSprite( apCanvas, &lPos, gAchieveSignInClass.mpSmallSausageSprites->mpSprite[ lColour ] );
			Graphic_FontPrintCentred( apCanvas, gAchieveSignInClass.mpSmallFont, &lRect, Achieve_GetUserName( lIndex ) );
		}
		else
		{
			lRect.mY = lY;
			apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, 0 );
		}

		lIndex++;
		lY += dACHIEVE_SI_PROFILE_H;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_KeyRender( sGraphicCanvas * apCanvas, const U16 aKeyIndex, const U16 aSelectedFlag )
* ACTION   : Achieve_SignIn_KeyRender
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_KeyRender( sGraphicCanvas * apCanvas, const U16 aKeyIndex, const U16 aSelectedFlag )
{
	sGraphicPos			lPos;
	sGraphicCanvas *	lpLogic;

	lpLogic = Screen_GetpLogicGraphic();
	lPos.mX  = (aKeyIndex % 14) << 4;
	lPos.mY  = (aKeyIndex / 14) << 4;
	lPos.mX += dACHIEVE_SI_SOFTKEY_X;
	lPos.mY += dACHIEVE_SI_SOFTKEY_Y;
	lpLogic->mpFuncs->DrawSprite( apCanvas, &lPos, gAchieveSignInClass.mpSoftKeySprites[aSelectedFlag]->mpSprite[ aKeyIndex ] );
	switch( aKeyIndex )
	{
	case	eACH_SI_KEY_CAPS0:
	case	eACH_SI_KEY_DELETE0:
	case	eACH_SI_KEY_ENTER0:
		lPos.mX += 16;
		lpLogic->mpFuncs->DrawSprite( apCanvas, &lPos, gAchieveSignInClass.mpSoftKeySprites[aSelectedFlag]->mpSprite[ aKeyIndex+1 ] );
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_SetArea( const U16 aAreaIndex )
* ACTION   : Achieve_SignIn_SetArea
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_SetArea( const U16 aAreaIndex )
{
	Assert( aAreaIndex < eACH_SI_AREA_LIMIT );
	gAchieveSignInClass.mKeyRedrawFlag = 2;
	gAchieveSignInClass.mProfileRedrawFlag = 2;
	gAchieveSignInClass.mCurrentArea = aAreaIndex;
	switch( aAreaIndex )
	{
	case eACH_SI_AREA_PROFILE:
		Achieve_SignIn_NameFromProfile();
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_NameFromProfile( void )
* ACTION   : Achieve_SignIn_NameFromProfile
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_NameFromProfile( void )
{
	const char * lpName;
	U16			i;

	lpName = Achieve_GetUserName( gAchieveSignInClass.mProfileTopIndex + gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_PROFILE ].mY );
	if( lpName )
	{
		i = 0;
		while( lpName[ i ] && (i<15) )
		{
			gAchieveSignInClass.mUserName[ i ] = lpName[ i ];
			i++;
		}
		gAchieveSignInClass.mUserName[ i ] = 0;
		gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX = i;
		gAchieveSignInClass.mNameRedrawFlag = 2;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SignIn_CursorRender( void )
* ACTION   : Achieve_SignIn_CursorRender
* CREATION : 23.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SignIn_CursorRender( void )
{
	char				lString[ 18 ];
	U16					i;
	sGraphicCanvas *	lpBack;
	sGraphicCanvas *	lpLogic;
	sGraphicPos			lPos;
	sGraphicRect		lRect;

	lpBack  = Screen_GetpBackGraphic();
	lpLogic = Screen_GetpLogicGraphic();

	lpLogic->mpFuncs->DrawSprite( lpLogic, &gAchieveSignInBigSausagePos, gAchieveSignInClass.mpBigSausageSprites->mpSprite[ 0 ] );
	Achieve_SignIn_PrintCentred( &gAchieveSignInClass.mUserName[0], gAchieveSignInClass.mpFont, &gAchieveSignInUserNameRect, lpLogic );
	gAchieveSignInClass.mNameRedrawFlag--;
	lPos.mX = 0;
	lPos.mY = gAchieveSignInUserNameRect.mY + dACHIEVE_SI_CURSOR_Y;
	for( i=0; (S16)i<gAchieveSignInClass.mCursorPositions[ eACH_SI_AREA_NAME ].mX; i++ )
	{
		lString[ i ] = gAchieveSignInClass.mUserName[ i ];
	}
	lString[ i ] = 0;
	lPos.mX = 160 + (Font_GetStringWidth(gAchieveSignInClass.mpFont,lString)>>1);

	gAchieveSignInClass.mCursorAnimFrame += dACHIEVE_SI_CURSOR_SPEED;
	if( gAchieveSignInClass.mCursorAnimFrame & 0x8000  )
	{
		lRect.mX = lPos.mX;
		lRect.mY = lPos.mY;
		lRect.mWidth = gAchieveSignInClass.mpCursorSprites->mpSprite[ 0 ]->mWidth;
		lRect.mHeight = gAchieveSignInClass.mpCursorSprites->mpSprite[ 0 ]->mHeight;
		lpLogic->mpFuncs->Blit( lpLogic, &lPos, &lRect, lpBack );
	}
	else
	{
		lpLogic->mpFuncs->DrawSprite( lpLogic, &lPos, gAchieveSignInClass.mpCursorSprites->mpSprite[ 0 ] );
	}
}

void	Achieve_SignIn_BorderDraw( sGraphicCanvas * apCanvas, const sGraphicRect * apBorder, const U16 aColour )
{
	sGraphicRect	lRect;

	lRect.mX = apBorder->mX;
	lRect.mY = apBorder->mY;
	lRect.mWidth = apBorder->mWidth;
	lRect.mHeight = 1;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, aColour );

	lRect.mX = apBorder->mX;
	lRect.mY = apBorder->mY;
	lRect.mWidth = 1;
	lRect.mHeight = apBorder->mHeight;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, aColour );

	lRect.mX = apBorder->mX + apBorder->mWidth;
	lRect.mY = apBorder->mY;
	lRect.mWidth = 1;
	lRect.mHeight = apBorder->mHeight;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, aColour );

	lRect.mX = apBorder->mX;
	lRect.mY = apBorder->mY + apBorder->mHeight;
	lRect.mWidth = apBorder->mWidth;
	lRect.mHeight = 1;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, aColour );
}


/* ################################################################################ */
