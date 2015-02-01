#ifndef	INCLUDED_ACH_MAIN_H
#define	INCLUDED_ACH_MAIN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/PACKAGE.H>
#include	<GODLIB/CLOCK/CLOCK.H>
#include	<GODLIB/INPUT/INPUT.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_USER_LIMIT	32


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_TYPE_U32,
	eACH_TYPE_TIME,
};

enum
{
	eACH_SORT_HI,
	eACH_SORT_LO,
};

enum
{
	eACH_TASK_FLAG_UNLOCKED  = (1<<0),
	eACH_TASK_FLAG_DISPLAYED = (1<<1),
	eACH_TASK_FLAG_HIDDEN    = (1<<2),
};

enum
{
	eACH_INPUT_PACKET_LEFT  = (1<<0),
	eACH_INPUT_PACKET_RIGHT = (1<<1),
	eACH_INPUT_PACKET_DOWN  = (1<<2),
	eACH_INPUT_PACKET_UP    = (1<<3),
	eACH_INPUT_PACKET_FIRE  = (1<<7),
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveUserName
{
	char	mName[16];
} sAchieveUserName;

typedef struct sAchieveStatDef
{
	U16			mType;
	const char * mpName;
} sAchieveStatDef;

typedef struct sAchieveScoreTableDef
{
	const char *		mpTableName;
	U16					mEntryCount;
	U16					mType;
	U16					mSortDirection;
	S32 *				mpValues;
	sAchieveUserName *	mpNames;
} sAchieveScoreTableDef;

typedef struct sAchieveTaskDef
{
	U16				mPoints;
	U16				mFlags;
	const char *	mpName;
} sAchieveTaskDef;

typedef struct sAchieveGame
{
	U32		mGameID;
	U32		mBuildID;
	char*	mpName;

	U16		mScoreTableCount;
	U16		mStatCount;
	U16		mTaskCount;

	sAchieveStatDef *		mpStatDefs;
	sAchieveScoreTableDef *	mpScoreTableDefs;
	sAchieveTaskDef *		mpTaskDefs;

} sAchieveGame;

typedef struct sAchieveSpecs
{
	U32	mExpansion;

	U16	mTosVersionHi;
	U16	mTosVersionLo;
	U16	mEmuVersion;
	U16	mMachine;

	U16	mRamST;
	U16	mRamTT;
	U16	mInputs;

	U8	mCPU;
	U8	mFPU;
	U8	mEmu;
	U8	mDrive;

} sAchieveSpecs;

typedef struct sAchieveScoreTableValues
{
	U16		mEntryCount;
	U32 *	mpValues;
} sAchieveScoreTableValues;

typedef struct	sAchieveUser
{
	sAchieveUserName			mUserName;
	U32 *						mpStatValues;
	U8 *						mpTaskValues;
	sAchieveScoreTableValues *	mpScoreTableValues;
	struct sAchieveUser *		mpUserNext;
} sAchieveUser;

typedef struct sAchieveHeader
{
	U32	mID;
	U32	mVersion;
	U32	mKey;
	U32	mCheckSum;

	U32	mSaveID;
	U32	mSaveCounter;

	U32	mGameChunkSize;
	U32	mUserChunkSize;

	U16	mUserCount;

	sAchieveSpecs		mSpecs;
	sAchieveGame		mGameInfo;
	sAchieveUser 		mUserInfo;

} sAchieveHeader;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Achieve_AppInit( void );
void			Achieve_AppDeInit( void );

void			Achieve_Create( const U16 aScoreTableCount, const U16 aStatCount, const U16 aTaskCount, void * apBuffer, const U32 aBufferSize );
void			Achieve_Destroy( void );

void			Achieve_SetGameInfo( const char * apGameName, const U32 aGameID, const U32 aBuildID );
void			Achieve_SetUserInfo( const char * apUserName );
U16				Achieve_GetUserCount( void );
const char *	Achieve_GetUserName( const U16 aIndex );
U16				Achieve_GetUserIndex( const char * apUserName );
sAchieveUser *	Achieve_GetpUserCurrent( void );

void			Achieve_Specs_InputMark( U16 aInputType );
void			Achieve_Specs_Update( const sAchieveSpecs * apSpecs );

void			Achieve_Stat_SetStatDef( const U16 aStatIndex, const sAchieveStatDef * apStat );
void			Achieve_Stat_Add( const U16 aStatIndex, const U32 aStatValue );
void			Achieve_Stat_Max( const U16 aStatIndex, const U32 aStatValue );
void			Achieve_Stat_Update( const U16 aStatIndex, const U32 aStatValue );
U32				Achieve_Stat_Get( const U16 aStatIndex );
U16				Achieve_Stat_GetCount( void );
const char *	Achieve_Stat_GetName( const U16 aStatIndex );
U16				Achieve_Stat_GetType( const U16 aStatIndex );

void			Achieve_Tasks_SetTaskDef( const U16 aTaskIndex, const sAchieveTaskDef * apTask );
U16				Achieve_Tasks_GetCount( void );
U16				Achieve_Tasks_GetUnLockedCount( void );
U16				Achieve_Tasks_GetLockedCount( void );
const char *	Achieve_Tasks_GetpTitle( const U16 aTaskIndex );
U32				Achieve_Task_IsUnLocked( const U16 aTaskIndex );
U32				Achieve_Task_GetPoints( const U16 aTaskIndex );
U16				Achieve_Tasks_GetNewUnLockedCount( void );
U16				Achieve_Tasks_GetNewUnLockedIndex( const U16 aIndex );
void			Achieve_Task_UnLock( const U16 aTaskIndex );
void			Achieve_Tasks_ClearNewUnLocked( void );
U32				Achieve_Tasks_GetPointsTotal( void );
U32				Achieve_Tasks_GetPointsUnLocked( void );

void			Achieve_ScoreTable_SetScoreTableDef( const U16 aTableIndex, const sAchieveScoreTableDef * apScoreTableDef );
S16				Achieve_ScoreTable_SetNewScore( const U16 aTableIndex, const S32 aValue );
U8				Achieve_ScoreTable_IsOnTable( const U16 aTableIndex, const S32 aValue );
char *			Achieve_ScoreTable_GetpName( const U16 aTableIndex, const U16 aEntryIndex );
S32				Achieve_ScoreTable_GetScore( const U16 aTableIndex, const U16 aEntryIndex );
const char *	Achieve_ScoreTable_GetpTableName( const U16 aTableIndex );
U16				Achieve_ScoreTable_GetEntryCount( const U16 aTableIndex );
U16				Achieve_ScoreTables_GetCount( void );
U16				Achieve_ScoreTable_GetType( const U16 aTableIndex );
U16				Achieve_ScoreTable_GetSortDirection( const U16 aTableIndex );

void			Achieve_Delocate( sAchieveHeader * apHeader );
U8				Achieve_Relocate( sAchieveHeader * apHeader );

void			Achieve_DataLoad( sAchieveHeader * apHeader );
void			Achieve_DataUnLoad( sAchieveHeader * apHeader );

U8				Achieve_Load( const char * apFileName, void * apBuffer, const U32 aBufferSize );
U8				Achieve_Save( const char * apFileName );

sAchieveHeader*	Achieve_Clone( sAchieveHeader * apHeader );
void			Achieve_Combine( sAchieveHeader * apDest,  const sAchieveHeader * apSrc0 );


/* ################################################################################ */

#endif	/*	INCLUDED_KERNEL_H	*/
