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
	GOD_LL_REMOVE( sContext, gpContexts, mpNext, apContext );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_AssetClient_Find( const sContext * apContext, U32 aHashKey )
* ACTION   : Finds assetclient with specific filename hash in context
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_AssetClient_Add( sContext * apContext, struct sAssetClient * apClient )
* ACTION   : Adds an asset client to a context.
             return parent client (if exists)
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

sAssetClient *	Context_AssetClient_Add( sContext * apContext, struct sAssetClient * apClient )
{
	sAssetClient * client = Context_AssetClient_Find( apContext, apClient->mHashKey );
	if( client )
	{
		GOD_LL_INSERT( client->mpNext, mpNext, apClient );
	}
	else
	{
		GOD_LL_INSERT( apContext->mpAssetClients, mpContextNext, apClient );
	}
	apClient->mpContext = apContext;

	return client;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_AssetClient_Remove( struct sAssetClient * apClient )
* ACTION   : Removes an asset client from a context.
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

void	Context_AssetClient_Remove( struct sAssetClient * apClient )
{
	sAssetClient * client = Context_AssetClient_Find( apClient->mpContext, apClient->mHashKey );

	if( client == apClient )
	{
		sContext * context = (sContext*)apClient->mpContext;
		client = apClient->mpNext;
		GOD_LL_REMOVE( sAssetClient, context->mpAssetClients, mpContextNext, apClient );
		if( client )
		{
			GOD_LL_INSERT( context->mpAssetClients, mpContextNext, client );
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
	(void)aPrint;
#if 0	
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
#endif	
}


/* ################################################################################ */
