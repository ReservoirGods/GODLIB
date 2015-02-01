/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_RSB.H"

#include	"RSPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocator_RSBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_RSB_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_RSB_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_RSB_DoRelocate( void * apData, const U32 aSize, const U32 aID );


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
	gpRelocator_RSBReloc = Relocater_Create( "RSB", Relocator_RSB_IsType, 0, 0, Relocator_RSB_DoRelocate, Relocator_RSB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DeInit( void )
* ACTION   : Relocator_RSB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_RSB_DeInit( void )
{
	Relocater_Destroy( gpRelocator_RSBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_RSB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_RSB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	RleSpriteBlock_Delocate( (sRleSpriteBlock*)apData );	
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_RSB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_RSB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_RSB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	RleSpriteBlock_Relocate( (sRleSpriteBlock*)apData );
	return( 1 );
}


/* ################################################################################ */
