/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"DBGCHAN.H"
#include	<GODLIB\CLI\CLI.H>
#include	<GODLIB\FILE\FILE.H>
#include	<STRING.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct	sDebugChannel
{
	U8			mActiveFlag;
	U8			mDestFlags;
	sFileHandle	mFileHandle;
} sDebugChannel;

char * gDebugChannelFileNames[ eDEBUGCHANNEL_LIMIT ] =
{
	"..\\LOGS\\ASSET.LOG",
	"..\\LOGS\\GAME.LOG",
	"..\\LOGS\\GODLIB.LOG",
	"..\\LOGS\\MEMORY.LOG",
	"..\\LOGS\\TOOL.LOG",
	"..\\LOGS\\USER.LOG",
};

char gDebugChannelString[ 1024 ];


/* ###################################################################################
#  DATA
################################################################################### */

void	DebugChannel_FileOpen( U16 aChannel, char * apFileName ); 
void	DebugChannel_FileClose( U16 aChannel ); 
void	DebugChannel_FileWrite( U16 aChannel, const char * aString ); 
void	DebugChannel_CliWrite( U16 aChannel, const char * aString ); 
void	DebugChannel_ScreenWrite( U16 aChannel, const char * aString ); 
void	DebugChannel_SteemWrite( U16 aChannel, const char * aString );


/* ###################################################################################
#  DATA
################################################################################### */

sDebugChannel	gDebugChannels[ eDEBUGCHANNEL_LIMIT ];


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_AppInit( void )
* ACTION   : DebugChannel_AppInit
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_AppInit( void )
{
	U16	i;

	for( i=0; i<eDEBUGCHANNEL_LIMIT; i++ )
	{
		gDebugChannels[ i ].mActiveFlag = 0;
		gDebugChannels[ i ].mDestFlags   = eDEBUGCHANNEL_DEST_SCREEN;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_AppDeInit( void )
* ACTION   : DebugChannel_AppDeInit
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_AppDeInit( void )
{
	U16	i;

	for( i=0; i<eDEBUGCHANNEL_LIMIT; i++ )
	{
		DebugChannel_DeActivate( i );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_Activate( U16 aChannel, U16 aDestFlags )
* ACTION   : DebugChannel_Activate
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_Activate( U16 aChannel, U16 aDestFlags )
{
	gDebugChannels[ aChannel ].mActiveFlag = 1;
	gDebugChannels[ aChannel ].mDestFlags  = (U8)aDestFlags;
	if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_FILE )
	{
		DebugChannel_FileOpen( aChannel, gDebugChannelFileNames[ aChannel ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_DeActivate( U16 aChannel )
* ACTION   : DebugChannel_DeActivate
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_DeActivate( U16 aChannel )
{
	if( gDebugChannels[ aChannel ].mActiveFlag )
	{
		if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_FILE )
		{
			DebugChannel_FileClose( aChannel );
		}
		gDebugChannels[ aChannel ].mActiveFlag = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_FileOpen( U16 aChannel, char * apFileName )
* ACTION   : DebugChannel_FileOpen
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_FileOpen( U16 aChannel, char * apFileName )
{
	gDebugChannels[ aChannel ].mFileHandle = File_Create( apFileName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_FileClose( U16 aChannel )
* ACTION   : DebugChannel_FileClose
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_FileClose( U16 aChannel )
{
	File_Close( gDebugChannels[ aChannel ].mFileHandle );
	gDebugChannels[ aChannel ].mFileHandle = 0;
	gDebugChannels[ aChannel ].mDestFlags &= ~eDEBUGCHANNEL_DEST_FILE;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_FileWrite( U16 aChannel, const char * aString )
* ACTION   : DebugChannel_FileWrite
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_FileWrite( U16 aChannel, const char * apString )
{
	S32	lLen;

	if( apString && gDebugChannels[ aChannel ].mFileHandle )
	{
		lLen = strlen( apString );
		File_Write( gDebugChannels[ aChannel ].mFileHandle, lLen, apString );
		File_Write( gDebugChannels[ aChannel ].mFileHandle, 2, "\r\n" );
/*		File_Close( gDebugChannels[ aChannel ].mFileHandle );
		gDebugChannels[ aChannel ].mFileHandle = File_Open( gDebugChannelFileNames[ aChannel ] );
		File_SeekFromEnd( gDebugChannels[ aChannel ].mFileHandle, 0 );*/
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_StringAdd_Internal( U16 aChannel, const char * apString )
* ACTION   : DebugChannel_StringAdd_Internal
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_StringAdd_Internal( U16 aChannel, const char * apString )
{
	if( gDebugChannels[ aChannel ].mActiveFlag && apString )
	{
		if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_CLI )
		{
			DebugChannel_CliWrite( aChannel, apString );
		}
		if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_FILE )
		{
			DebugChannel_FileWrite( aChannel, apString );
		}
		if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_SCREEN )
		{
			DebugChannel_ScreenWrite( aChannel, apString );
		}
		if( gDebugChannels[ aChannel ].mDestFlags & eDEBUGCHANNEL_DEST_STEEM )
		{
			DebugChannel_SteemWrite( aChannel, apString );
		}
	}
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_CliWrite( U16 aChannel, const char * apString )
* ACTION   : DebugChannel_CliWrite
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_CliWrite( U16 aChannel, const char * apString )
{
	(void)aChannel;
	if( apString )
	{
		Cli_PrintLine( apString );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_ScreenWrite( U16 aChannel, const char * apString )
* ACTION   : DebugChannel_ScreenWrite
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_ScreenWrite( U16 aChannel, const char * apString )
{
	(void)aChannel;
	if( apString )
	{
#ifdef	dGODLIB_PLATFORM_WIN
		OutputDebugStr( apString );
		OutputDebugStr( "\n" );
#endif
		printf( apString );
		printf( "\n" );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_SteemWrite( U16 aChannel, const char * apString )
* ACTION   : DebugChannel_SteemWrite
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	DebugChannel_SteemWrite( U16 aChannel, const char * apString )
{
	(void)aChannel;
	(void)apString;
#ifdef	dGODLIB_PLATFORM_ATARI
	*(U32*)0xFFFFC1F0L = (U32)apString;
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugChannel_GetActiveFlag( U16 aChannel )
* ACTION   : DebugChannel_GetActiveFlag
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	DebugChannel_GetActiveFlag( U16 aChannel )
{
	return( gDebugChannels[ aChannel ].mActiveFlag );
}
