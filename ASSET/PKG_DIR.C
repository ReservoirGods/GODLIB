/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PKG_DIR.H"

#include	"ASSET.H"
#include	"CONTEXT.H"
#include	"RELOCATE.H"

#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DATA
################################################################################### */

sGemDosDTA	gPackageDirDTA;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageDir_Load( sPackage * apPackage, const char * apDirName )
* ACTION   : PackageDir_Load
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageDir_Load( sPackage * apPackage, const char * apDirName )
{
	sAsset *	lpAsset;
	sContext *	lpContext;
	U32			lIndex;
	char		lString[ 128 ];

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageDir_Load(): %s", apPackage->mName );

	sprintf( lString, "%s\\*.*", apDirName );

	apPackage->mFileCount = 0;
	lpContext             = apPackage->mpContext;

	File_SetDTA( &gPackageDirDTA );

	if( !File_ReadFirst( lString, 1 ) )
	{
		do
		{
			apPackage->mFileCount++;
		} while( !File_ReadNext() );
	}
	
	if( apPackage->mFileCount )
	{
		apPackage->mpItems = (sPackageItem*)mMEMCALLOC( sizeof(sPackageItem) * apPackage->mFileCount );

		lIndex = 0;
		if( !File_ReadFirst( lString, 1 ) )
		{
			do
			{
				sprintf( lString, "%s\\%s", apDirName, gPackageDirDTA.mFileName );

				lpAsset = Context_AssetRegister( lpContext, gPackageDirDTA.mFileName );
				apPackage->mpItems[ lIndex ].mpAsset = lpAsset;
				if( lpAsset )
				{
					lpAsset->mSize  = gPackageDirDTA.mLength;
					if( lpContext->mSlowRamFlag )
					{
						lpAsset->mpData = File_LoadSlowRam( lString );
					}
					else
					{
						lpAsset->mpData = File_Load( lString );
					}
					lpAsset->mStatus = eASSET_STATUS_LOADED;
				}
				RelocaterManager_DoRelocate( lpAsset );
				RelocaterManager_DoInit( lpAsset );
				Asset_OnLoad( lpAsset );

				lIndex++;
			} while( (!File_ReadNext()) && (lIndex<apPackage->mFileCount) );
		}
	}

	return( apPackage->mFileCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageDir_UnLoad( sPackage * apPackage )
* ACTION   : PackageDir_UnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageDir_UnLoad( sPackage * apPackage )
{
	U32			i;
	U32			lRet;
	sAsset *	lpAsset;
	
	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageDir_UnLoad(): %s", apPackage->mName );

	lRet = 1;

	for( i=0; i<apPackage->mFileCount; i++ )
	{
		lpAsset = apPackage->mpItems[ i ].mpAsset;

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
			break;
		}
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageDir_Destroy( sPackage * apPackage )
* ACTION   : PackageDir_Destroy
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageDir_Destroy( sPackage * apPackage )
{
	U32			i;
	sAsset *	lpAsset;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageDir_Destroy(): %s", apPackage->mName );

	for( i=0; i<apPackage->mFileCount; i++ )
	{
		lpAsset = apPackage->mpItems[ i ].mpAsset;

		RelocaterManager_DoDeInit( lpAsset );
		RelocaterManager_DoDelocate( lpAsset );
		File_UnLoad( lpAsset->mpData );

		lpAsset->mpData  = 0;
		lpAsset->mSize   = 0;
		lpAsset->mStatus = eASSET_STATUS_NOTLOADED;
		Context_AssetUnRegister( apPackage->mpContext, lpAsset );
	}

	mMEMFREE( apPackage->mpItems );
	apPackage->mpItems = 0;
	apPackage->mFileCount = 0;
}


/* ################################################################################ */
