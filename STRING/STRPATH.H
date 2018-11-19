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

typedef struct sStringPathSplitter
{
	const char * mpPath;
	char 		 mFileName[ 14 ];
} sStringPathSplitter;

struct sString;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */


void			StringPath_Set( sStringPath * apDst, const sStringPath * apSrc );
void			StringPath_SetNT( sStringPath * apPath, const char * apSrc );
void			StringPath_SetSS( sStringPath * apDst, const struct sString * apSrc );

void			StringPath_SetExt( sStringPath * apPath, const char * apExt );

const char *	StringPath_GetpExt( const char * apPath );
const char *	StringPath_GetpFileName( const char * apPath );

void			StringPath_SetFileName( sStringPath * apPath, const char * apFileName );
void			StringPath_SetFileNameNoExt( sStringPath * apPath, const char * apFileName );

void			StringPath_Append( sStringPath * apPath, const char * apFile );

void			StringPath_Combine( sStringPath * apPath, const char * apDir, const char * apFileName );
void			StringPath_Combine2( sStringPath * apPath, const char * apDir0, const char * apDir1, const char * apFileName );

char			StringPath_GetDrive( const char * apPath );
U16				StringPath_IsAbsolute( const char * apPath );

void			StringPath_GetDirectory( sStringPath * apDst, const char * apSrc  );
void			StringPath_GetFolder( sStringPath * apDst, const char * apSrc );

void			StringPath_Compact( sStringPath * apDst, const char * apSrc );

/* functions to iterate through folder hierarchy in a path. to avoid memory allocations, they perform temporary modifications on the underlying stringpath */

char *	StringPath_GetFolderFirst( sStringPath * apPath );
char *	StringPath_GetFolderNext( sStringPath * apPath, char * apPrevous );

void			StringPathSplitter_Init( sStringPathSplitter * apSpliter, const char * apPath );
const char *	StringPathSplitter_Next( sStringPathSplitter * apSpliter );


#define			StringPath_IsSeperator( _aChar )	( ('\\' ==(_aChar)) || ('/'==(_aChar)) )

#define			StringPath_Begin( _aPath )	( _aPath->mChars )
#define			StringPath_End( _aPath )	( &_aPath->mChars[ sizeof(sStringPath) - 1 ] )


/* ################################################################################ */

#endif	/* INCLUDED_STRPATH_H */
