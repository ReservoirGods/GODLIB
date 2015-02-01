#ifndef	INCLUDED_ASSERT_H
#define	INCLUDED_ASSERT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<STRING.H>
#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  VARIABLE PROTOTYPES
################################################################################### */

extern	char gAssertString[ 1024 ];


/* ###################################################################################
#  FUNCTION PROTOTYPES
################################################################################### */



/* ###################################################################################
#  MACROS
################################################################################### */


#ifdef dGODLIB_PLATFORM_WIN
#define	dAssertTXT

void	Assert_Display( const char * apString );
char *	Assert_GetpString( void );

#define Assert( aExpr )																						\
	if( !(aExpr) )																							\
	{																										\
		sprintf( gAssertString, "\nAssertion Failed\n%s\n%s\nLine:%d\n", #aExpr, __FILE__, __LINE__ );		\
		Assert_Display( Assert_GetpString() );																\
	}


#else

/*#define	Assert( aExp )		if( !aExp ) { *(U32*)0 = 0; }*/
#define	Assert( aExp )
#define	Assert_Display( apString )
#define	Assert_GetpString()	0

#endif


/* ################################################################################ */

#endif	/*	INCLUDED_ASSERT_H	*/
