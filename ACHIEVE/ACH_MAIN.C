/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_MAIN.H"

#include	"ACH_LOGN.H"
#include	"ACH_SIGN.H"
#include	"ACH_GFX.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/CHECKSUM/CHECKSUM.H>
#include	<GODLIB/ENCRYPT/ENCRYPT.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/RANDOM/RANDOM.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACHIEVE_ID			mSTRING_TO_U32( 'A', 'C', 'H', 'V' )
#define	dACHIEVE_VERSION	0x0006

#define	mACH_DELOC( _a )	{if(apHeader->_a){ (*(U32*)(&apHeader->_a) -= (U32)apHeader); Endian_FromBigU32( &apHeader->_a );  }}
#define	mACH_RELOC( _a )	{if(apHeader->_a){ Endian_FromBigU32( &apHeader->_a ); *(U32*)&apHeader->_a += (U32)apHeader; 	}}
#define	mACH_DELOC2( _a )	{if(_a){ (*(U32*)(&_a) -= (U32)apHeader); Endian_FromBigU32( &_a );  }}
#define	mACH_RELOC2( _a )	{if(_a){ Endian_FromBigU32( &_a ); *(U32*)&_a += (U32)apHeader; 	}}


#define	dACH_GetUserCount				(gAchieveMainClass.mpHeader->mUserCount)
#define	dACH_GetpUser					(gAchieveMainClass.mpUser)

#define	dACH_GetTaskCount				(gAchieveMainClass.mpHeader->mGameInfo.mTaskCount)
#define	dACH_GetTaskUnlockFlag( _a )	(dACH_GetpUser->mpTaskValues[ (_a>>3) ] & (1<<(_a&7)) )
#define	dACH_SetTaskUnlockFlag( _a )	(dACH_GetpUser->mpTaskValues[ (_a>>3) ] |= (1<<(_a&7)) )


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveMainClass
{
	U16					mCurrentUserIndex;
	U32					mStringSizeCurrent;
	U32					mDataSizeCurrent;
	U32					mBufferOffset;
	U32					mBufferSize;
	U8 *				mpBufferBase;
	sAchieveUser *		mpUser;
	sAchieveHeader *	mpHeader;
}sAchieveMainClass;


/* ###################################################################################
#  DATA
################################################################################### */

