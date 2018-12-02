/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PKG_LNK.H"

#include	"ASSET.H"
#include	"CONTEXT.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/ASSET/RELOCATE.H>
#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/LINKFILE/LINKFILE.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	PackageLnk_FolderLoad(   sPackage * apPackage,sLinkFileFolder * apFolder, char * apParentName );
U32	PackageLnk_FolderUnLoad( sPackage * apPackage,sLinkFileFolder * apFolder, char * apParentName );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_FolderLoad( sPackage * apPackage, sLinkFileFolder * apFolder, char * apParentName )
* ACTION   : loads a linkfile folder
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32		PackageLnk_FolderLoad( sPackage * apPackage, sLinkFileFolder * apFolder, char * apParentName )
{
	U32 lRet = 1;
	U16 i;

	(void)apParentName;
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
#if 1		
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, pFile->mAsset.mHashKey );
		U16 size = 0;
		U32 lKey;
		
		for( size=0; pFile->mpFileName[size]; size++);
		lKey = Asset_BuildHash( pFile->mpFileName, (U16)size);

		GODLIB_ASSERT( pFile->mAsset.mHashKey == lKey );
		pFile->mAsset.mpData = (void*)pFile->mOffset;

		if( client )
		{
			client->mpAsset = &pFile->mAsset;
			RelocaterManager_DoRelocate( &pFile->mAsset );
			RelocaterManager_DoInit( &pFile->mAsset );
			lRet &= AssetClients_OnLoad( client, &pFile->mAsset );
		}
#else
		sAsset * ass = Context_AssetRegister( apPackage->mpContext, pFile->mpFileName );
		pFile->mpAsset = ass;
		if( ass )
		{
			ass->mStatus = eASSET_STATUS_LOADED;
			ass->mpData  = (void*)pFile->mOffset;
			ass->mSize   = pFile->mSize;

			RelocaterManager_DoRelocate( ass );
			RelocaterManager_DoInit( ass );
			lRet &= Asset_OnLoad( ass );
		}
#endif		
	}


	return lRet;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_FolderLoad2( sPackage * apPackage, sLinkFileFolder * apFolder )
* ACTION   : second pass over package items
			 this allows sAssetClients registers during reloactors DoInit or from other
			 asset loads to get service.
			 techinically this should run N times until no assets in the context are left
			 unresolved
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32		PackageLnk_FolderLoad2( sPackage * apPackage, sLinkFileFolder * apFolder )
{
	U16 i;
	U32 ret = 1;

	sAssetClient * client = apPackage->mpContext->mpAssetClients;
	for( ; client; client=client->mpContextNext)
	{
		if( !client->mpAsset )
		{
			for( i=0; i<apFolder->mFileCount; i++ )
			{
				sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
				if( pFile->mAsset.mHashKey == client->mHashKey )
				{
					RelocaterManager_DoRelocate( &pFile->mAsset );
					RelocaterManager_DoInit( &pFile->mAsset );
					ret &= AssetClients_OnLoad( client, &pFile->mAsset );
					break;
				}
			}
		}
	}
	return ret;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_LoadFromLinkFile( sPackage * apPackage, sLinkFile * apLinkFile )
* ACTION   : loads a linkfile package
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32		PackageLnk_LoadFromLinkFile( sPackage * apPackage, sLinkFile * apLinkFile )
{
	U32 ret = 1;
	apPackage->mpLinkFile = apLinkFile;

	if( !apLinkFile )
		return 0;

	ret &= PackageLnk_FolderLoad( apPackage, apPackage->mpLinkFile->mpRoot, 0 );
	ret &= PackageLnk_FolderLoad2( apPackage, apPackage->mpLinkFile->mpRoot );

	return ret;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_Load( sPackage * apPackage,const char * apDirName )
* ACTION   : loads a linkfile package
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32		PackageLnk_Load( sPackage * apPackage, const char * apDirName )
{
	sLinkFile * linkfile;
	linkfile = LinkFile_InitToRAM( (char*)apDirName );
	return PackageLnk_LoadFromLinkFile( apPackage, linkfile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_FolderUnLoad( sPackage * apPackage,sLinkFileFolder * apFolder, char * apParentName )
* ACTION   : unloads a folder
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageLnk_FolderUnLoad( sPackage * apPackage,sLinkFileFolder * apFolder, char * apParentName )
{
	U32 lRet = 1;
	U16 i;

	(void)apPackage;
	(void)apParentName;
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
#if 1	
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, pFile->mAsset.mHashKey );
		if( client )
		{
			RelocaterManager_DoDeInit( &pFile->mAsset );
			RelocaterManager_DoDelocate( &pFile->mAsset );
			AssetClients_OnUnLoad( client );
		}
#else	
		sAsset * ass = (sAsset*)pFile->mpAsset;
		if( ass )
		{
			RelocaterManager_DoDeInit( ass );
			RelocaterManager_DoDelocate( ass );
/*			lRet &= Asset_OnUnLoad( ass ); */

			ass->mStatus = eASSET_STATUS_NOTLOADED;
			ass->mpData  = 0;
			ass->mSize   = 0;
		}
#endif		
	}

	return lRet;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_UnLoad( sPackage * apPackage )
* ACTION   : unloads a package
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

U32		PackageLnk_UnLoad( sPackage * apPackage )
{
	PackageLnk_FolderUnLoad( apPackage, apPackage->mpLinkFile->mpRoot, 0 );

	LinkFile_DeInit( apPackage->mpLinkFile );
	mMEMFREE( apPackage->mpItems );

	apPackage->mpLinkFile = 0;
	apPackage->mpItems    = 0;
	apPackage->mFileCount = 0;

	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_Destroy( sPackage * apPackage )
* ACTION   : unloads a package
* CREATION : 01.12.2018 PNK
*-----------------------------------------------------------------------------------*/

void	PackageLnk_Destroy( sPackage * apPackage )
{
	PackageLnk_UnLoad( apPackage );
}


/* ################################################################################ */
