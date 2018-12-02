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

	/* pass one - invoke relocators */
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
		pFile->mAsset.mpData = (void*)pFile->mOffset;
		RelocaterManager_DoRelocate( &pFile->mAsset );
		RelocaterManager_DoInit( &pFile->mAsset );
	}

	/* pass two - service clients */
	/* techinically this should iterate until no more clients need to be services, as deps could be multiple levels deep */

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, pFile->mAsset.mHashKey );
		if( client )
		{
			lRet &= AssetClients_OnLoad( client, &pFile->mAsset );
		}
	}

	return lRet;
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
	U16 i;
	U32 lRet = 1;


	(void)apParentName;
	/* pass one - invoke unloads */

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, pFile->mAsset.mHashKey );
		if( client )
		{
			lRet &= AssetClients_OnUnLoad( client );
		}
	}


	/* pass two - invoke relocators */
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
		pFile->mAsset.mpData = 0;
		RelocaterManager_DoDeInit( &pFile->mAsset );
		RelocaterManager_DoDelocate( &pFile->mAsset );
	}


#if 0
	U32 lRet = 1;
	U16 i;

	(void)apPackage;
	(void)apParentName;
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sLinkFileFile * pFile = &apFolder->mpFiles[ i ];
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, pFile->mAsset.mHashKey );
		if( client )
		{
			RelocaterManager_DoDeInit( &pFile->mAsset );
			RelocaterManager_DoDelocate( &pFile->mAsset );
			AssetClients_OnUnLoad( client );
		}
	}
#endif
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
	if( apPackage->mpItems )
	{
		mMEMFREE( apPackage->mpItems );
	}

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
