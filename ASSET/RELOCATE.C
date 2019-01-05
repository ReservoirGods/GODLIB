/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"RELOCATE.H"

#include	"ASSET.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sRelocater *	Relocater_Create( const char * apExt, fReloc aIsType, fReloc aDoInit, fReloc aDoDeInit, fReloc aDoRelocate, fReloc aDoDelocate );
void			Relocater_Destroy( sRelocater * apReloc );


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocaters = 0;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_Init( void )
* ACTION   : RelocaterManager_Init
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	RelocaterManager_Init( void )
{
	gpRelocaters = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_DeInit( void )
* ACTION   : RelocaterManager_DeInit
* CREATION : 30.11.2003 PNK
*-----------------------------------------------------------------------------------*/

void	RelocaterManager_DeInit( void )
{
	sRelocater *	lpReloc;
	sRelocater *	lpRelocNext;

	lpReloc = gpRelocaters;

	while( lpReloc )
	{
		lpRelocNext = lpReloc->mpNext;
		lpReloc->mpNext = 0;
		lpReloc     = lpRelocNext;
	}

	gpRelocaters = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocater_Init( sRelocater * apReloc )
* ACTION   : Relocater_Init
* CREATION : 24.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void			Relocater_Init( sRelocater * apReloc, const char * apExt, fReloc aIsType, fReloc aDoInit, fReloc aDoDeInit, fReloc aDoRelocate, fReloc aDoDelocate )
{
	apReloc->mExtID     = Asset_BuildHash( apExt, 4 );

	apReloc->mFunctions[ eRELOC_FUNCTION_IsType ]      = aIsType;
	apReloc->mFunctions[ eRELOC_FUNCTION_DoInit ]      = aDoInit;
	apReloc->mFunctions[ eRELOC_FUNCTION_DoDeInit ]    = aDoDeInit;
	apReloc->mFunctions[ eRELOC_FUNCTION_DoDelocate ]  = aDoDelocate;
	apReloc->mFunctions[ eRELOC_FUNCTION_DoRelocate ]  = aDoRelocate;

	apReloc->mpNext     = gpRelocaters;
	gpRelocaters        = apReloc;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocater_DeInit( sRelocater * apReloc )
* ACTION   : Relocater_DeInit
* CREATION : 24.11.2018 PNK
*-----------------------------------------------------------------------------------*/

void			Relocater_DeInit( sRelocater * apReloc )
{
	sRelocater *	lpReloc;
	sRelocater *	lpRelocLast;

	lpReloc     = gpRelocaters;
	lpRelocLast = 0;

	while( (lpReloc) && (lpReloc != apReloc) )
	{
		lpRelocLast = lpReloc;
		lpReloc     = lpReloc->mpNext;
	}

	if( lpReloc == apReloc )
	{
		if( lpRelocLast )
		{
			lpRelocLast->mpNext = lpReloc->mpNext;
		}
		else
		{
			gpRelocaters = lpReloc->mpNext;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_Find( sAssetItem * apAsset )
* ACTION   : RelocaterManager_Find
* CREATION : 08.12.2003 PNK
*-----------------------------------------------------------------------------------*/

sRelocater *	RelocaterManager_Find( sAssetItem * apAsset )
{
	sRelocater *	lpReloc;


	lpReloc = gpRelocaters;

	while( lpReloc )
	{
		if( lpReloc->mExtID == apAsset->mExtension )
		{
			if( lpReloc->mFunctions[ eRELOC_FUNCTION_IsType ]( apAsset ) )
			{
				return( lpReloc );
			}
		}
		lpReloc = lpReloc->mpNext;
	}

	DebugLog_Printf2( "relocator not found for asset %lX ext %lX\n", apAsset->mHashKey, apAsset->mExtension );

	return( lpReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_DoInit( sAssetItem * apAsset )
* ACTION   : RelocaterManager_DoInit
* CREATION : 08.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	RelocaterManager_DoInit( sAssetItem * apAsset )
{
	sRelocater *	lpReloc;
	U32				lRet;

/*	DebugLog_Printf0( "RelocaterManager_DoInit()" );*/
	lRet    = 0;

	if( !(apAsset->mStatusBits & eASSET_STATUS_BIT_INITED) )
	{
		lpReloc = RelocaterManager_Find( apAsset );

		if( lpReloc )
		{
			if( lpReloc->mFunctions[ eRELOC_FUNCTION_DoInit ] )
			{
				lRet = lpReloc->mFunctions[ eRELOC_FUNCTION_DoInit ]( apAsset );
				if( lRet )
				{
					apAsset->mStatusBits |= eASSET_STATUS_BIT_INITED;
				}
			}
		}
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_DoDeInit( sAssetItem * apAsset )
* ACTION   : RelocaterManager_DoDeInit
* CREATION : 08.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	RelocaterManager_DoDeInit( sAssetItem * apAsset )
{
	sRelocater *	lpReloc;
	U32				lRet;

	lRet    = 0;

	if( apAsset->mStatusBits & eASSET_STATUS_BIT_INITED )
	{
		lpReloc = RelocaterManager_Find( apAsset );

		if( lpReloc )
		{
			if( lpReloc->mFunctions[ eRELOC_FUNCTION_DoDeInit ] )
			{
				lRet = lpReloc->mFunctions[ eRELOC_FUNCTION_DoDeInit ]( apAsset );
			}
		}
		apAsset->mStatusBits &= ~eASSET_STATUS_BIT_INITED;
	}

	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_DoDelocate( sAssetItem * apAsset )
* ACTION   : RelocaterManager_DoDelocate
* CREATION : 08.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	RelocaterManager_DoDelocate( sAssetItem * apAsset )
{
	sRelocater *	lpReloc;
	U32				lRet;

	lRet = 0;
	if( apAsset->mStatusBits & eASSET_STATUS_BIT_RELOCATED )
	{
		lpReloc = RelocaterManager_Find( apAsset );

		if( lpReloc )
		{
			if( lpReloc->mFunctions[ eRELOC_FUNCTION_DoDelocate ] )
			{
				lRet = lpReloc->mFunctions[ eRELOC_FUNCTION_DoDelocate ]( apAsset );
			}
		}
		apAsset->mStatusBits &= ~eASSET_STATUS_BIT_RELOCATED;
	}
	return( lRet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RelocaterManager_DoRelocate( sAssetItem * apAsset )
* ACTION   : RelocaterManager_DoRelocate
* CREATION : 08.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	RelocaterManager_DoRelocate( sAssetItem * apAsset )
{
	sRelocater *	lpReloc;
	U32				lRet;

/*	DebugLog_Printf0( "RelocaterManager_DoRelocate()" );*/
	lRet = 0;
	if( !(apAsset->mStatusBits & eASSET_STATUS_BIT_RELOCATED) )
	{
		lpReloc = RelocaterManager_Find( apAsset );

		if( lpReloc )
		{
			if( lpReloc->mFunctions[ eRELOC_FUNCTION_DoRelocate ] )
			{
				lRet = lpReloc->mFunctions[ eRELOC_FUNCTION_DoRelocate ]( apAsset );
				if( lRet )
				{
					apAsset->mStatusBits |= eASSET_STATUS_BIT_RELOCATED;
				}
			}
		}
	}
	return( lRet );
}


/* ################################################################################ */
