/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_FED.H"

#include	"FED.H"
#include	<GODLIB/ASSET/RELOCATE.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_FEDReloc;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_FED_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_FED_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_FED_DoRelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_FED_DoInit(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_FED_DoDeInit(   void * apData, const U32 aSize, const U32 aID );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_Init( void )
* ACTION   : Relocator_FED_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_FED_Init( void )
{
	 Relocater_Init( &gRelocator_FEDReloc, "FED", Relocator_FED_IsType, Relocator_FED_DoInit, Relocator_FED_DoDeInit, Relocator_FED_DoRelocate, Relocator_FED_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_DeInit( void )
* ACTION   : Relocator_FED_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_FED_DeInit( void )
{
	Relocater_DeInit( &gRelocator_FEDReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_FED_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_FED_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_FED_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_FED_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Fed_Relocate( (sFedHeader*)apData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_FED_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_FED_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;

/*	Fed_Delocate( (sFedHeader*)apData );*/
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_DoInit( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_FED_DoInit
* CREATION : 21.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_FED_DoInit( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;

	Fed_Init( (sFedHeader*)apData );
	Fed_Begin( (sFedHeader*)apData, "MAIN" );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_FED_DoDeInit( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_FED_DoDeInit
* CREATION : 21.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_FED_DoDeInit( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;

	Fed_DeInit( (sFedHeader*)apData );
	return( 1 );
}


/* ################################################################################ */
