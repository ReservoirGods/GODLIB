#ifndef	INCLUDED_VID_D3D_H
#define	INCLUDED_VID_D3D_H

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

void	VideoD3D_SetWindowHandle( HWND aHwnd );
S32		VideoD3D_Init( void );
void	VideoD3D_DeInit( void );
void	VideoD3D_DisplayScreen( const U16 * apScreen );
void	VideoD3D_ResolutionUpdate( void );
void	VideoD3D_VblFunc( void );

/*
void	VideoD3D_SetPal( const U16 * apPal );
void	VideoD3D_GetPal( U16 * apPal );
*/
#else

#define	VideoD3D_Init()	{}
#define	VideoD3D_DeInit()	{}
#define	VideoD3D_DisplayScreen( a )	{}
#define	VideoD3D_ResolutionUpdate()	{}
/*
#define	VideoD3D_SetPal( a )	{}
#define	VideoD3D_GetPal( a )	{}
*/

#endif

/* ################################################################################ */

#endif	/* INCLUDED_VID_D3D_H */
