/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_GSM.H"

#include	"GSM.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_GSMReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_GSM_IsType(     sAssetItem * apAsset );
U32	Relocator_GSM_DoDelocate( sAssetItem * apAsset );
U32	Relocator_GSM_DoRelocate( sAssetItem * apAsset );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_Init( void )
* ACTION   : Relocator_GSM_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_GSM_Init( void )
{
	Relocater_Init( &gRelocator_GSMReloc, "GSM", Relocator_GSM_IsType, 0, 0, Relocator_GSM_DoRelocate, Relocator_GSM_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DeInit( void )
* ACTION   : Relocator_GSM_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_GSM_DeInit( void )
{
	Relocater_DeInit( &gRelocator_GSMReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_IsType( sAssetItem * apAsset )
* ACTION   : Relocator_GSM_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_IsType( sAssetItem * apAsset )
{
	(void)apAsset;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DoDelocate( sAssetItem * apAsset )
* ACTION   : Relocator_GSM_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_DoDelocate( sAssetItem * apAsset )
{
	Gsm_Delocate( (sGsm*)apAsset->mpData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DoRelocate( sAssetItem * apAsset )
* ACTION   : Relocator_GSM_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_DoRelocate( sAssetItem * apAsset )
{
	Gsm_Relocate( (sGsm*)apAsset->mpData );
	return( 1 );
}


/* ################################################################################ */
