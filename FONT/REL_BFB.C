/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_BFB.H"

#include	"FONT.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_BFBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_BFB_IsType(     sAssetItem * apAsset );
U32	Relocator_BFB_DoDelocate( sAssetItem * apAsset );
U32	Relocator_BFB_DoRelocate( sAssetItem * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_Init( void )
* ACTION   : Relocator_BFB_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BFB_Init( void )
{
	Relocater_Init( &gRelocator_BFBReloc, "BFB", Relocator_BFB_IsType, 0, 0, Relocator_BFB_DoRelocate, Relocator_BFB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DeInit( void )
* ACTION   : Relocator_BFB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BFB_DeInit( void )
{
	Relocater_DeInit( &gRelocator_BFBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_IsType( sAssetItem * apAsset )
* ACTION   : Relocator_BFB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_IsType( sAssetItem * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DoDelocate( sAssetItem * apAsset )
* ACTION   : Relocator_BFB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_DoDelocate( sAssetItem * apAsset )
{
	Font_Delocate( (sFont*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DoRelocate( sAssetItem * apAsset )
* ACTION   : Relocator_BFB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_DoRelocate( sAssetItem * apAsset )
{
	Font_Relocate( (sFont*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
