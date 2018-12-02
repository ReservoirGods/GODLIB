/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ASSET.H"

#include	"CONTEXT.H"
#include	"PACKAGE.H"

#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>


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


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClients_OnLoad( sAssetClient * apClient, struct sAssetItem * apAsset )
* ACTION   : sets up data pointer and callbacks onload for all asset clients
* CREATION : 07.12.2003 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClients_OnLoad( sAssetClient * apClient, struct sAssetItem * apAsset )
* ACTION   : clears data pointer and callbacks onunload for all asset clients
* CREATION : 07.12.2003 PNK
*-----------------------------------------------------------------------------------*/

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
* FUNCTION : AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData )
* ACTION   : AssetClient_Init
* CREATION : 03.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void	AssetClient_Init( sAssetClient * apClient, const char * apFileName, const char * apContextName, void ** appData )
{
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

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AssetClient_DeInit( sAssetClient * apClient )
* ACTION   : AssetClient_DeInit
* CREATION : 03.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void			AssetClient_DeInit( sAssetClient * apClient )
{
	Context_AssetClient_Remove( apClient );
}


/* ################################################################################ */
