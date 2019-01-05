#ifndef	INCLUDED_FILE_H
#define	INCLUDED_FILE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>

#ifdef	dGODLIB_SYSTEM_D3D
#include	<WINDOWS.H>
#endif


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	sFileHandle	S32


/* ###################################################################################
#  STRUCTS
################################################################################### */

enum
{
	eFILEIDENTIFIER_FILENAME,
	eFILEIDENTIFIER_MASK,
	eFILEIDENTIFIER_PATH,

	eFILEIDENTIFIER_LIMIT
};

typedef struct sFileIdentifier
{
	char *	mpStrings[ eFILEIDENTIFIER_LIMIT ];
	U32		mMallocSizes[ eFILEIDENTIFIER_LIMIT ];
} sFileIdentifier;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		File_Init( void );
void		File_DeInit( void );

sFileHandle	File_Open(   const char * apFname);
sFileHandle	File_Create( const char * apFname);
sFileHandle	File_OpenRW(   const char * apFname);

S32			File_Read(  sFileHandle aHandle, U32 aBytes, void * apBuffer );
S32			File_Write( sFileHandle aHandle, U32 aBytes, const void * apBuffer );
S32			File_WriteString( sFileHandle aHandle, const char * apString );
S32			File_Close( sFileHandle aHandle );

S32			File_SeekFromStart(   sFileHandle aHandle, S32 aOffset );
S32			File_SeekFromCurrent( sFileHandle aHandle, S32 aOffset );
S32			File_SeekFromEnd(     sFileHandle aHandle, S32 aOffset );

S32			File_Delete( const char * apFname);
S32			File_Rename( const char * apOldFname, const char * apNewFname );

S32			File_GetAttribute( const char * apFname);
S32			File_SetAttribute( const char * apFname, U8 aAttrib );

S32			File_GetDateTime( sFileHandle aHandle, sGemDosDateTime * apDateTime );
S32			File_SetDateTime( sFileHandle aHandle, sGemDosDateTime * apDateTime );

U32			File_GetTime( const char * apFname );

sGemDosDTA* File_GetDTA( void );
void		File_SetDTA( sGemDosDTA * apDTA );

S32			File_ReadFirst( const char * apFspec, U16 aAttribs );
S32			File_ReadNext( void );

U8			File_Exists(  const char * apFileName );
S32			File_GetSize( const char * apFileName );

void *		File_Load(   const char * apFileName );
void *		File_LoadSlowRam( const char * apFileName );
U8			File_LoadAt( const char * apFileName, void * apBuffer );
void		File_UnLoad( void * apMem );

U8			File_Save( const char * apFileName, const void * apBuffer, U32 aBytes );

U16			File_Selector( const char * apTitle, sFileIdentifier * apID );

#ifdef	dGODLIB_SYSTEM_D3D
void		File_SetWindowHandle( HWND aHandle );
#endif

char *		File_FileName_ToExistingPath( const char * apFileName );

void		File_Identifier_Init( sFileIdentifier * apID );
void		File_Identifier_DeInit( sFileIdentifier * apID );
void		File_Identifier_SetFileName( sFileIdentifier * apID, const char * apFileName );
void		File_Identifier_SetPath( sFileIdentifier * apID, const char * apPath );
void		File_Identifier_SetMask( sFileIdentifier * apID, const char * apMask );
void		File_Identifier_FromFullName( sFileIdentifier * apID, const char * apFullName );
char *		File_Identifier_ToFullName( sFileIdentifier * apID );

#ifdef dGODLIB_PLATFORM_ATARI

#define		File_HandleIsValid( aHandle )	(aHandle>0)

#else

#define		File_HandleIsValid( aHandle )	(aHandle!=0)

#endif

/* ################################################################################ */

#endif	/*	INCLUDED_FILE_H */
