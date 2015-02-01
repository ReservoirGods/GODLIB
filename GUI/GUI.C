/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GUI.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/GUI/GUIDATA.H>
#include	<GODLIB/GUI/GUIEDIT.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/HASHTREE/HASHTREE.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<STRING.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

#define	dGUI_WINDOWSTACK_LIMIT	16
#define	dGUI_EVENTQ_LIMIT		32

enum
{
	eGUI_MODE_NORMAL,
	eGUI_MODE_TEXTEDIT,
	eGUI_MODE_HOLDING,

	eGUI_MODE_LIMIT
};

enum
{
	eGUI_MOUSEMODE_NORMAL,
	eGUI_MOUSEMODE_DRAGGING,

	eGUI_MOUSEMODE_LIMIT
};


/* ###################################################################################
#  DATA
################################################################################### */

typedef	struct	sGuiScreen
{
	S16	mWidth;
	S16	mHeight;
	U16	mColourDepth;
} sGuiScreen;


typedef	struct sGuiClass
{
	sGuiScreen		mScreen;
	sGuiMouse		mMouseCursor;
	sGuiMouse		mMouseCursorDelta;
	sGuiData *		mpData;
	sGuiTextLine	mTextLine;
	sGuiPos			mHeldPos;
	sGuiPos			mSliderHeldPos;
	sHashTree *		mpTree;
	U16				mMode;
	U16				mMouseMode;
	U16				mWindowStackPos;
	sGuiIKBD		mIKBD;
	sGuiWindow *	mpWindows[ dGUI_WINDOWSTACK_LIMIT ];
	sGuiInfo *		mpFocusControl;
	sGuiList *		mpFocusList;
	sGuiSlider *	mpFocusSlider;
	fGuiFocus		mpCustomFocus;
	fGuiDeFocus		mpCustomDeFocus;
	U16				mEventQueueHead;
	U16				mEventQueueTail;
	U8				mShutDownFlag;
	sGuiEvent		mEventQueue[ dGUI_EVENTQ_LIMIT ];
} sGuiClass;


