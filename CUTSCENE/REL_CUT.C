/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_CUT.H"

#include	"CUTSCENE.H"
#include	"CUT_SYS.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_CUTReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_CUT_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_CUT_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_CUT_DoRelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_CUT_OnLoad(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_CUT_OnUnLoad(   void * apData, const U32 aSize, const U32 aID );


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
* FUNCTION : Relocator_CUT_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_CUT_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_OnLoad( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_CUT_OnLoad
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_OnLoad( void * apData,const U32 aSize,const U32 aID )
{
	sCutAsset *	lpAss;
	sCutScene *	lpCut;
	sCutVar *	lpVar;
	U16			i;

	(void)aID;
	(void)aSize;

	lpCut = (sCutScene*)apData;
	if( lpCut )
	{
		for( i=0; i<lpCut->mAssetCount; i++ )
		{
			lpAss           = &lpCut->mpAssets[ i ];
			lpAss->mpClient = AssetClient_Register( lpAss->mpFileName, lpAss->mpContext, 0, 0, (void**)&lpAss->mpData );
		}
		for( i=0; i<lpCut->mVarCount; i++ )
		{
			lpVar = &lpCut->mpVars[ i ];

			lpVar->mpVar = HashTree_VarRegister( CutScene_System_GetpHashTree(), lpVar->mpVarName );
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_OnUnLoad( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_CUT_OnUnLoad
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_OnUnLoad( void * apData,const U32 aSize,const U32 aID )
{
	sCutAsset *	lpAss;
	sCutScene *	lpCut;
	sCutVar *	lpVar;
	U16			i;

	(void)aID;
	(void)aSize;

	lpCut = (sCutScene*)apData;
	if( lpCut )
	{
		for( i=0; i<lpCut->mAssetCount; i++ )
		{
			lpAss           = &lpCut->mpAssets[ i ];
			AssetClient_UnRegister( lpAss->mpClient );
		}
		for( i=0; i<lpCut->mVarCount; i++ )
		{
			lpVar = &lpCut->mpVars[ i ];
			HashTree_VarUnRegister( CutScene_System_GetpHashTree(), lpVar->mpVar );
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_CUT_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	CutScene_Delocate( (sCutScene*)apData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_CUT_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_CUT_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_CUT_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	CutScene_Relocate( (sCutScene*)apData );
	return( 1 );
}


/* ################################################################################ */
