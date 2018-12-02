/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PACKAGE.H"

#include	"ASSET.H"
#include	"CONTEXT.H"
#include	"PKG_DIR.H"
#include	"PKG_LNK.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRPATH.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dPACKAGE_OPQ_LIMIT	32


/* ###################################################################################
#  ENUMES
################################################################################### */

enum
{
	ePACKAGEOP_NULL,
	ePACKAGEOP_LOAD,
	ePACKAGEOP_UNLOAD,
};

enum
{
	ePACKAGESTATUS_NOTLOADED,
	ePACKAGESTATUS_LOADING,
	ePACKAGESTATUS_LOADED,
	ePACKAGESTATUS_UNLOADING,
	ePACKAGESTATUS_UNLOADED,
	ePACKAGESTATUS_RELOCATING,
	ePACKAGESTATUS_RELOCATED,
};


enum
{
	ePACKAGELOADER_NONE,
	ePACKAGELOADER_DIR,
	ePACKAGELOADER_LNK
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16			mCmd;
	sPackage *	mpPackage;
} sPackageOp;


/* ###################################################################################
#  DATA
################################################################################### */

const char *		gpPackageManagerFilePath = "UNLINK";
const char *		gpPackageManagerLinkPath = "DATA";
sPackage *	gpPackages;
sPackageOp	gPackageOpQueue[ dPACKAGE_OPQ_LIMIT ];
S32			gPackageOpQueueHead;
S32			gPackageOpQueueTail;
U16			gPackageLinkEnableFlag;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32			Package_Load( sPackage * apPackage );
U32			Package_UnLoad( sPackage * apPackage );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_Init( void )
* ACTION   : PackageManager_Init
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_Init( void )
{
	gpPackages = 0;
	gPackageOpQueueHead = 0;
	gPackageOpQueueTail = 0;
	gPackageLinkEnableFlag = 1;

	ContextManager_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_DeInit( void )
* ACTION   : PackageManager_DeInit
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_DeInit( void )
{
	ContextManager_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_Update( void )
* ACTION   : PackageManager_Update
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_Update( void )
{
	sPackageOp *	lpOp;

	while(	gPackageOpQueueHead != gPackageOpQueueTail )
	{
		lpOp = &gPackageOpQueue[ gPackageOpQueueHead ];

		switch( lpOp->mCmd )
		{
		case	ePACKAGEOP_NULL:
			break;

		case	ePACKAGEOP_LOAD:
/*			lpOp->mpPackage->mStatus = ePACKAGESTATUS_LOADING;*/
			if( !Package_Load( lpOp->mpPackage ) )
			{
				return;
			}
/*			lpOp->mpPackage->mStatus = ePACKAGESTATUS_LOADED;*/
			break;

		case	ePACKAGEOP_UNLOAD:
/*			lpOp->mpPackage->mStatus = ePACKAGESTATUS_UNLOADING;*/
			if( !Package_UnLoad( lpOp->mpPackage ) )
			{
				return;
			}
/*			lpOp->mpPackage->mStatus = ePACKAGESTATUS_UNLOADED;*/
			switch( lpOp->mpPackage->mLoaderType )
			{
			case	ePACKAGELOADER_DIR:
				PackageDir_Destroy( lpOp->mpPackage );
				break;
			case	ePACKAGELOADER_LNK:
				PackageLnk_Destroy( lpOp->mpPackage );
				break;
			}
			lpOp->mpPackage->mStatus = ePACKAGESTATUS_NOTLOADED;
			break;

		default:
			break;
		}

		gPackageOpQueueHead++;
		if( gPackageOpQueueHead >= dPACKAGE_OPQ_LIMIT )
		{
			gPackageOpQueueHead = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_Load( sPackage * apPackage )
* ACTION   : PackageManager_Load
* CREATION : 05.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_Load( sPackage * apPackage )
{
	sPackageOp *	lpOp;


	lpOp = &gPackageOpQueue[ gPackageOpQueueTail ];

	lpOp->mCmd      = ePACKAGEOP_LOAD;
	lpOp->mpPackage = apPackage;

	gPackageOpQueueTail++;
	if( gPackageOpQueueTail >= dPACKAGE_OPQ_LIMIT )
	{
		gPackageOpQueueTail = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_UnLoad( sPackage * apPackage )
* ACTION   : PackageManager_UnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_UnLoad( sPackage * apPackage )
{
	sPackageOp *	lpOp;

	lpOp = &gPackageOpQueue[ gPackageOpQueueTail ];

	lpOp->mCmd      = ePACKAGEOP_UNLOAD;
	lpOp->mpPackage = apPackage;

	gPackageOpQueueTail++;
	if( gPackageOpQueueTail >= dPACKAGE_OPQ_LIMIT )
	{
		gPackageOpQueueTail = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_ReLoad( sPackage * apPackage )
* ACTION   : PackageManager_ReLoad
* CREATION : 05.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_ReLoad( sPackage * apPackage )
{
	switch( apPackage->mStatus )
	{
	case	ePACKAGESTATUS_NOTLOADED:
		PackageManager_Load( apPackage );
		break;

	case	ePACKAGESTATUS_LOADING:
	case	ePACKAGESTATUS_LOADED:
	case	ePACKAGESTATUS_RELOCATING:
	case	ePACKAGESTATUS_RELOCATED:
		PackageManager_UnLoad( apPackage );
		PackageManager_Load( apPackage );
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_SetLinkPath( const char * apPath )
* ACTION   : PackageManager_SetLinkPath
* CREATION : 02.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_SetLinkPath( const char * apPath )
{
	gpPackageManagerLinkPath = apPath;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_SetFilePath( const char * apPath )
* ACTION   : PackageManager_SetFilePath
* CREATION : 02.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_SetFilePath( const char * apPath )
{
	gpPackageManagerFilePath = apPath;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_SetLinkEnableFlag( U16 aLinkEnableFlag )
* ACTION   : PackageManager_SetLinkEnableFlag
* CREATION : 20.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void		PackageManager_SetLinkEnableFlag( U16 aLinkEnableFlag )
{
	gPackageLinkEnableFlag = aLinkEnableFlag;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Package_Init( sPackage * apPackage,const char * apName,const char * apContext )
* ACTION   : Package_Init
* CREATION : 05.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Package_Init( sPackage * apPackage,const char * apName,const char * apContext )
{
	U32			i;

	if( apPackage )
	{
		apPackage->mID         = Asset_BuildHash( apName, sizeof(apPackage->mName) );
		apPackage->mFileCount  = 0;
		apPackage->mLoaderType = ePACKAGELOADER_NONE;
		apPackage->mpContext   = ContextManager_ContextRegister( apContext );
		apPackage->mpItems     = 0;
		apPackage->mpNext      = gpPackages;
		apPackage->mStatus     = ePACKAGESTATUS_NOTLOADED;

		i = 0;
		while( (i<15) && (apName[i]) )
		{
			apPackage->mName[ i ] = apName[ i ];
			i++;
		}
		apPackage->mName[ i ] = 0;

		GOD_LL_INSERT( apPackage->mpContext->mpPackages, mpContextNext, apPackage );

		gpPackages           = apPackage;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Package_DeInit( sPackage * apPackage )
* ACTION   : Package_DeInit
* CREATION : 05.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Package_DeInit( sPackage * apPackage )
{
	sPackage *	lpPackage;
	sPackage *	lpPackageLast;

	lpPackage     = gpPackages;
	lpPackageLast = 0;

	while( (lpPackage) && (lpPackage != apPackage) )
	{
		lpPackageLast = lpPackage;
		lpPackage     = lpPackage->mpNext;
	}

	if( lpPackage == apPackage )
	{
		if( lpPackageLast )
		{
			lpPackageLast->mpNext = lpPackage->mpNext;
		}
		else
		{
			gpPackages = lpPackage->mpNext;
		}
	}
	ContextManager_ContextUnRegister( apPackage->mpContext );

	GOD_LL_REMOVE( sPackage, apPackage->mpContext->mpPackages, mpContextNext, apPackage );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Package_Load( sPackage * apPackage )
* ACTION   : Package_Load
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Package_Load( sPackage * apPackage )
{
	U32		lRet;
	sStringPath path;


	if( apPackage->mStatus != ePACKAGESTATUS_LOADED )
	{
		apPackage->mStatus     = ePACKAGESTATUS_LOADING;
		apPackage->mLoaderType = ePACKAGELOADER_NONE;

		if( gPackageLinkEnableFlag )
		{
			StringPath_Combine( &path, gpPackageManagerLinkPath, apPackage->mName );
			StringPath_SetExt( &path, ".LNK");
			lRet = PackageLnk_Load( apPackage, path.mChars );

			if( lRet )
			{
				apPackage->mLoaderType = ePACKAGELOADER_LNK;
			}
		}
		else
		{
			StringPath_Combine( &path, gpPackageManagerFilePath, apPackage->mName );
			lRet = PackageDir_Load( apPackage, path.mChars );

			if( lRet )
			{
				apPackage->mLoaderType = ePACKAGELOADER_DIR;
			}
		}
		if( lRet )
		{
			apPackage->mStatus = ePACKAGESTATUS_LOADED;
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Package_UnLoad( sPackage * apPackage )
* ACTION   : Package_UnLoad
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Package_UnLoad( sPackage * apPackage )
{
	U32			lRet;
/*	
	U32			i;
	sAsset *	lpAsset;
*/
	lRet = 1;

	if( (apPackage->mStatus != ePACKAGESTATUS_UNLOADED) || (apPackage->mStatus != ePACKAGESTATUS_NOTLOADED) )
	{
		apPackage->mStatus = ePACKAGESTATUS_UNLOADING;
#if 1
		apPackage->mStatus = ePACKAGESTATUS_UNLOADED;
#else
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
		if( lRet )
		{
			apPackage->mStatus = ePACKAGESTATUS_UNLOADED;
		}
#endif		
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_OpQueueIsEmpty( void )
* ACTION   : PackageManager_OpQueueIsEmpty
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	PackageManager_OpQueueIsEmpty( void )
{
	return( gPackageOpQueueHead == gPackageOpQueueTail );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_ShowAll( fPackagePrint aPrint )
* ACTION   : PackageManager_ShowAll
* CREATION : 11.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_ShowAll( fPackagePrint aPrint )
{
	(void)aPrint;
#if 0	
	sPackage *	lpPackage;
	sAsset *	lpAsset;
	sAssetClient *	lpClient;
	U16			lClientCount;
	char		lString[ 128 ];
	U32			i;

	lpPackage = gpPackages;

	while( lpPackage )
	{
		sprintf( lString, "PACKAGE : %s CON: %s", &lpPackage->mName[ 0 ], &lpPackage->mpContext->mName[ 0 ] );
		aPrint( lString );
		for( i=0; i<lpPackage->mFileCount; i++ )
		{
			lpAsset = lpPackage->mpItems[ i ].mpAsset;
			lClientCount = 0;
			lpClient = lpAsset->mpClients;
			while( lpClient )
			{
				lClientCount++;
				lpClient = lpClient->mpNext;
			}

			sprintf( lString, "%s %d", &lpAsset->mFileName[ 0 ], lClientCount );
			aPrint( lString );
		}
		lpPackage =lpPackage->mpNext;
	}
#endif	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_ShowUnused( fPackagePrint aPrint )
* ACTION   : PackageManager_ShowUnused
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_ShowUnused( fPackagePrint aPrint )
{
	(void)aPrint;
#if 0	
	sPackage *	lpPackage;
	sAsset *	lpAsset;
	sAssetClient *	lpClient;
	U16			lClientCount;
	char		lString[ 128 ];
	U32			i;

	lpPackage = gpPackages;

	while( lpPackage )
	{
		sprintf( lString, "PACKAGE : %s CON: %s", &lpPackage->mName[ 0 ], &lpPackage->mpContext->mName[ 0 ] );
		aPrint( lString );
		for( i=0; i<lpPackage->mFileCount; i++ )
		{
			lpAsset = lpPackage->mpItems[ i ].mpAsset;
			lClientCount = 0;
			lpClient = lpAsset->mpClients;
			while( lpClient )
			{
				lClientCount++;
				lpClient = lpClient->mpNext;
			}

			if( !lClientCount )
			{
				sprintf( lString, "%s %d", &lpAsset->mFileName[ 0 ], lClientCount );
				aPrint( lString );
			}
		}
		lpPackage =lpPackage->mpNext;
	}
#endif	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : PackageManager_ShowStatus( fPackagePrint aPrint )
* ACTION   : PackageManager_ShowStatus
* CREATION : 13.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	PackageManager_ShowStatus( fPackagePrint aPrint )
{
	sPackage *	lpPackage;
	char		lString[ 128 ];

	lpPackage = gpPackages;

	while( lpPackage )
	{
		sprintf( lString, "PACKAGE : %s CON: %s %d", &lpPackage->mName[ 0 ], &lpPackage->mpContext->mName[ 0 ], lpPackage->mStatus );
		aPrint( lString );
		lpPackage =lpPackage->mpNext;
	}
}


/* ################################################################################ */
