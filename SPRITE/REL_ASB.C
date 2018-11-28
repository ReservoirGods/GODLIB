/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_ASB.H"

#include	"ASPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_ASBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_ASB_IsType(     sAsset * apAsset );
U32	Relocator_ASB_DoDelocate( sAsset * apAsset );
U32	Relocator_ASB_DoRelocate( sAsset * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_Init( void )
* ACTION   : Relocator_ASB_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_ASB_Init( void )
{
	Relocater_Init( &gRelocator_ASBReloc, "ASB", Relocator_ASB_IsType, 0, 0, Relocator_ASB_DoRelocate, Relocator_ASB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DeInit( void )
* ACTION   : Relocator_ASB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_ASB_DeInit( void )
{
	Relocater_DeInit( &gRelocator_ASBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_IsType( sAsset * apAsset )
* ACTION   : Relocator_ASB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_IsType( sAsset * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DoDelocate( sAsset * apAsset )
* ACTION   : Relocator_ASB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_DoDelocate( sAsset * apAsset )
{
	AsmSpriteBlock_Delocate( (sAsmSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DoRelocate( sAsset * apAsset )
* ACTION   : Relocator_ASB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_DoRelocate( sAsset * apAsset )
{
	AsmSpriteBlock_Relocate( (sAsmSpriteBlock*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
