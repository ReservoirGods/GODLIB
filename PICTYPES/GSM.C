/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GSM.H"


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Gsm_Relocate( sGsm * apGsm )
* ACTION   : Gsm_Relocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Gsm_Relocate( sGsm * apGsm )
{
	if( apGsm )
	{
		Endian_FromBigU32( &apGsm->mHeader.mID         );
		Endian_FromBigU16( &apGsm->mHeader.mBlockCount );
		Endian_FromBigU16( &apGsm->mHeader.mBlockXSize );
		Endian_FromBigU16( &apGsm->mHeader.mBlockYSize );
		Endian_FromBigU16( &apGsm->mHeader.mColourMode );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gsm_Delocate( sGsm * apGsm )
* ACTION   : Gsm_Delocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Gsm_Delocate( sGsm * apGsm )
{
	if( apGsm )
	{
		Endian_FromBigU32( &apGsm->mHeader.mID         );
		Endian_FromBigU16( &apGsm->mHeader.mBlockCount );
		Endian_FromBigU16( &apGsm->mHeader.mBlockXSize );
		Endian_FromBigU16( &apGsm->mHeader.mBlockYSize );
		Endian_FromBigU16( &apGsm->mHeader.mColourMode );
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gsm_FromCanvas( sCanvas * apCanvas )
* ACTION   : Gsm_FromCanvas
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sGsm *	Gsm_FromCanvas( sCanvas * apCanvas )
{
	(void)apCanvas;
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Gsm_ToCanvas( sGsm * apGsm )
* ACTION   : Gsm_ToCanvas
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sCanvas *	Gsm_ToCanvas( sGsm * apGsm )
{
	(void)apGsm;
	return( 0 );	
}


/* ################################################################################ */