sGuiClass	gGuiClass;
char		gGuiString[ 128 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


sGuiWindow *	Gui_GetpWindow( sGuiClass * apData );
void			Gui_WindowOpen( sGuiClass * apData, sGuiWindow * apWindow );
void			Gui_WindowClose( sGuiClass * apData, sGuiWindow * apWindow );
void			Gui_WindowRefresh( sGuiClass * apData, sGuiWindow * apWindow );
U16				Gui_GetWindowIndex( sGuiClass * apData, sGuiWindow * apWindow );

void			GuiWindow_DeBounceSprings( sGuiWindow * apWindow );

void			Gui_SetWorldPos( sGuiClass * apData );
void			GuiButton_SetWorldPos( sGuiButton * apButton, const sGuiRect * apParentRect, const sGuiRect * apScroll );
void			GuiList_SetWorldPos( sGuiList * apSlider, const sGuiRect * apParentRect, const sGuiRect * apScroll );
void			GuiSlider_SetWorldPos( sGuiSlider * apSlider, const sGuiRect * apParentRect, const sGuiRect * apScroll );
void			GuiWindow_SetWorldPos( sGuiWindow * apWindow, const sGuiRect * apParentRect, const sGuiRect * apScroll );
void			GuiWindowControls_SetWorldPos( sGuiWindow * apWindow );
void			GuiInfo_SetAbsRect( sGuiInfo * apWindow, const sGuiRect * apParentRect );

void			GuiWindow_EventOpen( sGuiWindow * apWindow );
void			GuiWindow_EventClose( sGuiWindow * apWindow );
void			GuiWindow_EventRefresh( sGuiWindow * apWindow );

sGuiInfo *		Gui_GetFocusControl( sGuiClass * apData );
sGuiInfo *		GuiSlider_GetFocusControl( sGuiSlider * apSlider, S16 aX, S16 aY );
sGuiInfo *		GuiWindow_GetFocusControl( sGuiWindow * apWindow, S16 aX, S16 aY );

void			GuiRect_Build( const sGuiRect * apParent, const sGuiRect * apChild, sGuiRect * apNew );
void			GuiRect_BuildScrolled( const sGuiRect * apParent, const sGuiRect * apChild, sGuiRect * apNew, const sGuiRect * apScroll );
U8				GuiRect_ContainsPoint( const sGuiRect * apRect, const S16 aX, const S16 aY );

void			Gui_MouseUpdate( sGuiClass * apData );
void			Gui_IkbdUpdate( sGuiIKBD * apIKBD );

void			GuiInfo_FocusUpdate( sGuiInfo * apInfo, sGuiClass * apData );
void			GuiInfo_DeFocusUpdate( sGuiInfo * apInfo, sGuiClass * apData );
void			GuiInfo_HeldUpdate( sGuiInfo * apInfo, sGuiClass * apData );

void			GuiButton_FocusUpdate( sGuiButton * apButton, sGuiClass * apData );
void			GuiButton_DeFocusUpdate( sGuiButton * apButton, sGuiClass * apData );
void			GuiButton_HeldUpdate( sGuiButton * apButton, sGuiClass * apData );

void			GuiList_FocusUpdate( sGuiList * apList, sGuiClass * apData );

void			GuiSlider_FocusUpdate( sGuiSlider * apSlider, sGuiClass * apData );
void			GuiSlider_DeFocusUpdate( sGuiSlider * apSlider, sGuiClass * apData );
void			GuiSlider_HeldUpdate( sGuiSlider * apSlider, sGuiClass * apData );
void			GuiSlider_Update( sGuiSlider * apSlider, const U8 aUpdatFromPosFlag );

void			GuiSlider_LineAdd( sGuiSlider * apSlider );
void			GuiSlider_LineSub( sGuiSlider * apSlider );
void			GuiSlider_PageAdd( sGuiSlider * apSlider );
void			GuiSlider_PageSub( sGuiSlider * apSlider );
void			GuiSlider_ValueAdd( sGuiSlider * apSlider, const S32 aAdd );

void			GuiWindow_FocusUpdate( sGuiWindow * apButton, sGuiClass * apData );
void			GuiWindow_DeFocusUpdate( sGuiWindow * apButton, sGuiClass * apData );

void			GuiAction_Update( sGuiAction * apAction, sGuiMouse * apMouse, sGuiClass * apData );
U8				GuiMouse_GetButton( const U8 aOldState, const U8 aNewState );

S32				GuiVar_ReadS32( const sGuiVar * apVar );
void			GuiVar_WriteS32( sGuiVar * apVar, const S32 aValue );
void			Gui_OnStringVarWrite( sHashTreeVarClient * apClient );

void			GuiKeyActions_Update( sGuiClass * apClass );
void			GuiKeyAction_Update(const sGuiKeyAction * apKey,const sGuiIKBD * apIKBD);


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_Init( sHashTree * apTree )
* ACTION   : Gui_Init
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_Init( sHashTree * apTree )
{
	IKBD_EnableMouse();
	Memory_Clear( sizeof(sGuiClass), &gGuiClass );
	gGuiClass.mpData = 0;
	gGuiClass.mpTree = apTree;
	gGuiClass.mMouseCursor.mpSprite = 0;
	gGuiClass.mScreen.mColourDepth = 2;
	gGuiClass.mScreen.mHeight = 200;
	gGuiClass.mScreen.mWidth = 320;
	gGuiClass.mMode = eGUI_MODE_NORMAL;
	gGuiClass.mpFocusControl = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_DeInit( void )
* ACTION   : Gui_DeInit
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_DeInit( void )
{
	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_Update( void )
* ACTION   : Gui_Update
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	Gui_Update( void )
{
	sGuiClass *		lpData;
	sGuiInfo *		lpControl;
	sGuiEvent *		lpEvent;

	lpData = &gGuiClass;

	DebugLog_Printf0( "Gui_Update()" );
	while( lpData->mEventQueueHead != lpData->mEventQueueTail )
	{
		lpEvent = &lpData->mEventQueue[ lpData->mEventQueueHead ];
		switch( lpEvent->mEvent )
		{
		case	eGUIEVENT_BUTTON_LEFTCLICK:
		case	eGUIEVENT_BUTTON_LEFTHELD:
		case	eGUIEVENT_BUTTON_LEFTRELEASE:
		case	eGUIEVENT_BUTTON_RIGHTCLICK:
		case	eGUIEVENT_BUTTON_RIGHTHELD:
		case	eGUIEVENT_BUTTON_RIGHTRELEASE:
		case	eGUIEVENT_BUTTON_UPDATE:
			break;
		case	eGUIEVENT_WINDOW_CLOSE:
			DebugLog_Printf1( "Gui EventQ %d : WindowClose", lpData->mEventQueueHead );
			Gui_WindowClose( lpData, (sGuiWindow*)lpEvent->mpInfo );
			break;
		case	eGUIEVENT_WINDOW_OPEN:
			DebugLog_Printf1( "Gui EventQ %d: WindowOpen", lpData->mEventQueueHead );
			Gui_WindowOpen( lpData, (sGuiWindow*)lpEvent->mpInfo );
			break;
		case	eGUIEVENT_WINDOW_REFRESH:
			DebugLog_Printf1( "Gui EventQ %d: WindowRefresh", lpData->mEventQueueHead );
			Gui_WindowRefresh( lpData, (sGuiWindow*)lpEvent->mpInfo );
			break;
		}

		lpData->mEventQueueHead++;
		if( lpData->mEventQueueHead >= dGUI_EVENTQ_LIMIT )
		{
			lpData->mEventQueueHead = 0;
		}
	}


	Gui_MouseUpdate( lpData );
	Gui_IkbdUpdate( &gGuiClass.mIKBD );

	switch( gGuiClass.mMode )
	{
	case	eGUI_MODE_TEXTEDIT:
		if( lpData->mpFocusControl )
		{
			lpData->mpFocusControl->mRedrawFlag = 2;
		}
		switch( GuiTextLine_Update( &gGuiClass.mTextLine, &gGuiClass.mIKBD ) )
		{
		case	eGUI_TEXTLINE_WRITEBACK:
			if( lpData->mpFocusControl )
			{
				lpData->mpFocusControl->mEvent.mEvent = eGUIEVENT_BUTTON_UPDATE;
				DebugLog_Printf2( "Gui textline WRITEBACK %lx %d", &lpData->mpFocusControl->mEvent, lpData->mpFocusControl->mEvent.mEvent );
				HashTree_VarWrite( lpData->mpFocusControl->mpEventVar, &lpData->mpFocusControl->mEvent );
			}
		case	eGUI_TEXTLINE_CANCEL:
			gGuiClass.mMode = eGUI_MODE_NORMAL;
			GuiTextLine_DeInit( &gGuiClass.mTextLine );
			if( lpData->mpFocusControl )
			{
				lpData->mpFocusControl->mFlags &= ~eGUIINFO_FLAG_SELECTED;
				lpData->mpFocusControl->mRedrawFlag = 2;
			}
			break;
		}
		break;

	case	eGUI_MODE_HOLDING:
		if( lpData->mpFocusControl )
		{
			if( lpData->mMouseCursor.mButtonLeft != eGUI_MOUSEBUTTON_HELD )
			{
				lpData->mpFocusControl->mFlags &= ~eGUIINFO_FLAG_SELECTED;
				lpData->mpFocusControl->mRedrawFlag = 2;
				lpData->mMode = eGUI_MODE_NORMAL;
			}
			else
			{
				GuiInfo_HeldUpdate( lpData->mpFocusControl, &gGuiClass );
			}
		}
		break;

	case	eGUI_MODE_NORMAL:
	default:
		lpControl = Gui_GetFocusControl( lpData );

		if( (lpData->mpFocusControl) && (lpControl!=lpData->mpFocusControl) )
		{
			GuiInfo_DeFocusUpdate( lpData->mpFocusControl, &gGuiClass );
		}

		lpData->mpFocusControl = lpControl;
		GuiInfo_FocusUpdate( lpData->mpFocusControl, &gGuiClass );

		GuiKeyActions_Update( &gGuiClass );
		break;
	}

	return( (U8)(!gGuiClass.mShutDownFlag) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_DataAdd( sGuiData * apHeader )
* ACTION   : Gui_DataAdd
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_DataAdd( sGuiData * apHeader )
{
	U16	i;
	sGuiRect	lRect;

	gGuiClass.mpData = apHeader;

	GuiData_EventsRegister( apHeader, gGuiClass.mpTree );

	for( i=0; i<apHeader->mButtonCount; i++ )
	{
		if( apHeader->mpButtons[ i ].mString.mVar.mpName )
		{
			apHeader->mpButtons[ i ].mString.mVar.mpVarClient = HashTree_VarClientRegister( gGuiClass.mpTree, apHeader->mpButtons[ i ].mString.mVar.mpName, Gui_OnStringVarWrite, 0, 0, (U32)&apHeader->mpButtons[ i ] );
		}
	}

	if( apHeader->mCursorCount )
	{
		gGuiClass.mMouseCursor.mpSprite = apHeader->mpCursors[ 0 ].mpAsset;
	}

	for( i=0; i<apHeader->mWindowCount; i++ )
	{
		lRect.mX = 0;
		lRect.mY = 0;
		lRect.mWidth = gGuiClass.mScreen.mWidth;
		lRect.mHeight = gGuiClass.mScreen.mHeight;
		GuiWindow_SetWorldPos( &apHeader->mpWindows[i], &lRect, 0 );
	}
	Gui_WindowOpen( &gGuiClass, &apHeader->mpWindows[ 0 ] );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_DataRemove( sGuiData * apHeader )
* ACTION   : Gui_DataRemove
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_DataRemove( sGuiData * apHeader )
{
	U16	i;

	for( i=0; i<apHeader->mButtonCount; i++ )
	{
		if( apHeader->mpButtons[ i ].mString.mVar.mpVarClient )
		{
			HashTree_VarClientUnRegister( gGuiClass.mpTree, apHeader->mpButtons[ i ].mString.mVar.mpVarClient );
		}
	}

	gGuiClass.mpData = 0;	
	GuiData_EventsUnRegister( apHeader, gGuiClass.mpTree );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiLock_IsLocked( sGuiLock * apLock );
* ACTION   : GuiLock_IsLocked
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8				GuiLock_IsLocked( sGuiLock * apLock )
{
	U8	lLockedFlag;
	S32	lCheckVal;
	S32	lLockVal;

	lLockedFlag = 0;

	if( apLock )
	{
		lCheckVal = 0;
		if( apLock->mpLockValue )
		{
			if( apLock->mpLockValue->mpVar )
			{
				HashTree_VarRead( apLock->mpLockValue->mpVar, &lCheckVal, sizeof( lCheckVal ) );
			}
		}

		if( apLock->mpLockVar )
		{
			if( apLock->mpLockVar->mpVar )
			{
				HashTree_VarRead( apLock->mpLockVar->mpVar, &lLockVal, sizeof( lLockVal ) );
				if( lLockVal <= lCheckVal )
				{
					lLockedFlag = 1;
				}
			}
		}
	}

	return( lLockedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiLock_IsVisible( sGuiLock * apLock );
* ACTION   : GuiLock_IsVisible
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8				GuiLock_IsVisible( sGuiLock * apLock )
{
	U8	lVisFlag;
	S32	lCheckVal;
	S32	lVisVal;

	lVisFlag = 1;

	if( apLock )
	{
		lCheckVal = 0;
		if( apLock->mpVisValue )
		{
			if( apLock->mpVisValue->mpVar )
			{
				HashTree_VarRead( apLock->mpVisValue->mpVar, &lCheckVal, sizeof( lCheckVal ) );
			}
		}

		if( apLock->mpVisVar )
		{
			if( apLock->mpVisVar->mpVar )
			{
				HashTree_VarRead( apLock->mpVisVar->mpVar, &lVisVal, sizeof( lVisVal ) );
				if( lVisVal <= lCheckVal )
				{
					lVisFlag = 0;
				}
			}
		}
	}

	return( lVisFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetpWindow( sGuiClass * apData )
* ACTION   : Gui_GetpWindow
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiWindow *	Gui_GetpWindow( sGuiClass * apData )
{
	sGuiWindow *	lpWindow;

	DebugLog_Printf2( "Gui_GetpWindow() %lx stackpos %d", apData, apData->mWindowStackPos );

	lpWindow = 0;
	
	if( apData->mWindowStackPos )
	{
		lpWindow = apData->mpWindows[ apData->mWindowStackPos - 1 ];
	}
	return( lpWindow );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_EventOpen( sGuiWindow * apWindow )
* ACTION   : GuiWindow_EventOpen
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_EventOpen( sGuiWindow * apWindow )
{
	U16				i;
	sGuiWindow *	lpWindow;

	if( apWindow )
	{
		DebugLog_Printf2( "Window %lx mEvent.mpInfo %lx", apWindow, apWindow->mInfo.mEvent.mpInfo );
		apWindow->mInfo.mEvent.mEvent = eGUIEVENT_WINDOW_OPEN;
		HashTree_VarWrite( apWindow->mInfo.mpEventVar, &apWindow->mInfo.mEvent );
		for( i=0; i<apWindow->mControlCount; i++ )
		{
			lpWindow = (sGuiWindow*)apWindow->mppControls[ i ];
			if( eGUI_TYPE_WINDOW == lpWindow->mInfo.mType )
			{
				GuiWindow_EventOpen( lpWindow );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_EventClose( sGuiWindow * apWindow )
* ACTION   : GuiWindow_EventClose
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_EventClose( sGuiWindow * apWindow )
{
	U16				i;
	sGuiWindow *	lpWindow;

	if( apWindow )
	{
		apWindow->mInfo.mEvent.mEvent = eGUIEVENT_WINDOW_CLOSE;
		HashTree_VarWrite( apWindow->mInfo.mpEventVar, &apWindow->mInfo.mEvent );
		for( i=0; i<apWindow->mControlCount; i++ )
		{
			lpWindow = (sGuiWindow*)apWindow->mppControls[ i ];
			if( eGUI_TYPE_WINDOW == lpWindow->mInfo.mType )
			{
				GuiWindow_EventOpen( lpWindow );
			}
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_EventRefresh( sGuiWindow * apWindow )
* ACTION   : GuiWindow_EventRefresh
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_EventRefresh( sGuiWindow * apWindow )
{
	GuiWindow_EventClose( apWindow );
	GuiWindow_EventOpen( apWindow );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_WindowOpen( sGuiClass * apData, sGuiWindow * apWindow )
* ACTION   : Gui_WindowOpen
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void			Gui_WindowOpen( sGuiClass * apData, sGuiWindow * apWindow )
{
	sGuiRect	lRect;
	U16			lIndex;
	U16			i;

	apData->mpFocusControl = 0;
	apData->mpFocusList = 0;
	apData->mpFocusSlider = 0;
	apData->mMode = eGUI_MODE_NORMAL;

	GuiWindow_DeBounceSprings( apWindow );
	GuiWindow_EventOpen( apWindow );

	lIndex = Gui_GetWindowIndex( apData, apWindow );
	if( lIndex < apData->mWindowStackPos )
	{
		for( i=(U16)(lIndex+1); i<apData->mWindowStackPos; i++ )
		{
			apData->mpWindows[ i-1 ] = apData->mpWindows[ i ];
		}
		apData->mpWindows[ apData->mWindowStackPos-1 ] = apWindow;
	}
	else if( (apData->mWindowStackPos + 1) < dGUI_WINDOWSTACK_LIMIT )
	{
		apData->mpWindows[ apData->mWindowStackPos ] = apWindow;
		apData->mWindowStackPos++;
	}
	apWindow->mInfo.mRedrawFlag = 2;

	lRect.mX = 0;
	lRect.mY = 0;
	lRect.mWidth = apData->mScreen.mWidth;
	lRect.mHeight = apData->mScreen.mHeight;
	GuiWindow_SetWorldPos( apWindow, &lRect, 0 );

	DebugLog_Printf2( "Gui_WindowOpen() %lx stackpos %d", apWindow, apData->mWindowStackPos );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_WindowClose( sGuiClass * apData,sGuiWindow * apWindow )
* ACTION   : Gui_WindowClose
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_WindowClose( sGuiClass * apData,sGuiWindow * apWindow )
{
	U16	lIndex;
	U16	i;

	DebugLog_Printf0( "GuiWindow_Close()" );

	apData->mpFocusControl = 0;
	apData->mpFocusList = 0;
	apData->mpFocusSlider = 0;
	apData->mMode = eGUI_MODE_NORMAL;

	GuiWindow_EventClose( apWindow );

	lIndex = Gui_GetWindowIndex( apData, apWindow );
	if( lIndex < apData->mWindowStackPos )
	{
		for( i=(U16)(lIndex+1); i<apData->mWindowStackPos; i++ )
		{
			apData->mpWindows[ i-1 ] = apData->mpWindows[ i ];
		}
		apData->mWindowStackPos--;
	}
	for( i=0; i<apData->mWindowStackPos; i++ )
	{
		apData->mpWindows[ i ]->mInfo.mRedrawFlag = 2;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_WindowRefresh( sGuiClass * apData,sGuiWindow * apWindow )
* ACTION   : Gui_WindowRefresh
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_WindowRefresh( sGuiClass * apData,sGuiWindow * apWindow )
{
	apData->mpFocusControl = 0;
	apData->mpFocusList = 0;
	apData->mpFocusSlider = 0;
	apData->mMode = eGUI_MODE_NORMAL;

	apWindow->mInfo.mEvent.mEvent = eGUIEVENT_WINDOW_REFRESH;
	HashTree_VarWrite( apWindow->mInfo.mpEventVar, &apWindow->mInfo.mEvent );
	Gui_WorldPosUpdate();	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetWindowIndex( sGuiClass * apData,sGuiWindow * apWindow )
* ACTION   : Gui_GetWindowIndex
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Gui_GetWindowIndex( sGuiClass * apData,sGuiWindow * apWindow )
{
	U16	i;

	i = 0;
	while( (i<apData->mWindowStackPos) && (apWindow != apData->mpWindows[ i ]) )
	{
		i++;
	}

	return( i );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiButton_SetWorldPos( sGuiButton * apButton,const sGuiRect * apParentRect, const sGuiPos * apScroll )
* ACTION   : GuiButton_SetWorldPos
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiButton_SetWorldPos( sGuiButton * apButton,const sGuiRect * apParentRect, const sGuiRect * apScroll )
{
	sGuiRect	lRect;
	
	if( apButton )
	{
		lRect     = *apParentRect;
		lRect.mX += 2;
		lRect.mY += 2;
		lRect.mWidth -= 4;
		lRect.mHeight -= 4;
/*		GuiRect_BuildScrolled( apParentRect, &apButton->mInfo.mRectPair.mLocal, &apButton->mInfo.mRectPair.mWorld, apScroll );*/
		GuiRect_BuildScrolled( &lRect, &apButton->mInfo.mRectPair.mLocal, &apButton->mInfo.mRectPair.mWorld, apScroll );
		GuiRect_BuildScrolled( &apButton->mInfo.mRectPair.mWorld, &apButton->mSprite.mRectPair.mLocal, &apButton->mSprite.mRectPair.mWorld, apScroll );
		GuiRect_BuildScrolled( &apButton->mInfo.mRectPair.mWorld, &apButton->mString.mRects.mLocal,    &apButton->mString.mRects.mWorld, apScroll );
		DebugLog_Printf2( "GuiButton_SetWorldPos() %d %d", apButton->mInfo.mRectPair.mWorld.mX, apButton->mInfo.mRectPair.mWorld.mY );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiList_SetWorldPos( sGuiList * apSlider,const sGuiRect * apParentRect,const sGuiRect * apScroll )
* ACTION   : GuiList_SetWorldPos
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiList_SetWorldPos( sGuiList * apList,const sGuiRect * apParentRect,const sGuiRect * apScroll )
{
	sGuiInfo *	lpInfo;
	S16			lY,lH,lW;
	U16			i;

	if( apList )
	{
		GuiRect_BuildScrolled( apParentRect, &apList->mInfo.mRectPair.mLocal, &apList->mInfo.mRectPair.mWorld, apScroll );

		GuiButton_SetWorldPos( apList->mpButton, &apList->mInfo.mRectPair.mWorld, 0 );

		if( apList->mpWindow )
		{
			lY = 0;
			lW = apList->mInfo.mRectPair.mLocal.mWidth;
			lH = apList->mInfo.mRectPair.mLocal.mHeight;
			for( i=0; i<apList->mpWindow->mControlCount; i++ )
			{
				lpInfo = apList->mpWindow->mppControls[ i ];
				lpInfo->mRectPair.mLocal.mX = 0;
				lpInfo->mRectPair.mLocal.mY = lY;
				lpInfo->mRectPair.mLocal.mWidth = lW;
				lpInfo->mRectPair.mLocal.mHeight = lH;
				lY = (S16)(lY + lH);
			}
		}

		GuiWindow_SetWorldPos( apList->mpWindow, &apList->mInfo.mRectPair.mWorld, 0 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_SetWorldPos( sGuiSlider * apSlider,const sGuiRect * apParentRect, const sGuiPos * apScroll )
* ACTION   : GuiSlider_SetWorldPos
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_SetWorldPos( sGuiSlider * apSlider,const sGuiRect * apParentRect, const sGuiRect * apScroll )
{
	U16	i;
	sGuiRect *	lpRect;

	if( apSlider )
	{
		GuiRect_BuildScrolled( apParentRect, &apSlider->mInfo.mRectPair.mLocal, &apSlider->mInfo.mRectPair.mWorld, apScroll );
		
		switch( apSlider->mSliderType )
		{
		case	eGUI_SLIDER_HORIZONTAL:
			DebugLog_Printf0( "GuiSlider_SetWorldPos() horizontal" );
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = (S16)-apSlider->mButtonSize;
				lpRect->mY      = 0;
				lpRect->mWidth  = apSlider->mButtonSize;
				lpRect->mHeight = 0;
				apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mButtonType = eGUI_BUTTON_SLIDERLINEADD;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;;
				lpRect->mY      = 0;
				lpRect->mWidth  = apSlider->mButtonSize;
				lpRect->mHeight = 0;
				apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mButtonType = eGUI_BUTTON_SLIDERLINESUB;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = (S16)(-apSlider->mButtonSize*2);
				lpRect->mY      = 0;
				lpRect->mWidth  = (S16)(-apSlider->mButtonSize);
				lpRect->mHeight = 0;
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mButtonType = eGUI_BUTTON_SLIDERPAGEADD;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = apSlider->mButtonSize;
				lpRect->mY      = 0;
				lpRect->mWidth  = apSlider->mButtonSize;
				lpRect->mHeight = 0;
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mButtonType = eGUI_BUTTON_SLIDERPAGESUB;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = (S16)(apSlider->mButtonSize*2);
				lpRect->mY      = 0;
				lpRect->mWidth  = apSlider->mButtonSize;
				lpRect->mHeight = 0;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mButtonType = eGUI_BUTTON_SLIDERX;
			}
			break;

		case	eGUI_SLIDER_VERTICAL:
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;
				lpRect->mY      = 0;
				lpRect->mWidth  = 0;
				lpRect->mHeight = apSlider->mButtonSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mButtonType = eGUI_BUTTON_SLIDERLINEADD;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;
				lpRect->mY      = (S16)(-apSlider->mButtonSize);
				lpRect->mWidth  = 0;
				lpRect->mHeight = apSlider->mButtonSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mButtonType = eGUI_BUTTON_SLIDERLINESUB;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;
				lpRect->mY      = apSlider->mButtonSize;
				lpRect->mWidth  = 0;
				lpRect->mHeight = apSlider->mButtonSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mButtonType = eGUI_BUTTON_SLIDERPAGEADD;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;
				lpRect->mY      = (S16)(-(apSlider->mButtonSize*2));
				lpRect->mWidth  = 0;
				lpRect->mHeight = (S16)(-apSlider->mButtonSize);
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mButtonType = eGUI_BUTTON_SLIDERPAGESUB;
			}
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ] )
			{
				lpRect = &apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mLocal;
				lpRect->mX      = 0;
				lpRect->mY      = (S16)(apSlider->mButtonSize*2);
				lpRect->mWidth  = 0;
				lpRect->mHeight = apSlider->mButtonSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mButtonType = eGUI_BUTTON_SLIDERY;
			}
			break;
		}

		for( i=0; i<eGUI_SLIDERBUT_LIMIT; i++ )
		{
			if( apSlider->mpButtons[ i ] )
			{
				apSlider->mpButtons[ i ]->mpSlider = apSlider;
				GuiButton_SetWorldPos( apSlider->mpButtons[ i ], &apSlider->mInfo.mRectPair.mWorld, 0 );
			}
		}

		GuiSlider_Update( apSlider, 0 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_SetWorldPos( sGuiWindow * apWindow,const sGuiRect * apParentRect )
* ACTION   : GuiWindow_SetWorldPos
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_SetWorldPos( sGuiWindow * apWindow,const sGuiRect * apParentRect, const sGuiRect * apScroll )
{
	DebugLog_Printf1( "GuiWindow_SetWorldPos() %lx", apWindow );

	if( apWindow )
	{
		GuiRect_BuildScrolled( apParentRect, &apWindow->mInfo.mRectPair.mLocal, &apWindow->mInfo.mRectPair.mWorld, apScroll );
		GuiWindowControls_SetWorldPos( apWindow );
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindowControls_SetWorldPos( sGuiWindow * apWindow )
* ACTION   : GuiWindowControls_SetWorldPos
* CREATION : 23.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindowControls_SetWorldPos( sGuiWindow * apWindow )
{
	U16	i;

	if( apWindow )
	{
		for( i=0; i<apWindow->mControlCount; i++ )
		{
			switch( apWindow->mppControls[ i ]->mType )
			{
			case	eGUI_TYPE_BUTTON:
				GuiButton_SetWorldPos( (sGuiButton*)apWindow->mppControls[ i ], &apWindow->mInfo.mRectPair.mWorld, &apWindow->mCanvas );
				break;
			case	eGUI_TYPE_LIST:
				GuiList_SetWorldPos( (sGuiList*)apWindow->mppControls[ i ], &apWindow->mInfo.mRectPair.mWorld, &apWindow->mCanvas );
				break;
			case	eGUI_TYPE_SLIDER:
				GuiSlider_SetWorldPos( (sGuiSlider*)apWindow->mppControls[ i ], &apWindow->mInfo.mRectPair.mWorld, &apWindow->mCanvas );
				break;
			case	eGUI_TYPE_WINDOW:
				GuiWindow_SetWorldPos( (sGuiWindow*)apWindow->mppControls[ i ], &apWindow->mInfo.mRectPair.mWorld, &apWindow->mCanvas );
				break;
			}
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_WorldPosUpdate( void )
* ACTION   : Gui_WorldPosUpdate
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_WorldPosUpdate( void )
{
	DebugLog_Printf0( "Gui_WorldPosUpdate()" );
	Gui_SetWorldPos( &gGuiClass );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_SetWorldPos( sGuiClass * apData )
* ACTION   : Gui_SetWorldPos
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_SetWorldPos( sGuiClass * apData )
{
	sGuiRect	lRect;

	DebugLog_Printf1( "Gui_SetWorldPos() %lx", apData );
	if( apData )
	{
		lRect.mX = 0;
		lRect.mY = 0;
		lRect.mWidth = apData->mScreen.mWidth;
		lRect.mHeight = apData->mScreen.mHeight;

		GuiWindow_SetWorldPos( Gui_GetpWindow(apData), &lRect, 0 );
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetFocusControl( sGuiClass * apData )
* ACTION   : Gui_GetFocusControl
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiInfo *	Gui_GetFocusControl( sGuiClass * apData )
{
	sGuiInfo *	lpControl;

	lpControl = 0;
	if( apData )
	{
		lpControl = GuiWindow_GetFocusControl( Gui_GetpWindow(apData), apData->mMouseCursor.mX, apData->mMouseCursor.mY );
	}
	return( lpControl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_GetFocusControl( sGuiSlider * apSlider,S16 aX,S16 aY )
* ACTION   : GuiSlider_GetFocusControl
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiInfo *	GuiSlider_GetFocusControl( sGuiSlider * apSlider,S16 aX,S16 aY )
{
	sGuiInfo *	lpInfo;
	U16			i;
	
	lpInfo = 0;
	if( apSlider )
	{
		for( i=0; i<eGUI_SLIDERBUT_LIMIT; i++ )
		{
			lpInfo = (sGuiInfo*)apSlider->mpButtons[ i ];
			if( GuiRect_ContainsPoint( &lpInfo->mRectPair.mWorld, aX, aY ) )
			{
				return( lpInfo );
			}
		}
	}
	return( lpInfo );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_GetFocusControl( sGuiWindow * apWindow,const sGuiRect * apRect,S16 aX,S16 aY )
* ACTION   : GuiWindow_GetFocusControl
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiInfo *	GuiWindow_GetFocusControl( sGuiWindow * apWindow, S16 aX,S16 aY )
{
	sGuiInfo *	lpControl;
	U16			i;

	lpControl = 0;

	if( apWindow )
	{
		if( GuiRect_ContainsPoint( &apWindow->mInfo.mRectPair.mWorld, aX, aY ) )
		{
			lpControl = &apWindow->mInfo;
			for( i=0; i<apWindow->mControlCount; i++ )
			{
				if( GuiRect_ContainsPoint( &apWindow->mppControls[ i ]->mRectPair.mWorld, aX, aY ) )
				{
					switch( apWindow->mppControls[ i ]->mType )
					{
					case	eGUI_TYPE_BUTTON:
						lpControl = (sGuiInfo*)apWindow->mppControls[ i ];
						break;
					case	eGUI_TYPE_LIST:
						gGuiClass.mpFocusList = (sGuiList*)(apWindow->mppControls[ i ]);
						if( ((sGuiList*)(apWindow->mppControls[ i ]))->mOpenFlag )
						{
							lpControl = GuiWindow_GetFocusControl( ((sGuiList*)(apWindow->mppControls[ i ]))->mpWindow, aX, aY );
						}
						else
						{
							lpControl = (sGuiInfo*)apWindow->mppControls[ i ];
						}
						break;

					case	eGUI_TYPE_SLIDER:
						gGuiClass.mpFocusSlider = (sGuiSlider*)apWindow->mppControls[ i ];
						lpControl = GuiSlider_GetFocusControl( (sGuiSlider*)apWindow->mppControls[ i ], aX, aY );
						break;
					case	eGUI_TYPE_WINDOW:
						lpControl = GuiWindow_GetFocusControl( (sGuiWindow*)apWindow->mppControls[ i ], aX, aY );
						break;
					}
					return( lpControl );
				}
			}
		}
	}

	return( lpControl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiRect_Build( const sGuiRect * apParent,const sGuiRect * apChild,sGuiRect * apNew )
* ACTION   : GuiRect_Build
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiRect_Build( const sGuiRect * apParent,const sGuiRect * apChild,sGuiRect * apNew )
{
	if( (apParent) && (apChild) && (apNew) )
	{
		apNew->mX = (S16)(apParent->mX + apChild->mX);
		if( apChild->mX < 0 )
		{
			apNew->mX = (S16)(apNew->mX + apParent->mWidth + 1);
		}

		apNew->mY = (S16)(apParent->mY + apChild->mY);
		if( apChild->mY < 0 )
		{
			apNew->mY = (S16)(apNew->mY +apParent->mHeight + 1);
		}

		if( apChild->mWidth > 0 )
		{
			apNew->mWidth = apChild->mWidth;
		}
		else
		{
			apNew->mWidth = apParent->mWidth;
			apNew->mWidth = (S16)(apNew->mWidth - (apNew->mX - apParent->mX));
			apNew->mWidth =(S16)(apNew->mWidth + apChild->mWidth);
		}

		if( apChild->mHeight > 0 )
		{
			apNew->mHeight = apChild->mHeight;
		}
		else
		{
			apNew->mHeight = apParent->mHeight;
			apNew->mHeight = (S16)(apNew->mHeight -(apNew->mY - apParent->mY));
			apNew->mHeight = (S16)(apNew->mHeight + apChild->mHeight);
		}

		if( apNew->mWidth < 0 )
		{
			apNew->mWidth = 0;
		}
		if( apNew->mHeight < 0 )
		{
			apNew->mHeight = 0;
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiRect_BuildScrolled( const sGuiRect * apParent,const sGuiRect * apChild,sGuiRect * apNew,const sGuiPos * apScroll )
* ACTION   : GuiRect_BuildScrolled
* CREATION : 23.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiRect_BuildScrolled( const sGuiRect * apParent,const sGuiRect * apChild,sGuiRect * apNew,const sGuiRect * apScroll )
{
	if( (apParent) && (apChild) && (apNew) )
	{
		apNew->mX = (S16)(apParent->mX + apChild->mX);
		if( apChild->mX < 0 )
		{
			apNew->mX = (S16)(apNew->mX + apParent->mWidth +  1);
		}
		if( (apChild->mX > 0) && (apScroll) )
		{
			apNew->mX = (S16)(apNew->mX - apScroll->mX);
		}

		apNew->mY = (S16)(apParent->mY + apChild->mY);
		if( apChild->mY < 0 )
		{
			apNew->mY = (S16)(apNew->mY + apParent->mHeight +  1);
		}
		if( (apChild->mY > 0) && (apScroll) )
		{
			apNew->mY = (S16)(apNew->mY - apScroll->mY);
		}

		if( apChild->mWidth > 0 )
		{
			apNew->mWidth = apChild->mWidth;
		}
		else
		{
			apNew->mWidth = apParent->mWidth;
			apNew->mWidth =(S16)(apNew->mWidth - (apNew->mX - apParent->mX));
			apNew->mWidth =(S16)(apNew->mWidth + apChild->mWidth);
		}

		if( apChild->mHeight > 0 )
		{
			apNew->mHeight = apChild->mHeight;
		}
		else
		{
			apNew->mHeight = apParent->mHeight;
			apNew->mHeight =(S16)(apNew->mHeight - (apNew->mY - apParent->mY));
			apNew->mHeight = (S16)(apNew->mHeight + apChild->mHeight);
		}

		if( apNew->mWidth < 0 )
		{
			apNew->mWidth = 0;
		}
		if( apNew->mHeight < 0 )
		{
			apNew->mHeight = 0;
		}

	}
	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiRect_ContainsPoint( const sGuiRect * apRect,const S16 aX,const S16 aY )
* ACTION   : GuiRect_ContainsPoint
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	GuiRect_ContainsPoint( const sGuiRect * apRect,const S16 aX,const S16 aY )
{
	U8	lRes;
	S16	lX,lY;
	
	lRes = 0;

	if( apRect )
	{
		lX = (S16)(aX - apRect->mX);
		lY = (S16)(aY - apRect->mY);
		if( (lX >= 0) && (lX < apRect->mWidth) &&
			(lY >= 0) && (lY < apRect->mHeight) )
		{
			lRes = 1;
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_MouseUpdate( sGuiClass * apData )
* ACTION   : Gui_MouseUpdate
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_MouseUpdate( sGuiClass * apData )
{
	S16			lX,lY;
	U8			lLeft,lRight;
	
	lX     = IKBD_GetMouseX();
	lY     = IKBD_GetMouseY();
	lLeft  = IKBD_GetMouseButtonLeft();
	lRight = IKBD_GetMouseButtonRight();

	apData->mMouseCursorDelta.mX = (S16)(lX - apData->mMouseCursor.mX);
	apData->mMouseCursorDelta.mY = (S16)(lY - apData->mMouseCursor.mY);

	apData->mMouseCursor.mButtonLeft  = GuiMouse_GetButton( apData->mMouseCursor.mButtonLeft,  lLeft  );
	apData->mMouseCursor.mButtonRight = GuiMouse_GetButton( apData->mMouseCursor.mButtonRight, lRight );

	if( lX<0)
	{
		lX = 0;
		IKBD_SetMouseX( lX );
	}
	if( lX >= apData->mScreen.mWidth )
	{
		lX = (S16)(apData->mScreen.mWidth - 1);
		IKBD_SetMouseX( lX );
	}

	if( lY<0)
	{
		lY = 0;
		IKBD_SetMouseY( lY );
	}
	if( lY >= apData->mScreen.mHeight )
	{
		lY = (S16)(apData->mScreen.mHeight - 1);
		IKBD_SetMouseY( lY );
	}
	apData->mMouseCursor.mX = lX;
	apData->mMouseCursor.mY = lY;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiMouse_GetButton( const U8 aOldState,const U8 aNewState )
* ACTION   : GuiMouse_GetButton
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	GuiMouse_GetButton( const U8 aOldState,const U8 aNewState )
{
	U8	lState;

	switch( (U16)aOldState )
	{
	case	eGUI_MOUSEBUTTON_CLICKED:
	case	eGUI_MOUSEBUTTON_HELD:
		if( aNewState )
		{
			lState = eGUI_MOUSEBUTTON_HELD;
		}
		else
		{
			lState = eGUI_MOUSEBUTTON_RELEASED;
		}
		break;

	case	eGUI_MOUSEBUTTON_RELEASED:
	case	eGUI_MOUSEBUTTON_NONE:
	default:
		if( aNewState )
		{
			lState = eGUI_MOUSEBUTTON_CLICKED;
		}
		else
		{
			lState = eGUI_MOUSEBUTTON_NONE;
		}
		break;
	}

	return( lState );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_IkbdUpdate( sGuiIKBD * apIKBD )
* ACTION   : Gui_IkbdUpdate
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_IkbdUpdate( sGuiIKBD * apIKBD )
{
	apIKBD->mCount = 0;
	
	while( (IKBD_GetKbdBytesWaiting()) && (apIKBD->mCount < dGUI_IKBD_LIMIT) )
	{
		apIKBD->mData[ apIKBD->mCount ] = IKBD_PopKbdByte();
		apIKBD->mCount++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiInfo_FocusUpdate( sGuiInfo * apInfo,sGuiClass * apData )
* ACTION   : GuiInfo_FocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiInfo_FocusUpdate( sGuiInfo * apInfo,sGuiClass * apData )
{
	DebugLog_Printf1( "GuiInfo_FocusUpdate() %lx", apInfo );
	if( (apInfo) && (apData) )
	{
		switch( apInfo->mType )
		{
		case	eGUI_TYPE_BUTTON:
			GuiButton_FocusUpdate( (sGuiButton*)apInfo, apData );
			break;
		case	eGUI_TYPE_LIST:
			GuiList_FocusUpdate( (sGuiList*)apInfo, apData );
			break;
		case	eGUI_TYPE_WINDOW:
			GuiWindow_FocusUpdate( (sGuiWindow*)apInfo, apData );
			break;

		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiInfo_DeFocusUpdate( sGuiInfo * apInfo,sGuiClass * apData )
* ACTION   : GuiInfo_DeFocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiInfo_DeFocusUpdate( sGuiInfo * apInfo,sGuiClass * apData )
{
	DebugLog_Printf1( "GuiInfo_DeFocusUpdate() %lx", apInfo );
	if( (apInfo) && (apData) )
	{
		switch( apInfo->mType )
		{
		case	eGUI_TYPE_BUTTON:
			GuiButton_DeFocusUpdate( (sGuiButton*)apInfo, apData );
			break;
		case	eGUI_TYPE_WINDOW:
			GuiWindow_DeFocusUpdate( (sGuiWindow*)apInfo, apData );
			break;

		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiInfo_HeldUpdate( sGuiInfo * apInfo,sGuiClass * apData )
* ACTION   : GuiInfo_HeldUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiInfo_HeldUpdate( sGuiInfo * apInfo,sGuiClass * apData )
{
	DebugLog_Printf0( "GuiInfo_HeldUpdate()" );
	if( (apInfo) && (apData) )
	{
		apInfo->mEvent.mEvent = eGUIEVENT_BUTTON_LEFTHELD;
		HashTree_VarWrite( apInfo->mpEventVar, &apInfo->mEvent );

		switch( apInfo->mType )
		{
		case	eGUI_TYPE_BUTTON:
			GuiButton_HeldUpdate( (sGuiButton*)apInfo, apData );
			break;
		case	eGUI_TYPE_SLIDER:
			GuiSlider_HeldUpdate( (sGuiSlider*)apInfo, apData );
			break;
		}
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_FocusUpdate( sGuiSlider * apButton,sGuiClass * apData )
* ACTION   : GuiSlider_FocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_FocusUpdate( sGuiSlider * apSlider,sGuiClass * apData )
{
	(void)apSlider;
	(void)apData;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_FocusUpdate( sGuiWindow * apButton,sGuiClass * apData )
* ACTION   : GuiWindow_FocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_FocusUpdate( sGuiWindow * apWindow,sGuiClass * apData )
{
	DebugLog_Printf1( "GuiWindow_FocusUpdate() %lx", apWindow );
	(void)apWindow;
	(void)apData;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiWindow_DeFocusUpdate( sGuiWindow * apButton,sGuiClass * apData )
* ACTION   : GuiWindow_DeFocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiWindow_DeFocusUpdate( sGuiWindow * apWindow,sGuiClass * apData )
{
	DebugLog_Printf1( "GuiWindow_DeFocusUpdate() %lx", apWindow );
	(void)apWindow;
	(void)apData;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_DeFocusUpdate( sGuiSlider * apButton,sGuiClass * apData )
* ACTION   : GuiSlider_DeFocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_DeFocusUpdate( sGuiSlider * apSlider,sGuiClass * apData )
{
	(void)apSlider;
	(void)apData;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_HeldUpdate( sGuiSlider * apButton,sGuiClass * apData )
* ACTION   : GuiSlider_HeldUpdate
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_HeldUpdate( sGuiSlider * apSlider,sGuiClass * apData )
{
	U8	lHflag;
	S16	lPos;
	S16	lMinPos,lMaxPos;

	DebugLog_Printf0( "GuiSlider_HeldUpdate()" );
	if( apSlider && apData )
	{
		if( apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mButtonType == eGUI_BUTTON_SLIDERX )
		{
			lHflag = 1;
		}
		else
		{
			lHflag = 0;
		}

		DebugLog_Printf1(" GuiSlider_HeldUpdate() hflag:%d", lHflag );

		if( lHflag )
		{
			lMinPos = apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mX;
			lMinPos = (S16)(lMinPos + apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mWidth);
			lMaxPos = apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mX;

			lPos = apData->mSliderHeldPos.mX;
			lPos =(S16)( lPos + (apData->mMouseCursor.mX - apData->mHeldPos.mX));
			if( lPos < lMinPos )
			{
				lPos = lMinPos;
			}
			if( lPos > lMaxPos )
			{
				lPos = lMaxPos;
			}

			apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mWorld.mX = lPos;
		}
		else
		{
			lMinPos = apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mY;
			lMinPos = (S16)(lMinPos + apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mHeight);
			lMaxPos = apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mY;

			lPos = apData->mSliderHeldPos.mY;
			lPos =(S16)(lPos + (apData->mMouseCursor.mY - apData->mHeldPos.mY));

			if( lPos < lMinPos )
			{
				lPos = lMinPos;
			}
			if( lPos > lMaxPos )
			{
				lPos = lMaxPos;
			}

			apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mWorld.mY = lPos;
		}
		DebugLog_Printf3("lMinPos %d  lMaxPos %d  lPos %d", lMinPos, lMaxPos, lPos );

		GuiSlider_Update( apSlider, 1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_Update( sGuiSlider * apButton,const U8 aUpdatFromPosFlag )
* ACTION   : GuiSlider_Update
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_Update( sGuiSlider * apSlider,const U8 aUpdateFromPosFlag )
{
	S32	lPageSize;
	S32	lValue = 0;
	S32	lValueMin;
	S32	lValueMax;
	S32	lBackSize;
	S32	lPos;
	U8	lHflag;

	DebugLog_Printf1( "GuiSlider_Update() %lx", apSlider );

	if( apSlider )
	{
		if( apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ] )
		{
			if( apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mButtonType == eGUI_BUTTON_SLIDERX )
			{
				lHflag = 1;
			}
			else
			{
				lHflag = 0;
			}

			if( !aUpdateFromPosFlag )
			{
				if( apSlider->mpWindow )
				{
					if( lHflag )
					{
						lValue = apSlider->mpWindow->mCanvas.mX;
					}
					else
					{
						lValue = apSlider->mpWindow->mCanvas.mY;
					}
				}
				else if( apSlider->mInfo.mpValue->mpVar )
				{
					lValue = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpVar );
				}
				else
				{
					lValue = 0;
				}
			}

			if( lHflag )
			{
				DebugLog_Printf3( "sliderw :%d, addw %d subw %d", apSlider->mInfo.mRectPair.mWorld.mWidth, apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mWidth, apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mWidth );
				lBackSize  = apSlider->mInfo.mRectPair.mWorld.mWidth;
				lBackSize -= apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mWidth;
				lBackSize -= apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mWidth;
			}
			else
			{
				lBackSize  = apSlider->mInfo.mRectPair.mWorld.mHeight;
				lBackSize -= apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mHeight;
				lBackSize -= apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mHeight;
				DebugLog_Printf4( "sliderh :%d, addh %d subh %d backsize %ld", apSlider->mInfo.mRectPair.mWorld.mHeight, apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mHeight, apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mHeight, lBackSize );
			}

			if( apSlider->mpWindow )
			{
				lValueMin = 0;
				if( lHflag )
				{
					lValueMax  = apSlider->mpWindow->mCanvas.mWidth;
					lValueMax -= apSlider->mpWindow->mInfo.mRectPair.mWorld.mWidth;

					if( apSlider->mpWindow->mInfo.mRectPair.mLocal.mWidth )
					{
						lPageSize  = apSlider->mpWindow->mInfo.mRectPair.mLocal.mWidth;
						lPageSize *= lBackSize;
						lPageSize /= apSlider->mpWindow->mCanvas.mWidth;
					}
					else
					{
						lPageSize = 1;
					}
				}
				else
				{
					DebugLog_Printf3( "silder ch:%d wh:%d lh:%d", apSlider->mpWindow->mCanvas.mHeight, apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight, apSlider->mpWindow->mInfo.mRectPair.mLocal.mHeight );
					lValueMax  = apSlider->mpWindow->mCanvas.mHeight;
					lValueMax -= apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight;
					if( apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight )
					{
						lPageSize = apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight;
						lPageSize *= lBackSize;
						lPageSize /= apSlider->mpWindow->mCanvas.mHeight;
					}
					else
					{
						lPageSize = 1;
					}
				}
			}
			else
			{
				if( apSlider->mInfo.mpValue->mpValueMin )
				{
					lValueMin = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpValueMin );
				}
				else
				{
					lValueMin = 0;
				}

				if( apSlider->mInfo.mpValue->mpValueMax )
				{
					lValueMax = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpValueMax );
				}
				else
				{
					lValueMax = 0;
				}

				if( apSlider->mpPageSize )
				{
					lPageSize  = GuiVar_ReadS32( apSlider->mpPageSize );
					lPageSize *= lBackSize;
					lPageSize /=(lValueMax-lValueMin);
				}
				else
				{
					lPageSize = apSlider->mSizeMin;
				}
			}
			

			lBackSize -= lPageSize;

			DebugLog_Printf3( "lValue %ld  lValueMin %ld  lValueMax %ld", lValue, lValueMin, lValueMax );
			DebugLog_Printf2( "lPageSize %ld  lBackSize %ld ", lPageSize, lBackSize );

			if( aUpdateFromPosFlag )
			{
				if( lHflag )
				{
					lPos  = apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mWorld.mX;
					lPos -= apSlider->mpButtons[ eGUI_SLIDERBUT_SUB  ]->mInfo.mRectPair.mWorld.mX;
					lPos -= apSlider->mpButtons[ eGUI_SLIDERBUT_SUB  ]->mInfo.mRectPair.mWorld.mWidth;


				}
				else
				{
					lPos = apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN ]->mInfo.mRectPair.mWorld.mY;
					lPos -= apSlider->mpButtons[ eGUI_SLIDERBUT_ADD  ]->mInfo.mRectPair.mWorld.mY;
					lPos -= apSlider->mpButtons[ eGUI_SLIDERBUT_ADD  ]->mInfo.mRectPair.mWorld.mHeight;
/*					lPos = (lBackSize-lPos);*/
				}
				if( lBackSize )
				{
					lValue  = lPos * (lValueMax-lValueMin);
					lValue /= lBackSize;
				}
				else
				{
					lValue = 0;
				}

				DebugLog_Printf2( "lPos %ld  lValue %ld ", lPos, lValue );
			}

			if( lValue < lValueMin )
			{
				lValue = lValueMin;
			}
			if( lValue > lValueMax )
			{
				lValue = lValueMax;
			}

			if( lValue )
			{
				lPos  = lBackSize * lValue;
				lPos /= (lValueMax-lValueMin);
			}
			else
			{
				lPos = 0;
			}
			DebugLog_Printf2( "lPos %ld  lValue %ld ", lPos, lValue );


			if( lHflag )
			{
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mWidth = (S16)lPos;
				lPos += apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mX;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mX     = (S16)lPos;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mWidth = (S16)lPageSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mX     = (S16)(lPos + lPageSize);
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mWidth = (S16)(apSlider->mpButtons[ eGUI_SLIDERBUT_ADD ]->mInfo.mRectPair.mWorld.mX - apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mX);
			}
			else
			{
/*				lPos = (lBackSize-lPos);*/
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mHeight = (S16)lPos;
				lPos += apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mY;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mY      = (S16)lPos;
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mHeight = (S16)lPageSize;
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mY      = (S16)(lPos + lPageSize);
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mHeight = (S16)(apSlider->mpButtons[ eGUI_SLIDERBUT_SUB ]->mInfo.mRectPair.mWorld.mY - apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mY);
			}
			DebugLog_Printf5( "bah:%d my:%d mh:%d bsy:%d bsh:%d",
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKADD ]->mInfo.mRectPair.mWorld.mHeight,
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mY,
				apSlider->mpButtons[ eGUI_SLIDERBUT_MAIN    ]->mInfo.mRectPair.mWorld.mHeight,
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mY,
				apSlider->mpButtons[ eGUI_SLIDERBUT_BACKSUB ]->mInfo.mRectPair.mWorld.mHeight );
			apSlider->mInfo.mRedrawFlag = 2;

			if( aUpdateFromPosFlag )
			{
				if( apSlider->mpWindow )
				{
					if( lHflag )
					{
						apSlider->mpWindow->mCanvas.mX = (S16)lValue;
					}
					else
					{
						apSlider->mpWindow->mCanvas.mY = (S16)lValue;
					}
				}
				else
				{
					GuiVar_WriteS32( apSlider->mInfo.mpValue->mpVar, lValue );
				}
			}

			apSlider->mInfo.mRedrawFlag = 2;

			if( apSlider->mpWindow )
			{
				GuiWindowControls_SetWorldPos( apSlider->mpWindow );
				apSlider->mpWindow->mInfo.mRedrawFlag = 2;
			}
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_LineAdd( sGuiSlider * apSlider )
* ACTION   : GuiSlider_LineAdd
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_LineAdd( sGuiSlider * apSlider )
{
	S32	lAdd;

	if( apSlider )
	{
		if( apSlider->mpWindow )
		{
			switch( apSlider->mSliderType )
			{
			case	eGUI_SLIDER_VERTICAL:
				if( apSlider->mpWindow->mCanvas.mY  )
				{
					apSlider->mpWindow->mCanvas.mY--;
				}
				break;
			case	eGUI_SLIDER_HORIZONTAL:
				if( apSlider->mpWindow->mCanvas.mX > (apSlider->mpWindow->mCanvas.mWidth-apSlider->mInfo.mRectPair.mWorld.mWidth) )
				{
					apSlider->mpWindow->mCanvas.mX++;
				}
				break;
			}
			GuiSlider_Update( apSlider, 0 );
		}
		else if( apSlider->mInfo.mpValue )
		{

			if( apSlider->mpLineSize )
			{
				lAdd = GuiVar_ReadS32( apSlider->mpLineSize );
			}
			else
			{
				lAdd = 1;
			}
			GuiSlider_ValueAdd( apSlider, lAdd );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_LineSub( sGuiSlider * apSlider )
* ACTION   : GuiSlider_LineSub
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_LineSub( sGuiSlider * apSlider )
{
	S32	lAdd;

	if( apSlider )
	{
		if( apSlider->mpWindow )
		{
			switch( apSlider->mSliderType )
			{
			case	eGUI_SLIDER_VERTICAL:
				if( apSlider->mpWindow->mCanvas.mY < (apSlider->mpWindow->mCanvas.mHeight-apSlider->mInfo.mRectPair.mWorld.mHeight) )
				{
					apSlider->mpWindow->mCanvas.mY++;
				}
				break;
			case	eGUI_SLIDER_HORIZONTAL:
				if( apSlider->mpWindow->mCanvas.mX )
				{
					apSlider->mpWindow->mCanvas.mX--;
				}
				break;
			}
			GuiSlider_Update( apSlider, 0 );
		}
		else if( apSlider->mInfo.mpValue )
		{
			if( apSlider->mpLineSize )
			{
				lAdd = GuiVar_ReadS32( apSlider->mpLineSize );
			}
			else
			{
				lAdd = 1;
			}
			GuiSlider_ValueAdd( apSlider, -lAdd );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_PageAdd( sGuiSlider * apSlider )
* ACTION   : GuiSlider_PageAdd
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_PageAdd( sGuiSlider * apSlider )
{
	S32	lAdd;
	S16	lMax;

	if( apSlider )
	{
		if( apSlider->mpWindow )
		{
			switch( apSlider->mSliderType )
			{
			case	eGUI_SLIDER_VERTICAL:
				apSlider->mpWindow->mCanvas.mY = (S16)( apSlider->mpWindow->mCanvas.mY - apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight);
				if( apSlider->mpWindow->mCanvas.mY < 0 )
				{
					apSlider->mpWindow->mCanvas.mY = 0;
				}
				break;
			case	eGUI_SLIDER_HORIZONTAL:
				apSlider->mpWindow->mCanvas.mX = (S16)(apSlider->mpWindow->mCanvas.mX + apSlider->mpWindow->mInfo.mRectPair.mWorld.mWidth);
				lMax = (S16)(apSlider->mpWindow->mCanvas.mWidth - apSlider->mpWindow->mInfo.mRectPair.mWorld.mWidth);
				if( apSlider->mpWindow->mCanvas.mX > lMax )
				{
					apSlider->mpWindow->mCanvas.mX = lMax;
				}
				break;
			}
			GuiSlider_Update( apSlider, 0 );
		}
		else if( apSlider->mInfo.mpValue )
		{

			if( apSlider->mpPageSize )
			{
				lAdd = GuiVar_ReadS32( apSlider->mpPageSize );
			}
			else
			{
				lAdd = 2;
			}
			GuiSlider_ValueAdd( apSlider, lAdd );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_PageSub( sGuiSlider * apSlider )
* ACTION   : GuiSlider_PageSub
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_PageSub( sGuiSlider * apSlider )
{
	S32	lAdd;
	S16	lMax;

	if( apSlider )
	{
		if( apSlider->mpWindow )
		{
			switch( apSlider->mSliderType )
			{
			case	eGUI_SLIDER_VERTICAL:
				apSlider->mpWindow->mCanvas.mY = (S16)(apSlider->mpWindow->mCanvas.mY + apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight);
				lMax = (S16)(apSlider->mpWindow->mCanvas.mHeight - apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight);
				if( apSlider->mpWindow->mCanvas.mY > lMax )
				{
					apSlider->mpWindow->mCanvas.mY = lMax;
				}
				break;
			case	eGUI_SLIDER_HORIZONTAL:
				apSlider->mpWindow->mCanvas.mX = (S16)(apSlider->mpWindow->mCanvas.mX - apSlider->mpWindow->mInfo.mRectPair.mWorld.mWidth);
				if( apSlider->mpWindow->mCanvas.mX < 0 )
				{
					apSlider->mpWindow->mCanvas.mX = 0;
				}
				break;
			}
			GuiSlider_Update( apSlider, 0 );
		}
		else if( apSlider->mInfo.mpValue )
		{
			if( apSlider->mpPageSize )
			{
				lAdd = GuiVar_ReadS32( apSlider->mpPageSize );
			}
			else
			{
				lAdd = 2;
			}
			GuiSlider_ValueAdd( apSlider, -lAdd );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiSlider_ValueAdd( sGuiSlider * apSlider,const S32 aAdd )
* ACTION   : GuiSlider_ValueAdd
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiSlider_ValueAdd( sGuiSlider * apSlider,const S32 aAdd )
{
	S32	lValue;
	S32	lValueMax;
	S32	lValueMin;

	if( apSlider )
	{
		if( apSlider->mInfo.mpValue )
		{
			lValue    = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpVar );
			lValueMax = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpValueMax );
			lValueMin = GuiVar_ReadS32( apSlider->mInfo.mpValue->mpValueMin );

			lValue += aAdd;

			if( lValue < lValueMin )
			{
				lValue = lValueMin;
			}
			if( lValue > lValueMax )
			{
				lValue = lValueMax;
			}
			GuiVar_WriteS32( apSlider->mInfo.mpValue->mpVar, lValue );
			GuiSlider_Update( apSlider, 0 );
		}
		else if( apSlider->mpWindow )
		{
			switch( apSlider->mSliderType )
			{
			case	eGUI_SLIDER_VERTICAL:
				lValue     = apSlider->mpWindow->mCanvas.mY;
				lValueMin  = 0;
				lValueMax  = apSlider->mpWindow->mCanvas.mHeight;
				lValueMax -= apSlider->mpWindow->mInfo.mRectPair.mWorld.mHeight;

				if( lValue > lValueMax )
				{
					lValue = lValueMax;
				}
				if( lValue < 0 )
				{
					lValue = 0;
				}

				apSlider->mpWindow->mCanvas.mY = (S16)lValue;
				break;

			case	eGUI_SLIDER_HORIZONTAL:
				lValue     = apSlider->mpWindow->mCanvas.mX;
				lValueMin  = 0;
				lValueMax  = apSlider->mpWindow->mCanvas.mWidth;
				lValueMax -= apSlider->mpWindow->mInfo.mRectPair.mWorld.mWidth;

				if( lValue > lValueMax )
				{
					lValue = lValueMax;
				}
				if( lValue < 0 )
				{
					lValue = 0;
				}

				apSlider->mpWindow->mCanvas.mX = (S16)lValue;
				break;
			}
			GuiSlider_Update( apSlider, 0 );
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiButton_FocusUpdate( sGuiButton * apButton,sGuiClass * apData )
* ACTION   : GuiButton_FocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiButton_FocusUpdate( sGuiButton * apButton,sGuiClass * apData )
{
	sGuiMouse	lMouse;
	U16			i;

	DebugLog_Printf1( "GuiButton_FocusUpdate() %lx", apButton );

	lMouse.mButtonLeft  = apData->mMouseCursor.mButtonLeft;
	lMouse.mButtonRight = apData->mMouseCursor.mButtonRight;
	lMouse.mX           = (S16)(apData->mMouseCursor.mX - apButton->mInfo.mRectPair.mWorld.mX);
	lMouse.mY           = (S16)(apData->mMouseCursor.mY - apButton->mInfo.mRectPair.mWorld.mY);

	if( apButton->mButtonType == eGUI_BUTTON_HOVER )
	{
		if( !(apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED) )
		{
			apButton->mInfo.mFlags |= eGUIINFO_FLAG_SELECTED;
			apButton->mInfo.mRedrawFlag = 2;
		}
	}

	if( apButton->mButtonType == eGUI_BUTTON_CUSTOM )
	{
		if( apData->mpCustomFocus )
		{
			apData->mpCustomFocus( apButton, &lMouse );
		}
	}

	switch( (U16)apData->mMouseCursor.mButtonLeft )
	{
	case	eGUI_MOUSEBUTTON_CLICKED:

		if( apButton->mButtonType == eGUI_BUTTON_TEXTLINE )
		{
			if( GuiTextLine_Init( &apData->mTextLine, apButton->mString.mVar.mpVar ) )
			{ 
				apButton->mInfo.mFlags |= eGUIINFO_FLAG_SELECTED;
				apButton->mInfo.mRedrawFlag = 2;
				apData->mMode = eGUI_MODE_TEXTEDIT;
			}
		}

		if( apButton->mButtonType == eGUI_BUTTON_SWITCH )
		{
			apButton->mInfo.mFlags ^= eGUIINFO_FLAG_SELECTED;
			apButton->mInfo.mRedrawFlag = 2;
		}
		else if( apButton->mButtonType == eGUI_BUTTON_SPRING )
		{
			if( !apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED )
			{
				apButton->mInfo.mFlags |= eGUIINFO_FLAG_SELECTED;
				apButton->mInfo.mRedrawFlag = 2;
			}
		}

		if( apButton->mInfo.mpEventVar )
		{
			apButton->mInfo.mEvent.mEvent = eGUIEVENT_BUTTON_LEFTCLICK;
			HashTree_VarWrite( apButton->mInfo.mpEventVar, &apButton->mInfo.mEvent );
		}

		if( apButton->mpOnLeftClick )
		{
			GuiAction_Update( apButton->mpOnLeftClick, &lMouse, apData );
		}
		break;

	case	eGUI_MOUSEBUTTON_HELD:

		if( apButton->mInfo.mpEventVar )
		{
			apButton->mInfo.mEvent.mEvent = eGUIEVENT_BUTTON_LEFTHELD;
			HashTree_VarWrite( apButton->mInfo.mpEventVar, &apButton->mInfo.mEvent );
		}

		if( (apButton->mButtonType == eGUI_BUTTON_SPRING) || 
			(apButton->mButtonType == eGUI_BUTTON_SLIDERX) || 
			(apButton->mButtonType == eGUI_BUTTON_SLIDERY) ||
			(apButton->mButtonType == eGUI_BUTTON_SLIDERLINEADD) ||
			(apButton->mButtonType == eGUI_BUTTON_SLIDERLINESUB) ||
			(apButton->mButtonType == eGUI_BUTTON_SLIDERPAGEADD) ||
			(apButton->mButtonType == eGUI_BUTTON_SLIDERPAGESUB) 
			)
		{
			if( !apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED )
			{
				apButton->mInfo.mFlags |= eGUIINFO_FLAG_SELECTED;
				apButton->mInfo.mRedrawFlag = 2;
			}
			apData->mHeldPos.mX = apData->mMouseCursor.mX;
			apData->mHeldPos.mY = apData->mMouseCursor.mY;
			apData->mSliderHeldPos.mX = apButton->mInfo.mRectPair.mWorld.mX;
			apData->mSliderHeldPos.mY = apButton->mInfo.mRectPair.mWorld.mY;
			apData->mMode       = eGUI_MODE_HOLDING;
		}

		if( apButton->mpOnLeftHeld )
		{
			GuiAction_Update( apButton->mpOnLeftHeld, &lMouse, apData );
		}
		break;

	case	eGUI_MOUSEBUTTON_RELEASED:

		if( apButton->mInfo.mpEventVar )
		{
			apButton->mInfo.mEvent.mEvent = eGUIEVENT_BUTTON_LEFTRELEASE;
			HashTree_VarWrite( apButton->mInfo.mpEventVar, &apButton->mInfo.mEvent );
		}

		if( apButton->mButtonType == eGUI_BUTTON_SPRING )
		{
			if( apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED )
			{
				apButton->mInfo.mFlags &= ~eGUIINFO_FLAG_SELECTED;
				apButton->mInfo.mRedrawFlag = 2;
			}
		}

		if( apButton->mpOnLeftRelease )
		{
			GuiAction_Update( apButton->mpOnLeftRelease, &lMouse, apData );
		}
		break;
	}

	switch( (U16)apData->mMouseCursor.mButtonRight )
	{
	case	eGUI_MOUSEBUTTON_CLICKED:
		if( apButton->mpOnRightClick )
		{
			GuiAction_Update( apButton->mpOnRightClick, &lMouse, apData );
		}
		break;

	case	eGUI_MOUSEBUTTON_HELD:
		if( apButton->mpOnRightHeld )
		{
			GuiAction_Update( apButton->mpOnRightHeld, &lMouse, apData );
		}
		break;

	case	eGUI_MOUSEBUTTON_RELEASED:
		if( apButton->mpOnRightRelease )
		{
			GuiAction_Update( apButton->mpOnRightRelease, &lMouse, apData );
		}
		break;
	}

	if( apData->mIKBD.mCount && apButton->mpOnIKBD )
	{
		if( apButton->mpOnIKBD->mpVar )
		{
			for( i=0; i<apData->mIKBD.mCount; i++ )
			{
				HashTree_VarWrite( apButton->mpOnIKBD->mpVar, &apData->mIKBD.mData[ i ] );
			}
			apData->mIKBD.mCount = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiButton_DeFocusUpdate( sGuiButton * apButton,sGuiClass * apData )
* ACTION   : GuiButton_DeFocusUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiButton_DeFocusUpdate( sGuiButton * apButton,sGuiClass * apData )
{
	DebugLog_Printf1( "GuiButton_DeFocusUpdate() %lx", apButton );
	(void)apData;

	if( apButton->mButtonType == eGUI_BUTTON_CUSTOM )
	{
		if( apData->mpCustomDeFocus )
		{
			apData->mpCustomDeFocus( apButton );
		}
	}

	if( apButton->mButtonType == eGUI_BUTTON_HOVER )
	{
		if( apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED )
		{
			apButton->mInfo.mFlags &= ~eGUIINFO_FLAG_SELECTED;
			apButton->mInfo.mRedrawFlag = 2;
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiButton_HeldUpdate( sGuiButton * apButton,sGuiClass * apData )
* ACTION   : GuiButton_HeldUpdate
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiButton_HeldUpdate( sGuiButton * apButton,sGuiClass * apData )
{
	sGuiMouse	lMouse;

	DebugLog_Printf1( "GuiButton_HeldUpdate() %lx", apButton );

	if( apButton && apData )
	{
		lMouse.mButtonLeft  = apData->mMouseCursor.mButtonLeft;
		lMouse.mButtonRight = apData->mMouseCursor.mButtonRight;
		lMouse.mX           = (S16)(apData->mMouseCursor.mX - apButton->mInfo.mRectPair.mWorld.mX);
		lMouse.mY           = (S16)(apData->mMouseCursor.mY - apButton->mInfo.mRectPair.mWorld.mY);

		DebugLog_Printf3( "button type %d w:%d h:%d", apButton->mButtonType, apButton->mInfo.mRectPair.mWorld.mWidth, apButton->mInfo.mRectPair.mWorld.mHeight );

		switch( apButton->mButtonType )
		{
		case	eGUI_BUTTON_SLIDERLINEADD:
			GuiSlider_LineAdd( gGuiClass.mpFocusSlider );
			break;
		case	eGUI_BUTTON_SLIDERLINESUB:
			GuiSlider_LineSub( gGuiClass.mpFocusSlider );
			break;
		case	eGUI_BUTTON_SLIDERPAGEADD:
			GuiSlider_PageAdd( gGuiClass.mpFocusSlider );
			break;
		case	eGUI_BUTTON_SLIDERPAGESUB:
			GuiSlider_PageSub( gGuiClass.mpFocusSlider );
			break;
		case	eGUI_BUTTON_SLIDERX:
		case	eGUI_BUTTON_SLIDERY:
			GuiSlider_HeldUpdate( gGuiClass.mpFocusSlider, apData );
			break;
		}
		if( apButton->mpOnLeftHeld )
		{
			GuiAction_Update( apButton->mpOnLeftHeld, &lMouse, apData );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiList_FocusUpdate( sGuiList * apList,sGuiClass * apData )
* ACTION   : GuiList_FocusUpdate
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiList_FocusUpdate( sGuiList * apList,sGuiClass * apData )
{
	switch( (U16)apData->mMouseCursor.mButtonLeft )
	{
	case	eGUI_MOUSEBUTTON_CLICKED:
		apList->mOpenFlag         = 1;
		apList->mInfo.mRedrawFlag = 2;
		break;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiAction_Update( sGuiAction * apAction,sGuiMouse * apMouse,sGuiClass * apData )
* ACTION   : GuiAction_Update
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiAction_Update( sGuiAction * apAction,sGuiMouse * apMouse,sGuiClass * apData )
{
	S32	lMin,lMax,lVal;
	sGuiEvent	lEvent;


	(void)apData;
	if( apAction )
	{
		DebugLog_Printf1( "GuiAction_Update() %d", apAction->mAction );
		switch( apAction->mAction )
		{
		case	eGUI_ACTION_APP_EXIT:
			gGuiClass.mShutDownFlag = 1;
			break;

		case	eGUI_ACTION_ADD:
			DebugLog_Printf0( "ACTION: ADD" );
			if( apAction->mpValue )
			{
				if( apAction->mpValue->mpValueMin )
				{
					HashTree_VarRead( apAction->mpValue->mpValueMin->mpVar, &lMin, sizeof(lMin) );
				}
				else
				{
					lMin = apAction->mpValue->mMin;
				}

				if( apAction->mpValue->mpValueMax )
				{
					HashTree_VarRead( apAction->mpValue->mpValueMax->mpVar, &lMax, sizeof(lMax) );
				}
				else
				{
					lMax = apAction->mpValue->mMax;
				}

				DebugLog_Printf3( "MIN: %ld MAX: %ld var %ld", lMin, lMax, apAction->mpValue->mpVar );
				if( apAction->mpValue->mpVar )
				{
					lVal = GuiVar_ReadS32( apAction->mpValue->mpVar );

					DebugLog_Printf1( "lVal: %ld", lVal );
					lVal += apAction->mConstant;
					DebugLog_Printf1( "lVal modified: %ld", lVal );
					if( lVal < lMin )
					{
						lVal = lMin;
					}
					if( lVal > lMax )
					{
						lVal = lMax;
					}
					GuiVar_WriteS32( apAction->mpValue->mpVar, (S32)lVal );
				}

			}
			break;

		case	eGUI_ACTION_CALL:
		case	eGUI_ACTION_SET_CONSTANT:
			if( apAction->mpValue )
			{
				if( apAction->mpValue->mpVar )
				{
					GuiVar_WriteS32( apAction->mpValue->mpVar, (S32)apAction->mConstant );
				}
			}
			break;

		case	eGUI_ACTION_SET_MOUSE:
			if( apMouse )
			{
				if( apAction->mpValue )
				{
					if( apAction->mpValue->mpVar )
					{
						HashTree_VarWrite( apAction->mpValue->mpVar->mpVar, apMouse );
					}
				}
			}
			break;

		}
		if( apAction->mpWindowClose )
		{
			DebugLog_Printf0( "GuiAction_Update() windowclose" );
			lEvent.mEvent = eGUIEVENT_WINDOW_CLOSE;
			lEvent.mpInfo = (sGuiInfo*)apAction->mpWindowClose;
			GuiEventQueue_EventAdd( &lEvent );
		}
		if( apAction->mpWindowOpen )
		{
			DebugLog_Printf0( "GuiAction_Update() windowclose" );
			lEvent.mEvent = eGUIEVENT_WINDOW_OPEN;
			lEvent.mpInfo = (sGuiInfo*)apAction->mpWindowOpen;
			GuiEventQueue_EventAdd( &lEvent );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetWindowStackCount( void )
* ACTION   : Gui_GetWindowStackCount
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Gui_GetWindowStackCount( void )
{
	return( gGuiClass.mWindowStackPos );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetpWindowStack( void )
* ACTION   : Gui_GetpWindowStack
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiWindow **	Gui_GetpWindowStack( void )
{
	return( &gGuiClass.mpWindows[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetpMouse( void )
* ACTION   : Gui_GetpMouse
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiMouse *	Gui_GetpMouse( void )
{
	return( &gGuiClass.mMouseCursor );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetpHashTree( void )
* ACTION   : Gui_GetpHashTree
* CREATION : 17.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTree *	Gui_GetpHashTree( void )
{
	return( gGuiClass.mpTree );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_GetpTextLine( void )
* ACTION   : Gui_GetpTextLine
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

struct sGuiTextLine *	Gui_GetpTextLine( void )
{
	return( &gGuiClass.mTextLine );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiVar_ReadS32( const sGuiVar * apVar )
* ACTION   : GuiVar_ReadS32
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

S32	GuiVar_ReadS32( const sGuiVar * apVar )
{
	S8	lS8;
	U8	lU8;
	S16	lS16;
	U16	lU16;
	S32	lRes;

	lRes = 0;
	if( apVar )
	{
		if( apVar->mpVar )
		{
			switch( apVar->mType )
			{
			case	eGUI_VAR_S8:
				HashTree_VarRead( apVar->mpVar, &lS8, sizeof(lS8) );
				lRes = (S32)lS8;
				break;
			case	eGUI_VAR_S16:
				HashTree_VarRead( apVar->mpVar, &lS16, sizeof(lS16) );
				lRes = (S32)lS16;
				break;
			case	eGUI_VAR_U8:
				HashTree_VarRead( apVar->mpVar, &lU8, sizeof(lU8) );
				lRes = (S32)lU8;
				break;
			case	eGUI_VAR_U16:
				HashTree_VarRead( apVar->mpVar, &lU16, sizeof(lU16) );
				lRes = (S32)lU16;
				break;

			case	eGUI_VAR_S32:
			case	eGUI_VAR_U32:
			case	eGUI_VAR_FP32:
			case	eGUI_VAR_STRING:
				HashTree_VarRead( apVar->mpVar, &lRes, sizeof(lRes) );
				break;
			default:
				lRes = 0;
				break;
			}
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiVar_WriteS32( sGuiVar * apVar,const S32 aValue )
* ACTION   : GuiVar_WriteS32
* CREATION : 22.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiVar_WriteS32( sGuiVar * apVar,const S32 aValue )
{
	S8		lS8;
	U8		lU8;
	S16		lS16;
	U16		lU16;
	S32		lS32;

	if( apVar )
	{
		if( apVar->mpVar )
		{
			switch( apVar->mType )
			{
			case	eGUI_VAR_S8:
				lS8 = (S8)aValue;
				HashTree_VarWrite( apVar->mpVar, &lS8 );
				break;
			case	eGUI_VAR_S16:
				lS16 = (S16)aValue;
				HashTree_VarWrite( apVar->mpVar, &lS16 );
				break;
			case	eGUI_VAR_U8:
				lU8 = (U8)aValue;
				HashTree_VarWrite( apVar->mpVar, &lU8 );
				break;
			case	eGUI_VAR_U16:
				lU16 = (U16)aValue;
				HashTree_VarWrite( apVar->mpVar, &lU16 );
				break;

			case	eGUI_VAR_S32:
			case	eGUI_VAR_U32:
			case	eGUI_VAR_FP32:
			case	eGUI_VAR_STRING:
				lS32 = aValue;
				HashTree_VarWrite( apVar->mpVar, &lS32 );
				break;
			}
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_RequestWindowRefresh( sGuiWindow * apWindow )
* ACTION   : Gui_RequestWindowRefresh
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_RequestWindowRefresh( sGuiWindow * apWindow )
{
	sGuiEvent	lEvent;
	lEvent.mEvent = eGUIEVENT_WINDOW_REFRESH;
	lEvent.mpInfo = (sGuiInfo*)apWindow;
	GuiEventQueue_EventAdd( &lEvent );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiEventQueue_EventAdd( sGuiEvent * apEvent )
* ACTION   : GuiEventQueue_EventAdd
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiEventQueue_EventAdd( sGuiEvent * apEvent )
{
	gGuiClass.mEventQueue[ gGuiClass.mEventQueueTail ] = *apEvent;
	gGuiClass.mEventQueueTail++;
	if( gGuiClass.mEventQueueTail >= dGUI_EVENTQ_LIMIT )
	{
		gGuiClass.mEventQueueTail = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sGuiWindow * Gui_GetpWindow(const char * apName)
* ACTION   : Gui_GetpWindow
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiWindow * Gui_GetpWindowFromName(const char * apName)
{
	sGuiWindow *	lpWindow;
	sGuiData *		lpData;
	U32				lHash;
	U16				i;
	
	lpWindow = 0;
	if( apName )
	{
		lpData   = gGuiClass.mpData;
		lHash    = HashTree_BuildHash( apName );
		i        = 0;
		while( (i<lpData->mWindowCount) && (!lpWindow) )
		{
			if( lpData->mpWindows[ i ].mInfo.mHash == lHash ) 
			{
				lpWindow = &lpData->mpWindows[ i ];
			}
			i++;
		}
	}

	return( lpWindow );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiButton_Select(sGuiButton * apButton)
* ACTION   : GuiButton_Select
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiButton_Select(sGuiButton * apButton)
{
	if( apButton );
	{
		apButton->mInfo.mFlags     |= eGUIINFO_FLAG_SELECTED;	
		apButton->mInfo.mRedrawFlag = 2;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiButton_DeSelect(sGuiButton * apButton)
* ACTION   : GuiButton_DeSelect
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiButton_DeSelect(sGuiButton * apButton)
{
	if( apButton )
	{
		apButton->mInfo.mFlags     &= ~eGUIINFO_FLAG_SELECTED;	
		apButton->mInfo.mRedrawFlag = 2;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiWindow_DeBounceSprings(sGuiWindow * apWindow)
* ACTION   : GuiWindow_DeBounceSprings
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiWindow_DeBounceSprings(sGuiWindow * apWindow)
{
	sGuiButton *	lpButton;
	sGuiSlider *	lpSlider;
	U16				i,j;
	
	if( apWindow )
	{
		for( i=0; i<apWindow->mControlCount; i++ )
		{
			switch( apWindow->mppControls[ i ]->mType )
			{
			case	eGUI_TYPE_BUTTON:
				lpButton = (sGuiButton*)apWindow->mppControls[ i ];
				if( lpButton->mButtonType != eGUI_BUTTON_SWITCH )
				{
					GuiButton_DeSelect( lpButton );
				}
				break;
			case	eGUI_TYPE_LIST:
				break;
			case	eGUI_TYPE_SLIDER:
				lpSlider = (sGuiSlider*)apWindow->mppControls[ i ];
				for( j=0; j<eGUI_SLIDERBUT_LIMIT; j++ )
				{
					GuiButton_DeSelect( lpSlider->mpButtons[ j ] );
				}
				break;
			case	eGUI_TYPE_WINDOW:
				GuiWindow_DeBounceSprings( (sGuiWindow*)apWindow->mppControls[ i ] );
				break;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_OnStringVarWrite( sHashTreeVarClient * apClient )
* ACTION   : Gui_OnStringVarWrite
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_OnStringVarWrite( sHashTreeVarClient * apClient )
{
	sGuiButton *	lpButton;
	
	lpButton = (sGuiButton*)apClient->mUserData;
	if( lpButton )
	{
		if( lpButton->mString.mVar.mpVarClient == apClient )
		{
			lpButton->mInfo.mRedrawFlag = 2;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_SetCustomFocus( fGuiFocus apFocus )
* ACTION   : Gui_SetCustomFocus
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_SetCustomFocus( fGuiFocus apFocus )
{
	gGuiClass.mpCustomFocus = apFocus;		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gui_SetCustomDeFocus( fGuiDeFocus apDeFocus )
* ACTION   : Gui_SetCustomDeFocus
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Gui_SetCustomDeFocus( fGuiDeFocus apDeFocus )
{
	gGuiClass.mpCustomDeFocus = apDeFocus;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiKeyActions_Update(sGuiClass * apClass)
* ACTION   : GuiKeyActions_Update
* CREATION : 05.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiKeyActions_Update(sGuiClass * apClass)
{
	sGuiWindow *	lpWindow;
	U16				i;
	
	lpWindow = Gui_GetpWindow( apClass );
	if( lpWindow )
	{
		for( i=0; i<lpWindow->mKeyActionCount; i++ )
		{
			GuiKeyAction_Update( lpWindow->mppKeyActions[ i ], &apClass->mIKBD );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiKeyAction_Update(const sGuiKeyAction * apKey,const sGuiIKBD * apIKBD)
* ACTION   : GuiKeyAction_Update
* CREATION : 05.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiKeyAction_Update(const sGuiKeyAction * apKey,const sGuiIKBD * apIKBD)
{
	U16	i;
	U8	lSpecial;

	DebugLog_Printf0( "GuiKeyAction_Update" );

	if( apKey )
	{
		lSpecial = 0;
		if( IKBD_GetKeyStatus(eIKBDSCAN_ALTERNATE) )
		{
			lSpecial |= eGUIKEYSPECIAL_ALT;
		}
		if( IKBD_GetKeyStatus(eIKBDSCAN_CONTROL) )
		{
			lSpecial |= eGUIKEYSPECIAL_CTRL;
		}
		if( IKBD_GetKeyStatus(eIKBDSCAN_LEFTSHIFT) || IKBD_GetKeyStatus(eIKBDSCAN_RIGHTSHIFT) )
		{
			lSpecial |= eGUIKEYSPECIAL_SHIFT;
		}

		switch( apKey->mKeyCondition )
		{
		case	eGUIKEYSTATUS_HELD:
			if( IKBD_GetKeyStatus( apKey->mScanCode ) && (lSpecial==apKey->mSpecialStatus) )
			{
				GuiAction_Update( apKey->mpAction, 0, &gGuiClass );
			}
			break;
		case	eGUIKEYSTATUS_PRESSED:
			for( i=0; i<apIKBD->mCount; i++ )
			{
				if( (apIKBD->mData[ i ] == apKey->mScanCode) && (lSpecial==apKey->mSpecialStatus) )
				{
					GuiAction_Update( apKey->mpAction, 0, &gGuiClass );
				}
			}
			break;
		case	eGUIKEYSTATUS_RELEASED:
			for( i=0; i<apIKBD->mCount; i++ )
			{
				if( (apIKBD->mData[ i ] == (0x80+apKey->mScanCode)) && (lSpecial==apKey->mSpecialStatus) )
				{
					GuiAction_Update( apKey->mpAction, 0, &gGuiClass );
				}
			}
			break;
		default:
			break;
		}
	}
}


/* ################################################################################ */
