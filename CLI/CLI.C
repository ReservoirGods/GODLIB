

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CLI.H"

#ifdef	dCLI

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/FONT8X8/FONT8X8.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/VIDEO/VIDEO.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dCLI_CMD_LIMIT			128
#define	dCLI_STRING_LIMIT		40
#define	dCLI_HISTORY_LIMIT		128
#define	dCLI_DISPLAYLINES_LIMIT	25
#define	dCLI_LINES_LIMIT		128


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	char *			mpCommandString;
	fCliCommandFunc	mfCommand;
	char *			mpHelpTxt;
} sCliCommand;


typedef	struct sCli
{
	S16			mTabIndex;
	S16			mTabbingFlag;
	U16			mCursorX;
	S16			mCurrentLineIndex;
	S16			mHistoryLineIndex;
	S16			mDisplayLineIndex;
	U16			mDisplayLoopedFlag;
	U16			mHistoryLoopedFlag;
	S16			mCommandCount;
	U16			mInitialisedFlag;
	U16 *		mpCliSavedScreen;
	U16			mSavedPal[ 16 ];
	U16			mPal[ 16 ];
	sCliCommand	mCommand[ dCLI_CMD_LIMIT ];
	char		mDisplayLine[ dCLI_LINES_LIMIT ][ dCLI_STRING_LIMIT ];
	char		mHistoryLine[ dCLI_HISTORY_LIMIT ][ dCLI_STRING_LIMIT ];
	char		mCurrentLine[ dCLI_STRING_LIMIT ];
	char		mTabStub[ dCLI_STRING_LIMIT ];
} sCli;


/* ###################################################################################
#  DATA
################################################################################### */

