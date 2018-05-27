/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_COM.H"

#include	"ACH_MAIN.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/THREAD/THREAD.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_COM_TOKEN_LIMIT		16
#define	dACH_COM_TOKENSPACE_LIMIT	1024
#define	dACH_COM_BUFFERSIZE			(64L*1024L)


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_COMPILER_BLOCK_UNKNOWN,
	eACH_COMPILER_BLOCK_STATS,
	eACH_COMPILER_BLOCK_TASKS,
	eACH_COMPILER_BLOCK_SCORETABLE,
	eACH_COMPILER_BLOCK_LIMIT,
};

enum
{
	eACH_COMPILER_TYPE_UNKNOWN,
	eACH_COMPILER_TYPE_U32,
	eACH_COMPILER_TYPE_TIME,
	eACH_COMPILER_TYPE_LIMIT,
};

enum
{
	eACH_COMPILER_SORT_UNKNOWN,
	eACH_COMPILER_SORT_ASCENDING,
	eACH_COMPILER_SORT_DESCENDING,
	eACH_COMPILER_SORT_LIMIT,
};

enum
{
	eACH_COMPILER_SCOREDEF_UNKNOWN,

	eACH_COMPILER_SCOREDEF_ENTRYCOUNT,
	eACH_COMPILER_SCOREDEF_SORT,
	eACH_COMPILER_SCOREDEF_TYPE,
	eACH_COMPILER_SCOREDEF_TITLE,