sAchieveMainClass	gAchieveMainClass;
char	gAchieveAppendChars[] =
{
	'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Specs_Set( sAchieveSpecs * apSecs );

S16					Achieve_ScoreTable_SetNewScore_Internal( sAchieveHeader * apHeader, const U16 aTableIndex, const S32 aValue, const sAchieveUserName * apUserName );
sAchieveHeader *	Achieve_Serialise( sAchieveHeader * apHeader, U32 * apSize );
U8					Achieve_FileNameExists( const char * apFileName );
void				Achieve_FileNameBuild( void );
U32					Achieve_CheckSumBuild( sAchieveHeader * apHeader );
char *				Achieve_StringAdd( const char * apString );
void				Achieve_StringRemove( const char * apString );
void *				Achieve_DataAdd( const U32 aSize );
void				Achieve_DataRemove( void * apData );
void				Achieve_SetUser( const U16 aIndex );
void				Achieve_UserCreate( const char * apName );
S32					Achieve_StrLen( const char * apString );
void				Achieve_StrCpy( char * apDst, const char * apSrc, const U32 aDstLen );
U8					Achieve_StrCmp( const char * apStr0, const char * apStr1 );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_AppInit( void )
* ACTION   : Achieve_AppInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_AppInit( void )
{
	U16	i;
	U8 *	lpMem;

	lpMem = (U8*)&gAchieveMainClass;
	for(i =0; i<sizeof(sAchieveMainClass); i++ )
	{
		lpMem[i] = 0;
	}
	Achieve_Login_AppInit();
	Achieve_Gfx_AppInit();
/*	Achieve_Show_AppInit();
	Achieve_UnLocked_AppInit();*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_AppDeInit( void )
* ACTION   : Achieve_AppDeInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_AppDeInit( void )
{
/*	Achieve_Show_AppDeInit();
	Achieve_UnLocked_AppDeInit();*/
	Achieve_Gfx_AppDeInit();
	Achieve_Login_AppDeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Create( const U16 aScoreTableCount, const U16 aStatCount, const U16 aTaskCount, void * apBuffer, const U32 aBufferSize )
* ACTION   : Achieve_Create
* CREATION : 4.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Create( const U16 aScoreTableCount, const U16 aStatCount, const U16 aTaskCount, void * apBuffer, const U32 aBufferSize )
{
	sAchieveHeader * lpH;

	gAchieveMainClass.mBufferSize = aBufferSize;
	gAchieveMainClass.mBufferOffset = 0;
	gAchieveMainClass.mDataSizeCurrent = 0;
	gAchieveMainClass.mStringSizeCurrent = 0;
	gAchieveMainClass.mpBufferBase = (U8*)apBuffer;

	lpH = (sAchieveHeader*)Achieve_DataAdd( sizeof(sAchieveHeader) );
	gAchieveMainClass.mpHeader = lpH;
	lpH->mID          = dACHIEVE_ID;
	lpH->mVersion     = dACHIEVE_VERSION;
	lpH->mSaveID      = Random_Get();
	lpH->mSaveCounter = 0;
	lpH->mGameInfo.mStatCount   = aStatCount;
	lpH->mGameInfo.mTaskCount   = aTaskCount;
	lpH->mGameInfo.mScoreTableCount = aScoreTableCount;

	lpH->mGameInfo.mpStatDefs       = (sAchieveStatDef*)Achieve_DataAdd( sizeof(sAchieveStatDef) * aStatCount  );
	lpH->mGameInfo.mpScoreTableDefs = (sAchieveScoreTableDef*)Achieve_DataAdd( sizeof(sAchieveScoreTableDef) * aScoreTableCount );
	lpH->mGameInfo.mpTaskDefs       = (sAchieveTaskDef*)Achieve_DataAdd( sizeof(sAchieveTaskDef) * aTaskCount );

	lpH->mUserCount = 0;
	lpH->mUserInfo.mpUserNext = 0;
	gAchieveMainClass.mpUser = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Destroy( void )
* ACTION   : Achieve_Destroy
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Destroy( void )
{
	sAchieveHeader *	lpH;

	lpH = gAchieveMainClass.mpHeader;

	Achieve_DataRemove( lpH->mGameInfo.mpStatDefs );
	lpH->mGameInfo.mpStatDefs = 0;
	Achieve_DataRemove( lpH->mGameInfo.mpScoreTableDefs );
	lpH->mGameInfo.mpScoreTableDefs = 0;
	Achieve_DataRemove( gAchieveMainClass.mpHeader );
	gAchieveMainClass.mpHeader = 0;
	gAchieveMainClass.mBufferSize = 0;
	gAchieveMainClass.mpBufferBase = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SetGameInfo( const char * apGameName, const U32 aGameID, const U32 aBuildID )
* ACTION   : Achieve_SetGameInfo
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SetGameInfo( const char * apGameName, const U32 aGameID, const U32 aBuildID )
{
	sAchieveHeader *	lpH;

	lpH = gAchieveMainClass.mpHeader;
	lpH->mGameInfo.mpName = Achieve_StringAdd( apGameName );
	lpH->mGameInfo.mBuildID = aBuildID;
	lpH->mGameInfo.mGameID  = aGameID;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GetUserCount( void )
* ACTION   : Achieve_GetUserCount
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_GetUserCount( void )
{
	U16	lRes = 0;
	if( gAchieveMainClass.mpHeader )
	{
		lRes = gAchieveMainClass.mpHeader->mUserCount;
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GetUserName( const U16 aIndex )
* ACTION   : Achieve_GetUserName
* CREATION : 18.1.2008 PNK
*-----------------------------------------------------------------------------------*/

const char *	Achieve_GetUserName( const U16 aIndex )
{
	U16	i;
	sAchieveUser *	lpUser;

	Assert( aIndex < dACH_GetUserCount );

	i      = 0;
	lpUser = &gAchieveMainClass.mpHeader->mUserInfo;
	while( (i<aIndex) )
	{
		lpUser = lpUser->mpUserNext;
		i++;
	}

	return( lpUser->mUserName.mName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GetUserIndex( const char * apUserName )
* ACTION   : Achieve_GetUserIndex
* CREATION : 16.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_GetUserIndex( const char * apUserName )
{
	sAchieveUser *	lpUser;
	U16	lIndex;

	lpUser = &gAchieveMainClass.mpHeader->mUserInfo;
	for( lIndex=0; lIndex<dACH_GetUserCount; lIndex++ )
	{
		if( !Achieve_StrCmp( lpUser->mUserName.mName, apUserName ) )
		{
			return( lIndex );
		}
		lpUser = lpUser->mpUserNext;
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SetUserInfo( const char * apUserName )
* ACTION   : Achieve_SetUserInfo
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SetUserInfo( const char * apUserName )
{
	sAchieveUser *	lpUser;
	U16					i;

	if( gAchieveMainClass.mpHeader )
	{
		lpUser = &gAchieveMainClass.mpHeader->mUserInfo;
		for( i=0; i<dACH_GetUserCount; i++ )
		{
			if( !Achieve_StrCmp( apUserName, lpUser->mUserName.mName ) )
			{
				Achieve_SetUser( i );
				return;
			}
			lpUser = lpUser->mpUserNext;
		}

		Achieve_UserCreate( apUserName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_SetStatDef( const U16 aStatIndex, const sAchieveStatDef * apStat )
* ACTION   : Achieve_Stat_SetStatDef
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Stat_SetStatDef( const U16 aStatIndex, const sAchieveStatDef * apStat )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aStatIndex < lpH->mGameInfo.mStatCount );
	lpH->mGameInfo.mpStatDefs[ aStatIndex ].mType  = apStat->mType;
	lpH->mGameInfo.mpStatDefs[ aStatIndex ].mpName = Achieve_StringAdd( apStat->mpName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_Add( const U16 aStatIndex, const U32 aStatValue )
* ACTION   : Achieve_Stat_Add
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Stat_Add( const U16 aStatIndex, const U32 aStatValue )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aStatIndex < lpH->mGameInfo.mStatCount );
	dACH_GetpUser->mpStatValues[ aStatIndex ] += aStatValue;
	lpH->mCheckSum = Achieve_CheckSumBuild( lpH );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_Update( const U16 aStatIndex, const U32 aStatValue )
* ACTION   : Achieve_Stat_Update
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Stat_Update( const U16 aStatIndex, const U32 aStatValue )
{
	sAchieveHeader *	lpH;

	if( dACH_GetpUser )
	{
		lpH    = gAchieveMainClass.mpHeader;
		Assert( aStatIndex < lpH->mGameInfo.mStatCount );
		dACH_GetpUser->mpStatValues[ aStatIndex ] = aStatValue;
		lpH->mCheckSum = Achieve_CheckSumBuild( lpH );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_Max( const U16 aStatIndex, const U32 aStatValue )
* ACTION   : Achieve_Stat_Max
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Stat_Max( const U16 aStatIndex, const U32 aStatValue )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aStatIndex < lpH->mGameInfo.mStatCount );
	if( aStatValue > dACH_GetpUser->mpStatValues[ aStatIndex ] )
	{
		dACH_GetpUser->mpStatValues[ aStatIndex ] = aStatValue;
		lpH->mCheckSum = Achieve_CheckSumBuild( lpH );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_Get( const U16 aStatIndex )
* ACTION   : Achieve_Stat_Get
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U32	Achieve_Stat_Get( const U16 aStatIndex )
{
	Assert( aStatIndex < gAchieveMainClass.mpHeader->mGameInfo.mStatCount );
	return( dACH_GetpUser->mpStatValues[ aStatIndex ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_GetCount( void )
* ACTION   : Achieve_Stat_GetCount
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Stat_GetCount( void )
{
	return( gAchieveMainClass.mpHeader->mGameInfo.mStatCount );
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_GetName( const U16 aStatIndex )
* ACTION   : Achieve_Stat_GetName
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

const char *	Achieve_Stat_GetName( const U16 aStatIndex )
{
	Assert( aStatIndex < gAchieveMainClass.mpHeader->mGameInfo.mStatCount );
	return( gAchieveMainClass.mpHeader->mGameInfo.mpStatDefs[ aStatIndex ].mpName );
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Stat_GetType( const U16 aStatIndex )
* ACTION   : Achieve_Stat_GetType
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Stat_GetType( const U16 aStatIndex )
{
	Assert( aStatIndex < gAchieveMainClass.mpHeader->mGameInfo.mStatCount );
	return( gAchieveMainClass.mpHeader->mGameInfo.mpStatDefs[ aStatIndex ].mType );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_SetTaskDef( const U16 aTaskIndex, const sAchieveTaskDef * apTask )
* ACTION   : Achieve_Tasks_SetTaskDef
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Tasks_SetTaskDef( const U16 aTaskIndex, const sAchieveTaskDef * apTask )
{
	sAchieveHeader *	lpH;
	sAchieveTaskDef *	lpDst;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTaskIndex < dACH_GetTaskCount );
	lpDst = &lpH->mGameInfo.mpTaskDefs[ aTaskIndex ];
	lpDst->mPoints = apTask->mPoints;
	lpDst->mFlags  = apTask->mFlags;
	lpDst->mpName  = Achieve_StringAdd( apTask->mpName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_SetScoreTableDef( const U16 aTableIndex, const sAchieveScoreTableDef * apScoreTableDef )
* ACTION   : Achieve_ScoreTable_SetScoreTableDef
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_ScoreTable_SetScoreTableDef( const U16 aTableIndex, const sAchieveScoreTableDef * apScoreTableDef )
{
	U16 i,j;
	sAchieveHeader *	lpH;
	sAchieveScoreTableDef *	lpDst;

	lpH    = gAchieveMainClass.mpHeader;

	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );

	lpDst = &lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ];

	lpDst->mEntryCount    = apScoreTableDef->mEntryCount;
	lpDst->mpTableName    = Achieve_StringAdd( apScoreTableDef->mpTableName );
	lpDst->mSortDirection = apScoreTableDef->mSortDirection;
	lpDst->mType          = apScoreTableDef->mType;
	lpDst->mpValues       = (S32*)Achieve_DataAdd( apScoreTableDef->mEntryCount * 4 );
	lpDst->mpNames        = (sAchieveUserName*)Achieve_DataAdd( apScoreTableDef->mEntryCount * sizeof(sAchieveUserName) );


	for( i=0; i<apScoreTableDef->mEntryCount; i++ )
	{
		j = 0;
		while( (j<15) && (apScoreTableDef->mpNames[ i ].mName[ j ]) )
		{
			lpDst->mpNames[ i ].mName[ j ] = apScoreTableDef->mpNames[ i ].mName[ j ];
			j++;
		}
		lpDst->mpNames[ i ].mName[ j ] = 0;
		lpDst->mpValues[ i ] = apScoreTableDef->mpValues[ i ];
	}

	lpH->mCheckSum = Achieve_CheckSumBuild( lpH );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_IsOnTable( const U16 aTableIndex, const S32 aValue )
* ACTION   : Achieve_ScoreTable_IsOnTable
* CREATION : 17.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_ScoreTable_IsOnTable( const U16 aTableIndex, const S32 aValue )
{
	U8					lRes;
	sAchieveHeader *	lpH;
	sAchieveScoreTableDef * lpTable;

	lRes = 0;
	lpH  = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	lpTable = &lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ];
	if( lpTable->mSortDirection == eACH_SORT_HI )
	{
		if( aValue > lpTable->mpValues[ lpTable->mEntryCount - 1 ] )
		{
			lRes = 1;
		}
	}
	else
	{
		if( aValue < lpTable->mpValues[ lpTable->mEntryCount - 1 ] )
		{
			lRes = 1;
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_SetNewScore_Internal( sAchieveHeader * apHeader, const U16 aTableIndex, const S32 aValue, const sAchieveUserName * apUserName )
* ACTION   : Achieve_ScoreTable_SetNewScore_Internal
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

S16	Achieve_ScoreTable_SetNewScore_Internal( sAchieveHeader * apHeader, const U16 aTableIndex, const S32 aValue, const sAchieveUserName * apUserName )
{
	S16	i;
	S16	lIndex;
	sAchieveScoreTableDef * lpTable;

	lIndex  = 0;
	lpTable = &apHeader->mGameInfo.mpScoreTableDefs[ aTableIndex ];

	if( lpTable->mSortDirection == eACH_SORT_HI )
	{
		while( (lIndex < (S16)lpTable->mEntryCount) && (aValue < lpTable->mpValues[ lIndex ]) )
		{
			lIndex++;
		}
	}
	else
	{
		while( (lIndex < (S16)lpTable->mEntryCount) && (aValue > lpTable->mpValues[ lIndex ]) )
		{
			lIndex++;
		}
	}

	if( lIndex < (S16)lpTable->mEntryCount)
	{
		i = (S16)lpTable->mEntryCount - 2;
		while( i>=lIndex )
		{
			lpTable->mpValues[ i+1 ] = lpTable->mpValues[ i ];
			lpTable->mpNames[ i+1 ]  = lpTable->mpNames[ i ];
			i--;
		}

		lpTable->mpValues[ lIndex ] = aValue;
		lpTable->mpNames[ lIndex ]  = *apUserName;
	}
	apHeader->mCheckSum = Achieve_CheckSumBuild( apHeader );

	return( lIndex );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_SetNewScore( const U16 aTableIndex, const S32 aValue )
* ACTION   : Achieve_ScoreTable_SetNewScore
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

S16		Achieve_ScoreTable_SetNewScore( const U16 aTableIndex, const S32 aValue )
{
	return( Achieve_ScoreTable_SetNewScore_Internal( gAchieveMainClass.mpHeader, aTableIndex, aValue, &dACH_GetpUser->mUserName ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetpName( const U16 aTableIndex, const U16 aEntryIndex )
* ACTION   : Achieve_ScoreTable_GetpName
* CREATION : 17.1.2008 PNK
*-----------------------------------------------------------------------------------*/

char *	Achieve_ScoreTable_GetpName( const U16 aTableIndex, const U16 aEntryIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( &lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mpNames[ aEntryIndex ].mName[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetScore( const U16 aTableIndex, const U16 aEntryIndex )
* ACTION   : Achieve_ScoreTable_GetScore
* CREATION : 17.1.2008 PNK
*-----------------------------------------------------------------------------------*/

S32	Achieve_ScoreTable_GetScore( const U16 aTableIndex, const U16 aEntryIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mpValues[ aEntryIndex ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetpTableName( const U16 aTableIndex )
* ACTION   : Achieve_ScoreTable_GetpTableName
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

const char *	Achieve_ScoreTable_GetpTableName( const U16 aTableIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mpTableName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetEntryCount( const U16 aTableIndex )
* ACTION   : Achieve_ScoreTable_GetEntryCount
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_ScoreTable_GetEntryCount( const U16 aTableIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mEntryCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTables_GetCount( void )
* ACTION   : Achieve_ScoreTables_GetCount
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_ScoreTables_GetCount( void )
{
	return( gAchieveMainClass.mpHeader->mGameInfo.mScoreTableCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetType( const U16 aTableIndex )
* ACTION   : Achieve_ScoreTable_GetType
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_ScoreTable_GetType( const U16 aTableIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mType );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_ScoreTable_GetSortDirection( const U16 aTableIndex )
* ACTION   : Achieve_ScoreTable_GetSortDirection
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_ScoreTable_GetSortDirection( const U16 aTableIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTableIndex < lpH->mGameInfo.mScoreTableCount );
	return( lpH->mGameInfo.mpScoreTableDefs[ aTableIndex ].mSortDirection );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Specs_InputMark( U16 aInputType )
* ACTION   : Achieve_Specs_InputMark
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Specs_InputMark( U16 aInputType )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	lpH->mSpecs.mInputs |= (1<<aInputType);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Specs_Update( const sAchieveSpecs * apSpecs )
* ACTION   : Achieve_Specs_Update
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Specs_Update( const sAchieveSpecs * apSpecs )
{
	if( apSpecs )
	{
		gAchieveMainClass.mpHeader->mSpecs = *apSpecs;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Load( const char * apFileName, void * apBuffer, const U32 aBufferSize )
* ACTION   : Achieve_Load
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Load( const char * apFileName, void * apBuffer, const U32 aBufferSize )
{
	U8	lRes;
	S32	lSize;

	lRes = 0;
	lSize = File_GetSize( apFileName );

	gAchieveMainClass.mBufferSize = aBufferSize;
	gAchieveMainClass.mpBufferBase = (U8*)apBuffer;

	if( lSize > 0 )
	{
		Assert( lSize < (S32)gAchieveMainClass.mBufferSize );
		gAchieveMainClass.mBufferOffset = lSize;
		if( File_LoadAt( apFileName, gAchieveMainClass.mpBufferBase ) )
		{
			gAchieveMainClass.mpHeader = (sAchieveHeader*)gAchieveMainClass.mpBufferBase;
			Endian_FromBigU32( &gAchieveMainClass.mpHeader->mKey );
			Encrypt_DeScramble( gAchieveMainClass.mpBufferBase, lSize, gAchieveMainClass.mpHeader->mKey );
			if( Achieve_Relocate( gAchieveMainClass.mpHeader ) )
			{
				gAchieveMainClass.mpUser = &gAchieveMainClass.mpHeader->mUserInfo;
				lRes = 1;
			}
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Save( const char * apFileName )
* ACTION   : Achieve_Save
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Save( const char * apFileName )
{
	U32				lSize;
	U8				lRes;
	sAchieveHeader *	lpH;
	U32				lKey;

	lpH    = gAchieveMainClass.mpHeader;
	lSize  = gAchieveMainClass.mBufferOffset;

	lpH->mSaveCounter++;

	Achieve_Delocate( lpH );
	lKey = Random_Get();
	Encrypt_Scramble( lpH, lSize, lKey );
	lpH->mKey = lKey;
	Endian_FromBigU32( &lpH->mKey );
	File_Save( apFileName, lpH, lSize );

	Encrypt_DeScramble( lpH, lSize, lKey );
	Achieve_Relocate( lpH );

	lRes = 1;

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetCount( void )
* ACTION   : Achieve_Tasks_GetCount
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Tasks_GetCount( void )
{
	return( dACH_GetTaskCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetUnLockedCount( void )
* ACTION   : Achieve_Tasks_GetUnLockedCount
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Tasks_GetUnLockedCount( void )
{
	U16	lCount;
	U16	i;

	lCount = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( Achieve_Task_IsUnLocked( i ) )
		{
			lCount++;
		}
	}

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetLockedCount( void )
* ACTION   : Achieve_Tasks_GetLockedCount
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Tasks_GetLockedCount( void )
{
	U16	lCount;
	U16	i;

	lCount = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( !Achieve_Task_IsUnLocked( i ) )
		{
			lCount++;
		}
	}

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetpTitle( const U16 aTaskIndex )
* ACTION   : Achieve_Tasks_GetpTitle
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

const char *	Achieve_Tasks_GetpTitle( const U16 aTaskIndex )
{
	const char * lpTitle;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lpTitle = 0;
	if( aTaskIndex < dACH_GetTaskCount )
	{
		lpTitle = lpH->mGameInfo.mpTaskDefs[ aTaskIndex ].mpName;
	}

	return( lpTitle );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Task_IsUnLocked( const U16 aTaskIndex )
* ACTION   : Achieve_Task_IsUnLocked
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U32	Achieve_Task_IsUnLocked( const U16 aTaskIndex )
{
	Assert( aTaskIndex < dACH_GetTaskCount );
	return( dACH_GetTaskUnlockFlag(aTaskIndex) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Task_GetPoints( const U16 aTaskIndex )
* ACTION   : Achieve_Task_GetPoints
* CREATION : 17.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U32	Achieve_Task_GetPoints( const U16 aTaskIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	return( lpH->mGameInfo.mpTaskDefs[ aTaskIndex ].mPoints );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetNewUnLockedCount( void )
* ACTION   : Achieve_Tasks_GetNewUnLockedCount
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Tasks_GetNewUnLockedCount( void )
{
	U16	lCount;
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lCount = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( (!( lpH->mGameInfo.mpTaskDefs[ i ].mFlags & eACH_TASK_FLAG_DISPLAYED )) &&
			( dACH_GetTaskUnlockFlag(i) ) )
		{
			lCount++;
		}
	}

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetNewUnLockedTitle( const U16 aIndex )
* ACTION   : Achieve_Tasks_GetNewUnLockedTitle
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

const char *	Achieve_Tasks_GetNewUnLockedTitle( const U16 aIndex )
{
	U16	lCount;
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lCount = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( (!( lpH->mGameInfo.mpTaskDefs[ i ].mFlags & eACH_TASK_FLAG_DISPLAYED )) &&
			( dACH_GetTaskUnlockFlag(i) ) )
		{
			lCount++;
			if( aIndex == lCount )
			{
				return( lpH->mGameInfo.mpTaskDefs[ i ].mpName );
			}
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetNewUnLockedIndex( const U16 aIndex )
* ACTION   : Achieve_Tasks_GetNewUnLockedIndex
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_Tasks_GetNewUnLockedIndex( const U16 aIndex )
{
	U16	lCount;
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lCount = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( (!( lpH->mGameInfo.mpTaskDefs[ i ].mFlags & eACH_TASK_FLAG_DISPLAYED )) &&
			( dACH_GetTaskUnlockFlag(i) ) )
		{
			if( aIndex == lCount )
			{
				return( i );
			}
			lCount++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Task_UnLock( const U16 aTaskIndex )
* ACTION   : Achieve_Task_UnLock
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Task_UnLock( const U16 aTaskIndex )
{
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;
	Assert( aTaskIndex < dACH_GetTaskCount );
	dACH_SetTaskUnlockFlag( aTaskIndex );
	lpH->mCheckSum = Achieve_CheckSumBuild( lpH );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_ClearNewUnLocked( void )
* ACTION   : Achieve_Tasks_ClearNewUnLocked
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Tasks_ClearNewUnLocked( void )
{
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( dACH_GetTaskUnlockFlag(i) )
		{
			lpH->mGameInfo.mpTaskDefs[ i ].mFlags |= eACH_TASK_FLAG_DISPLAYED;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetPointsTotal( void )
* ACTION   : Achieve_Tasks_GetPointsTotal
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U32	Achieve_Tasks_GetPointsTotal( void )
{
	U32	lPoints;
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lPoints = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		lPoints += lpH->mGameInfo.mpTaskDefs[ i ].mPoints;
	}

	return( lPoints );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Tasks_GetPointsUnLocked( void )
* ACTION   : Achieve_Tasks_GetPointsUnLocked
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U32	Achieve_Tasks_GetPointsUnLocked( void )
{
	U32	lPoints;
	U16	i;
	sAchieveHeader *	lpH;

	lpH    = gAchieveMainClass.mpHeader;

	lPoints = 0;
	for( i=0; i<dACH_GetTaskCount; i++ )
	{
		if( dACH_GetTaskUnlockFlag(i) )
		{
			lPoints += lpH->mGameInfo.mpTaskDefs[ i ].mPoints;
		}
	}

	return( lPoints );
}




/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Serialise( sAchieveHeader * apHeader, U32 * apSize )
* ACTION   : Achieve_Serialise
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

sAchieveHeader *	Achieve_Serialise( sAchieveHeader * apHeader, U32 * apSize )
{
#if	0
	sAchieveHeader * lpHeader;
	U32				lDataSize;
	U32				lStringSize;
	U8 *			lpBase;
	U32				lDataOff;
	U32				lStringOff;
	U16	i;

	lDataSize   = sizeof(sAchieveHeader);
	lStringSize = 0;

	lDataSize += sizeof(sAchieveScoreTable) * apHeader->mScoreTableCount;
	lDataSize += sizeof(sAchieveVariable) * apHeader->mStatCount;
	lDataSize += 4L * apHeader->mStatCount;
	lDataSize += sizeof(sAchieveTask) * apHeader->mTaskCount;

	for( i=0; i<apHeader->mStatCount; i++ )
	{
		lStringSize += ( String_StrLen( apHeader->mStats.mpVariables[ i ].mpName ) + 1 );
	}
	for( i=0; i<apHeader->mTaskCount; i++ )
	{
		lStringSize += ( String_StrLen( apHeader->mTasks.mpTasks[ i ].mpName ) + 1 );
	}
	for( i=0; i<apHeader->mScoreTableCount; i++ )
	{
		lDataSize   += (4L *apHeader->mpScoreTables[ i ].mEntryCount);
		lDataSize   += sizeof(sAchieveUserName) * apHeader->mpScoreTables[ i ].mEntryCount;
		lStringSize += ( String_StrLen( apHeader->mpScoreTables[ i ].mpTableName ) + 1 );
	}

	if( apSize )
	{
		*apSize = lDataSize + lStringSize;
	}

	lpBase     = (U8*)mMEMCALLOC( lDataSize + lStringSize );
	lDataOff   = 0;
	lStringOff = lDataSize;

	lpHeader  = (sAchieveHeader*)lpBase;
	Memory_Copy( sizeof(sAchieveHeader), apHeader, lpHeader );

	lDataOff  = sizeof(sAchieveHeader);
	lpHeader->mpScoreTables = (sAchieveScoreTable*)&lpBase[ lDataOff ];
	lDataOff += sizeof(sAchieveScoreTable) * apHeader->mScoreTableCount;
	lpHeader->mStats.mpVariables = (sAchieveVariable*)&lpBase[ lDataOff ];
	lDataOff += sizeof(sAchieveVariable) * apHeader->mStatCount;
	lpHeader->mStats.mpValues = (U32*)&lpBase[ lDataOff ];
	lDataOff += 4L * apHeader->mStatCount;
	lpHeader->mTasks.mpTasks = (sAchieveTask*)&lpBase[ lDataOff ];
	lDataOff += sizeof(sAchieveTask) * apHeader->mTaskCount;

	Memory_Copy( (sizeof(sAchieveScoreTable) * apHeader->mScoreTableCount), apHeader->mpScoreTables,      lpHeader->mpScoreTables      );
	Memory_Copy( (sizeof(sAchieveVariable)   * apHeader->mStatCount),       apHeader->mStats.mpVariables, lpHeader->mStats.mpVariables );
	Memory_Copy( (4L * apHeader->mStatCount),                               apHeader->mStats.mpValues,    lpHeader->mStats.mpValues    );
	Memory_Copy( (sizeof(sAchieveTask)       * apHeader->mTaskCount),       apHeader->mTasks.mpTasks,     lpHeader->mTasks.mpTasks     );

	for( i=0; i<apHeader->mStatCount; i++ )
	{
		lpHeader->mStats.mpVariables[ i ].mpName = (const char*)&lpBase[ lStringOff ];
		String_StrCpy( (char*)lpHeader->mStats.mpVariables[ i ].mpName, apHeader->mStats.mpVariables[ i ].mpName );
		lStringOff += String_StrLen( apHeader->mStats.mpVariables[ i ].mpName ) + 1;
	}
	for( i=0; i<apHeader->mTaskCount; i++ )
	{
		lpHeader->mTasks.mpTasks[ i ].mpName = (const char*)&lpBase[ lStringOff ];
		String_StrCpy( (char*)lpHeader->mTasks.mpTasks[ i ].mpName, apHeader->mTasks.mpTasks[ i ].mpName );
		lStringOff += String_StrLen( apHeader->mTasks.mpTasks[ i ].mpName ) + 1;
	}

	for( i=0; i<apHeader->mScoreTableCount; i++ )
	{
		lpHeader->mpScoreTables[ i ].mpValues = (S32*)&lpBase[ lDataOff ];
		lDataOff += (4L * apHeader->mpScoreTables[ i ].mEntryCount);
		Memory_Copy( (4L * apHeader->mpScoreTables[ i ].mEntryCount), apHeader->mpScoreTables[ i ].mpValues, lpHeader->mpScoreTables[ i ].mpValues );
		lpHeader->mpScoreTables[ i ].mpNames = (sAchieveUserName*)&lpBase[ lDataOff ];
		lDataOff += sizeof(sAchieveUserName) * apHeader->mpScoreTables[ i ].mEntryCount;
		Memory_Copy( sizeof(sAchieveUserName) * apHeader->mpScoreTables[ i ].mEntryCount, apHeader->mpScoreTables[ i ].mpNames, lpHeader->mpScoreTables[ i ].mpNames );
		lpHeader->mpScoreTables[ i ].mpTableName = (const char*)&lpBase[ lStringOff ];
		String_StrCpy( (char*)lpHeader->mpScoreTables[ i ].mpTableName, apHeader->mpScoreTables[ i ].mpTableName );
		lStringOff += String_StrLen( apHeader->mpScoreTables[ i ].mpTableName ) + 1;
	}

	Assert( lDataOff == lDataSize );
	Assert( lStringOff-lDataSize == lStringSize );

	return( lpHeader );
#endif
	(void)apHeader;
	(void)apSize;
	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Delocate( sAchieveHeader * apHeader )
* ACTION   : Achieve_Delocate
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void				Achieve_Delocate( sAchieveHeader * apHeader )
{
	U16	i,j,k;
	sAchieveUser *	lpUser;
	sAchieveUser *	lpNext;

	if( apHeader )
	{
		lpUser = &apHeader->mUserInfo;
		for( i=0; i<apHeader->mUserCount; i++ )
		{
			for( j=0; j<apHeader->mGameInfo.mScoreTableCount; j++ )
			{
				for( k=0; k<apHeader->mGameInfo.mpScoreTableDefs[ j ].mEntryCount; k++ )
				{
					Endian_FromBigU32( &lpUser->mpScoreTableValues[ j ].mpValues[ k ] );
				}
				Endian_FromBigU16( &lpUser->mpScoreTableValues[ j ].mEntryCount );
				mACH_DELOC2( lpUser->mpScoreTableValues[ j ].mpValues );
			}
			for( j=0; j<apHeader->mGameInfo.mStatCount; j++ )
			{
				Endian_FromBigU32( &lpUser->mpStatValues[ j ] );
			}

			mACH_DELOC2( lpUser->mpStatValues );
			mACH_DELOC2( lpUser->mpTaskValues );
			mACH_DELOC2( lpUser->mpScoreTableValues );

			lpNext = lpUser->mpUserNext;
			mACH_DELOC2( lpUser->mpUserNext );
			lpUser = lpNext;
		}
		Endian_FromBigU16( &apHeader->mUserCount );


		for( i=0; i<apHeader->mGameInfo.mScoreTableCount; i++ )
		{
			for( j=0; j<apHeader->mGameInfo.mpScoreTableDefs[ i ].mEntryCount; j++ )
			{
				Endian_FromBigU32( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mpValues[ j ] );
			}
			Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mEntryCount );
			Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mType );
			Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mSortDirection );


			mACH_DELOC( mGameInfo.mpScoreTableDefs[ i ].mpTableName );
			mACH_DELOC( mGameInfo.mpScoreTableDefs[ i ].mpValues    );
			mACH_DELOC( mGameInfo.mpScoreTableDefs[ i ].mpNames     );
		}
		for( i=0; i<apHeader->mGameInfo.mStatCount; i++ )
		{
			Endian_FromBigU16( &apHeader->mGameInfo.mpStatDefs[ i ].mType );
			mACH_DELOC( mGameInfo.mpStatDefs[ i ].mpName );
		}
		for( i=0; i<apHeader->mGameInfo.mTaskCount; i++ )
		{
			Endian_FromBigU16( &apHeader->mGameInfo.mpTaskDefs[ i ].mFlags );
			Endian_FromBigU16( &apHeader->mGameInfo.mpTaskDefs[ i ].mPoints );
			mACH_DELOC( mGameInfo.mpTaskDefs[ i ].mpName );
		}

		mACH_DELOC( mGameInfo.mpScoreTableDefs );
		mACH_DELOC( mGameInfo.mpStatDefs );
		mACH_DELOC( mGameInfo.mpTaskDefs );

		Endian_FromBigU32( &apHeader->mID );
		Endian_FromBigU32( &apHeader->mVersion );

		Endian_FromBigU32( &apHeader->mCheckSum );

		Endian_FromBigU32( &apHeader->mSaveID );
		Endian_FromBigU32( &apHeader->mSaveCounter );

		Endian_FromBigU32( &apHeader->mGameChunkSize );
		Endian_FromBigU32( &apHeader->mUserChunkSize );

		Endian_FromBigU16( &apHeader->mGameInfo.mScoreTableCount );
		Endian_FromBigU16( &apHeader->mGameInfo.mStatCount );
		Endian_FromBigU16( &apHeader->mGameInfo.mTaskCount );

		Endian_FromBigU16( &apHeader->mSpecs.mTosVersionHi );
		Endian_FromBigU16( &apHeader->mSpecs.mTosVersionLo );
		Endian_FromBigU16( &apHeader->mSpecs.mEmuVersion );
		Endian_FromBigU16( &apHeader->mSpecs.mMachine );
		Endian_FromBigU16( &apHeader->mSpecs.mRamST );
		Endian_FromBigU16( &apHeader->mSpecs.mRamTT );
		Endian_FromBigU16( &apHeader->mSpecs.mInputs );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Relocate( sAchieveHeader * apHeader )
* ACTION   : Achieve_Relocate
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U8				Achieve_Relocate( sAchieveHeader * apHeader )
{
	sAchieveUser *	lpUser;
	U16	i,j,k;
	U8	lRes;

	lRes = 0;
	if( apHeader )
	{
		Endian_FromBigU32( &apHeader->mID );
		Endian_FromBigU32( &apHeader->mVersion );

		if( (dACHIEVE_VERSION==apHeader->mVersion) && (dACHIEVE_ID==apHeader->mID) )
		{
			lRes = 1;
			Endian_FromBigU32( &apHeader->mGameInfo.mGameID );
			Endian_FromBigU32( &apHeader->mGameInfo.mBuildID );

			Endian_FromBigU16( &apHeader->mGameInfo.mScoreTableCount );
			Endian_FromBigU16( &apHeader->mGameInfo.mStatCount );
			Endian_FromBigU16( &apHeader->mGameInfo.mTaskCount );

			Endian_FromBigU16( &apHeader->mSpecs.mTosVersionHi );
			Endian_FromBigU16( &apHeader->mSpecs.mTosVersionLo );
			Endian_FromBigU16( &apHeader->mSpecs.mEmuVersion );
			Endian_FromBigU16( &apHeader->mSpecs.mMachine );
			Endian_FromBigU16( &apHeader->mSpecs.mRamST );
			Endian_FromBigU16( &apHeader->mSpecs.mRamTT );
			Endian_FromBigU16( &apHeader->mSpecs.mInputs );

			mACH_RELOC( mGameInfo.mpScoreTableDefs );
			mACH_RELOC( mGameInfo.mpStatDefs );
			mACH_RELOC( mGameInfo.mpTaskDefs );

			for( i=0; i<apHeader->mGameInfo.mScoreTableCount; i++ )
			{
				mACH_RELOC( mGameInfo.mpScoreTableDefs[ i ].mpNames     );
				mACH_RELOC( mGameInfo.mpScoreTableDefs[ i ].mpTableName );
				mACH_RELOC( mGameInfo.mpScoreTableDefs[ i ].mpValues    );

				Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mEntryCount );
				Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mType );
				Endian_FromBigU16( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mSortDirection );

				for( j=0; j<apHeader->mGameInfo.mpScoreTableDefs[ i ].mEntryCount; j++ )
				{
					Endian_FromBigU32( &apHeader->mGameInfo.mpScoreTableDefs[ i ].mpValues[ j ] );
				}
			}
			for( i=0; i<apHeader->mGameInfo.mStatCount; i++ )
			{
				mACH_RELOC( mGameInfo.mpStatDefs[ i ].mpName );
				Endian_FromBigU16( &apHeader->mGameInfo.mpStatDefs[ i ].mType );
			}
			for( i=0; i<apHeader->mGameInfo.mTaskCount; i++ )
			{
				mACH_RELOC( mGameInfo.mpTaskDefs[ i ].mpName );
				Endian_FromBigU16( &apHeader->mGameInfo.mpTaskDefs[ i ].mFlags );
				Endian_FromBigU16( &apHeader->mGameInfo.mpTaskDefs[ i ].mPoints );
			}

			Endian_FromBigU16( &apHeader->mUserCount );
			lpUser = &apHeader->mUserInfo;
			for( i=0; i<apHeader->mUserCount; i++ )
			{
				mACH_RELOC2( lpUser->mpStatValues );
				mACH_RELOC2( lpUser->mpTaskValues );
				mACH_RELOC2( lpUser->mpScoreTableValues );
				mACH_RELOC2( lpUser->mpUserNext );

				for( j=0; j<apHeader->mGameInfo.mScoreTableCount; j++ )
				{
					Endian_FromBigU16( &lpUser->mpScoreTableValues[ j ].mEntryCount );
					mACH_RELOC2( lpUser->mpScoreTableValues[ j ].mpValues );
					for( k=0; k<apHeader->mGameInfo.mpScoreTableDefs[ j ].mEntryCount; k++ )
					{
						Endian_FromBigU32( &lpUser->mpScoreTableValues[ j ].mpValues[ k ] );
					}
				}
				for( j=0; j<apHeader->mGameInfo.mStatCount; j++ )
				{
					Endian_FromBigU32( &lpUser->mpStatValues[ j ] );
				}

				lpUser = lpUser->mpUserNext;
			}
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GetpUserCurrent( void )
* ACTION   : Achieve_GetpUserCurrent
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

sAchieveUser *	Achieve_GetpUserCurrent( void )
{
	return( dACH_GetpUser );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_DataLoad( sAchieveHeader * apHeader )
* ACTION   : Achieve_DataLoad
* CREATION : 29.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_DataLoad( sAchieveHeader * apHeader )
{
	gAchieveMainClass.mpHeader = apHeader;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_DataUnLoad( sAchieveHeader * apHeader )
* ACTION   : Achieve_DataUnLoad
* CREATION : 29.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_DataUnLoad( sAchieveHeader * apHeader )
{
	if( apHeader == gAchieveMainClass.mpHeader )
	{
		gAchieveMainClass.mpHeader = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Clone( sAchieveHeader * apHeader )
* ACTION   : Achieve_Clone
* CREATION : 2.2.2008 PNK
*-----------------------------------------------------------------------------------*/

sAchieveHeader *	Achieve_Clone( sAchieveHeader * apHeader )
{
	U32	lSize;

	lSize = 0;

	return( Achieve_Serialise( apHeader, &lSize ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Combine( sAchieveHeader * apDst, const sAchieveHeader * apSrc )
* ACTION   : Achieve_Combine
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Combine( sAchieveHeader * apDst, const sAchieveHeader * apSrc )
{
	(void)apDst;
	(void)apSrc;
#if	0
	U16	i,j;
	U32	lMask;

	Assert( apDst );
	Assert( apSrc );
	Assert( apDst != apSrc );

	if( apSrc->mGameInfo.mGameID == apDst->mGameInfo.mGameID )
	{
		for( i=0; i<apSrc->mStatCount; i++ )
		{
			apDst->mStats.mpValues[ i ] += apSrc->mStats.mpValues[ i ];
		}

		for( i=0; i<apSrc->mTaskCount; i++ )
		{
			apDst->mTasks.mpTasks[ i ].mFlags |= apSrc->mTasks.mpTasks[ i ].mFlags;
		}

		for( i=0; i<apSrc->mScoreTableCount; i++ )
		{
			lMask = 1L;
			for( j=0; j<apSrc->mpScoreTables[ i ].mEntryCount; j++ )
			{
				if( apSrc->mpScoreTables[ i ].mActiveBits & lMask )
				{
					Achieve_ScoreTable_SetNewScore_Internal( apDst, i,apSrc->mpScoreTables[ i ].mpValues[ j ], &apSrc->mpScoreTables[ i ].mpNames[ j ] );
				}
				lMask <<= 1L;
			}
		}
	}
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_CheckSumBuild( sAchieveHeader * apHeader )
* ACTION   : Achieve_CheckSumBuild
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U32		Achieve_CheckSumBuild( sAchieveHeader * apHeader )
{
	sCheckSumFletcher	lFletch;
	U32	lSum;
	U16	i,j;

	lSum = 0;

	if( apHeader && apHeader->mUserCount && dACH_GetpUser )
	{
		sAchieveUser *	lpUser;

		lpUser = dACH_GetpUser;

		CheckSum_Fletcher_Init( &lFletch );


		for( i=0; i<apHeader->mGameInfo.mScoreTableCount; i++ )
		{
			sAchieveScoreTableValues *	lpTable = lpUser->mpScoreTableValues;

			CheckSum_Fletcher_U16( &lFletch, lpTable->mEntryCount );

			for( j=0; j<lpTable->mEntryCount; j++ )
			{
				CheckSum_Fletcher_U32( &lFletch, lpTable->mpValues[ j ] );
			}
		}

		for( i=0; i<apHeader->mGameInfo.mStatCount; i++ )
		{
			CheckSum_Fletcher_U32( &lFletch, lpUser->mpStatValues[ i ] );
		}

		for( i=0; i<(apHeader->mGameInfo.mTaskCount>>3); i++ )
		{
			CheckSum_Fletcher_U8( &lFletch, lpUser->mpTaskValues[i] );
		}

		lSum = CheckSum_Fletcher_Get( &lFletch );
	}

	return( lSum );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_StringAdd( const char * apString )
* ACTION   : Achieve_StringAdd
* CREATION : 1.6.2009 PNK
*-----------------------------------------------------------------------------------*/

char *	Achieve_StringAdd( const char * apString )
{
	char *	lpString;

	lpString = (char*)&gAchieveMainClass.mpBufferBase[ gAchieveMainClass.mBufferOffset ];
	if( apString )
	{
		S32	lLen;
		S32	i;

		lLen     = Achieve_StrLen( apString ) + 1;
		if( lLen & 1 )
		{
			lLen++;
		}

		Assert( (gAchieveMainClass.mBufferOffset + lLen) < gAchieveMainClass.mBufferSize );

		for( i=0; i<lLen; i++ )
		{
			lpString[ i ] = apString[ i ];
		}
		gAchieveMainClass.mBufferOffset += lLen;
		gAchieveMainClass.mStringSizeCurrent += lLen;
	}
	Assert( !(gAchieveMainClass.mBufferOffset & 1) )

	return( lpString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_StringRemove( const char * apString )
* ACTION   : Achieve_StringRemove
* CREATION : 1.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_StringRemove( const char * apString )
{
	(void)apString;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_SetUser( const U16 aIndex )
* ACTION   : Achieve_SetUser
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_SetUser( const U16 aIndex )
{
	sAchieveUser *	lpUser;
	U16	lIndex;

	gAchieveMainClass.mCurrentUserIndex = aIndex;

	lIndex = 0;
	lpUser = &gAchieveMainClass.mpHeader->mUserInfo;
	while( lIndex < aIndex )
	{
		lIndex++;
		lpUser = lpUser->mpUserNext;
	}
	gAchieveMainClass.mpUser = lpUser;

	for( lIndex =0; lIndex<gAchieveMainClass.mpHeader->mGameInfo.mTaskCount; lIndex++ )
	{
		if( Achieve_Task_IsUnLocked(lIndex) )
		{
			gAchieveMainClass.mpHeader->mGameInfo.mpTaskDefs[ lIndex ].mFlags |= eACH_TASK_FLAG_DISPLAYED;
		}
		else
		{
			gAchieveMainClass.mpHeader->mGameInfo.mpTaskDefs[ lIndex ].mFlags &= ~eACH_TASK_FLAG_DISPLAYED;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UserCreate( const char * apName )
* ACTION   : Achieve_UserCreate
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UserCreate( const char * apName )
{
	sAchieveUser *	lpUser;
	U16				i;

	if( gAchieveMainClass.mpHeader->mUserCount )
	{
		sAchieveUser *	lpUserOld;

		lpUser = (sAchieveUser*)Achieve_DataAdd( sizeof(sAchieveUser) );
		lpUser->mpUserNext = 0;

		lpUserOld = &gAchieveMainClass.mpHeader->mUserInfo;
		while( lpUserOld->mpUserNext )
		{
			lpUserOld = lpUserOld->mpUserNext;
		}
		lpUserOld->mpUserNext = lpUser;
	}
	else
	{
		lpUser = &gAchieveMainClass.mpHeader->mUserInfo;
	}
	Achieve_StrCpy( lpUser->mUserName.mName, apName, 16 );
	lpUser->mpScoreTableValues = (sAchieveScoreTableValues*)Achieve_DataAdd( sizeof(sAchieveScoreTableValues) * gAchieveMainClass.mpHeader->mGameInfo.mScoreTableCount );
	for( i=0; i<gAchieveMainClass.mpHeader->mGameInfo.mScoreTableCount; i++ )
	{
		lpUser->mpScoreTableValues[ i ].mpValues = (U32*)Achieve_DataAdd( sizeof(U32) * gAchieveMainClass.mpHeader->mGameInfo.mpScoreTableDefs[ i ].mEntryCount );
	}
	lpUser->mpStatValues       = (U32*)Achieve_DataAdd( sizeof(U32) * gAchieveMainClass.mpHeader->mGameInfo.mStatCount );
	lpUser->mpTaskValues       = (U8*)Achieve_DataAdd( (gAchieveMainClass.mpHeader->mGameInfo.mTaskCount + 7) >> 3 );

	gAchieveMainClass.mpHeader->mUserCount++;
	Achieve_SetUser( gAchieveMainClass.mpHeader->mUserCount-1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_DataAdd( const U32 aSize )
* ACTION   : Achieve_DataAdd
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void *	Achieve_DataAdd( const U32 aSize )
{
	void *	lpData;
	U32		lSize;

	lSize = aSize;
	if( lSize & 1 )
	{
		lSize++;
	}

	Assert( (gAchieveMainClass.mBufferOffset + lSize) < gAchieveMainClass.mBufferSize );

	lpData = &gAchieveMainClass.mpBufferBase[ gAchieveMainClass.mBufferOffset ];
	gAchieveMainClass.mBufferOffset += lSize;
	gAchieveMainClass.mDataSizeCurrent += lSize;

	Assert( !(gAchieveMainClass.mBufferOffset & 1) )

	return( lpData );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_DataRemove( void * apData )
* ACTION   : Achieve_DataRemove
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_DataRemove( void * apData )
{
	(void)apData;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_StrLen( const char * apString )
* ACTION   : Achieve_StrLen
* CREATION : 5.6.2009 PNK
*-----------------------------------------------------------------------------------*/

S32	Achieve_StrLen( const char * apString )
{
	S32	lLen;

	lLen = 0;
	if( apString )
	{
		while( apString[ lLen ] )
		{
			lLen++;
		}
	}
	return( lLen );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_StrCpy( char * apDst, const char * apSrc, const U32 aDstLen )
* ACTION   : Achieve_StrCpy
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void 	Achieve_StrCpy( char * apDst, const char * apSrc, const U32 aDstLen )
{
	U32	lIndex;
	if( apDst && apSrc )
	{
		lIndex = 1;
		while( (*apSrc) && (lIndex<aDstLen) )
		{
			*apDst++ = *apSrc++;
			lIndex++;
		}
		*apDst++ = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_StrCmp( const char * apStr0, const char * apStr1 )
* ACTION   : Achieve_StrCmp
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_StrCmp( const char * apStr0, const char * apStr1 )
{
	U8	lVal0,lVal1;

	if( apStr0 && apStr1 )
	{
		while( *apStr0 && *apStr1 )
		{
			lVal0 = *apStr0++;
			lVal1 = *apStr1++;

			if( lVal0 != lVal1 )
			{
				return( 1 );
			}
		}
	}
	else if( apStr0 || apStr1 )
	{
		return( 1 );
	}

	return( 0 );
}



/* ################################################################################ */
