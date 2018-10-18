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

typedef struct sStringCaseTester
{
	const char * mpCase0;
	const char * mpCase1;
}sStringCaseTester;

sStringCaseTester gStringCaseTesters[]=
{
	{ "A", "a" },
	{ "z", "Z" },
	{ "ThIS", "thiS" },
	{ "facLoN", "FACLOn" },
};

GOD_UNIT_TEST( String )
{
	U16 i;
	sStringPath	lPath0;
	char lBigString[ sizeof( sStringPath ) + 64 ];

	/*
	STRING PATH
	*/

	for( i = 0; i < sizeof( lBigString ) - 1; i++ )
		lBigString[ i ] = 'a';
	lBigString[ sizeof( lBigString ) - 1 ] = 0;

	/* check that setNT stays in bounds*/
	{
		GOD_UNIT_TEST_GUARDED_STRUCT( sStringPath, lPathG );
		{
			sStringPath * lpS = GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( sStringPath, lPathG );
			StringPath_SetNT( lpS, lBigString );
			GOD_UNIT_TEST_EXPECT( 0 == *StringPath_End( lpS ), "set not null terminating" );
		}
		GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( sStringPath, lPathG );
	}

	/* check that setExt stays in bounds*/
	{
		GOD_UNIT_TEST_GUARDED_STRUCT( sStringPath, lPathG );
		{
			sStringPath * lpS = GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( sStringPath, lPathG );
			StringPath_SetNT( lpS, "b.txt" );
			StringPath_SetExt( lpS, lBigString );
			GOD_UNIT_TEST_EXPECT( 0 == *StringPath_End( lpS ), "set not null terminating" );
		}
		GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( sStringPath, lPathG );
	}

	/* check that append stays in bounds*/
	{
		GOD_UNIT_TEST_GUARDED_STRUCT( sStringPath, lPathG );
		{
			sStringPath * lpS = GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( sStringPath, lPathG );
			StringPath_SetNT( lpS, "b.txt" );
			StringPath_Append( lpS, lBigString );
			GOD_UNIT_TEST_EXPECT( 0 == *StringPath_End( lpS ), "set not null terminating" );
		}
		GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( sStringPath, lPathG );
	}

	/* check that setfilname stays in bounds*/
	{
		GOD_UNIT_TEST_GUARDED_STRUCT( sStringPath, lPathG );
		{
			sStringPath * lpS = GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( sStringPath, lPathG );
			StringPath_SetNT( lpS, "b.txt" );
			StringPath_SetFileName( lpS, lBigString );
			GOD_UNIT_TEST_EXPECT( 0 == *StringPath_End( lpS ), "set not null terminating" );
		}
		GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( sStringPath, lPathG );
	}

	/* check that combine stays in bounds*/
	{
		GOD_UNIT_TEST_GUARDED_STRUCT( sStringPath, lPathG );
		{
			sStringPath * lpS = GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( sStringPath, lPathG );
			StringPath_Combine( lpS, lBigString, lBigString );
			GOD_UNIT_TEST_EXPECT( 0 == *StringPath_End( lpS ), "set not null terminating" );
		}
		GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( sStringPath, lPathG );
	}

	for( i = 0; i < mARRAY_COUNT( gStrPathTestDefs ); i++ )
	{
		sStringPath	lDir;
		sStringPath lFolder;
		sStrPathTestDef * lpDef = &gStrPathTestDefs[ i ];
		const char * lpExt;
		const char * lpFileName;
		char lDrive;

		StringPath_SetNT( &lPath0, lpDef->mpPath );
		lpExt		= StringPath_GetpExt( lPath0.mChars );
		lpFileName	= StringPath_GetpFileName( lPath0.mChars );
		lDrive		= StringPath_GetDrive( lPath0.mChars );
		StringPath_GetFolder( &lFolder, lPath0.mChars );
		StringPath_GetDirectory( &lDir, lPath0.mChars );
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

		StringPath_SetNT( &lPath0, lpCom->mpPathLong );
		StringPath_Compact( &lShort, lPath0.mChars );
/*		printf( "compact: %s [%s]\n", lShort.mChars, lpCom->mpPathShort );*/
		GOD_UNIT_TEST_EXPECT( 0 == String_StrCmp( lpCom->mpPathShort, lShort.mChars ), "couldn't compact path" );

		GODLIB_ASSERT( 0 == String_StrCmp( lpCom->mpPathShort, lShort.mChars ) );
		StringPath_SetNT( &lPath0, lpCom->mpPathLong );
		StringPath_Compact( &lShort, lPath0.mChars );
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
		U32 len,len0,len1;


		for (len = 0; gStringCatTesters[i].mpCat[len]; len++);
		for (len0 = 0; gStringCatTesters[i].mpStr0[len0]; len0++);
		for (len1 = 0; gStringCatTesters[i].mpStr1[len1]; len1++);

		String_Init(&lGodCopy, 0);

		/* append */

		String_Init(&lGodString, gStringCatTesters[i].mpStr0);

		String_Append( &lGodString, gStringCatTesters[i].mpStr1);
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


		/* Set */

		String_Init(&lGodString, 0);

		String_Set(&lGodString, gStringCatTesters[i].mpStr0);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpStr0), "set");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len0), "set string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for set");

		String_Set(&lGodString, gStringCatTesters[i].mpStr1);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpStr1), "set");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len1), "set string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for set");

		String_Set2(&lGodString, gStringCatTesters[i].mpStr0, gStringCatTesters[i].mpStr1);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpCat), "set2");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len), "set2 string length fail");
		GOD_UNIT_TEST_EXPECT((String_IsDynamic(&lGodString)), "Dynamic Flag not set for set2");

		String_DeInit(&lGodString);

		String_SetStatic(&lGodString, gStringCatTesters[i].mpStr0, len0);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpStr0), "setstatic");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len0), "setstatic string length fail");
		GOD_UNIT_TEST_EXPECT((!String_IsDynamic(&lGodString)), "Dynamic Flag set for setstatic");
		GOD_UNIT_TEST_EXPECT((gStringCatTesters[i].mpStr0 == lGodString.mpChars), "setstaic not using same pointer");

		String_SetStatic(&lGodString, gStringCatTesters[i].mpStr1, len1);
		GOD_UNIT_TEST_EXPECT(0 == String_StrCmp(lGodString.mpChars, gStringCatTesters[i].mpStr1), "setstatic");
		GOD_UNIT_TEST_EXPECT((String_GetLength(&lGodString) == len1), "setstatic string length fail");
		GOD_UNIT_TEST_EXPECT((!String_IsDynamic(&lGodString)), "Dynamic Flag set for setstatic");
		GOD_UNIT_TEST_EXPECT((gStringCatTesters[i].mpStr1 == lGodString.mpChars), "setstaic not using same pointer");


		String_DeInit(&lGodCopy);
	}

	for (i = 0; i < mARRAY_COUNT(gStringCaseTesters); i++)
	{
		sString lGodString0;
		sString lGodString1;
		sString lGodStringCopy;
		sString lGodStringFix;
		U32 len0;

		for (len0 = 0; gStringCaseTesters[i].mpCase0[len0]; len0++);

		String_Init(&lGodString0, gStringCaseTesters[i].mpCase0);
		String_Init(&lGodString1, gStringCaseTesters[i].mpCase1);
		String_Init(&lGodStringCopy, 0);
		String_Init(&lGodStringFix, 0);

		GOD_UNIT_TEST_EXPECT((0 != String_StrCmp(gStringCaseTesters[i].mpCase0, gStringCaseTesters[i].mpCase1)), "String_StrCmp fail");
		GOD_UNIT_TEST_EXPECT((0 != String_StrCmp(gStringCaseTesters[i].mpCase0, lGodString1.mpChars)), "String_StrCmp fail");

		GOD_UNIT_TEST_EXPECT((0 == String_StrCmpi(gStringCaseTesters[i].mpCase0, gStringCaseTesters[i].mpCase1)), "String_StrCmpi fail");
		GOD_UNIT_TEST_EXPECT((0 == String_StrCmpi(gStringCaseTesters[i].mpCase0, lGodString0.mpChars)), "String_StrCmpi fail");
		GOD_UNIT_TEST_EXPECT((0 == String_StrCmpi(gStringCaseTesters[i].mpCase1, lGodString1.mpChars)), "String_StrCmpi fail");

		GOD_UNIT_TEST_EXPECT((0 == String_IsEqual(&lGodString0, &lGodString1)), "string is equal fail");
		GOD_UNIT_TEST_EXPECT((0 == String_IsEqual(0, &lGodString1)), "string is equal fail");
		GOD_UNIT_TEST_EXPECT((0 == String_IsEqual(&lGodString0, 0)), "string is equal fail");

		String_Copy(&lGodStringCopy, &lGodString0);
		String_SetStatic(&lGodStringFix, gStringCaseTesters[i].mpCase0, len0);
		GOD_UNIT_TEST_EXPECT((String_IsEqual(&lGodString0, &lGodStringCopy)), "string is equal fail");
		GOD_UNIT_TEST_EXPECT((String_IsEqual(&lGodString0, &lGodStringFix)), "string is equal fail");
		GOD_UNIT_TEST_EXPECT((!String_IsEqual(&lGodString1, &lGodStringFix)), "string is equal fail");

		String_DeInit(&lGodStringFix);
		String_DeInit(&lGodStringCopy);
		String_DeInit(&lGodString1);
		String_DeInit(&lGodString0);
	}
}