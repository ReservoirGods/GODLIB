#ifndef	INCLUDED_CLI_H
#define	INCLUDED_CLI_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

typedef	void	(*fCliCommandFunc)( const char * apArgs );


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	char	gCliString[ 1024 ];


#ifdef	dCLI

void			Cli_Init( void );
void			Cli_DeInit( void );
void			Cli_Update( void );
void			Cli_Main( void );
void			Cli_Main_ExitRequest( void );

void			Cli_FileProcess( char * apFileName );

void			Cli_CmdInit( char * apCommandString, fCliCommandFunc afCmd );
void			Cli_CmdDeInit( char * apCommandString );

void			Cli_PrintLine( const char * apTxt );

S32				Cli_GrabNumber( const char * apTxt );
U16 *			Cli_GetpSavedPal( void );
U16 *			Cli_GetpPal( void );


#define			Cli_Action( aAction )	aAction

#define			Cli_PrintfLine1( apTxt, apVal0 )									sprintf( gCliString, apTxt, apVal0 );									Cli_PrintLine( gCliString )
#define			Cli_PrintfLine2( apTxt, apVal0, apVal1 )							sprintf( gCliString, apTxt, apVal0, apVal1 );							Cli_PrintLine( gCliString )
#define			Cli_PrintfLine3( apTxt, apVal0, apVal1, apVal2 )					sprintf( gCliString, apTxt, apVal0, apVal1, apVal2 );					Cli_PrintLine( gCliString )
#define			Cli_PrintfLine4( apTxt, apVal0, apVal1, apVal2, apVal3 )			sprintf( gCliString, apTxt, apVal0, apVal1, apVal2, apVal3 );			Cli_PrintLine( gCliString )
#define			Cli_PrintfLine5( apTxt, apVal0, apVal1, apVal2, apVal3, apVal4 )	sprintf( gCliString, apTxt, apVal0, apVal1, apVal2, apVal3, apVal4 ); Cli_PrintLine( gCliString )

#define			Cli_Assert( aExpr )				\
	if( !(aExpr) )								\
	{											\
		Cli_PrintLine( #aExpr );				\
		Cli_Main();								\
	}

#else


#define			Cli_Init()
#define			Cli_DeInit()
#define			Cli_Update()
#define			Cli_Main()

#define			Cli_FileProcess( _a )

#define			Cli_CmdInit( apCommandString, afCmd )
#define			Cli_CmdDeInit( apCommandString )
#define			Cli_PrintLine( apTxt )

#define			Cli_GrabNumber( apTxt )	0
#define			Cli_GetpSavedPal()
#define			Cli_GetpPal()

#define			Cli_Action( aAction )

#define			Cli_PrintfLine1( apTxt, apVal0 )
#define			Cli_PrintfLine2( apTxt, apVal0, apVal1 )
#define			Cli_PrintfLine3( apTxt, apVal0, apVal1, apVal2 )
#define			Cli_PrintfLine4( apTxt, apVal0, apVal1, apVal2, apVal3 )
#define			Cli_PrintfLine5( apTxt, apVal0, apVal1, apVal2, apVal3, apVal4 )

#define			Cli_Assert( aExpr )

#endif


/* ################################################################################ */

#endif	/*	INCLUDED_CLI_H	*/
