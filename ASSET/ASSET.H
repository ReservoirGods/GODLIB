#ifndef	INCLUDED_ASSET_H
#define	INCLUDED_ASSET_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/CONTEXT.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	U32	(*fAsset)( void * apData, const U32 aSize, const U32 aUserData );


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eASSET_STATUS_NOTLOADED,
	eASSET_STATUS_LOADED,
	eASSET_STATUS_UNLOADING,
	eASSET_STATUS_UNLOADED,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sAssetClient
{
	fAsset	OnLoad;
	fAsset	OnUnLoad;
	void **					mppData;
	U32						mUserData;
	struct	sAsset *		mpAsset;
	struct	sAssetClient *	mpNext;
} sAssetClient;


typedef	struct sAsset
{
	char				mFileName[ 16 ];
	U16					mStatus;
	U8					mRelocFlag;
	U8					mInitFlag;
	U32					mID;
	U32					mExtID;
	S32					mRefCount;
	U32					mSize;
	void *				mpData;
	sAssetClient *		mpClients;
	struct sContext *	mpContext;
	struct sAsset *		mpNext;
} sAsset;

typedef	struct sAssetEntry
{
	U16		mIndex;
	char *	mpFileName;
	char *	mpContext;
	void **	mppData;
	fAsset	mfOnLoad;
	fAsset	mfOnUnLoad;
} sAssetEntry;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32				Asset_BuildHash( const char * apString );

sAsset *		Asset_Create( const char * apFileName );
void			Asset_Destroy( sAsset * apAsset );

U32				Asset_OnLoad( sAsset * apAsset );
U32				Asset_OnUnLoad( sAsset * apAsset );

/* legacy API, asset system manages client memory */

sAssetClient *	AssetClient_Register( const char * apFileName, const char * apContextName, fAsset aOnLoad, fAsset aOnUnload, void ** appData );
void			AssetClient_UnRegister( sAssetClient * apAsset );

/* new API, callee manages asset client memory */

void			AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData );
void			AssetClient_DeInit( sAssetClient * apClient );


void			AssetEntry_ArrayBuild( sAssetEntry * apEntry, sAssetClient ** appArray );


/* ################################################################################ */

#endif	/* INCLUDED_ASSET_H */
