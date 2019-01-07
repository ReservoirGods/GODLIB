#ifndef	INCLUDED_DBGCHAN_H
#define	INCLUDED_DBGCHAN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	char gDebugChannelString[ 1024 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

enum
{
	eDEBUGCHANNEL_DEST_CLI    = 1<<0,
	eDEBUGCHANNEL_DEST_FILE   = 1<<1,
	eDEBUGCHANNEL_DEST_SCREEN = 1<<2,
	eDEBUGCHANNEL_DEST_STEEM  = 1<<3,
	eDEBUGCHANNEL_DEST_ALL    = (eDEBUGCHANNEL_DEST_STEEM | eDEBUGCHANNEL_DEST_SCREEN | eDEBUGCHANNEL_DEST_FILE | eDEBUGCHANNEL_DEST_CLI )
};

enum
{
	eDEBUGCHANNEL_ASSET,
	eDEBUGCHANNEL_GAME,
	eDEBUGCHANNEL_GODLIB,
	eDEBUGCHANNEL_MEMORY,
	eDEBUGCHANNEL_TOOL,
	eDEBUGCHANNEL_USER,
	eDEBUGCHANNEL_LIMIT,
};


/* ###################################################################################
#  DEFINES
################################################################################### */
/*#define	dDEBUG*/
#ifdef	dDEBUG

#define	DebugChannel_Action( _aChannel, _aAction )		{if( DebugChannel_GetActiveFlag( _aChannel ) ){ _aAction }}
#define	DebugChannel_StringAdd( _aChannel, _aString )	{ DebugChannel_StringAdd_Internal( _aChannel, _aString ); }

#define	DebugChannel_Printf0( _aChannel, _aString )												{ DebugChannel_StringAdd_Internal( _aChannel, _aString ); }
#define	DebugChannel_Printf1( _aChannel, _aString, _aArg0 )										{ sprintf(gDebugChannelString,_aString,_aArg0); DebugChannel_StringAdd_Internal( _aChannel, gDebugChannelString ); }
#define	DebugChannel_Printf2( _aChannel, _aString, _aArg0, _aArg1 )								{ sprintf(gDebugChannelString,_aString,_aArg0,_aArg1); DebugChannel_StringAdd_Internal( _aChannel, gDebugChannelString ); }
#define	DebugChannel_Printf3( _aChannel, _aString, _aArg0, _aArg1, _aArg2 )						{ sprintf(gDebugChannelString,_aString,_aArg0,_aArg1,_aArg2); DebugChannel_StringAdd_Internal( _aChannel, gDebugChannelString ); }
#define	DebugChannel_Printf4( _aChannel, _aString, _aArg0, _aArg1, _aArg2, _aArg3 )				{ sprintf(gDebugChannelString,_aString,_aArg0,_aArg1,_aArg2,_aArg3); DebugChannel_StringAdd_Internal( _aChannel, gDebugChannelString ); }
#define	DebugChannel_Printf5( _aChannel, _aString, _aArg0, _aArg1, _aArg2, _aArg3, _aArg4 )		{ sprintf(gDebugChannelString,_aString,_aArg0,_aArg1,_aArg2,_aArg3,_aArg4); DebugChannel_StringAdd_Internal( _aChannel, gDebugChannelString ); }

void	DebugChannel_Activate( U16 aChannel, U16 aDestFlags );

#else

#define	DebugChannel_Action( _aChannel, _aAction )
#define	DebugChannel_StringAdd( _aChannel, _aString )

#define	DebugChannel_Printf0( _aChannel, _aString )
#define	DebugChannel_Printf1( _aChannel, _aString, _aArg0 )
#define	DebugChannel_Printf2( _aChannel, _aString, _aArg0, _aArg1 )
#define	DebugChannel_Printf3( _aChannel, _aString, _aArg0, _aArg1, _aArg2 )
#define	DebugChannel_Printf4( _aChannel, _aString, _aArg0, _aArg1, _aArg2, _aArg3 )
#define	DebugChannel_Printf5( _aChannel, _aString, _aArg0, _aArg1, _aArg2, _aArg3, _aArg4 )

#define	DebugChannel_Activate( aChannel, aDestFlags )

#endif

void	DebugChannel_AppInit( void );
void	DebugChannel_AppDeInit( void );

void	DebugChannel_DeActivate( U16 aChannel );
U8		DebugChannel_GetActiveFlag( U16 aChannel );

void	DebugChannel_StringAdd_Internal( U16 aChannel, const char * apString );


/* ################################################################################ */

#endif	/*	INCLUDED_DBGCHAN_H	*/
