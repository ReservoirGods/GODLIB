/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_BSB.H"

#include	"SPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_BSBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_BSB_IsType(     sAssetItem * apAsset );
U32	Relocator_BSB_DoDelocate( sAssetItem * apAsset );
U32	Relocator_BSB_DoRelocate( sAssetItem * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_Init( void )
* ACTION   : Relocator_BSB_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BSB_Init( void )
{
	Relocater_Init( &gRelocator_BSBReloc, "BSB", Relocator_BSB_IsType, 0, 0, Relocator_BSB_DoRelocate, Relocator_BSB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DeInit( void )
* ACTION   : Relocator_BSB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BSB_DeInit( void )
{
	Relocater_DeInit( &gRelocator_BSBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_IsType( sAssetItem * apAsset )
* ACTION   : Relocator_BSB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_IsType( sAssetItem * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DoDelocate( sAssetItem * apAsset )
* ACTION   : Relocator_BSB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_DoDelocate( sAssetItem * apAsset )
{
	Sprite_BlockDelocate( (sSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DoRelocate( sAssetItem * apAsset )
* ACTION   : Relocator_BSB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_DoRelocate( sAssetItem * apAsset )
{
	Sprite_BlockRelocate( (sSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
