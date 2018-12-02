#ifndef	INCLUDED_ASSET_H
#define	INCLUDED_ASSET_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	U32	(*fAsset)( void * apData, const U32 aSize, const U32 aUserData );


/* ###################################################################################
#  ENUMS
################################################################################### */


enum
{
	eASSET_STATUS_BIT_LOADED    = (1<<0),
	eASSET_STATUS_BIT_INITED    = (1<<1),
	eASSET_STATUS_BIT_RELOCATED = (1<<2),
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAssetItem
{
	void *	mpData;
	U32		mSize;
	U32		mHashKey;
	U32		mExtension;
	U32		mStatusBits;
} sAssetItem;


typedef	struct sAssetClient
{
	/* start to consolidate this into a single structure */
	char	mFileName[ 12 ];
	U32		mHashKey;
	struct sContext * mpContext;

	fAsset	OnLoad;
	fAsset	OnUnLoad;
	void **					mppData;
	U32						mUserData;
	struct	sAssetItem *	mpAsset;
	struct	sAssetClient *	mpNext;
	struct	sAssetClient *	mpContextNext;
} sAssetClient;



/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32				Asset_BuildHash( const char * apString, U16 aStringLength );

void			AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData );
void			AssetClient_DeInit( sAssetClient * apClient );

U32				AssetClients_OnLoad( sAssetClient * apClient, struct sAssetItem * apAsset );
U32				AssetClients_OnUnLoad( sAssetClient * apClient );


/* ################################################################################ */

#endif	/* INCLUDED_ASSET_H */
