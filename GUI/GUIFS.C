/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GUIFS.H"

#include	"GUI.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/DRIVE/DRIVE.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/TOKENISE/TOKENISE.H>

#include	<STDLIB.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eGUIFS_VAR_DRIVE,
	eGUIFS_VAR_FILE,
	eGUIFS_VAR_FILEMASK,
	eGUIFS_VAR_FILENAME,
	eGUIFS_VAR_FILEPATH,
	eGUIFS_VAR_FOLDER,
	eGUIFS_VAR_TITLE,

	eGUIFS_VAR_LIMIT
};

enum
{
	eGUIFS_VARCLIENT_OK,
	eGUIFS_VARCLIENT_CANCEL,
	eGUIFS_VARCLIENT_DRIVE,
	eGUIFS_VARCLIENT_FILE,
	eGUIFS_VARCLIENT_FILEMASK,
	eGUIFS_VARCLIENT_FILENAME,
	eGUIFS_VARCLIENT_FILEPATH,
	eGUIFS_VARCLIENT_FILEWINDOW,
	eGUIFS_VARCLIENT_FOLDER,
	eGUIFS_VARCLIENT_FOLDERBACK,
	eGUIFS_VARCLIENT_WINDOW,

	eGUIFS_VARCLIENT_LIMIT
};

enum
{
	eGUIFS_STRING_DRIVE,
	eGUIFS_STRING_FILEMASK,
	eGUIFS_STRING_FILENAME,
	eGUIFS_STRING_FILEPATH,
	eGUIFS_STRING_TITLE,

	eGUIFS_STRING_LIMIT
};

#define	dGUIFS_BUTTON_HEIGHT	10
#define	dGUIFS_BUTTON_STRINGX	10


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sGuiFSClass
{
	sHashTree *				mpTree;
	sHashTreeVar *			mpVars[ eGUIFS_VAR_LIMIT ];
	sHashTreeVarClient *	mpVarClients[ eGUIFS_VARCLIENT_LIMIT ];
	sString	*				mpStrings[ eGUIFS_STRING_LIMIT ];
	sString					mStrings[ eGUIFS_STRING_LIMIT ];
	U16						mFileCount;
	U16						mFolderCount;
	U32						mAllocFlag;
	sGemDosDTA				mDTA;
	sGemDosDTA *			mpDTAs;
	sGuiFSInfo *			mpInfo;
} sGuiFSClass;


/* ###################################################################################
#  DATA
################################################################################### */

sGuiFSClass	gGuiFS;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		GuiFS_GetFolderCount( char * apFileSpec );
U16		GuiFS_GetFileCount( char * apFileSpec );
void	GuiFS_FileListWindowBuild( sGuiWindow * apWindow );
void	GuiFS_OnFileWindow( sHashTreeVarClient  * apVar );
void	GuiFS_OnFilePath( sHashTreeVarClient  * apVar );
void	GuiFS_OnFolderBack( sHashTreeVarClient  * apVar );
void	GuiFS_OnDrive( sHashTreeVarClient * apVar );
void	GuiFS_OnFile( sHashTreeVarClient * apVar );
void	GuiFS_OnFolder( sHashTreeVarClient * apVar );
void	GuiFS_OnOK( sHashTreeVarClient * apVar );
void	GuiFS_OnCancel( sHashTreeVarClient * apVar );

