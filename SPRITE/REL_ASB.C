/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_ASB.H"

#include	"ASPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocator_ASBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_ASB_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_ASB_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_ASB_DoRelocate( void * apData, const U32 aSize, const U32 aID );


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
	gpRelocator_ASBReloc = Relocater_Create( "ASB", Relocator_ASB_IsType, 0, 0, Relocator_ASB_DoRelocate, Relocator_ASB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DeInit( void )
* ACTION   : Relocator_ASB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_ASB_DeInit( void )
{
	Relocater_Destroy( gpRelocator_ASBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_ASB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_ASB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	AsmSpriteBlock_Delocate( (sAsmSpriteBlock*)apData );	
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_ASB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_ASB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_ASB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	AsmSpriteBlock_Relocate( (sAsmSpriteBlock*)apData );
	return( 1 );
}


/* ################################################################################ */
