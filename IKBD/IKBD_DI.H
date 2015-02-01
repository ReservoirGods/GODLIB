#ifndef	INCLUDED_IKBD_DI_H
#define	INCLUDED_IKBD_DI_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#ifdef	dGODLIB_SYSTEM_D3D
#include	<d3dx9.h>
#include	<windows.h>
#endif


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

#ifdef	dGODLIB_SYSTEM_D3D

void	IKBD_DI_Init( void );
void	IKBD_DI_DeInit( void );

void	IKBD_DI_Update( void );

void	IKBD_DI_SetWindowHandle( HWND aHwnd );

#else

#define	IKBD_DI_Init()
#define	IKBD_DI_DeInit()
#define	IKBD_DI_Update()

#endif


/* ################################################################################ */

#endif	/* INCLUDED_IKBD_DI_H */
