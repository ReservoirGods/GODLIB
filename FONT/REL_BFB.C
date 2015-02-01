/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_BFB.H"

#include	"FONT.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater *	gpRelocator_BFBReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_BFB_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_BFB_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_BFB_DoRelocate( void * apData, const U32 aSize, const U32 aID );


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
	gpRelocator_BFBReloc = Relocater_Create( "BFB", Relocator_BFB_IsType, 0, 0, Relocator_BFB_DoRelocate, Relocator_BFB_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DeInit( void )
* ACTION   : Relocator_BFB_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_BFB_DeInit( void )
{
	Relocater_Destroy( gpRelocator_BFBReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BFB_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;

	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BFB_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;

	Font_Delocate( (sFont*)apData );	
	return( 1 );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_BFB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_BFB_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_BFB_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;

	Font_Relocate( (sFont*)apData );
	return( 1 );
}


/* ################################################################################ */