int		GuiFS_DTASort( const void * apDTA0, const void * apDTA1 );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_Init( sHashTree * apTree )
* ACTION   : GuiFS_Init
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_Init( sHashTree * apTree )
{
	char	lString[ 512 ];
	U16		lDrive,i;
	Memory_Clear( sizeof(sGuiFSClass), &gGuiFS );

	gGuiFS.mpTree = apTree;

	lDrive = Drive_GetDrive();

	lString[ 0 ] = (char)('A' + lDrive);
	lString[ 1 ] = 0;

	String_Create( &gGuiFS.mStrings[ eGUIFS_STRING_DRIVE    ], lString );
	String_Create( &gGuiFS.mStrings[ eGUIFS_STRING_FILEMASK ], "*.*" );
	String_Create( &gGuiFS.mStrings[ eGUIFS_STRING_FILENAME ], "TEST.GOD" );
	lString[ 0 ] = 0;
	Drive_GetPath( 0, lString );
	String_Create( &gGuiFS.mStrings[ eGUIFS_STRING_FILEPATH ], lString );
	String_Create( &gGuiFS.mStrings[ eGUIFS_STRING_TITLE    ], "FILE SELECTOR" );

	for( i = 0; i < eGUIFS_STRING_LIMIT; i++ )
		gGuiFS.mpStrings[ i ] = &gGuiFS.mStrings[ i ];

	gGuiFS.mpVars[ eGUIFS_VAR_DRIVE    ] = HashTree_VarInit( apTree, "GUI\\FS\\DRIVE",		sizeof(sString*), &gGuiFS.mpStrings[ eGUIFS_STRING_DRIVE    ]  );
	gGuiFS.mpVars[ eGUIFS_VAR_FILE     ] = HashTree_VarInit( apTree, "GUI\\FS\\FILE",		sizeof(sGuiEvent), 0  );
	gGuiFS.mpVars[ eGUIFS_VAR_FILEMASK ] = HashTree_VarInit( apTree, "GUI\\FS\\FILEMASK",	sizeof(sString*), &gGuiFS.mpStrings[ eGUIFS_STRING_FILEMASK ]  );
	gGuiFS.mpVars[ eGUIFS_VAR_FILENAME ] = HashTree_VarInit( apTree, "GUI\\FS\\FILENAME",	sizeof(sString*), &gGuiFS.mpStrings[ eGUIFS_STRING_FILENAME ]  );
	gGuiFS.mpVars[ eGUIFS_VAR_FILEPATH ] = HashTree_VarInit( apTree, "GUI\\FS\\FILEPATH",	sizeof(sString*), &gGuiFS.mpStrings[ eGUIFS_STRING_FILEPATH ]  );
	gGuiFS.mpVars[ eGUIFS_VAR_TITLE    ] = HashTree_VarInit( apTree, "GUI\\FS\\TITLE",		sizeof(sString*), &gGuiFS.mpStrings[ eGUIFS_STRING_TITLE    ]  );
	gGuiFS.mpVars[ eGUIFS_VAR_FOLDER   ] = HashTree_VarInit( apTree, "GUI\\FS\\FOLDER",		sizeof(sGuiEvent), 0  );

	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_OK         ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FS_OK",		GuiFS_OnOK,			0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_CANCEL     ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FS_CANCEL",	GuiFS_OnCancel,		0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_DRIVE      ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_DRIVE",		GuiFS_OnDrive,		0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILE       ] = HashTree_VarClientRegister( apTree, "GUI\\FS\\FILE",					GuiFS_OnFile,		0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEWINDOW ] = HashTree_VarClientRegister( apTree, "GUI\\WINDOWS\\WND_FILELIST",		GuiFS_OnFileWindow, 0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEMASK   ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FILEMASK",		GuiFS_OnFilePath,   0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILENAME   ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FILENAME",		0,					0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEPATH   ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FILEPATH",		GuiFS_OnFilePath,   0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FOLDER     ] = HashTree_VarClientRegister( apTree, "GUI\\FS\\FOLDER",					GuiFS_OnFolder,		0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FOLDERBACK ] = HashTree_VarClientRegister( apTree, "GUI\\BUTTONS\\BUTT_FOLDERBACK",	GuiFS_OnFolderBack, 0, 0, 0 );
	gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_WINDOW     ] = HashTree_VarClientRegister( apTree, "GUI\\WINDOWS\\WND_FS",			0, 0, 0, 0 );

	gGuiFS.mAllocFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_DeInit( sHashTree * apTree )
