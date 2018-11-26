/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ASSET.H"

#include	"CONTEXT.H"
#include	"PACKAGE.H"

#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<STRING.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_BuildHash( const char * apString )
* ACTION   : Asset_BuildHash
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Asset_BuildHash( const char * apString )
{
	U32	lHash;
	U32	lTemp;

	lHash = 0;
	while ( *apString )
	{
		lHash = ( lHash << 4L ) + *apString++;
		lTemp = lHash & 0xF0000000L;
		if( lTemp )
		{
			lHash ^= lTemp >> 24L;
		}
		lHash &= ~lTemp;
	}

	return( lHash );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_Create( const char * apFileName )
* ACTION   : Asset_Create
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

sAsset *	Asset_Create( const char * apFileName )
{
	sAsset *	lpAsset;
	char		lExtString[ 16 ];
	U32			i,j;

	lpAsset = (sAsset*)mMEMCALLOC( sizeof(sAsset) );

	if( lpAsset )
	{
		i = strlen( apFileName );
		if( i )
		{
			do
			{
				i--;
			} while( (i) && (apFileName[i]!='.') );
			if( i )
			{
				j = 0;
				do
				{
					i++;
					lExtString[ j ] = apFileName[ i ];
					j++;
				} while( (j<15) && (apFileName[ i ]) );
			}
		}

		lpAsset->mExtID     = Asset_BuildHash( lExtString );
		lpAsset->mID        = Asset_BuildHash( apFileName );
		lpAsset->mpClients  = 0;
		lpAsset->mpData     = 0;
		lpAsset->mpNext     = 0;
		lpAsset->mInitFlag  = 0;
		lpAsset->mRelocFlag = 0;
		lpAsset->mSize      = 0;
		lpAsset->mStatus    = eASSET_STATUS_NOTLOADED;

		i = 0;
		while( (i<15) && (apFileName[i]) )
		{
			lpAsset->mFileName[ i ] = apFileName[ i ];
			i++;
		}
		lpAsset->mFileName[ i ] = 0;

	}

	return( lpAsset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_Destroy( sAsset * apAsset )
* ACTION   : Asset_Destroy
* CREATION : 07.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Asset_Destroy( sAsset * apAsset )
{
	mMEMFREE( apAsset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_OnLoad( sAsset * apAsset )
* ACTION   : Asset_OnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Asset_OnLoad( sAsset * apAsset )
{
	U32				lRet;
	sAssetClient *	lpClient;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "Asset_OnLoad(): %s", apAsset->mFileName );

	lpClient = apAsset->mpClients;

	lRet = 1;
	while( lpClient )
	{
		if( lpClient->mppData )
		{
			*lpClient->mppData = apAsset->mpData;
		}
		if( lpClient->OnLoad )
		{
			lRet    &= lpClient->OnLoad( apAsset->mpData, apAsset->mSize, lpClient->mUserData );
		}
		lpClient = lpClient->mpNext;
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_OnUnLoad( sAsset * apAsset )
* ACTION   : Asset_OnUnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Asset_OnUnLoad( sAsset * apAsset )
{
	U32				lRet;
	sAssetClient *	lpClient;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "Asset_OnUnLoad(): %s", apAsset->mFileName );

	lpClient = apAsset->mpClients;

	lRet = 1;
	while( lpClient )
	{
		if( lpClient->OnUnLoad )
		{
			lRet    &= lpClient->OnUnLoad( apAsset->mpData, apAsset->mSize, lpClient->mUserData );
		}
		if( lpClient->mppData )
		{
			*lpClient->mppData = 0;
		}
		lpClient = lpClient->mpNext;
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData )
* ACTION   : AssetClient_Init
* CREATION : 03.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void	AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData )
{
	sContext * lpContext = ContextManager_ContextRegister( apContextName );
	sAsset * lpAsset     = Context_AssetRegister( lpContext, apFileName );

	lpAsset->mpContext = lpContext;

	apClient->mpAsset  = lpAsset;
	apClient->mppData  = appData;
	apClient->mpNext   = lpAsset->mpClients;

	lpAsset->mpClients = apClient;

	if( lpAsset->mStatus == eASSET_STATUS_LOADED )
	{
		if( apClient->mppData )
			*apClient->mppData = lpAsset->mpData;

		if( apClient->OnLoad )
			apClient->OnLoad( lpAsset->mpData, lpAsset->mSize, apClient->mUserData );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_Register( const char * apFileName,const char * apContextName,fAsset aOnLoad,fAsset aOnUnload,void ** appData )
* ACTION   : AssetClient_Register
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sAssetClient *	AssetClient_Register( const char * apFileName,const char * apContextName,fAsset aOnLoad,fAsset aOnUnLoad,void ** appData )
{
	sAssetClient * lpClient  = (sAssetClient*)mMEMCALLOC( sizeof( sAssetClient ) );
	if( lpClient )
	{
		lpClient->OnLoad   = aOnLoad;
		lpClient->OnUnLoad = aOnUnLoad;

		AssetClient_Init( lpClient, apFileName, apContextName, appData );
	}
	return( lpClient );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_DeInit( sAssetClient * apClient )
* ACTION   : AssetClient_DeInit
* CREATION : 03.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void			AssetClient_DeInit( sAssetClient * apClient )
{
	sAsset *		lpAsset;
	sContext *		lpContext;

	lpAsset   = apClient->mpAsset;
	lpContext = lpAsset->mpContext;

	GOD_LL_REMOVE( sAssetClient, lpAsset->mpClients, mpNext, apClient );
	Context_AssetUnRegister( lpContext, lpAsset );
	ContextManager_ContextUnRegister( lpContext );

}

/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_UnRegister( sAssetClient * apClient )
* ACTION   : AssetClient_UnRegister
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	AssetClient_UnRegister( sAssetClient * apClient )
{
	AssetClient_DeInit( apClient );
	mMEMFREE( apClient );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetEntry_ArrayBuild( sAssetEntry * apEntry,sAssetClient ** appArray )
* ACTION   : AssetEntry_ArrayBuild
* CREATION : 15.04.2004 PNK
*-----------------------------------------------------------------------------------*/
/*
void	AssetEntry_ArrayBuild( sAssetEntry * apEntry,sAssetClient ** appArray )
{
	while( apEntry->mpContext )
	{
		appArray[ apEntry->mIndex ] = AssetClient_Register( apEntry->mpFileName, apEntry->mpContext, apEntry->mfOnLoad, apEntry->mfOnUnLoad, apEntry->mppData );
		apEntry++;
	}
}
*/

/* ################################################################################ */
