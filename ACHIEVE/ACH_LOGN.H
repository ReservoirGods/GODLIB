
#ifndef	INCLUDED_ACH_LOGN_H
#define	INCLUDED_ACH_LOGN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

enum
{
	eACH_LOGIN_EVENT_NONE,
	eACH_LOGIN_EVENT_CHAR_IN,
	eACH_LOGIN_EVENT_MOVE_UP,
	eACH_LOGIN_EVENT_MOVE_DOWN,
	eACH_LOGIN_EVENT_ENTER,
	eACH_LOGIN_EVENT_DELETE,
};

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Login_AppInit( void );
void	Achieve_Login_AppDeInit( void );

void	Achieve_Login_Init( void );
void	Achieve_Login_DeInit( void );

U16		Achieve_Login_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey );
void	Achieve_Login_Event( const U16 aEvent );


/* ################################################################################ */

#endif	/*	INCLUDED_SIGN_H	*/
