#include	<GODLIB\UNITTEST\UNITTEST.H>

#include	<GODLIB\STRING\STRING.H>
#include	<GODLIB\STRING\STRPATH.H>


typedef struct sStrPathTestDef
{
	const char * mpPath;
	const char * mpDir;
	const char * mpFolder;
	const char * mpFileName;
	const char * mpExt;
	char		mDrive;
	U16			mAbsolute;
} sStrPathTestDef;

sStrPathTestDef	gStrPathTestDefs[] =
{
	{ "J:\\ROOT.FILE",			"J:",			"",			"ROOT.FILE", ".FILE", 'J', 1 },
	{ "C:\\ATARI\\COOL.TXT",	"C:\\ATARI",	"ATARI",	"COOL.TXT", ".TXT", 'C', 1 },
	{ "C:/ATARI/COOL.TXT",		"C:/ATARI",		"ATARI",	"COOL.TXT", ".TXT", 'C', 1 },
	{ "RELATIVE/SUB/FILE.YEH",	"RELATIVE/SUB", "SUB",		"FILE.YEH", ".YEH", 0, 0 },
	{ "/RELATIVE/SUB/FILE.YEH",	"/RELATIVE/SUB", "SUB",		"FILE.YEH", ".YEH", 0, 0 },
	{ "LOTS\\OF/FOLDERS\\IN\\THIS\\ONE.PNG",	"LOTS\\OF/FOLDERS\\IN\\THIS", "THIS",		"ONE.PNG", ".PNG", 0, 0 },
};

typedef struct sStrPathTestCompact
{
	const char * mpPathLong;
	const char * mpPathShort;
} sStrPathTestCompact;

sStrPathTestCompact gStrPathCompacts[] =
{
	{ "D:\\IGNORE\\THESE\\..\\..", "D:" },
	{ "C:\\ATARI\\..", "C:" },
	{ "D:\\ILOVEMY\\ST\\..", "D:\\ILOVEMY" },
	{ "D:\\AROUND\\AND\\..\\BACK\\AGAIN", "D:\\AROUND\\BACK\\AGAIN" },

	{ "\\..", "" },
	{ "\\", "" },
	{ "\\..\\..", "" },
	{ "/BOO/..", "" },
	{ "/CATS/IN/../../WHO/LET/../DOGS/OUT", "/WHO/DOGS/OUT" },

};

GOD_UNIT_TEST( String )
{
	U16 i;
	sStringPath	lPath0;

	for( i = 0; i < mARRAY_COUNT( gStrPathTestDefs ); i++ )
	{
		sStringPath	lDir;
		sStringPath lFolder;
		sStrPathTestDef * lpDef = &gStrPathTestDefs[ i ];
		const char * lpExt;
		const char * lpFileName;
		char lDrive;

		StringPath_Set( &lPath0, lpDef->mpPath );
		lpExt		= StringPath_GetpExt( &lPath0 );
		lpFileName	= StringPath_GetpFileName( &lPath0 );
		lDrive		= StringPath_GetDrive( &lPath0 );
		StringPath_GetFolder( &lFolder, &lPath0 );
		StringPath_GetDirectory( &lDir, &lPath0 );
/*		printf( "dir: %s [%s]\n", lDir.mChars, lpDef->mpDir );*/
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpDef->mpDir, lDir.mChars ), "GetDir failure" );
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpDef->mpFolder, lFolder.mChars ), "GetFolder failure" );
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpDef->mpFileName, lpFileName ), "GetFilename failure" );
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpDef->mpExt, lpExt ), "GetExtension failure" );
	}

	for( i = 0; i < mARRAY_COUNT( gStrPathCompacts ); i++ )
	{
		sStrPathTestCompact * lpCom = &gStrPathCompacts[ i ];
		sStringPath lShort;

		StringPath_Set( &lPath0, lpCom->mpPathLong );
		StringPath_Compact( &lShort, &lPath0 );
		printf( "compact: %s [%s]\n", lShort.mChars, lpCom->mpPathShort );
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpCom->mpPathShort, lShort.mChars ), "couldn't compact path" );
	}
}