	eACH_COMPILER_SCOREDEF_LIMIT,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveCompilerLine
{
	U32		mTokenCount;
	U32		mTokenSpaceOffset;
	char *	mpTokens[ dACH_COM_TOKENSPACE_LIMIT ] ;
	char	mTokenSpace[ dACH_COM_TOKENSPACE_LIMIT ];
	U8		mTerminator;
	U8		mInTokenFlag;
}sAchieveCompilerLine;


typedef struct sAchieveCompiler
{
	sAchieveScoreTableDef	mScoreTableDef;
	char	mScoreTableName[ 1024 ];
	U32 *	mpScoreTableEntryCounts;
	void *				mpBuffer;
	U32					mBufferSize;
	S32	mFileHandle;
	U16	mTaskCount;
	U16	mStatCount;
	U16	mScoreTableCount;
	U16	mScoreTableIndex;
	U16	mScoreTableEntryIndex;
	U16	mStatIndex;
	U16 mTaskIndex;
	S16	mBraceLevel;
	S16	mBraceLevelCached;
	U16	mBlockType;
	U16	mLineIndex;
	U16	mErrorCount;
}sAchieveCompiler;


/* ###################################################################################
#  DATA
################################################################################### */

sAchieveCompiler	gAchieveCompiler;
typedef U8 (*fAchieveCompilerOnLine)( sThread * apThread, sAchieveCompilerLine * apLine );

sTagString	gAchieveCompilerBlockStrings[ eACH_COMPILER_BLOCK_LIMIT ] =
{
	{	eACH_COMPILER_BLOCK_UNKNOWN,		"[UNKNOWN]"			},
	{	eACH_COMPILER_BLOCK_STATS,			"[STATS]"			},
	{	eACH_COMPILER_BLOCK_TASKS,			"[TASKS]"			},
	{	eACH_COMPILER_BLOCK_SCORETABLE,		"[SCORETABLE]"		},
};

sTagString	gAchieveCompilerScoreDefStrings[ eACH_COMPILER_SCOREDEF_LIMIT ] =
{
	{	eACH_COMPILER_SCOREDEF_UNKNOWN,		"UNKNOWN"		},

	{	eACH_COMPILER_SCOREDEF_ENTRYCOUNT,	"ENTRYCOUNT"	},
	{	eACH_COMPILER_SCOREDEF_SORT,		"SORT"			},
	{	eACH_COMPILER_SCOREDEF_TYPE,		"TYPE"			},
	{	eACH_COMPILER_SCOREDEF_TITLE,		"TITLE"			},
};

sTagString	gAchieveCompilerTypeStrings[ eACH_COMPILER_TYPE_LIMIT ] =
{
	{	eACH_COMPILER_TYPE_UNKNOWN,		"UNKNOWN"		},
	{	eACH_COMPILER_TYPE_U32,			"U32"			},
	{	eACH_COMPILER_TYPE_TIME,		"TIME"			},
};

sTagString	gAchieveCompilerSortStrings[ eACH_COMPILER_SORT_LIMIT ] =
{
	{	eACH_COMPILER_SORT_UNKNOWN,		"UNKNOWN"		},
	{	eACH_COMPILER_SORT_ASCENDING,	"ASCENDING"		},
	{	eACH_COMPILER_SORT_DESCENDING,	"DESCENDING"	},
};

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Compiler_Pass( const char * apData, const U32 aSize, fAchieveCompilerOnLine afOnLine );
void	Achieve_Compiler_Line_Init( sAchieveCompilerLine * apLine );
void	Achieve_Compiler_Line_DeInit( sAchieveCompilerLine * apLine );
U8		Achieve_Compiler_Line_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine, U8 aChar );
U8		Achieve_Compiler_Pass1_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine );
U8		Achieve_Compiler_Pass2_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine );
U8		Achieve_Compiler_Pass3_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine );
U8		Achieve_Compiler_PassSyntax_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine );
U16		Achieve_Compiler_GetBlockType( sAchieveCompilerLine * apLine );
void	Achieve_Compiler_LineError( char * apString, char * apString2 );
void	Achieve_Compiler_Output_BlockStart( const char * apString);
void	Achieve_Compiler_Output_BlockEnd( void );
void	Achieve_Compiler_Output_String( const char * apString);


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_AppInit( void )
* ACTION   : Achieve_Compiler_AppInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_AppInit( void )
{
	memset( &gAchieveCompiler, 0, sizeof(sAchieveCompiler) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_AppDeInit( void )
* ACTION   : Achieve_Compiler_AppDeInit
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Compile( const char * apSrcFileName, const char * apDstFileName )
* ACTION   : Achieve_Compiler_Compile
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Compile( const char * apSrcFileName, const char * apDstFileName )
{
	char *	lpSrc;

	(void)apDstFileName;
	lpSrc = (char*)File_Load( apSrcFileName );

	if( lpSrc )
	{
		U32	lSize;
		U32	lEntryCount,i;

		lSize = File_GetSize( apSrcFileName );

		gAchieveCompiler.mFileHandle = File_Create( "AWD_ERR.LOG" );
		Achieve_Compiler_Pass( lpSrc, lSize, Achieve_Compiler_PassSyntax_Consumer_Thread );
		File_Close( gAchieveCompiler.mFileHandle );

		if( !gAchieveCompiler.mErrorCount )
		{
			Achieve_Compiler_Pass( lpSrc, lSize, Achieve_Compiler_Pass1_Consumer_Thread );

			if( gAchieveCompiler.mScoreTableCount )
			{
				gAchieveCompiler.mpScoreTableEntryCounts = (U32*)mMEMCALLOC( 4 * gAchieveCompiler.mScoreTableCount );
			}
			gAchieveCompiler.mScoreTableIndex = 0;

			Achieve_Compiler_Pass( lpSrc, lSize, Achieve_Compiler_Pass2_Consumer_Thread );

			if( gAchieveCompiler.mScoreTableCount )
			{
				lEntryCount = gAchieveCompiler.mpScoreTableEntryCounts[ 0 ];
				for( i=1; i<gAchieveCompiler.mScoreTableCount; i++ )
				{
					if( gAchieveCompiler.mpScoreTableEntryCounts[ i ] > lEntryCount )
					{
						lEntryCount = gAchieveCompiler.mpScoreTableEntryCounts[ i ];
					}
				}

				gAchieveCompiler.mScoreTableDef.mpValues = (S32*)mMEMCALLOC( 4 * lEntryCount );
				gAchieveCompiler.mScoreTableDef.mpNames  = (sAchieveUserName*)mMEMCALLOC( sizeof(sAchieveUserName) * lEntryCount );
			}


			gAchieveCompiler.mBufferSize = dACH_COM_BUFFERSIZE;
			gAchieveCompiler.mpBuffer    = mMEMCALLOC( gAchieveCompiler.mBufferSize );
			Achieve_Create( gAchieveCompiler.mScoreTableCount, gAchieveCompiler.mStatCount, gAchieveCompiler.mTaskCount, gAchieveCompiler.mpBuffer, gAchieveCompiler.mBufferSize );

			gAchieveCompiler.mScoreTableIndex = 0;
			Achieve_Compiler_Pass( lpSrc, lSize, Achieve_Compiler_Pass3_Consumer_Thread );
			Achieve_SetUserInfo( "TEST" );

			Achieve_Save( apDstFileName );
			Achieve_Destroy();

			if( gAchieveCompiler.mpScoreTableEntryCounts )
			{
				mMEMFREE( gAchieveCompiler.mpScoreTableEntryCounts );
				gAchieveCompiler.mpScoreTableEntryCounts = 0;
			}
			if( gAchieveCompiler.mScoreTableDef.mpValues )
			{
				mMEMFREE( gAchieveCompiler.mScoreTableDef.mpValues );
				gAchieveCompiler.mScoreTableDef.mpValues = 0;
			}
			if( gAchieveCompiler.mScoreTableDef.mpNames )
			{
				mMEMFREE( gAchieveCompiler.mScoreTableDef.mpNames );
				gAchieveCompiler.mScoreTableDef.mpNames = 0;
			}
			if( gAchieveCompiler.mpBuffer )
			{
				mMEMFREE( gAchieveCompiler.mpBuffer );
				gAchieveCompiler.mpBuffer = 0;
			}
		}
		File_UnLoad( lpSrc );
	}
	else
	{
		printf( "ERROR: Couldn't load file: %s\n", apSrcFileName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_DeCompile( const char * apSrcFileName, const char * apDstFileName )
* ACTION   : Achieve_Compiler_DeCompile
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_DeCompile( const char * apSrcFileName, const char * apDstFileName )
{
	if( File_Exists( apSrcFileName ) )
	{
		gAchieveCompiler.mBufferSize = dACH_COM_BUFFERSIZE;
		gAchieveCompiler.mpBuffer    = mMEMCALLOC( gAchieveCompiler.mBufferSize );
		if( Achieve_Load( apSrcFileName, gAchieveCompiler.mpBuffer, gAchieveCompiler.mBufferSize ) )
		{
			char	lString[ 1024 ];
			char *	lpTypeName;
			U16	i,j,k;

			gAchieveCompiler.mFileHandle = File_Create( apDstFileName );

			gAchieveCompiler.mBraceLevel = 0;

			Achieve_Compiler_Output_BlockStart( "[STATS]" );
			for( i=0; i<Achieve_Stat_GetCount(); i++ )
			{
				switch( Achieve_Stat_GetType(i) )
				{
				case	eACH_TYPE_U32:
					lpTypeName = "U32";
					break;
				case	eACH_TYPE_TIME:
					lpTypeName = "TIME";
					break;
				default:
					lpTypeName = "UNKNOWN";
					break;
				}
				sprintf( lString, "%s \"%s\"", lpTypeName, Achieve_Stat_GetName( i ) );
				Achieve_Compiler_Output_String( lString );
			}
			Achieve_Compiler_Output_BlockEnd();

			Achieve_Compiler_Output_BlockStart( "[TASKS]" );
			for( i=0; i<Achieve_Tasks_GetCount(); i++ )
			{
				sprintf( lString, "\"%s\" %d", Achieve_Tasks_GetpTitle( i ), Achieve_Task_GetPoints(i) );
				Achieve_Compiler_Output_String( lString );
			}
			Achieve_Compiler_Output_BlockEnd();

			for( i=0; i<Achieve_ScoreTables_GetCount(); i++ )
			{
				Achieve_Compiler_Output_BlockStart( "[SCORETABLE]" );

				sprintf( lString, "TITLE \"%s\"", Achieve_ScoreTable_GetpTableName(i) );
				Achieve_Compiler_Output_String( lString );

				sprintf( lString, "ENTRYCOUNT %d", Achieve_ScoreTable_GetEntryCount(i) );
				Achieve_Compiler_Output_String( lString );

				switch( Achieve_ScoreTable_GetType(i) )
				{
				case	eACH_TYPE_U32:
					lpTypeName = "U32";
					break;
				case	eACH_TYPE_TIME:
					lpTypeName = "TIME";
					break;
				default:
					lpTypeName = "UNKNOWN";
					break;
				}

				sprintf( lString, "TYPE %s", lpTypeName );
				Achieve_Compiler_Output_String( lString );

				switch( Achieve_ScoreTable_GetSortDirection(i) )
				{
				case	eACH_SORT_HI:
					lpTypeName = "ASCENDING";
					break;
				case	eACH_SORT_LO:
					lpTypeName = "DESCENDING";
					break;
				default:
					lpTypeName = "UNKNOWN";
					break;
				}

				sprintf( lString, "SORT %s", lpTypeName );
				Achieve_Compiler_Output_String( lString );

				Achieve_Compiler_Output_BlockStart( "[ENTRIES]" );
				for( j=0; j<Achieve_ScoreTable_GetEntryCount(i); j++ )
				{
					sprintf( lString, "\"%s\" %ld", Achieve_ScoreTable_GetpName( i, j ), Achieve_ScoreTable_GetScore( i, j ) );
					Achieve_Compiler_Output_String( lString );
				}
				Achieve_Compiler_Output_BlockEnd();


				Achieve_Compiler_Output_BlockEnd();
			}

			for( i=0; i<Achieve_GetUserCount(); i++ )
			{
				Achieve_Compiler_Output_BlockStart( "[USER]" );

				sprintf( lString, "NAME \"%s\"", Achieve_GetUserName(i) );
				Achieve_Compiler_Output_String( lString );

				Achieve_Compiler_Output_BlockStart( "[STATS]" );
				for( j=0; j<Achieve_Stat_GetCount(); j++ )
				{
					sprintf( lString, "\"%s\" %ld", Achieve_Stat_GetName( j ), Achieve_Stat_Get( j ) );
					Achieve_Compiler_Output_String( lString );
				}
				Achieve_Compiler_Output_BlockEnd();

				Achieve_Compiler_Output_BlockStart( "[TASKS]" );
				for( j=0; j<Achieve_Tasks_GetCount(); j++ )
				{
					sprintf( lString, "\"%s\"", Achieve_Tasks_GetpTitle(j) );
					Achieve_Compiler_Output_String( lString );
				}
				Achieve_Compiler_Output_BlockEnd();

				for( j=0; j<Achieve_ScoreTables_GetCount(); j++ )
				{
					Achieve_Compiler_Output_BlockStart( "[SCORETABLE]" );
					for( k=0; k<Achieve_ScoreTable_GetEntryCount(j); k++ )
					{
						sprintf( lString, "\"%s\" %ld", Achieve_ScoreTable_GetpName( j, k ), Achieve_ScoreTable_GetScore( j, k ) );
						Achieve_Compiler_Output_String( lString );
					}
					Achieve_Compiler_Output_BlockEnd();
				}

				Achieve_Compiler_Output_BlockEnd();
			}

			File_Close( gAchieveCompiler.mFileHandle );
		}

		if( gAchieveCompiler.mpBuffer )
		{
			mMEMFREE( gAchieveCompiler.mpBuffer );
			gAchieveCompiler.mpBuffer = 0;
		}
		gAchieveCompiler.mBufferSize = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Output_BlockStart( const char * apString )
* ACTION   : Achieve_Compiler_Output_BlockStart
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Output_BlockStart( const char * apString )
{
	Achieve_Compiler_Output_String( apString );
	Achieve_Compiler_Output_String( "{" );
	gAchieveCompiler.mBraceLevel++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Output_BlockEnd( )
* ACTION   : Achieve_Compiler_Output_BlockEnd
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Output_BlockEnd( )
{
	Assert( gAchieveCompiler.mBraceLevel );
	gAchieveCompiler.mBraceLevel--;
	Achieve_Compiler_Output_String( "}" );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Output_String( const char * apString )
* ACTION   : Achieve_Compiler_Output_String
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Output_String( const char * apString )
{
	char	lString[ 128 ];

	Assert( apString );
	if( gAchieveCompiler.mBraceLevel )
	{
		U16	i;
		for( i=0; i<gAchieveCompiler.mBraceLevel; i++ )
		{
			lString[ i ] = '\t';
		}
		lString[ i ] = 0;

		File_Write( gAchieveCompiler.mFileHandle, String_StrLen(lString), lString );
	}
	File_Write( gAchieveCompiler.mFileHandle, String_StrLen(apString), apString );
	File_Write( gAchieveCompiler.mFileHandle, 2, "\r\n" );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Pass( const char * apData, const U32 aSize, fAchieveCompilerOnLine afOnLine )
* ACTION   : Achieve_Compiler_Pass
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Pass( const char * apData, const U32 aSize, fAchieveCompilerOnLine afOnLine )
{
	sThread					lThread;
	sThread					lThread2;
	sAchieveCompilerLine	lLine;
	U32						lOffset = 0;

	mTHREAD_INIT( &lThread2 );

	gAchieveCompiler.mLineIndex = 0;
	while( lOffset < aSize )
	{
		mTHREAD_INIT( &lThread );
		Achieve_Compiler_Line_Init( &lLine );
		while( (lOffset<aSize) && (apData[ lOffset ]) && (apData[ lOffset ]!= 10) && (apData[ lOffset ]!=13) )
		{
			Achieve_Compiler_Line_Consumer_Thread( &lThread, &lLine, apData[lOffset] );
			lOffset++;
		}
		gAchieveCompiler.mLineIndex++;

		if( lLine.mInTokenFlag )
		{
			lLine.mTokenSpace[ lLine.mTokenSpaceOffset ] = 0;
		}

		if( lLine.mTokenCount )
		{
			if( lLine.mpTokens[0][0] == '{' )	gAchieveCompiler.mBraceLevel++;
			if( lLine.mpTokens[0][0] == '}' )	gAchieveCompiler.mBraceLevel--;

			afOnLine( &lThread2, &lLine );
		}
		while( (lOffset<aSize) &&  ((apData[ lOffset ]== 10) || (apData[ lOffset ]==13)) )
		{
			lOffset++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Pass1_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_Pass1_Consumer_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Compiler_Pass1_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
{
	mTHREAD_BEGIN( apThread );

	/* search for block */

	mTHREAD_WAIT_UNTIL( apThread, Achieve_Compiler_GetBlockType(apLine) );
	mTHREAD_WAIT_UNTIL( apThread, apLine->mpTokens[0][0] == '{' );
	gAchieveCompiler.mBraceLevelCached = gAchieveCompiler.mBraceLevel;


	if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_STATS )
	{
		while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
		{
			if( apLine->mTokenCount >= 2 )
			{
				gAchieveCompiler.mStatCount++;
			}
			mTHREAD_YIELD( apThread );
		}
	}
	else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_TASKS )
	{
		while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
		{
			if( apLine->mTokenCount >= 2 )
			{
				gAchieveCompiler.mTaskCount++;
			}
			mTHREAD_YIELD( apThread );
		}
	}
	else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_SCORETABLE )
	{
		gAchieveCompiler.mScoreTableCount++;
		mTHREAD_YIELD( apThread );
	}

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Pass2_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_Pass2_Consumer_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Compiler_Pass2_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
{
	mTHREAD_BEGIN( apThread );

	/* search for block */

	mTHREAD_WAIT_UNTIL( apThread, Achieve_Compiler_GetBlockType(apLine) == eACH_COMPILER_BLOCK_SCORETABLE );

	/* search for open brackets */
	mTHREAD_WAIT_UNTIL( apThread, apLine->mpTokens[0][0] == '{' );
	gAchieveCompiler.mBraceLevelCached = gAchieveCompiler.mBraceLevel;
	mTHREAD_YIELD( apThread );

	while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
	{
		mTHREAD_WAIT_UNTIL( apThread, gAchieveCompiler.mBraceLevel == gAchieveCompiler.mBraceLevelCached );
		if( apLine->mTokenCount >= 2 )
		{
			if( !String_StrCmpi( apLine->mpTokens[ 0 ], "ENTRYCOUNT" ) )
			{
				gAchieveCompiler.mpScoreTableEntryCounts[ gAchieveCompiler.mScoreTableIndex ] = String_ToValue( apLine->mpTokens[ 1 ] );
			}
		}
		mTHREAD_YIELD( apThread );
	}
	gAchieveCompiler.mScoreTableIndex++;


	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Pass3_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_Pass3_Consumer_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Compiler_Pass3_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
{
	mTHREAD_BEGIN( apThread );

	/* search for block */

	mTHREAD_WAIT_UNTIL( apThread, Achieve_Compiler_GetBlockType(apLine) );

	/* search for open brackets */
	mTHREAD_WAIT_UNTIL( apThread, apLine->mpTokens[0][0] == '{' );
	gAchieveCompiler.mBraceLevelCached = gAchieveCompiler.mBraceLevel;

	if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_STATS )
	{
		while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
		{
			if( apLine->mTokenCount >= 2 )
			{
				U32	lType;
				sAchieveStatDef	lDef;

				lType = sTagString_GetID( apLine->mpTokens[0], gAchieveCompilerTypeStrings, eACH_COMPILER_TYPE_LIMIT );
				lDef.mpName = apLine->mpTokens[1];
				if( eACH_COMPILER_TYPE_TIME == lType )
				{
					lDef.mType = eACH_TYPE_TIME;
					Achieve_Stat_SetStatDef( gAchieveCompiler.mStatIndex, &lDef );
					gAchieveCompiler.mStatIndex++;
				}
				else if( eACH_COMPILER_TYPE_U32 == lType )
				{
					lDef.mType = eACH_TYPE_U32;
					Achieve_Stat_SetStatDef( gAchieveCompiler.mStatIndex, &lDef );
					gAchieveCompiler.mStatIndex++;
				}
			}
			mTHREAD_YIELD( apThread );
		}
	}
	else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_TASKS )
	{
		while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
		{
			if( apLine->mTokenCount >= 2 )
			{
				sAchieveTaskDef	lDef;

				lDef.mFlags = 0;
				lDef.mpName = apLine->mpTokens[0];
				lDef.mPoints = (U16)String_ToValue( apLine->mpTokens[1] );
				Achieve_Tasks_SetTaskDef( gAchieveCompiler.mTaskIndex, &lDef );
				gAchieveCompiler.mTaskIndex++;
			}
			mTHREAD_YIELD( apThread );
		}
	}
	else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_SCORETABLE )
	{
		while( gAchieveCompiler.mBraceLevel >= gAchieveCompiler.mBraceLevelCached )
		{
			if( apLine->mTokenCount >= 2 )
			{
				if( gAchieveCompiler.mBraceLevel > gAchieveCompiler.mBraceLevelCached )
				{
					String_StrCpy2( &gAchieveCompiler.mScoreTableDef.mpNames[ gAchieveCompiler.mScoreTableEntryIndex ].mName[ 0 ], apLine->mpTokens[ 0 ], 15 );
					gAchieveCompiler.mScoreTableDef.mpValues[ gAchieveCompiler.mScoreTableEntryIndex ] = String_ToValue( apLine->mpTokens[1] );
					gAchieveCompiler.mScoreTableEntryIndex++;
				}
				else
				{
					U32	lCmd;
					lCmd = sTagString_GetID( apLine->mpTokens[0], gAchieveCompilerScoreDefStrings, eACH_COMPILER_SCOREDEF_LIMIT );

					if( eACH_COMPILER_SCOREDEF_ENTRYCOUNT == lCmd )
					{
						gAchieveCompiler.mScoreTableDef.mEntryCount = (U16)String_ToValue( apLine->mpTokens[1] );
					}
					else if( eACH_COMPILER_SCOREDEF_SORT == lCmd )
					{
						U32	lSort;
						lSort = sTagString_GetID( apLine->mpTokens[1], gAchieveCompilerSortStrings, eACH_COMPILER_SORT_LIMIT );
						if( eACH_COMPILER_SORT_ASCENDING == lSort )
						{
							gAchieveCompiler.mScoreTableDef.mSortDirection = eACH_SORT_HI;
						}
						else if( eACH_COMPILER_SORT_DESCENDING == lSort )
						{
							gAchieveCompiler.mScoreTableDef.mSortDirection = eACH_SORT_LO;
						}
					}
					else if(eACH_COMPILER_SCOREDEF_TYPE == lCmd )
					{
						U32	lType;
						lType = sTagString_GetID( apLine->mpTokens[1], gAchieveCompilerTypeStrings, eACH_COMPILER_TYPE_LIMIT );
						if( eACH_COMPILER_TYPE_TIME == lType )
						{
							gAchieveCompiler.mScoreTableDef.mType = eACH_TYPE_TIME;
						}
						else if( eACH_COMPILER_TYPE_U32 == lType )
						{
							gAchieveCompiler.mScoreTableDef.mType = eACH_TYPE_U32;
						}
					}
					else if(eACH_COMPILER_SCOREDEF_TITLE == lCmd )
					{
						String_StrCpy2( &gAchieveCompiler.mScoreTableName[0], apLine->mpTokens[1], 1023 );
						gAchieveCompiler.mScoreTableDef.mpTableName = &gAchieveCompiler.mScoreTableName[0];
					}
				}
			}
			mTHREAD_YIELD( apThread );
		}
		Achieve_ScoreTable_SetScoreTableDef( gAchieveCompiler.mScoreTableIndex, &gAchieveCompiler.mScoreTableDef );
		gAchieveCompiler.mScoreTableIndex++;
		gAchieveCompiler.mScoreTableEntryIndex = 0;
	}

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Line_Init( sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_Line_Init
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_Line_Init( sAchieveCompilerLine * apLine )
{
	memset( apLine, 0, sizeof(sAchieveCompilerLine) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_Line_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine, U8 aChar )
* ACTION   : Achieve_Compiler_Line_Consumer_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Compiler_Line_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine, U8 aChar )
{
	mTHREAD_BEGIN( apThread );

	/*skip whitespace*/
	apLine->mInTokenFlag = 0;
	mTHREAD_WAIT_UNTIL( apThread, (aChar>' ') );

	apLine->mpTokens[ apLine->mTokenCount ] = &apLine->mTokenSpace[ apLine->mTokenSpaceOffset ];
	apLine->mTokenCount++;

	if( ('"' == aChar) || ('\'' == aChar) )
	{
		apLine->mTerminator = aChar;
		mTHREAD_YIELD( apThread )
		while( apLine->mTerminator != aChar )
		{
			apLine->mInTokenFlag = 1;
			apLine->mTokenSpace[ apLine->mTokenSpaceOffset ] = aChar;
			apLine->mTokenSpaceOffset++;
			mTHREAD_YIELD( apThread );
		}
	}
	else
	{
		while( aChar > ' ' )
		{
			apLine->mInTokenFlag = 1;
			apLine->mTokenSpace[ apLine->mTokenSpaceOffset ] = aChar;
			apLine->mTokenSpaceOffset++;
			mTHREAD_YIELD( apThread );
		}
	}
	apLine->mTokenSpace[ apLine->mTokenSpaceOffset ] = 0;
	apLine->mTokenSpaceOffset++;

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_GetBlockType( sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_GetBlockType
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Compiler_GetBlockType( sAchieveCompilerLine * apLine )
{
	gAchieveCompiler.mBlockType = 0;
	if( apLine->mTokenCount )
	{
		gAchieveCompiler.mBlockType = (U16)sTagString_GetID( apLine->mpTokens[0], gAchieveCompilerBlockStrings, eACH_COMPILER_BLOCK_LIMIT );
	}

	return( gAchieveCompiler.mBlockType );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_PassSyntax_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
* ACTION   : Achieve_Compiler_PassSyntax_Consumer_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Compiler_PassSyntax_Consumer_Thread( sThread * apThread, sAchieveCompilerLine * apLine )
{
	mTHREAD_BEGIN( apThread );

	/* find block header */

	do
	{
		Achieve_Compiler_GetBlockType( apLine );
		if( ( apLine->mpTokens[0][0] == '[' ) && (!gAchieveCompiler.mBlockType) )
		{
			Achieve_Compiler_LineError( "UNKNOWN BLOCK TYPE ", apLine->mpTokens[ 0 ] );
		}
		if( apLine->mpTokens[0][0] == '{' )
		{
			Achieve_Compiler_LineError( "OPENING BRACE WITHOUT MATCHING BLOCK", 0 );
		}
		mTHREAD_YIELD( apThread );
	}while( !gAchieveCompiler.mBlockType );

	while( !gAchieveCompiler.mBraceLevel )
	{
		if( apLine->mTokenCount )
		{
			Achieve_Compiler_LineError( "OPEN BRACE EXPECTED AFTER BLOCK HEADER", apLine->mpTokens[0] );
		}
		mTHREAD_YIELD( apThread );
	}
	if( apLine->mpTokens[0][0] == '{' )
	{
		mTHREAD_YIELD( apThread );
	}

	while( gAchieveCompiler.mBraceLevel )
	{
		if( apLine->mpTokens[0][0] == '}' )
		{
			if( apLine->mTokenCount > 2 )
			{
				Achieve_Compiler_LineError( "TOO MANY TOKENS (BRACE SHOULD BE ON ITS OWN LINE)", 0 );
			}
		}
		else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_STATS )
		{
			if( apLine->mTokenCount > 2 )
			{
				Achieve_Compiler_LineError( "TOO MANY TOKENS (DO YOU NEED QUOTEMARKS?)", 0 );
			}
			if( apLine->mTokenCount == 1 )
			{
				Achieve_Compiler_LineError( "TOO FEW TOKENS (SHOULD BE: TYPE STATNAME)", apLine->mpTokens[0] );
			}

			if( apLine->mTokenCount >= 1 )
			{
				if( 0 == sTagString_GetID( apLine->mpTokens[0], gAchieveCompilerTypeStrings, eACH_COMPILER_TYPE_LIMIT ) )
				{
					Achieve_Compiler_LineError( "UNKOWN STAT TYPE ", apLine->mpTokens[ 0 ] );
				}
			}
		}
		else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_TASKS )
		{
			if( apLine->mTokenCount > 2 )
			{
				Achieve_Compiler_LineError( "TOO MANY TOKENS (DO YOU NEED QUOTEMARKS?)", 0 );
			}
			if( apLine->mTokenCount == 1 )
			{
				Achieve_Compiler_LineError( "TOO FEW TOKENS (SHOULD BE: TASKNAME POINTS)", apLine->mpTokens[0] );
			}
			if( apLine->mTokenCount >= 2 )
			{
				S32	lValue;

				lValue = String_ToValue( apLine->mpTokens[ 1 ] );
				if( (lValue <= 0) || (lValue > 1024) )
				{
					Achieve_Compiler_LineError( "TASK VALUE OUT OF RANGE", apLine->mpTokens[1] );
				}
			}
		}
		else if( gAchieveCompiler.mBlockType == eACH_COMPILER_BLOCK_SCORETABLE )
		{
			if( apLine->mTokenCount > 2 )
			{
				Achieve_Compiler_LineError( "TOO MANY TOKENS (DO YOU NEED QUOTEMARKS?)", 0 );
			}
			if( apLine->mTokenCount == 1 )
			{
				if( String_StrCmpi("[entries]", apLine->mpTokens[0]) )
				{
					if( '{' == apLine->mpTokens[0][0] )
					{
						if( gAchieveCompiler.mBraceLevel > 2 )
						{
							Achieve_Compiler_LineError( "TOO MANY OPEN BRACKETS", 0 );
						}
					}
					else
					{
						Achieve_Compiler_LineError( "TOO FEW TOKENS (SHOULD BE: TYPE STATNAME)", apLine->mpTokens[0] );
					}
				}
			}

			if( (gAchieveCompiler.mBraceLevel == 1) && ( apLine->mTokenCount >= 2 ) )
			{
				U32	lCmd;
				lCmd = sTagString_GetID( apLine->mpTokens[0], gAchieveCompilerScoreDefStrings, eACH_COMPILER_SCOREDEF_LIMIT );

				if( eACH_COMPILER_SCOREDEF_UNKNOWN == lCmd )
				{
					Achieve_Compiler_LineError( "UNKOWN SCOREDEF TYPE ", apLine->mpTokens[ 0 ] );
				}
				else if( eACH_COMPILER_SCOREDEF_SORT == lCmd )
				{
					if( 0 == sTagString_GetID( apLine->mpTokens[1], gAchieveCompilerSortStrings, eACH_COMPILER_SORT_LIMIT ) )
					{
						Achieve_Compiler_LineError( "UNKOWN SCORE SORT TYPE ", apLine->mpTokens[ 1 ] );
					}
				}
				else if( eACH_COMPILER_SCOREDEF_TYPE == lCmd )
				{
					if( 0 == sTagString_GetID( apLine->mpTokens[1], gAchieveCompilerTypeStrings, eACH_COMPILER_TYPE_LIMIT ) )
					{
						Achieve_Compiler_LineError( "UNKOWN SCORE TYPE ", apLine->mpTokens[ 1 ] );
					}
				}
			}
		}
		mTHREAD_YIELD( apThread );
	}

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Compiler_LineError( char * apString, char * apString2 )
* ACTION   : Achieve_Compiler_LineError
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Compiler_LineError( char * apString, char * apString2 )
{
	char	lString[ 4096 ];

	gAchieveCompiler.mBraceLevel = 0;

	sprintf( lString, "ERROR @ LINE %d: ", gAchieveCompiler.mLineIndex );

	File_Write( gAchieveCompiler.mFileHandle, String_StrLen(lString), lString );
	printf( lString );

	if( apString )
	{
		File_Write( gAchieveCompiler.mFileHandle, String_StrLen(apString), apString );
		printf( apString );
	}
	if( apString2 )
	{
		File_Write( gAchieveCompiler.mFileHandle, String_StrLen(apString2), apString2 );
		printf( apString2 );
	}
	File_Write( gAchieveCompiler.mFileHandle, 2, "\r\n" );
	printf( "\n" );
	gAchieveCompiler.mErrorCount++;
}


/* ################################################################################ */
