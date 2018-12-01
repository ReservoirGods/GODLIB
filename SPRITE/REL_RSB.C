/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_RSB.H"

#include	"RSPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_RSBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_RSB_IsType(     sAssetItem * apAsset );
U32	Relocator_RSB_DoDelocate( sAssetItem * apAsset );
U32	Relocator_RSB_DoRelocate( sAssetItem * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_Init( void )
* ACTION   : Relocator_RSB_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_RSB_Init( void )
{
	Relocater_Init( &gRelocator_RSBReloc, "RSB", Relocator_RSB_IsType, 0, 0, Relocator_RSB_DoRelocate, Relocator_RSB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DeInit( void )
* ACTION   : Relocator_RSB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_RSB_DeInit( void )
{
	Relocater_DeInit( &gRelocator_RSBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_IsType( sAssetItem * apAsset )
* ACTION   : Relocator_RSB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_IsType( sAssetItem * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DoDelocate( sAssetItem * apAsset )
* ACTION   : Relocator_RSB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_DoDelocate( sAssetItem * apAsset )
{
	RleSpriteBlock_Delocate( (sRleSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DoRelocate( sAssetItem * apAsset )
* ACTION   : Relocator_RSB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_DoRelocate( sAssetItem * apAsset )
{
	RleSpriteBlock_Relocate( (sRleSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
