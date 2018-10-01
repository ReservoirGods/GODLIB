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

static const char *	gpStrings[] =
{
	"",
	"SOME STRING",
	"DO",
	"1",
	"     ",
};

typedef struct sStrCatTester
{
	const char * mpStr0;
	const char * mpStr1;
	const char * mpCat;
}sStrCatTester;

static sStrCatTester gStringCatTesters[]=
{
	{ "", "", "" },
	{ "A", "", "A" },
	{ "", "Z", "Z" },
	{ "one", "", "one" },
	{ "", "behind", "behind" },
	{ "get", "together", "gettogether" },
	{ "general", "big", "generalbig" },
};

GOD_UNIT_TEST( String )
{
	U16 i;
	sStringPath	lPath0;

	/*
	STRING PATH
	*/

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
/*		printf( "compact: %s [%s]\n", lShort.mChars, lpCom->mpPathShort );*/
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpCom->mpPathShort, lShort.mChars ), "couldn't compact path" );
	}

	/*
	STRING
	*/

	for (i = 0; i < mARRAY_COUNT(gpStrings); i++)
	{
		sString lGodString;
		U32 len;
		const char * lpString = gpStrings[i];
		for (len = 0; lpString[len]; len++);

		String_Init( &lGodString, lpString);

		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag Set Incorrectly");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "string length fail");

		String_DeInit(&lGodString );

		GOD_UNIT_TEST_EXPECT((!String_IsDynamic(&lGodString)), "Dynamic Flag not cleared");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == 0), "string length failed to clear");
	}

	for (i = 0; i < mARRAY_COUNT(gStringCatTesters); i++)
	{
		sString lGodString;
		sString lGodStringA;
		sString lGodStringB;
		sString lGodCopy;
		U32 len;

		String_Init(&lGodCopy, 0);

		/* append */

		String_Init(&lGodString, gStringCatTesters[i].mpStr0);

		String_Append( &lGodString, gStringCatTesters[i].mpStr1);
		for (len = 0; gStringCatTesters[i].mpCat[len]; len++);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "Append Fail");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "append string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for append");

		String_Copy(&lGodCopy, &lGodString);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodCopy.mpChars, lGodString.mpChars), "Copy Fail");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodCopy) == len), "copy string length fail");
		String_DeInit(&lGodString);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodCopy.mpChars, gStringCatTesters[i].mpCat), "Copy after deinit Fail");

		/* cat = basic */

		String_Init(&lGodString, 0);
		String_Init(&lGodStringA, gStringCatTesters[i].mpStr0);
		String_Init(&lGodStringB, gStringCatTesters[i].mpStr1);

		String_Cat(&lGodString, &lGodStringA, &lGodStringB );
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "strcat Fail");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "cat string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for cat");

		String_Copy(&lGodCopy, &lGodString);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodCopy.mpChars, lGodString.mpChars), "Copy Fail");

		String_DeInit(&lGodStringB);
		String_DeInit(&lGodStringA);
		String_DeInit(&lGodString);

		/* cat = alias first arg */

		String_Init(&lGodString, gStringCatTesters[i].mpStr0);
		String_Init(&lGodStringB, gStringCatTesters[i].mpStr1);

		String_Cat(&lGodString, &lGodString, &lGodStringB);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "strcat with resued arg 1 Fail");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "cat string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for cat");

		String_DeInit(&lGodStringB);
		String_DeInit(&lGodString);

		/* cat = alias second arg */

		String_Init(&lGodString, gStringCatTesters[i].mpStr1);
		String_Init(&lGodStringA, gStringCatTesters[i].mpStr0);

		String_Cat(&lGodString, &lGodStringA, &lGodString);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "strcat with reused arg 2 Fail");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "cat string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for cat");

		String_DeInit(&lGodStringA);
		String_DeInit(&lGodString);

		/* prepend */


		String_Init(&lGodString, gStringCatTesters[i].mpStr1);
		String_Prepend(&lGodString, gStringCatTesters[i].mpStr0);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "prepend");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "prepend string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for prepend");
		String_DeInit(&lGodString);

		String_DeInit(&lGodCopy);

	}
}