sCli	gCli;
char	gCliString[ 1024 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Cli_Main( void );
sCliCommand *	Cli_GetpCommand( char * apCommandString );

void			Cli_StringClear( char * apString );
void			Cli_StringCopy( char * apDst, const char * apSrc );
U32				Cli_StringLength( const char * apString );
U8				Cli_StringsAreEqual( const char * apStr0, const char * apStr1 );
void			Cli_InitCurrentLine( void);
void			Cli_DrawCli( void );
void			Cli_DrawHistory( void );
void			Cli_DrawCurrentLine( void );
void			Cli_ProcessCurrentLine( void );
void			Cli_DeleteChar( void );
void			Cli_BackSpaceChar( void );
void			Cli_TabExpand( void );
void			Cli_GetPrevLineHistory( void );
void			Cli_GetNextLineHistory( void );
void			Cli_GetPrevCharHistory( void );
void			Cli_GetNextCharHistory( void );
void			Cli_InsertChar( U8 aChar );
void			Cli_ScrollUp( void );
void			Cli_ScrollDown( void );

void			Cli_ClearScreen( U16 * apScreen );
void			Cli_CopyScreen(  U16 * apSrc, U16 * apDst );
void			Cli_DrawBox( U16 * apScreen, U16 aX, U16 aY, U16 aWidth, U16 aHeight );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_Init( void )
* ACTION   : called at start of app
* CREATION : 26.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_Init( void )
{
	U16	i;
	U16	lWhite;


	lWhite = 0xFFF;
	for( i=0; i<16; i++ )
	{
		if( i )
		{
			Endian_WriteBigU16( &gCli.mPal[ i ], lWhite );
		}
		else
		{
			gCli.mPal[ i ] = 0;
		}
	}

	for( i=0; i<dCLI_CMD_LIMIT; i++ )
	{
		gCli.mCommand[ i ].mpCommandString = 0;
		gCli.mCommand[ i ].mfCommand       = 0;
		gCli.mCommand[ i ].mpHelpTxt       = 0;
	}

	for( i=0; i<dCLI_HISTORY_LIMIT; i++ )
	{
		Cli_StringClear( &gCli.mHistoryLine[ i ][ 0 ] );
	}

	for( i=0; i<dCLI_DISPLAYLINES_LIMIT; i++ )
	{
		Cli_StringClear( &gCli.mDisplayLine[ i ][ 0 ] );
	}

	Cli_StringClear( &gCli.mCurrentLine[ 0 ] );
	Cli_StringClear( &gCli.mTabStub[ 0 ] );

	gCli.mCursorX           = 0;
	gCli.mCurrentLineIndex  = 0;
	gCli.mDisplayLineIndex  = 0;
	gCli.mDisplayLoopedFlag = 0;
	gCli.mHistoryLineIndex  = 0;
	gCli.mHistoryLoopedFlag = 0;
	gCli.mCommandCount      = 0;
	gCli.mTabIndex          = 0;
	gCli.mTabbingFlag       = 0;

	gCli.mpCliSavedScreen = mMEMALLOC( 32000L );
	gCli.mInitialisedFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_AppDeInit( void )
* ACTION   : called at end of app
* CREATION : 26.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_DeInit( void )
{
	U16	i;

	for( i=0; i<dCLI_CMD_LIMIT; i++ )
	{
		gCli.mCommand[ i ].mpCommandString = 0;
		gCli.mCommand[ i ].mfCommand       = 0;
		gCli.mCommand[ i ].mpHelpTxt       = 0;
	}

	mMEMFREE( gCli.mpCliSavedScreen );
	gCli.mpCliSavedScreen = 0;
	gCli.mInitialisedFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_CmdInit( char * apCommandString, fCliCommandFunc afCmd, char * apHelpTxt )
* ACTION   : registers a command
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_CmdInit( char * apCommandString, fCliCommandFunc afCmd )
{
	U16	i;

	Cli_CmdDeInit( apCommandString );

	for( i=0; i<dCLI_CMD_LIMIT; i++ )
	{
		if( !gCli.mCommand[ i ].mpCommandString )
		{
			gCli.mCommand[ i ].mpCommandString = apCommandString;
			gCli.mCommand[ i ].mfCommand       = afCmd;
			gCli.mCommand[ i ].mpHelpTxt       = "HELP";
			gCli.mCommandCount++;
			return;
		}
	}

	/*	no free slots for new command */
	Assert( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetpCommand( char * apCommandString )
* ACTION   : finds sCliCommand structure for apCommandString
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

sCliCommand *	Cli_GetpCommand( char * apCommandString )
{
	U16	i;

	if( apCommandString )
	{
		for( i=0; i<dCLI_CMD_LIMIT; i++ )
		{
			if( gCli.mCommand[ i ].mpCommandString )
			{
/*				if( Cli_StringsAreEqual( apCommandString, gCli.mCommand[ i ].mpCommandString ) )*/
				if( !String_StrCmpi( apCommandString, gCli.mCommand[ i ].mpCommandString ) )
				{
					return( &gCli.mCommand[ i ] );
				}
			}
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_CmdDeInit( char * apCommandString )
* ACTION   : removes command from CLI
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_CmdDeInit( char * apCommandString )
{
	sCliCommand *	apCmd;

	apCmd = Cli_GetpCommand( apCommandString );
	if( apCmd )
	{
		apCmd->mpCommandString = 0;
		apCmd->mfCommand       = 0;
		apCmd->mpHelpTxt       = 0;
		gCli.mCommandCount--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_FileProcess( char * apFileName )
* ACTION   : Cli_FileProcess
* CREATION : 28.12.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Cli_FileProcess( char * apFileName )
{
	char *	lpFileText;
	U32		lFileSize;
	U32		lOffset;
	U8		lDataFlag;

	if( File_Exists(apFileName) )
	{
		lFileSize  = File_GetSize( apFileName );
		lpFileText = File_Load( apFileName );

		lDataFlag = 0;
		lOffset   = 0;
		Cli_InitCurrentLine();

		while( lOffset < lFileSize )
		{
			switch( lpFileText[ lOffset ] )
			{
			case	10:
			case	13:
				if( lDataFlag )
				{
					Cli_ProcessCurrentLine();
					lDataFlag = 0;
				}
				break;
			default:
				Cli_InsertChar( lpFileText[ lOffset ] );
				lDataFlag = 1;
				break;
			}
			lOffset++;
		}

		File_UnLoad( lpFileText );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_Update( void )
* ACTION   : called every game frame
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Cli_Update( void )
{
	if( eIKBDSCAN_ESC == IKBD_GetLastKeyPress() )
	{
		IKBD_ClearLastKeyPress();
		Cli_Main();
		IKBD_ClearLastKeyPress();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_Main( void )
* ACTION   : cli processing loop
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Cli_Main()
{
	U8				lExitFlag;
	U8				lChar;
	sVideoConfig	lVideoConfig;

	if( gCli.mInitialisedFlag )
	{
		Video_GetConfig( &lVideoConfig );
		Cli_CopyScreen(  Video_GetpPhysic(), gCli.mpCliSavedScreen );
		Cli_ClearScreen( Video_GetpPhysic() );
		Video_GetPalST( &gCli.mSavedPal[ 0 ] );

		Video_SetResolution( 320, 200, eVIDEO_MODE_4PLANE, 320 );
		Video_SetPalST( &gCli.mPal[ 0 ] );

		lExitFlag = 0;

		Cli_ClearScreen( Video_GetpPhysic() );
		Cli_InitCurrentLine();
		Cli_DrawCli();

		while( IKBD_GetKeyStatus( eIKBDSCAN_ESC ) )
		{
			IKBD_Update();
		}
		while( IKBD_GetKbdBytesWaiting() ) IKBD_PopKbdByte();

		IKBD_ClearKeyPressedFlag();

		while( !lExitFlag )
		{
			Video_SetPhysic( Video_GetpPhysic() );
			IKBD_Update();

/*			while( !IKBD_GetKeyPressedFlag() )
			{
				IKBD_Update();
			}*/

			if( IKBD_GetKeyPressedFlag() )
			{
			lChar = IKBD_GetLastKeyPress();
			IKBD_ClearKeyPressedFlag();

			switch( lChar )
			{
			case	eIKBDSCAN_ESC:
				lExitFlag = 1;
				break;
			case	eIKBDSCAN_NUMPADENTER:
			case	eIKBDSCAN_RETURN:
				Cli_ProcessCurrentLine();
				Cli_DrawCli();
				break;
			case	eIKBDSCAN_DELETE:
				Cli_DeleteChar();
				break;
			case	eIKBDSCAN_BACKSPACE:
				Cli_BackSpaceChar();
				break;
			case	eIKBDSCAN_TAB:
				Cli_TabExpand();
				break;
			case	eIKBDSCAN_UPARROW:
				if( (IKBD_GetKeyStatus( eIKBDSCAN_LEFTSHIFT )) || (IKBD_GetKeyStatus( eIKBDSCAN_RIGHTSHIFT)) )
				{
					Cli_ScrollUp();
				}
				else
				{
					Cli_GetPrevLineHistory();
				}
				break;
			case	eIKBDSCAN_DOWNARROW:
				if( (IKBD_GetKeyStatus( eIKBDSCAN_LEFTSHIFT )) || (IKBD_GetKeyStatus( eIKBDSCAN_RIGHTSHIFT)) )
				{
					Cli_ScrollDown();
				}
				else
				{
					Cli_GetNextLineHistory();
				}
				break;
			case	eIKBDSCAN_LEFTARROW:
				Cli_GetPrevCharHistory();
				break;
			case	eIKBDSCAN_RIGHTARROW:
				Cli_GetNextCharHistory();
				break;
			default:
				lChar = IKBD_ConvertScancodeAscii( lChar );
				Cli_InsertChar( lChar );
				break;
			}
			Cli_DrawCurrentLine();
			}
		}

		while( IKBD_GetKbdBytesWaiting() ) IKBD_PopKbdByte();
		IKBD_ClearLastKeyPress();

		Video_SetConfig( &lVideoConfig );
		Video_SetPalST( &gCli.mSavedPal[ 0 ] );
		Cli_CopyScreen( gCli.mpCliSavedScreen, Video_GetpPhysic() );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_InitCurrentLine()
* ACTION   : inits the current cli line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_InitCurrentLine()
{
	Cli_StringClear( &gCli.mCurrentLine[ 0 ] );
	Cli_StringClear( &gCli.mTabStub[ 0 ] );
	gCli.mTabIndex    = 0;
	gCli.mTabbingFlag = 0;
	gCli.mCursorX     = 0;
	gCli.mHistoryLineIndex = gCli.mCurrentLineIndex;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_InsertChar( U8 aChar )
* ACTION   : inserts character aChar into command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_InsertChar( U8 aChar )
{
	if( ( aChar >= ' ' ) &&  ( aChar <128 ) )
	{
		gCli.mCurrentLine[ gCli.mCursorX   ] = aChar;
		gCli.mCurrentLine[ gCli.mCursorX+1 ] = 0;

		if( gCli.mCursorX + 2 < dCLI_STRING_LIMIT )
			gCli.mCursorX++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_DeleteChar()
* ACTION   : deletes char at current cursor position on command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_DeleteChar()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_BackSpaceChar()
* ACTION   : deletes char before cursor position
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_BackSpaceChar()
{
	gCli.mCurrentLine[ gCli.mCursorX ] = 0;
	if( gCli.mCursorX > 0 )
	{
		gCli.mCursorX--;
		gCli.mCurrentLine[ gCli.mCursorX   ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_ProcessCurrentLine()
* ACTION   : parses command line and processes relevant commands
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_ProcessCurrentLine()
{
	char			lString[ dCLI_STRING_LIMIT+2 ];
	sCliCommand *	apCmd;
	U16				lIndex;

	Cli_PrintLine( &gCli.mCurrentLine[ 0 ] );

	lIndex = 0;
	while( (lIndex<dCLI_STRING_LIMIT) && (gCli.mCurrentLine[lIndex]) && (gCli.mCurrentLine[lIndex]!=' ') )
	{
		lString[ lIndex ] = gCli.mCurrentLine[ lIndex ];
		lIndex++;
	}
	lString[ lIndex ] = 0;

	while( gCli.mCurrentLine[ lIndex ] == ' ' )
	{
		lIndex++;
	}

	if( Cli_StringLength( lString ) )
	{
		apCmd = Cli_GetpCommand( lString );
		if( apCmd )
		{
			apCmd->mfCommand( &gCli.mCurrentLine[ lIndex ] );
		}
	}

	Cli_StringCopy( &gCli.mHistoryLine[ gCli.mCurrentLineIndex ][ 0 ], &gCli.mCurrentLine[ 0 ] );
	gCli.mCurrentLineIndex++;
	if( gCli.mCurrentLineIndex >= dCLI_HISTORY_LIMIT )
	{
		gCli.mCurrentLineIndex  = 0;
		gCli.mHistoryLoopedFlag = 1;
	}
	Cli_InitCurrentLine();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_TabExpand()
* ACTION   : tab expands current line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_TabExpand()
{
	U32		lLen;
	U16		i,j;

	if( !gCli.mTabbingFlag )
	{
		Cli_StringCopy( &gCli.mTabStub[ 0 ], &gCli.mCurrentLine[ 0 ] );
		gCli.mTabIndex    = -1;
		gCli.mTabbingFlag = 1;
	}

	lLen = Cli_StringLength( &gCli.mTabStub[ 0 ] );
	for( i=0; i<dCLI_CMD_LIMIT; i++ )
	{
		gCli.mTabIndex++;
		if( gCli.mTabIndex >= dCLI_CMD_LIMIT )
			gCli.mTabIndex = 0;

		if( gCli.mCommand[ gCli.mTabIndex ].mpCommandString )
		{
			j = 0;
			while( ( j<lLen ) && ( gCli.mTabStub[ j ] == gCli.mCommand[ gCli.mTabIndex ].mpCommandString[ j ] ) )
			{
				j++;
			}

			if( j==lLen )
			{
				Cli_StringCopy( &gCli.mCurrentLine[ 0 ], gCli.mCommand[ gCli.mTabIndex ].mpCommandString );
				gCli.mCursorX = (U16)Cli_StringLength( &gCli.mCurrentLine[ 0 ] );
				Cli_DrawCurrentLine();
				return;
			}
		}
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetPrevLineHistory()
* ACTION   : grabs a char from history and places in current command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_GetPrevCharHistory()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetPrevLineHistory()
* ACTION   : grabs a char from history and places in current command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_GetNextCharHistory()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetPrevLineHistory()
* ACTION   : grabs a command from history and places in current command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_GetPrevLineHistory()
{
	gCli.mHistoryLineIndex--;
	if( gCli.mHistoryLineIndex < 0 )
	{
		if( gCli.mHistoryLoopedFlag )
		{
			gCli.mHistoryLineIndex = dCLI_HISTORY_LIMIT - 1;
		}
		else
		{
			gCli.mHistoryLineIndex = 0;
		}
	}


	Cli_StringCopy( &gCli.mCurrentLine[ 0 ], & gCli.mHistoryLine[ gCli.mHistoryLineIndex ][ 0 ] );
	gCli.mCursorX = (U16)Cli_StringLength( &gCli.mCurrentLine[ 0 ] );
	Cli_DrawCurrentLine();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetNextLineHistory()
* ACTION   : grabs a command from history and places in current command line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_GetNextLineHistory()
{
	gCli.mHistoryLineIndex++;
	if( gCli.mHistoryLineIndex > gCli.mCurrentLineIndex )
	{
		if( !gCli.mHistoryLoopedFlag )
		{
			gCli.mHistoryLineIndex = gCli.mCurrentLineIndex;
		}
	}
	if( gCli.mHistoryLineIndex >= dCLI_HISTORY_LIMIT )
	{
		gCli.mHistoryLineIndex = 0;
	}

	Cli_StringCopy( &gCli.mCurrentLine[ 0 ], & gCli.mHistoryLine[ gCli.mHistoryLineIndex ][ 0 ] );
	gCli.mCursorX = (U16)Cli_StringLength( &gCli.mCurrentLine[ 0 ] );
	Cli_DrawCurrentLine();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_ClearString( char * apString )
* ACTION   : clears a string
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_StringClear( char * apString )
{
	U16	i;

	for( i=0; i<dCLI_STRING_LIMIT; i++ )
	{
		apString[ i ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_StringLength( const char * apString )
* ACTION   : returns length of a string
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

U32				Cli_StringLength( const char * apString )
{
	U32	lLen;

	lLen = 0;

	if( apString )
	{
		while( *apString++ )
		{
			lLen++;
		}
	}

	return( lLen );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_StringsAreEqual( const char * apStr0, const char * apStr1 )
* ACTION   : compares two strings, returns 1 if they are equal
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

U8				Cli_StringsAreEqual( const char * apStr0, const char * apStr1 )
{
	if( !apStr0 )
	{
		if( !apStr1 )
		{
			return( 1 );
		}
		return( 0 );
	}
	if( !apStr1 )
	{
		return( 0 );
	}

	while( (*apStr0) && (*apStr1) )
	{
		if( *apStr0++ != * apStr1++ )
		{
			return( 0 );
		}
	}

	if( *apStr0 != * apStr1 )
	{
		return( 0 );
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_StringCopy( char * apDst, const char * apSrc )
* ACTION   : copires string apSrc to apDst
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_StringCopy( char * apDst, const char * apSrc )
{
	U16	i;

	Cli_StringClear( apDst );

	if( apSrc )
	{
		i = 0;
		while( (i<dCLI_STRING_LIMIT) && (apSrc[i]) )
		{
			apDst[ i ] = apSrc[ i ];
			i++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_DrawCli()
* ACTION   : draws CLI
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_DrawCli()
{
	Cli_ClearScreen( Video_GetpPhysic() );
	Cli_DrawHistory();
	Cli_DrawCurrentLine();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_PrintLine( const char * apTxt )
* ACTION   : prints string apTxt on CLI screen
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_PrintLine( const char * apTxt )
{
	Cli_StringCopy( &gCli.mDisplayLine[ gCli.mDisplayLineIndex ][ 0 ], apTxt );
	gCli.mDisplayLineIndex++;
	if( gCli.mDisplayLineIndex >= dCLI_LINES_LIMIT )
	{
		gCli.mDisplayLoopedFlag = 1;
		gCli.mDisplayLineIndex  = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_DrawHistory( void )
* ACTION   : prints all previous CLI text
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_DrawHistory( void )
{
	S16	lLineIndex;
	S16	lLineY;
	U16 * lpPhysic;

	lpPhysic = Video_GetpPhysic();
	if( lpPhysic )
	{
		lLineIndex = gCli.mDisplayLineIndex;

		for( lLineY = dCLI_DISPLAYLINES_LIMIT-2; lLineY >=0; lLineY-- )
		{
			lLineIndex--;
			if( lLineIndex < 0 )
			{
				if( gCli.mDisplayLoopedFlag )
				{
					lLineIndex = dCLI_LINES_LIMIT-1;
				}
				else
				{
					return;
				}
			}
			Font8x8_Print( &gCli.mDisplayLine[ lLineIndex ][ 0 ], lpPhysic, 0, (U16)(lLineY*8) );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_DrawCurrentLine( void )
* ACTION   : prints current line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void		Cli_DrawCurrentLine( void )
{
	S16		lY;
	char	lString[ dCLI_STRING_LIMIT+4 ];

	lY = (dCLI_DISPLAYLINES_LIMIT-1)*8;

	Cli_DrawBox( Video_GetpPhysic(), 0, lY, 320, 8 );
	sprintf( lString, ">%s*", &gCli.mCurrentLine[0] );
	Font8x8_Print( lString, Video_GetpPhysic(), 0, lY );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GrabNumber( char * apTxt )
* ACTION   : prints current line
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

S32				Cli_GrabNumber( const char * apTxt )
{
	S32	lVal;
	S32	lSign;
	U8	lMode;

	while( *apTxt == ' ' )
	{
		apTxt++;
	}

	if( *apTxt == '-' )
	{
		lSign = -1;
		apTxt++;
	}
	else
	{
		lSign = 1;
	}

	lMode = 0;
	if( *apTxt == '$' )
	{
		lMode = 1;
		apTxt++;
	}
	if( apTxt[ 1 ] == 'x' )
	{
		lMode = 1;
		apTxt += 2;
	}

	lVal = 0;
	if( lMode )
	{
		while(
				( (*apTxt >= '0') && (*apTxt <= '9') )
			||	( (*apTxt >= 'a') && (*apTxt <= 'f') )
			||	( (*apTxt >= 'A') && (*apTxt <= 'F') )
			)
		{
			lVal *= 16L;
			if( (*apTxt >= '0') && (*apTxt <= '9') )
			{
				lVal += *apTxt - '0';
			}
			else if( (*apTxt >= 'a') && (*apTxt <= 'f') )
			{
				lVal += (*apTxt - 'a')+10;
			}
			else if( (*apTxt >= 'A') && (*apTxt <= 'F') )
			{
				lVal += (*apTxt - 'A')+10;
			}
			apTxt++;
		}
	}
	else
	{
		while( (*apTxt >= '0') && (*apTxt <= '9') )
		{
			lVal *= 10L;
			lVal += *apTxt - '0';
			apTxt++;
		}
	}

	lVal *= lSign;
	return( lVal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetpSavedPal( void )
* ACTION   : returns pointer to saved palette
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

U16 *			Cli_GetpSavedPal( void )
{
	return( &gCli.mSavedPal[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_GetpPal( void )
* ACTION   : returns pointer to CLI palette
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

U16 *			Cli_GetpPal( void )
{
	return( &gCli.mPal[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_ClearScreen( U16 * apScreen )
* ACTION   : clears screen apScreen
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_ClearScreen( U16 * apScreen )
{
	U16	i;

	if( apScreen )
	{
		for( i=0; i<(32000/2); i++ )
		{
			*apScreen++ = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_CopyScreen( U16 * apSrc, const U16 * apDst )
* ACTION   : copies screen apSrc to apDst
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_CopyScreen( U16 * apSrc, U16 * apDst )
{
	U16	i;

	if( apSrc && apDst )
	{
		for( i=0; i<(32000/2); i++ )
		{
			*apDst++ = *apSrc++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_DrawBox( U16 * apScreen, U16 aX, U16 aY, U16 aWidth, U16 aHeight )
* ACTION   : clears a box on apScreen at aX,aY of width aWidth and height aHeight
* CREATION : 30.01.01 PNK
*-----------------------------------------------------------------------------------*/

void			Cli_DrawBox( U16 * apScreen, U16 aX, U16 aY, U16 aWidth, U16 aHeight )
{
	U16	*	lpScreen;
	U16		lY;
	U16		i;

	for( lY = aY; lY<(aY+aHeight); lY++ )
	{
		lpScreen  = apScreen;
		lpScreen += (lY*80);
		lpScreen += (aX>>4)<<2;
		for( i=0; i<(aWidth>>4); i++ )
		{
			*lpScreen++ = 0;
			*lpScreen++ = 0;
			*lpScreen++ = 0;
			*lpScreen++ = 0;
		}
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_ScrollUp( void )
* ACTION   : Cli_ScrollUp
* CREATION : 27.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Cli_ScrollUp( void )
{
	if( gCli.mDisplayLineIndex )
	{
		gCli.mDisplayLineIndex--;
	}
	else
	{
		if( gCli.mDisplayLoopedFlag )
		{
			gCli.mDisplayLineIndex = dCLI_LINES_LIMIT - 1;
		}
	}
	Cli_DrawCli();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Cli_ScrollDown( void )
* ACTION   : Cli_ScrollDown
* CREATION : 27.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Cli_ScrollDown( void )
{
	if( gCli.mDisplayLineIndex  < (dCLI_LINES_LIMIT-1) )
	{
		gCli.mDisplayLineIndex++;
	}
	else
	{
		if( gCli.mDisplayLoopedFlag )
		{
			gCli.mDisplayLineIndex = 0;
		}
	}
	Cli_DrawCli();
}


#endif
/* ################################################################################ */


