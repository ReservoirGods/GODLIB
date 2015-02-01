/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_BSB.H"

#include	"SPRITE.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocator_BSBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_BSB_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_BSB_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_BSB_DoRelocate( void * apData, const U32 aSize, const U32 aID );


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
	gpRelocator_BSBReloc = Relocater_Create( "BSB", Relocator_BSB_IsType, 0, 0, Relocator_BSB_DoRelocate, Relocator_BSB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DeInit( void )
* ACTION   : Relocator_BSB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BSB_DeInit( void )
{
	Relocater_Destroy( gpRelocator_BSBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BSB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BSB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Sprite_BlockDelocate( (sSpriteBlock*)apData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BSB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BSB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BSB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Sprite_BlockRelocate( (sSpriteBlock*)apData );
	return( 1 );
}


/* ################################################################################ */
