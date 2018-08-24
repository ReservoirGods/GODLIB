/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"BIOS.H"

/* ###################################################################################
#  CODE
################################################################################### */

#ifndef	dGODLIB_PLATFORM_ATARI

void			Bios_PipeConsole( void * apBuffer, U32 aBufferSize )
{
	(void)apBuffer;
	(void)aBufferSize;
}

void			Bios_UnPipeConsole( void )
{
}

U32		Bios_GetPipeOffset( void )
{
	return(0);
}

void		Bios_ClearPipeOffset( void )
{

}

#endif
