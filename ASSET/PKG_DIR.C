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
#include	<GODLIB/FILE/FILE_PTN.H>
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
	sFilePattern pattern;

	apPackage->mFileCount = 0;
	if( FilePattern_Init( &pattern, apDirName ) )
	{
		while( FilePattern_Next( &pattern ) )
			apPackage->mFileCount++;
	}

	if( FilePattern_Init( &pattern, apDirName ) )
	{
		U16 itemIndex = 0;
		apPackage->mpItems = (sAssetItem*)mMEMCALLOC( sizeof(sAssetItem) * apPackage->mFileCount );
		for( ;FilePattern_Next( &pattern ) && (itemIndex<apPackage->mFileCount); itemIndex++ )
		{
			sAssetItem * item = &apPackage->mpItems[ itemIndex ];
			const char * pExt = StringPath_GetpExt(pattern.mPath.mChars);

			item->mSize = File_GetSize( pattern.mPath.mChars );
			item->mpData = File_Load( pattern.mPath.mChars );
			item->mHashKey = Asset_BuildHash( StringPath_GetpFileName(pattern.mPath.mChars), 12 );
			item->mStatusBits = eASSET_STATUS_BIT_LOADED;
			if( pExt )
			{
				if( '.' == *pExt )
					pExt++;
				item->mExtension = Asset_BuildHash( pExt, 4 );
			}

			RelocaterManager_DoRelocate( item );
			RelocaterManager_DoInit( item );
		}


		/* pass two - service clients */
		/* techinically this should iterate until no more clients need to be services, as deps could be multiple levels deep */

		for( itemIndex=0; itemIndex<apPackage->mFileCount; itemIndex++ )
		{
			sAssetItem * item = &apPackage->mpItems[ itemIndex ];
			sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, item->mHashKey );
			if( client )
			{
				AssetClients_OnLoad( client, item);
			}
		}

	}

	return 1;


#if 0
	sAsset *	lpAsset;
	sContext *	lpContext;
	U32			lIndex;
	char		lString[ 128 ];

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageDir_Load(): %s", apPackage->mName );

	sprintf( lString, "%s\\*.*", apDirName );

	apPackage->mFileCount = 0;
	lpContext             = apPackage->mpContext;

	File_SetDTA( &gPackageDirDTA );

	if( !File_ReadFirst( lString, dGEMDOS_FA_READONLY | dGEMDOS_FA_ARCHIVE | dGEMDOS_FA_DIR ) )
	{
		do
		{
			apPackage->mFileCount++;
		} while( !File_ReadNext() );
	}

	if( apPackage->mFileCount )
	{
		apPackage->mpItems = (sAssetItem*)mMEMCALLOC( sizeof(sAssetItem) * apPackage->mFileCount );

		lIndex = 0;
		if( !File_ReadFirst( lString, dGEMDOS_FA_READONLY | dGEMDOS_FA_ARCHIVE | dGEMDOS_FA_DIR ) )
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
				/* 2d0 fix */
				RelocaterManager_DoRelocate( 0 );
				RelocaterManager_DoInit( 0 );
				Asset_OnLoad( lpAsset );

				lIndex++;
			} while( (!File_ReadNext()) && (lIndex<apPackage->mFileCount) );
		}
	}

	return( apPackage->mFileCount );
#endif	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageDir_UnLoad( sPackage * apPackage )
* ACTION   : PackageDir_UnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageDir_UnLoad( sPackage * apPackage )
{
	U16 i;

	/* pass one - invoke unloads */

	for( i=0; i<apPackage->mFileCount; i++ )
	{
		sAssetItem * item = &apPackage->mpItems[ i ];
		sAssetClient * client = Context_AssetClient_Find( apPackage->mpContext, item->mHashKey );
		if( client )
		{
			AssetClients_OnUnLoad( client );
		}
	}

	/* pass two - invoke relocators */
	for( i=0; i<apPackage->mFileCount; i++ )
	{
		sAssetItem * item = &apPackage->mpItems[ i ];
		
		RelocaterManager_DoDeInit( item );
		RelocaterManager_DoDelocate( item );

		File_UnLoad( item->mpData );

		item->mpData = 0;
		item->mSize = 0;
	}

	return 1;

#if 0
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
#endif	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageDir_Destroy( sPackage * apPackage )
* ACTION   : PackageDir_Destroy
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageDir_Destroy( sPackage * apPackage )
{
	PackageDir_UnLoad( apPackage );
	if( apPackage->mpItems )
	{
		mMEMFREE( apPackage->mpItems );
	}
	apPackage->mFileCount = 0;

	(void)apPackage;
#if 0
	U32			i;
	sAsset *	lpAsset;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "PackageDir_Destroy(): %s", apPackage->mName );

	for( i=0; i<apPackage->mFileCount; i++ )
	{
		lpAsset = apPackage->mpItems[ i ].mpAsset;

		/* 2do fix */
		RelocaterManager_DoDeInit( 0 );
		RelocaterManager_DoDelocate( 0 );
		File_UnLoad( lpAsset->mpData );

		lpAsset->mpData  = 0;
		lpAsset->mSize   = 0;
		lpAsset->mStatus = eASSET_STATUS_NOTLOADED;
		Context_AssetUnRegister( apPackage->mpContext, lpAsset );
	}

	mMEMFREE( apPackage->mpItems );
	apPackage->mpItems = 0;
	apPackage->mFileCount = 0;
#endif	
}

U8		PackageDir_AssetLoad( sPackage * apPackage, struct sAssetClient * apClient )
{
	(void)apPackage;
	(void)apClient;
	return 0;
}

/* ################################################################################ */
