#ifndef	INCLUDED_STRPATH_H
#define	INCLUDED_STRPATH_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sStringPath
{
	char	mChars[ 256 ];
} sStringPath;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

const char *	StringPath_GetpExt( sStringPath * apPath );
void			StringPath_SetExt( sStringPath * apPath, const char * apExt );

void			StringPath_Copy( sStringPath * apDst, const sStringPath * apSrc );

const char *	StringPath_GetpFileName( sStringPath * apPath );
void			StringPath_SetFileName( sStringPath * apPath, const char * apFileName );

void			StringPath_Set( sStringPath * apPath, const char * apFile );
void			StringPath_Append( sStringPath * apPath, const char * apFile );

void			StringPath_Combine( sStringPath * apPath, const char * apDir, const char * apFileName );
void			StringPath_Combine2( sStringPath * apPath, const char * apDir0, const char * apDir1, const char * apFileName );


/* ################################################################################ */

#endif	/* INCLUDED_STRPATH_H */
