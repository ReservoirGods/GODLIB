/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_GSM.H"

#include	"GSM.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocator_GSMReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_GSM_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_GSM_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_GSM_DoRelocate( void * apData, const U32 aSize, const U32 aID );


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
	gpRelocator_GSMReloc = Relocater_Create( "GSM", Relocator_GSM_IsType, 0, 0, Relocator_GSM_DoRelocate, Relocator_GSM_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DeInit( void )
* ACTION   : Relocator_GSM_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_GSM_DeInit( void )
{
	Relocater_Destroy( gpRelocator_GSMReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_GSM_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_GSM_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Gsm_Delocate( (sGsm*)apData );	
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_GSM_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_GSM_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_GSM_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Gsm_Relocate( (sGsm*)apData );
	return( 1 );
}


/* ################################################################################ */