* ACTION   : GuiFS_DeInit
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_DeInit( sHashTree * apTree )
{
	U16	i;

	for( i=0; i<eGUIFS_VAR_LIMIT; i++ )
	{
		HashTree_VarDeInit( apTree, gGuiFS.mpVars[ i ] );
	}

	for( i=0; i<eGUIFS_VARCLIENT_LIMIT; i++ )
	{
		HashTree_VarClientUnRegister( apTree, gGuiFS.mpVarClients[ i ] );
	}

	for( i=0; i<eGUIFS_STRING_LIMIT; i++ )
	{
		String_Destroy( &gGuiFS.mStrings[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_GetFolderCount( char * apFileSpec )
* ACTION   : GuiFS_GetFolderCount
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	GuiFS_GetFolderCount( char * apFileSpec )
{
	U16			lCount;


	File_SetDTA( &gGuiFS.mDTA );

	lCount = 0;
	if( !File_ReadFirst( apFileSpec, dGEMDOS_FA_READONLY+dGEMDOS_FA_DIR ) )
	{
		do
		{
			if( gGuiFS.mDTA.mAttrib & dGEMDOS_FA_DIR )
			{
				if( (String_StrCmpi( gGuiFS.mDTA.mFileName, "..")) && (String_StrCmpi( gGuiFS.mDTA.mFileName, ".")) )
				{
					lCount++;
				}
			}
		} while( !File_ReadNext() );
	}

	DebugLog_Printf2( "GuiFS_GetFolderCount() %s %d", apFileSpec, lCount );

	return( lCount );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_GetFileCount( char * apFileSpec )
* ACTION   : GuiFS_GetFileCount
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	GuiFS_GetFileCount( char * apFileSpec )
{
	U16			lCount;


	File_SetDTA( &gGuiFS.mDTA );

	lCount = 0;
	if( !File_ReadFirst( apFileSpec, dGEMDOS_FA_READONLY ) )
	{
		do
		{
			if( (String_StrCmpi( gGuiFS.mDTA.mFileName, "..")) && (String_StrCmpi( gGuiFS.mDTA.mFileName, ".")) )
			{
				lCount++;
			}
		} while( !File_ReadNext() );
	}

	DebugLog_Printf2( "GuiFS_GetFileCount() %s %d", apFileSpec, lCount );

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_FileListWindowBuild( sGuiWindow * apWindow )
* ACTION   : GuiFS_FileListWindowBuild
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_FileListWindowBuild( sGuiWindow * apWindow )
{
	sString			lPath;
	sString			lString;
	sGuiButton *	lpButtons;
	S16				lY;
	U16				lCount;
	U16				lFileCount;
	U16				lFolderCount;
	U16				i,j;
	U32				lSize;

	for( i=0; i<apWindow->mControlCount; i++ )
	{
		HashTree_VarUnRegister( gGuiFS.mpTree, apWindow->mppControls[ i ]->mpEventVar );
	}

	if( apWindow->mppControls && gGuiFS.mAllocFlag )
	{
		mMEMFREE( apWindow->mppControls[ 0 ] );
		mMEMFREE( apWindow->mppControls );
		apWindow->mppControls   = 0;
		apWindow->mControlCount = 0;
	}
	if( gGuiFS.mpDTAs )
	{
		mMEMFREE( gGuiFS.mpDTAs );
		gGuiFS.mpDTAs = 0;
	}

	String_Create( &lPath, gGuiFS.mStrings[ eGUIFS_STRING_DRIVE ].mpChars );
	String_Append( &lPath, ":\\" );
	String_Append( &lPath, gGuiFS.mStrings[ eGUIFS_STRING_FILEPATH ].mpChars );
	String_Append( &lPath, "\\" );


	String_Create( &lString, lPath.mpChars );
	String_Append( &lString, gGuiFS.mStrings[ eGUIFS_STRING_FILEMASK ].mpChars );
	String_Append( &lPath, "*.*" );

	lFolderCount = GuiFS_GetFolderCount( lPath.mpChars );
	lFileCount   = GuiFS_GetFileCount( lString.mpChars );

	lCount       = (U16)(lFileCount + lFolderCount);
	if( lCount )
	{
		gGuiFS.mAllocFlag = 1;

		lSize  = lCount;
		lSize *= sizeof( sGemDosDTA );
		gGuiFS.mpDTAs = (sGemDosDTA*)mMEMCALLOC( lSize );

		i = 0;

		File_SetDTA( &gGuiFS.mDTA );
		if( !File_ReadFirst( lPath.mpChars, dGEMDOS_FA_DIR ) )
		{
			do
			{
				if( gGuiFS.mDTA.mAttrib & dGEMDOS_FA_DIR )
				{
					if( (String_StrCmpi( gGuiFS.mDTA.mFileName, "..")) && (String_StrCmpi( gGuiFS.mDTA.mFileName, ".")) )
					{
						gGuiFS.mpDTAs[ i ] = gGuiFS.mDTA;
						for( j=0; j<13; j++)
						{
							gGuiFS.mpDTAs[ i ].mFileName[ j+1 ] = gGuiFS.mDTA.mFileName[ j ];
						}
						gGuiFS.mpDTAs[ i ].mFileName[ 0 ] = 3;
						i++;
					}
				}
			} while( !File_ReadNext() );
		}

		File_SetDTA( &gGuiFS.mDTA );
		if( !File_ReadFirst( lString.mpChars, dGEMDOS_FA_READONLY ) )
		{
			do
			{
				if( (String_StrCmpi( gGuiFS.mDTA.mFileName, "..")) && (String_StrCmpi( gGuiFS.mDTA.mFileName, ".")) )
				{
					gGuiFS.mpDTAs[ i++ ] = gGuiFS.mDTA;
				}
			} while( !File_ReadNext() );
		}

		if( lFolderCount )
		{
			qsort( &gGuiFS.mpDTAs[ 0 ], lFolderCount, sizeof(sGemDosDTA), GuiFS_DTASort );
		}
		if( lFileCount )
		{
			qsort( &gGuiFS.mpDTAs[ lFolderCount ], lFileCount, sizeof(sGemDosDTA), GuiFS_DTASort );
		}

		lSize  = lCount;
		lSize *= sizeof( sGuiButton );

		lpButtons = (sGuiButton*)mMEMCALLOC( lSize );


		lY = 1;
		for( i=0; i<lCount; i++ )
		{
			lpButtons[ i ].mInfo.mType                    = eGUI_TYPE_BUTTON;
			lpButtons[ i ].mInfo.mEvent.mpInfo            =&lpButtons[ i ].mInfo;
			lpButtons[ i ].mInfo.mEvent.mEvent            = 0;
			lpButtons[ i ].mInfo.mRectPair.mLocal.mX      = 2;
			lpButtons[ i ].mInfo.mRectPair.mLocal.mWidth  = -2;
			lpButtons[ i ].mInfo.mRectPair.mLocal.mY      = lY;
			lpButtons[ i ].mInfo.mRectPair.mLocal.mHeight = dGUIFS_BUTTON_HEIGHT;
			lpButtons[ i ].mButtonType                    = eGUI_BUTTON_HOVER;
			lpButtons[ i ].mString.mRects.mLocal.mX       = dGUIFS_BUTTON_STRINGX;
			lpButtons[ i ].mString.mpTitle                = &gGuiFS.mpDTAs[ i ].mFileName[ 0 ];
			if( gGuiFS.mpDTAs[ i ].mAttrib & dGEMDOS_FA_DIR )
			{
				lpButtons[ i ].mString.mRects.mLocal.mX       = dGUIFS_BUTTON_STRINGX-6;
				lpButtons[ i ].mInfo.mpEventVar = HashTree_VarRegister( gGuiFS.mpTree, "GUI\\FS\\FOLDER" );
			}
			else
			{
				lpButtons[ i ].mInfo.mpEventVar = HashTree_VarRegister( gGuiFS.mpTree, "GUI\\FS\\FILE" );
			}
			lY += dGUIFS_BUTTON_HEIGHT;
		}

		apWindow->mppControls     = (sGuiInfo**)mMEMCALLOC( lCount * 4 );
		for( i=0; i<lCount; i++ )
		{
			apWindow->mppControls[ i ] = (sGuiInfo*)&lpButtons[ i ];
		}
		apWindow->mControlCount   = lCount;
		if( lY > apWindow->mInfo.mRectPair.mWorld.mHeight )
		{
			apWindow->mCanvas.mHeight = (S16)(lY + 4);
		}
		else
		{
			apWindow->mCanvas.mHeight = apWindow->mInfo.mRectPair.mWorld.mHeight;
		}
		apWindow->mCanvas.mY = 0;
		apWindow->mInfo.mRedrawFlag = 2;

		Gui_WorldPosUpdate();
	}

	String_Destroy( &lString );
	String_Destroy( &lPath );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnFileWindow( sHashTreeVarClient  * apVar )
* ACTION   : GuiFS_OnFileWindow
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnFileWindow( sHashTreeVarClient  * apClient )
{
	sGuiEvent *		lpEvent;
	sGuiWindow *	lpWindow;

	lpEvent  = (sGuiEvent*)apClient->mpVar->mpData;
	lpWindow = (sGuiWindow*)lpEvent->mpInfo;
	DebugLog_Printf1( "GuiFS_OnFileWindow() %p", lpWindow );
	if( lpWindow )
	{
		DebugLog_Printf1( "window name: %s", lpWindow->mInfo.mpName );
		GuiFS_FileListWindowBuild(lpWindow);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnFilePath( sHashTreeVarClient  * apClient)
* ACTION   : GuiFS_OnFilePath
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnFilePath( sHashTreeVarClient  * apClient )
{
	sGuiWindow *	lpWindow;
	sGuiEvent *		lpEvent;
	sHashTreeVar *	lpVar;

	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( lpEvent )
	{
		DebugLog_Printf1( "GuiFS_OnFilePath() %d", lpEvent->mEvent );
		if( eGUIEVENT_BUTTON_UPDATE == lpEvent->mEvent )
		{
			DebugLog_Printf0( "GuiFS_OnFilePath() eGUIEVENT_BUTTON_UPDATE" );
			lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEWINDOW ]->mpVar;
			if( lpVar )
			{
				lpEvent = (sGuiEvent*)lpVar->mpData;
				if( lpEvent )
				{
					lpWindow = (sGuiWindow*)lpEvent->mpInfo;
					if( lpWindow )
					{
						Gui_RequestWindowRefresh( lpWindow );
		/*				GuiFS_FileListWindowBuild(lpWindow);*/
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnDrive( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnDrive
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnDrive( sHashTreeVarClient  * apClient )
{
	sGuiWindow *	lpWindow;
	sGuiEvent *		lpEvent;
	sHashTreeVar *	lpVar;

	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( lpEvent )
	{
		if( eGUIEVENT_BUTTON_UPDATE == lpEvent->mEvent )
		{
			DebugLog_Printf0( "GuiFS_OnDrive()" );
			lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEWINDOW ]->mpVar;
			if( lpVar )
			{
				lpEvent = (sGuiEvent*)lpVar->mpData;
				if( lpEvent )
				{
					lpWindow = (sGuiWindow*)lpEvent->mpInfo;
					if( lpWindow )
					{
						Gui_RequestWindowRefresh( lpWindow );
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnFolderBack( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnFolderBack
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnFolderBack( sHashTreeVarClient  * apClient )
{
	sGuiWindow *	lpWindow;
	sGuiEvent *		lpEvent;
	sString 		lString;
	sString * 		lpFilePath;
	sHashTreeVar *	lpVar;
	U16				i;


	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( eGUIEVENT_BUTTON_LEFTCLICK == lpEvent->mEvent )
	{
		lpVar    = gGuiFS.mpVars[ eGUIFS_VAR_FILEPATH ];
		DebugLog_Printf0( "GuiFS_OnFolderBack()" );

		if( lpVar )
		{
			lpFilePath = *(sString**)lpVar->mpData;
			if( lpFilePath )
			{
				if( String_GetLength( lpFilePath ) )
				{
					String_Create( &lString, lpFilePath->mpChars );
					i = (U16)(String_GetLength( &lString )-1);
					while( (i) && (lString.mpChars[ i ] != '\\') && (lString.mpChars[ i ] != '/') )
					{
						i--;
					}
					lString.mpChars[ i ] = 0;
					String_Update( lpFilePath, lString.mpChars );
					String_Destroy( &lString );

					HashTree_VarWrite( lpVar, &lpFilePath );

/*					lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEPATH ]->mpVar;
					if( lpVar )
					{
						lpEvent = (sGuiEvent*)lpVar->mpData;
						if( lpEvent )
						{
							lpButton = (sGuiButton*)lpEvent->mpInfo;
							lpButton->mInfo.mRedrawFlag = 2;
						}
					}
*/
					lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEWINDOW ]->mpVar;
					if( lpVar )
					{
						lpEvent = (sGuiEvent*)lpVar->mpData;
						if( lpEvent )
						{
							lpWindow = (sGuiWindow*)lpEvent->mpInfo;
							if( lpWindow )
							{
								Gui_RequestWindowRefresh( lpWindow );
/*								GuiFS_FileListWindowBuild(lpWindow);*/
							}
						}
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnFile( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnFile
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnFile( sHashTreeVarClient  * apClient )
{
	sGuiEvent *		lpEvent;
	sGuiButton *	lpButton;
	sHashTreeVar *	lpVar;

	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( lpEvent )
	{
		if( eGUIEVENT_BUTTON_LEFTCLICK == lpEvent->mEvent )
		{
			DebugLog_Printf0( "GuiFS_OnFile()" );
			lpButton = (sGuiButton*)lpEvent->mpInfo;
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_FILENAME ], lpButton->mString.mpTitle );


			lpVar = gGuiFS.mpVars[ eGUIFS_VAR_FILENAME ];
			if( lpVar )
			{
				HashTree_VarWrite( lpVar, &gGuiFS.mStrings[ eGUIFS_STRING_FILENAME ] );
			}
/*
			lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILENAME ]->mpVar;
			if( lpVar )
			{
				lpEvent = (sGuiEvent*)lpVar->mpData;
				if( lpEvent )
				{
					lpButton = (sGuiButton*)lpEvent->mpInfo;
					lpButton->mInfo.mRedrawFlag = 2;
				}
			}
*/
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnFolder( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnFolder
* CREATION : 29.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnFolder( sHashTreeVarClient  * apClient )
{
	sGuiWindow *	lpWindow;
	sGuiEvent *		lpEvent;
	sString *		lpFilePath;
	sHashTreeVar *	lpVar;
	sGuiButton *	lpButton;

	lpEvent  = (sGuiEvent*)apClient->mpVar->mpData;
	if( eGUIEVENT_BUTTON_LEFTCLICK == lpEvent->mEvent )
	{
		DebugLog_Printf0( "GuiFS_OnFolder()" );
		lpButton = (sGuiButton*)lpEvent->mpInfo;

		lpVar    = gGuiFS.mpVars[ eGUIFS_VAR_FILEPATH ];

		if( lpVar && lpButton )
		{
			lpFilePath = *(sString**)lpVar->mpData;
			if( lpFilePath )
			{
				if( String_GetLength( lpFilePath ) )
				{
					String_Append( lpFilePath, "\\" );
				}
				String_Append( lpFilePath, &lpButton->mString.mpTitle[ 1 ] );

				HashTree_VarWrite( lpVar, &lpFilePath );

/*				lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEPATH ]->mpVar;
				if( lpVar )
				{
					lpEvent = (sGuiEvent*)lpVar->mpData;
					if( lpEvent )
					{
						lpButton = (sGuiButton*)lpEvent->mpInfo;
						lpButton->mInfo.mRedrawFlag = 2;
					}
				}
*/
				lpVar = gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_FILEWINDOW ]->mpVar;
				if( lpVar )
				{
					lpEvent = (sGuiEvent*)lpVar->mpData;
					if( lpEvent )
					{
						lpWindow = (sGuiWindow*)lpEvent->mpInfo;
						if( lpWindow )
						{
							Gui_RequestWindowRefresh( lpWindow );
/*							GuiFS_FileListWindowBuild(lpWindow);*/
						}
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_Open( sGuiFSInfo * apInfo )
* ACTION   : GuiFS_Open
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_Open( sGuiFSInfo * apInfo )
{
	sGuiWindow *	lpWindow;
	sHashTreeVar *	lpVar;
	sGuiEvent *		lpEvent;
	sGuiEvent		lEvent;

	gGuiFS.mpInfo = apInfo;
	lpVar =gGuiFS.mpVarClients[ eGUIFS_VARCLIENT_WINDOW ]->mpVar;
	if( lpVar )
	{
		lpEvent = (sGuiEvent*)lpVar->mpData;
		if( lpEvent )
		{
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_DRIVE    ], apInfo->mDrive.mpChars    );
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_FILEMASK ], apInfo->mFileMask.mpChars );
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_FILENAME ], apInfo->mFileName.mpChars );
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_FILEPATH ], apInfo->mFilePath.mpChars );
			String_Update( &gGuiFS.mStrings[ eGUIFS_STRING_TITLE    ], apInfo->mTitle.mpChars );

			lpWindow = (sGuiWindow*)lpEvent->mpInfo;
			lEvent.mEvent = eGUIEVENT_WINDOW_OPEN;
			lEvent.mpInfo = lpWindow;
			GuiEventQueue_EventAdd( &lEvent );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnOK( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnOK
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnOK( sHashTreeVarClient  * apClient )
{
	sGuiEvent *		lpEvent;
	sGuiFSInfo *	lpInfo;

	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( lpEvent )
	{
		if( eGUIEVENT_BUTTON_LEFTCLICK == lpEvent->mEvent )
		{
			GuiButton_DeSelect( (sGuiButton*)lpEvent->mpInfo );
			lpInfo = gGuiFS.mpInfo;
			if( lpInfo )
			{
				lpInfo->mButton    = eGUIFS_BUTTON_OK;
				String_Update( &lpInfo->mDrive,    gGuiFS.mStrings[ eGUIFS_STRING_DRIVE    ].mpChars );
				String_Update( &lpInfo->mFileMask, gGuiFS.mStrings[ eGUIFS_STRING_FILEMASK ].mpChars );
				String_Update( &lpInfo->mFileName, gGuiFS.mStrings[ eGUIFS_STRING_FILENAME ].mpChars );
				String_Update( &lpInfo->mFilePath, gGuiFS.mStrings[ eGUIFS_STRING_FILEPATH ].mpChars );
				lpInfo->mpCB( lpInfo );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_OnCancel( sHashTreeVarClient  * apClient )
* ACTION   : GuiFS_OnCancel
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFS_OnCancel( sHashTreeVarClient  * apClient )
{
	sGuiEvent *		lpEvent;
	sGuiFSInfo *	lpInfo;

	lpEvent = (sGuiEvent*)apClient->mpVar->mpData;
	if( lpEvent )
	{
		if( eGUIEVENT_BUTTON_LEFTCLICK == lpEvent->mEvent )
		{
			GuiButton_DeSelect( (sGuiButton*)lpEvent->mpInfo );
			lpInfo = gGuiFS.mpInfo;
			if( lpInfo )
			{
				lpInfo->mButton    = eGUIFS_BUTTON_CANCEL;
				lpInfo->mpCB( lpInfo );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFSInfo_Init( sGuiFSInfo * apInfo,const char * apTitle,const char * apMask,const char * apFileName )
* ACTION   : GuiFSInfo_Init
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiFSInfo_Init( sGuiFSInfo * apInfo,const char * apTitle,const char * apMask,const char * apFileName )
{
	char	lString[ 512 ];
	U16		lDrive;

	if( apInfo )
	{
		lDrive = Drive_GetDrive();

		lString[ 0 ] = (char)('A' + lDrive);
		lString[ 1 ] = 0;

		String_Create( &apInfo->mDrive, lString );
		String_Create( &apInfo->mFileMask, apMask );
		String_Create( &apInfo->mFileName, apFileName );
		Drive_GetPath( 0, lString );
		String_Create( &apInfo->mFilePath, lString );
		String_Create( &apInfo->mTitle, apTitle );
		apInfo->mpCB       = 0;
		apInfo->mButton    = eGUIFS_BUTTON_CANCEL;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiFSInfo_DeInit(sGuiFSInfo * apInfo)
* ACTION   : GuiFSInfo_DeInit
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiFSInfo_DeInit(sGuiFSInfo * apInfo)
{
	if( apInfo )
	{
		String_Destroy( &apInfo->mDrive );
		String_Destroy( &apInfo->mFileMask );
		String_Destroy( &apInfo->mFileName );
		String_Destroy( &apInfo->mFilePath );
		String_Destroy( &apInfo->mTitle );

		apInfo->mpCB       = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sString * GuiFSInfo_FullNameBuild(sGuiFSInfo * apInfo)
* ACTION   : GuiFSInfo_FullNameBuild
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sString * GuiFSInfo_FullNameBuild(sGuiFSInfo * apInfo)
{
	sString *	lpString = 0;

	if( apInfo )
	{
		lpString = mMEMCALLOC( sizeof( sString ) );
		if( lpString )
		{
			String_Create( lpString, apInfo->mDrive.mpChars );
			String_Append( lpString, ":\\" );
			String_Append( lpString, apInfo->mFilePath.mpChars );
			String_Append( lpString, "\\" );
			String_Append( lpString, apInfo->mFileName.mpChars );
		}
	}

	return( lpString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiFS_DTASort( const void * apDTA0,const void * apDTA1 )
* ACTION   : GuiFS_DTASort
* CREATION : 05.03.2004 PNK
*-----------------------------------------------------------------------------------*/

int	GuiFS_DTASort( const void * apDTA0,const void * apDTA1 )
{
	sGemDosDTA *	lpDTA0;
	sGemDosDTA *	lpDTA1;
	int				lRes;
	U16				i;

	lpDTA0 = (sGemDosDTA*)apDTA0;
	lpDTA1 = (sGemDosDTA*)apDTA1;

	lRes = 0;

	i = 0;
	while( (i<14) && (!lRes) )
	{
		lRes = lpDTA0->mFileName[ i ] - lpDTA1->mFileName[ i ];
		i++;
	}

	return( lRes );
}


/* ################################################################################ */
