/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_CUT.H"

#include	"CUTSCENE.H"
#include	"CUT_SYS.H"


#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/ASSET/CONTEXT.H>
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_CUTReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_CUT_IsType(     sAssetItem * apAsset );
U32	Relocator_CUT_DoDelocate( sAssetItem * apAsset );
U32	Relocator_CUT_DoRelocate( sAssetItem * apAsset );
U32	Relocator_CUT_OnLoad(     sAssetItem * apAsset );
U32	Relocator_CUT_OnUnLoad(   sAssetItem * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_Init( void )
* ACTION   : Relocator_CUT_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_CUT_Init( void )
{
	Relocater_Init( &gRelocator_CUTReloc, "CUT", Relocator_CUT_IsType, Relocator_CUT_OnLoad, Relocator_CUT_OnUnLoad, Relocator_CUT_DoRelocate, Relocator_CUT_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_DeInit( void )
* ACTION   : Relocator_CUT_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_CUT_DeInit( void )
{
	Relocater_DeInit( &gRelocator_CUTReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_IsType( sAssetItem * apAsset )
* ACTION   : Relocator_CUT_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_IsType( sAssetItem * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_OnLoad( sAssetItem * apAsset )
* ACTION   : Relocator_CUT_OnLoad
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_OnLoad( sAssetItem * apAsset )
{
	sCutAsset *	lpAss;
	sCutScene *	lpCut;
	sCutVar *	lpVar;
	U16			i;

	lpCut = (sCutScene*)apAsset->mpData;
	if( lpCut )
	{
		for( i=0; i<lpCut->mAssetCount; i++ )
		{
			lpAss           = &lpCut->mpAssets[ i ];
			AssetClient_Init( &lpAss->mClient, lpAss->mpFileName, lpAss->mpContext, (void**)&lpAss->mpData );
		}
		for( i=0; i<lpCut->mVarCount; i++ )
		{
			lpVar = &lpCut->mpVars[ i ];

			HashTree_VarClient_Init( &lpVar->mVarClient, CutScene_System_GetpHashTree(), lpVar->mpVarName, 0 );
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_OnUnLoad( sAssetItem * apAsset )
* ACTION   : Relocator_CUT_OnUnLoad
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_OnUnLoad( sAssetItem * apAsset )
{
	sCutAsset *	lpAss;
	sCutScene *	lpCut;
	sCutVar *	lpVar;
	U16			i;

	lpCut = (sCutScene*)apAsset->mpData;
	if( lpCut )
	{
		for( i=0; i<lpCut->mAssetCount; i++ )
		{
			lpAss           = &lpCut->mpAssets[ i ];
			AssetClient_DeInit( &lpAss->mClient );
		}
		for( i=0; i<lpCut->mVarCount; i++ )
		{
			lpVar = &lpCut->mpVars[ i ];
			HashTree_VarClient_DeInit( &lpVar->mVarClient, CutScene_System_GetpHashTree() );
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_DoDelocate( sAssetItem * apAsset )
* ACTION   : Relocator_CUT_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_DoDelocate( sAssetItem * apAsset )
{
	CutScene_Delocate( (sCutScene*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_DoRelocate( sAssetItem * apAsset )
* ACTION   : Relocator_CUT_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_DoRelocate( sAssetItem * apAsset )
{
	CutScene_Relocate( (sCutScene*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
