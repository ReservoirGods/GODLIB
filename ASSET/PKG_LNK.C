/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PKG_LNK.H"

#include	"ASSET.H"
#include	"CONTEXT.H"

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
* FUNCTION : PackageLnk_Load( sPackage * apPackage,const char * apDirName )
* ACTION   : PackageLnk_Load
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageLnk_Load( sPackage * apPackage,const char * apDirName )
{
	U32	lRet;

	DebugChannel_Printf2( eDEBUGCHANNEL_ASSET, "PackageLnk_Load(): %s %p", apDirName, apPackage );

	lRet = 0;

	apPackage->mpLinkFile = LinkFile_InitToRAM( (char*)apDirName );

	if( apPackage->mpLinkFile )
	{
		apPackage->mpItems    = (sPackageItem*)mMEMCALLOC( sizeof(sPackageItem) * apPackage->mpLinkFile->mTotalFileCount );
		apPackage->mFileCount = 0;
		lRet = PackageLnk_FolderLoad( apPackage, apPackage->mpLinkFile->mpRoot, 0 );
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_UnLoad( sPackage * apPackage )
* ACTION   : PackageLnk_UnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageLnk_UnLoad( sPackage * apPackage )
{
	U32	lRet;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageLnk_UnLoad(): %p", apPackage );

	lRet = 1;

	if( apPackage->mpLinkFile )
	{
		lRet = PackageLnk_FolderUnLoad( apPackage, apPackage->mpLinkFile->mpRoot, 0 );
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_Destroy( sPackage * apPackage )
* ACTION   : PackageLnk_Destroy
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageLnk_Destroy( sPackage * apPackage )
{
	U16			i;
	sAsset *	lpAsset;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageLnk_Destroy(): %p", apPackage );

	for( i=0; i<apPackage->mFileCount; i++ )
	{
		lpAsset = apPackage->mpItems[ i ].mpAsset;

		RelocaterManager_DoDeInit( lpAsset );
		RelocaterManager_DoDelocate( lpAsset );
/*		File_UnLoad( lpAsset->mpData );*/

		lpAsset->mpData  = 0;
		lpAsset->mSize   = 0;
		lpAsset->mStatus = eASSET_STATUS_NOTLOADED;
	}

	LinkFile_DeInit( apPackage->mpLinkFile );
	mMEMFREE( apPackage->mpItems );

	apPackage->mpLinkFile = 0;
	apPackage->mpItems    = 0;
	apPackage->mFileCount = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_FolderLoad( sPackage * apPackage,sLinkFileFolder * apFolder )
* ACTION   : PackageLnk_FolderLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageLnk_FolderLoad( sPackage * apPackage,sLinkFileFolder * apFolder, char * apParentName )
{
	U32				i;
	U32				lRet;
	sAsset *		lpAsset;
	sLinkFileFile *	lpFile;
	char			lName[ 128 ];

	DebugChannel_Printf2( eDEBUGCHANNEL_ASSET, "PackageLnk_FolderLoad(): %s %p", apFolder->mpFolderName, apPackage );

	lRet = 1;

	if( apFolder )
	{
		for( i=0; i<apFolder->mFileCount; i++ )
		{
			lpFile = &apFolder->mpFiles[ i ];
			if( apParentName )
			{
				sprintf( lName, "%s\\%s", apParentName, lpFile->mpFileName );
			}
			else
			{
				sprintf( lName, lpFile->mpFileName );
			}
			lpAsset = Context_AssetRegister( apPackage->mpContext, lName );

			apPackage->mpItems[ apPackage->mFileCount ].mpAsset = lpAsset;

			if( lpAsset )
			{
				lpAsset->mStatus = eASSET_STATUS_LOADED;
				lpAsset->mpData  = (void*)lpFile->mOffset;
				lpAsset->mSize   = lpFile->mSize;

				RelocaterManager_DoRelocate( lpAsset );
				RelocaterManager_DoInit( lpAsset );
				lRet &= Asset_OnLoad( lpAsset );
			}

			apPackage->mFileCount++;
		}

		for( i=0; i<apFolder->mFolderCount; i++ )
		{
			if( apFolder->mpFolders )
			{
				if( apParentName )
				{
					sprintf( lName, "%s\\%s", apParentName, apFolder->mpFolderName );
				}
				else
				{
					sprintf( lName, apFolder->mpFolderName );
				}
				lRet &= PackageLnk_FolderLoad( apPackage, &apFolder->mpFolders[ i ], lName );
			}
		}
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageLnk_FolderUnLoad( sPackage * apPackage,sLinkFileFolder * apFolder,char * apParentName )
* ACTION   : PackageLnk_FolderUnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageLnk_FolderUnLoad( sPackage * apPackage,sLinkFileFolder * apFolder,char * apParentName )
{
	U32				i;
	U32				lRet;
	sAsset *		lpAsset;
	sLinkFileFile *	lpFile;
	char			lName[ 128 ];

	lRet = 1;

	if( apFolder )
	{
		for( i=0; i<apFolder->mFileCount; i++ )
		{
			lpFile = &apFolder->mpFiles[ i ];
			if( apParentName )
			{
				sprintf( lName, "%s\\%s", apParentName, lpFile->mpFileName );
			}
			else
			{
				sprintf( lName, lpFile->mpFileName );
			}
			lpAsset = Context_AssetRegister( apPackage->mpContext, lName );
			if( lpAsset )
			{
				switch( lpAsset->mStatus )
				{
				case	eASSET_STATUS_LOADED:
				case	eASSET_STATUS_UNLOADING:
					if( Asset_OnUnLoad( lpAsset ) )
					{
						lpAsset->mStatus = eASSET_STATUS_UNLOADED;
					}
					else
					{
						lpAsset->mStatus = eASSET_STATUS_UNLOADING;
						lRet = 0;
					}
				}
			}
		}

		if( apFolder->mpFolders )
		{
			if( apParentName )
			{
				sprintf( lName, "%s\\%s", apParentName, apFolder->mpFolderName );
			}
			else
			{
				sprintf( lName, apFolder->mpFolderName );
			}
			lRet &= PackageLnk_FolderLoad( apPackage, apFolder->mpFolders, lName );
		}
	}

	return( lRet );
}


/* ################################################################################ */
