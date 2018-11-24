/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REL_SPL.H"

#include	"AUDIO.H"
#include	<GODLIB/ASSET/RELOCATE.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  DATA
################################################################################### */

sRelocater 	gRelocator_SPLReloc;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32	Relocator_SPL_IsType(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_SPL_DoInit(     void * apData, const U32 aSize, const U32 aID );
U32	Relocator_SPL_DoDelocate( void * apData, const U32 aSize, const U32 aID );
U32	Relocator_SPL_DoRelocate( void * apData, const U32 aSize, const U32 aID );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_Init( void )
* ACTION   : Relocator_SPL_Init
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_SPL_Init( void )
{
	Relocater_Init( &gRelocator_SPLReloc, "SPL", Relocator_SPL_IsType, Relocator_SPL_DoInit, 0, Relocator_SPL_DoRelocate, Relocator_SPL_DoDelocate );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_DeInit( void )
* ACTION   : Relocator_SPL_DeInit
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Relocator_SPL_DeInit( void )
{
	Relocater_DeInit( &gRelocator_SPLReloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_IsType( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_SPL_IsType
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_SPL_IsType( void * apData,const U32 aSize,const U32 aID )
{
	(void)apData;
	(void)aSize;
	(void)aID;
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_DoDelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_SPL_DoDelocate
* CREATION : 19.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_SPL_DoDelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Audio_DmaSound_Delocate( (sAudioDmaSound*)apData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_DoRelocate( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_SPL_DoRelocate
* CREATION : 06.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_SPL_DoRelocate( void * apData,const U32 aSize,const U32 aID )
{
	(void)aSize;
	(void)aID;
	Audio_DmaSound_Relocate( (sAudioDmaSound*)apData );
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Relocator_SPL_DoInit( void * apData,const U32 aSize,const U32 aID )
* ACTION   : Relocator_SPL_DoInit
* CREATION : 27.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Relocator_SPL_DoInit( void * apData,const U32 aSize,const U32 aID )
{
	U32	i;
	sAudioDmaSound *	lpSpl;
	S8 *				lpSnd;

	(void)aSize;
	(void)aID;

	lpSpl = (sAudioDmaSound*)apData;
	lpSnd = (S8*)lpSpl->mpSound;

	if( MCH_FALCON == System_GetMCH() )
	{
		for( i=0; i<lpSpl->mLength; i++ )
		{
			lpSnd[ i ] >>= 1;
		}
	}
	return( 1 );
}


/* ################################################################################ */
