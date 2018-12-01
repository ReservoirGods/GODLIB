/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CONTEXT.H"

#include	"ASSET.H"
#include	"PACKAGE.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>
#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  DATA
################################################################################### */

sContext *	gpContexts = 0;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_Init( const char * apName )
* ACTION   : Context_Init
* CREATION : 24.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void				Context_Init( sContext * apContext, const char * apName )
{
	U16			i;

	apContext->mID        = Asset_BuildHash( apName, sizeof(apContext->mName) );
	apContext->mpAssets   = 0;
	apContext->mpAssetClients = 0;
	apContext->mpNext     = gpContexts;
	apContext->mpPackages = 0;
	apContext->mRefCount  = 0;
	apContext->mAllocFlag = 0;

	i = 0;
	while( (i<15) && (apName[i]) )
	{
		apContext->mName[ i ] = apName[ i ];
		i++;
	}
	apContext->mName[ i ] = 0;

	GOD_LL_INSERT( gpContexts, mpNext, apContext );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_DeInit( const char * apName )
* ACTION   : Context_DeInit
* CREATION : 24.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void				Context_DeInit( sContext * apContext )
{
	sAsset *	lpAsset;
	sAsset *	lpAssetNext;

	lpAsset = apContext->mpAssets;
	while( lpAsset )
	{
		lpAssetNext = lpAsset->mpNext;
		Asset_Destroy( lpAsset );
		lpAsset     = lpAssetNext;
	}

	GOD_LL_REMOVE( sContext, gpContexts, mpNext, apContext );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_GetAsset( sContext * apContext,const char * apName )
* ACTION   : Context_GetAsset
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

sAsset *	Context_AssetRegister( sContext * apContext,const char * apName )
{
	sAsset *	lpAsset;
	U32			lHash;

	lHash   = Asset_BuildHash( apName, sizeof(apContext->mName) );

	GOD_LL_FIND( apContext->mpAssets, mpNext, mID, lHash, lpAsset );

	if( !lpAsset )
	{
		lpAsset = Asset_Create( apName );
		if( lpAsset )
		{
			GOD_LL_INSERT( apContext->mpAssets, mpNext, lpAsset );
		}
	}
	if( lpAsset )
	{
		lpAsset->mRefCount++;
	}

	return( lpAsset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_AssetUnRegister( sContext * apContext,struct sAsset * apAsset )
* ACTION   : Context_AssetUnRegister
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Context_AssetUnRegister( sContext * apContext, sAsset * apAsset )
{
	if( apAsset )
	{
		apAsset->mRefCount--;
		if( apAsset->mRefCount <= 0 )
		{
			GOD_LL_REMOVE( sAsset, apContext->mpAssets, mpNext, apAsset );
			Asset_Destroy( apAsset );
		}
	}
}

struct sAssetClient* Context_AssetClient_Find( const sContext * apContext, U32 aHashKey )
{
	sAssetClient * client;

	for( client = apContext->mpAssetClients; client; client = client->mpContextNext )
	{
		if( client->mHashKey == aHashKey )
			break;
	}
	return client;
}

sAssetClient *	Context_AssetClient_Add( sContext * apContext, struct sAssetClient * apClient )
{
	sAssetClient * client = Context_AssetClient_Find( apContext, apClient->mHashKey );
	if( client )
	{
		GOD_LL_INSERT( client->mpNext, mpNext, apClient );
	}
	else
	{
		/* need to find dangling assets in package here */
		/*		
		sPackage * package;
		for( package=apContext->mpPackages; package; package=package->mpContextNext )
		{
			if( PackageManager_AssetLoad( package, apClient ) )
				break;
		}
		*/

		GOD_LL_INSERT( apContext->mpAssetClients, mpContextNext, apClient );
	}
	apClient->mpContext = apContext;

	return client;
}

void	Context_AssetClient_Remove( struct sAssetClient * apClient )
{
	sAssetClient * client = Context_AssetClient_Find( apClient->mpContext, apClient->mHashKey );

	if( client == apClient )
	{
		client = apClient->mpNext;
		GOD_LL_REMOVE( sAssetClient, apClient->mpContext->mpAssetClients, mpContextNext, apClient );
		if( client )
		{
			GOD_LL_INSERT( apClient->mpContext->mpAssetClients, mpContextNext, client );
		}
	}
	else if( client )
	{
		GOD_LL_REMOVE( sAssetClient, client->mpNext, mpContextNext, apClient );
	}
	apClient->mpNext = 0;
	apClient->mpContext = 0;
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_Init( void )
* ACTION   : ContextManager_Init
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_Init( void )
{
	/* note - clients may have already registered contexts at this point */
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_DeInit( void )
* ACTION   : ContextManager_DeInit
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_DeInit( void )
{
	sContext *	lpContext;
	sContext *	lpContextNext;

	lpContext = gpContexts;

	while( lpContext )
	{
		/* nothing should be referring to this context now */
		GODLIB_ASSERT( 0 == lpContext->mRefCount );

		lpContextNext = lpContext->mpNext;
		lpContext->mpNext = 0;

		/* we should have unregistered all assets at this point */
		GODLIB_ASSERT( !lpContext->mpAssets );
		lpContext->mpAssets = 0;

		/* we should have unregistered all packages at this point */
		GODLIB_ASSERT( !lpContext->mpPackages );
		lpContext->mpPackages = 0;
		lpContext     = lpContextNext;
	}

	gpContexts = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_Update( void )
* ACTION   : ContextManager_Update
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_Update( void )
{
	sContext *	lpContext;

	lpContext = gpContexts;

	while( lpContext )
	{
		lpContext = lpContext->mpNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_ContextRegister( const char * apName )
* ACTION   : ContextManager_ContextRegister
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

sContext *	ContextManager_ContextRegister( const char * apName )
{
	U32			lHash;
	sContext *	lpContext;

	lHash = Asset_BuildHash( apName, sizeof(lpContext->mName) );

	GOD_LL_FIND( gpContexts, mpNext, mID, lHash, lpContext );

	if( !lpContext )
	{
		/* we should now statically create all contexts, not dynamically */
		/* client code should create static contexts and call context_init */
		GODLIB_ASSERT( lpContext );
/*		lpContext = Context_Create( apName ); */
	}
	if( lpContext )
	{
		lpContext->mRefCount++;
	}

	return( lpContext );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_ContextUnRegister( sContext * apContext )
* ACTION   : ContextManager_ContextUnRegister
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_ContextUnRegister( sContext * apContext )
{
	if( apContext )
	{
		apContext->mRefCount--;

		/* shouldn't unregister more than we register */
		GODLIB_ASSERT( apContext->mRefCount >= 0 );
/*		
		if( apContext->mRefCount <= 0 )
		{
			Context_Destroy( apContext );
		}
*/		
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_ShowAll( fContextPrint aPrint )
* ACTION   : ContextManager_ShowAll
* CREATION : 11.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_ShowAll( fContextPrint aPrint )
{
	sContext *	lpContext;
	sPackage *	lpPackage;
	sAsset *	lpAsset;
	U32			i;
	char		lString[ 128 ];

	lpContext = gpContexts;

	while( lpContext )
	{
		sprintf( lString, "CONTEXT : %s", &lpContext->mName[ 0 ] );
		aPrint( lString );

		lpPackage = lpContext->mpPackages;
		while( lpPackage )
		{
			sprintf( lString, "PACKAGE : %s", &lpPackage->mName[ 0 ] );
			aPrint( lString );

			for( i=0; i<lpPackage->mFileCount; i++ )
			{
				lpAsset = lpPackage->mpItems[ i ].mpAsset;
				sprintf( lString, "ASSET : %s", &lpAsset->mFileName[ 0 ] );
				aPrint( lString );
			}
		}

		lpAsset = lpContext->mpAssets;
		i       = 0;
		while( lpAsset )
		{
			i++;
			lpAsset = lpAsset->mpNext;
		}
		sprintf( lString, "ASSETCOUNT : %ld", i );
		aPrint( lString );

		lpContext = lpContext->mpNext;
	}
}


/* ################################################################################ */
