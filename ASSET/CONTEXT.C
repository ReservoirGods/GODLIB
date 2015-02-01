/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CONTEXT.H"

#include	"ASSET.H"
#include	"PACKAGE.H"

#include	<GODLIB/LINKLIST/GOD_LL.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DATA
################################################################################### */

sContext *	gpContexts;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_Create( const char * apName )
* ACTION   : Context_Create
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

sContext *	Context_Create( const char * apName )
{
	sContext *	lpContext;
	U32			i;

	lpContext = (sContext*)mMEMCALLOC( sizeof( sContext ) );

	if( lpContext )
	{
		lpContext->mID        = Asset_BuildHash( apName );
		lpContext->mpAssets   = 0;
		lpContext->mpNext     = gpContexts;
		lpContext->mpPackages = 0;
		lpContext->mRefCount  = 0;

		i = 0;
		while( (i<15) && (apName[i]) )
		{
			lpContext->mName[ i ] = apName[ i ];
			i++;
		}
		lpContext->mName[ i ] = 0;

		GOD_LL_INSERT( gpContexts, mpNext, lpContext );
	}

	return( lpContext );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Context_Destroy( sContext * apContext )
* ACTION   : Context_Destroy
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Context_Destroy( sContext * apContext )
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
	mMEMFREE( apContext );
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

	lHash   = Asset_BuildHash( apName );

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : ContextManager_Init( void )
* ACTION   : ContextManager_Init
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	ContextManager_Init( void )
{
	gpContexts = 0;
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
		lpContextNext = lpContext->mpNext;
		mMEMFREE( lpContext );
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

	lHash = Asset_BuildHash( apName );

	GOD_LL_FIND( gpContexts, mpNext, mID, lHash, lpContext );

	if( !lpContext )
	{
		lpContext = Context_Create( apName );
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
		if( apContext->mRefCount <= 0 )
		{
			Context_Destroy( apContext );
		}
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
