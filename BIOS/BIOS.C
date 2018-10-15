/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"BIOS.H"

/* ###################################################################################
#  CODE
################################################################################### */

#ifndef	dGODLIB_PLATFORM_ATARI

S32				Bios_Bconin( const S16 aDev )
{
	(void)aDev;
	return 0;
}
S32				Bios_Bconout( const S16 aDev, const U16 aChar )
{
	(void)aDev;
	(void)aChar;
	return 0;
}
S32				Bios_Bconstat( const S16 aDev )
{
	(void)aDev;
	return 0;
}
S32				Bios_Bcostat( const S16 aDev )
{
	(void)aDev;
	return 0;
}


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
