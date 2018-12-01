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
* FUNCTION : Asset_BuildHash( const char * apString, U16 aStringLength )
* ACTION   : Asset_BuildHash
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Asset_BuildHash( const char * apString, U16 aStringLength )
{
	U32	lHash;
	U32	lTemp;

	lHash = 0;
	while ( *apString && aStringLength )
	{
		lHash = ( lHash << 4L ) + *apString++;
		lTemp = lHash & 0xF0000000L;
		if( lTemp )
		{
			lHash ^= lTemp >> 24L;
		}
		lHash &= ~lTemp;
		aStringLength--;
	}

	return( lHash );
}

void	Asset_Init( sAsset * apAsset, const char * apFileName )
{
	char		lExtString[ 16 ];
	U32			i,j;

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

	apAsset->mExtID     = Asset_BuildHash( lExtString, sizeof(lExtString) );
	apAsset->mID        = Asset_BuildHash( apFileName, sizeof(apAsset->mFileName) );
	apAsset->mpClients  = 0;
	apAsset->mpData     = 0;
	apAsset->mpNext     = 0;
	apAsset->mInitFlag  = 0;
	apAsset->mRelocFlag = 0;
	apAsset->mSize      = 0;
	apAsset->mStatus    = eASSET_STATUS_NOTLOADED;

	i = 0;
	while( (i<15) && (apFileName[i]) )
	{
		apAsset->mFileName[ i ] = apFileName[ i ];
		i++;
	}
	apAsset->mFileName[ i ] = 0;

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Asset_Create( const char * apFileName )
* ACTION   : Asset_Create
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

sAsset *	Asset_Create( const char * apFileName )
{
	sAsset *	lpAsset;

	lpAsset = (sAsset*)mMEMCALLOC( sizeof(sAsset) );

	if( lpAsset )
	{
		Asset_Init( lpAsset, apFileName );
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

U32			AssetClients_OnLoad( sAssetClient * apClient, struct sAssetItem * apAsset )
{
	U32 ret = 1;
	sAssetClient * client;

	for( client = apClient; client; client=client->mpNext )
	{
		client->mpAsset = apAsset;

		if( apAsset )
		{
			if( client->mppData )
			{
				*client->mppData = apClient->mpAsset->mpData;
			}
			if( client->OnLoad )
			{
				ret    &= client->OnLoad( apClient->mpAsset->mpData, apClient->mpAsset->mSize, client->mUserData );
			}
		}
	}

	return ret;
}

U32			AssetClients_OnUnLoad( sAssetClient * apClient )
{
	U32 ret = 1;
	sAssetClient * client;

	for( client = apClient; client; client=client->mpNext )
	{
		if( client->mpAsset )
		{
			if( client->OnUnLoad )
			{
				ret    &= client->OnUnLoad( apClient->mpAsset->mpData, apClient->mpAsset->mSize, client->mUserData );
			}
		}
		if( client->mppData )
		{
			*client->mppData = 0;
		}

		client->mpAsset = 0;
	}
	return ret;
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
#if 1
	U16 i;
	sContext * lpContext = ContextManager_ContextRegister( apContextName );
	sAssetClient * parent;

	for( i=0; i<12 && apFileName[i]; i++ )
	{
		apClient->mFileName[i] = apFileName[i];
	}
	for( ; i<12 ; i++ )
	{
		apClient->mFileName[i] = 0;
	}

	apClient->mHashKey = Asset_BuildHash( apFileName, sizeof(apClient->mFileName) );

	apClient->mppData = appData;

	parent = Context_AssetClient_Add( lpContext, apClient );
	if( parent && parent->mpAsset )
	{
		if( apClient->mppData )
			*apClient->mppData = parent->mpAsset->mpData;

		if( apClient->OnLoad )
			apClient->OnLoad( parent->mpAsset->mpData, parent->mpAsset->mSize, apClient->mUserData );			
	}


#else	
	U16 i;
	sContext * lpContext = ContextManager_ContextRegister( apContextName );
	sAsset * lpAsset     = Context_AssetRegister( lpContext, apFileName );

	Context_AssetClient_Add( lpContext, apClient );

	lpAsset->mpContext = lpContext;

	apClient->mpAsset  = lpAsset;
	apClient->mppData  = appData;
	apClient->mpNext   = lpAsset->mpClients;

	apClient->mpData = 0;
	apClient->mSize = 0;

	lpAsset->mpClients = apClient;
	for( i=0; i<12 && apFileName[i]; i++ )
	{
		lpAsset->mFileName[i] = apFileName[i];
		apClient->mFileName[i] = apFileName[i];
	}
	for( ; i<12 ; i++ )
	{
		lpAsset->mFileName[i] = 0;
		apClient->mFileName[i] = 0;
	}

	if( lpAsset->mStatus == eASSET_STATUS_LOADED )
	{
		if( apClient->mppData )
			*apClient->mppData = lpAsset->mpData;

		if( apClient->OnLoad )
			apClient->OnLoad( lpAsset->mpData, lpAsset->mSize, apClient->mUserData );
	}
#endif	
}

#if 0
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
* FUNCTION : AssetClient_UnRegister( sAssetClient * apClient )
* ACTION   : AssetClient_UnRegister
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	AssetClient_UnRegister( sAssetClient * apClient )
{
	AssetClient_DeInit( apClient );
	mMEMFREE( apClient );
}

#endif

/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_DeInit( sAssetClient * apClient )
* ACTION   : AssetClient_DeInit
* CREATION : 03.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void			AssetClient_DeInit( sAssetClient * apClient )
{
/*	sAsset *		lpAsset;*/

	Context_AssetClient_Remove( apClient );
#if 0
	lpAsset   = apClient->mpAsset;
	if( lpAsset )
	{
		sContext *		lpContext = lpAsset->mpContext;

		GOD_LL_REMOVE( sAssetClient, lpAsset->mpClients, mpNext, apClient );
		Context_AssetUnRegister( lpContext, lpAsset );
		ContextManager_ContextUnRegister( lpContext );
	}
#endif
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
