/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: FILE.C
::
:: Low level file access routines
::
:: This file contains all the platform specific file manipulation routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FILE.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DRIVE/DRIVE.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>

#ifdef dGODLIB_COMPILER_GCC
#include	<dirent.h>
#include	<sys/stat.h>
#endif

#ifdef	dGODLIB_SYSTEM_TOS
#ifndef __VBCC__
#include	"AES.H"
#endif
#endif

#ifdef	dGODLIB_PLATFORM_WIN
#include	<IO.H>
#include	<TIME.H>
#include	<WINDOWS.H>
#endif	

#define	mFILE_CHAR_IS_SLASH( _a )	(('\\'==(_a)) || ('/'==(_a)))


/* ###################################################################################
#  DATA
################################################################################### */

sGemDosDTA *	gpFileDTA = 0;
S32				gFileFindHandle;
U16				gFileFindAttribs;

#ifdef	dGODLIB_COMPILER_GCC
struct dirent *		gpFileDirentEntry;
DIR *				gpFileDirentDIR;
sFileIdentifier 	gFileFindID;
#endif

#ifdef	dGODLIB_PLATFORM_WIN
typedef	struct _finddata_t	_finddata_t;
#endif


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

#ifdef	dGODLIB_PLATFORM_WIN
void	File_FindDataToDTA( const _finddata_t * apFindData, sGemDosDTA * apDTA );
#endif
#ifdef	dGODLIB_COMPILER_GCC
void	File_StatToDTA( const struct stat * apStat, sGemDosDTA * apDTA, const char * apFileName );
#endif

void	File_FileIdentifier_SetString( sFileIdentifier * apID, const U16 aIndex, const char * apString );
extern	U16	File_FileSelectorAES( const char * apTitle, const char * apPath, const char * apFile );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Open( const char * apFname )
* ACTION   : opens a file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

sFileHandle		File_Open( const char * apFname )
{
#ifdef	dGODLIB_PLATFORM_ATARI
/*	change this because HATARI doesn't like opening files in readwrite mode
	return( (sFileHandle)GemDos_Fopen( apFname, dGEMDOS_S_READWRITE ) );*/ 
	return( (sFileHandle)GemDos_Fopen( apFname, dGEMDOS_S_READ ) );
#else	
	sFileHandle	lHandle;
	lHandle = (sFileHandle)fopen( apFname, "rb" );
	if( !lHandle )
	{
		lHandle = -1;
	}
	return( lHandle );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Create( const char * apFname )
* ACTION   : creates a file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

sFileHandle		File_Create( const char * apFname )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( (sFileHandle)GemDos_Fcreate( apFname, 0 ) );
#else	
	return( (sFileHandle)fopen( apFname, "wb" ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Read( sFileHandle aHandle U32 aBytes, void * apBuffer )
* ACTION   : reads aBytes from file with handle aHandle to buffer apBuffer
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_Read( sFileHandle aHandle, U32 aBytes, void * apBuffer )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fread( (S16)aHandle, aBytes, apBuffer ) );
#else	
	return( fread( apBuffer, 1, aBytes, (FILE*)aHandle ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Write( sFileHandle aHandle U32 aBytes, const void * apBuffer )
* ACTION   : writes aBytes from file with handle aHandle to buffer apBuffer
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_Write( sFileHandle aHandle, U32 aBytes, const void * apBuffer )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fwrite( (S16)aHandle, aBytes, apBuffer ) );
#else	
	return( fwrite( apBuffer, 1, aBytes, (FILE*)aHandle ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SeekFromStart( sFileHandle aHandle, S32 aOffset )
* ACTION   : seeks aOffset bytes from start of file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_SeekFromStart( sFileHandle aHandle, S32 aOffset )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fseek( aOffset, (S16)aHandle, SEEK_SET ) );
#else	
	return( fseek( (FILE*)aHandle, aOffset, SEEK_SET ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SeekFromCurrent( sFileHandle aHandle, S32 aOffset )
* ACTION   : seeks aOffset bytes from current file position
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_SeekFromCurrent( sFileHandle aHandle, S32 aOffset )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fseek( aOffset, (S16)aHandle, SEEK_CUR ) );
#else	
	return( fseek( (FILE*)aHandle, aOffset, SEEK_CUR ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SeekFromEnd( sFileHandle aHandle, S32 aOffset )
* ACTION   : seeks aOffset bytes from end of file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_SeekFromEnd( sFileHandle aHandle, S32 aOffset )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fseek( aOffset, (S16)aHandle, SEEK_END ) );
#else	
	return( fseek( (FILE*)aHandle, aOffset, SEEK_END ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Close( S32 aHandle )
* ACTION   : closes currently open file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_Close( sFileHandle aHandle )
{
	S32 lRes = -1;
#ifdef	dGODLIB_PLATFORM_ATARI
	lRes = GemDos_Fclose( (S16)aHandle );
#else
	if( aHandle )
	{
		lRes = fclose( (FILE*)aHandle );
	}
#endif
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Delete( const char * apFname )
* ACTION   : deletes a file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_Delete( const char * apFname )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fdelete( apFname ) );
#else
	return( remove( apFname ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Rename( const char * apOldFname, const char * apNewFname )
* ACTION   : renames a file - can also be used to move a file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_Rename( const char * apOldFname, const char * apNewFname )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Frename( 0, apOldFname, apNewFname ) );
#else
	return( rename( apOldFname, apNewFname ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_GetAttribute( const char * apFname )
* ACTION   : gets attribute of specified file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_GetAttribute( const char * apFname )
{
	return( GemDos_Fattrib( apFname, 0, 0 ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SetAttribute( const char * apFname, U8 aAttrib )
* ACTION   : sets attribute of specified file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_SetAttribute( const char * apFname, U8 aAttrib )
{
	return( GemDos_Fattrib( apFname, 1, aAttrib ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_GetTime( const char * apFname )
* ACTION   : File_GetTime
* CREATION : 12.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	File_GetTime( const char * apFname )
{
	U32	lTime;


#ifdef dGODLIB_PLATFORM_ATARI
	S32				lHandle;
	sGemDosDateTime	lDateTime;

	lTime = 0;
	lHandle = GemDos_Fopen( apFname, dGEMDOS_S_READWRITE );
	if( lHandle >= 0 )
	{
		GemDos_Fdatime( &lDateTime, (S16)lHandle, 0 );
		GemDos_Fclose( (S16)lHandle );
		lTime  =  (lDateTime.mPackedDateTime>>16L) & 31L;
		lTime |= ((lDateTime.mPackedDateTime>>21L) & 63L)<<5L;
		lTime |= ((lDateTime.mPackedDateTime>>27L) & 31L)<<11L;
		lTime |= ((lDateTime.mPackedDateTime     ) & 31L)<<16L;
		lTime |= ((lDateTime.mPackedDateTime>>5L ) & 15L)<<21L;
		lTime |= ((lDateTime.mPackedDateTime>>9L ) &127L)<<25L;
	}

#elif defined(dGODLIB_COMPILER_GCC)
	struct stat	lStat;
	sGemDosDTA	lDTA;

	lTime = 0;
	if( stat( apFname, &lStat ) >= 0 )
	{
		File_StatToDTA( &lStat, &lDTA, apFname );
		lTime  = lDTA.mDate << 16L;
		lTime |= lDTA.mTime;
	}

#elif defined(dGODLIB_PLATFORM_WIN)
	_finddata_t	lFindData;
	sGemDosDTA	lDTA;
	S32			lFindHandle;

	lTime       = 0;
	lFindHandle = _findfirst( apFname, &lFindData );
	if( lFindHandle >= 0 )
	{
		File_FindDataToDTA( &lFindData, &lDTA );
		_findclose( lFindHandle );
		lTime  = lDTA.mDate << 16L;
		lTime |= lDTA.mTime;
	}
#endif

	return( lTime );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_GetDateTime( sFileHandle aHandle, DOSTIME * apDateTime )
* ACTION   : gets date & time of specified file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_GetDateTime( sFileHandle aHandle, sGemDosDateTime * apDateTime )
{
	apDateTime->mPackedDateTime = 0;
	return( GemDos_Fdatime( apDateTime, (S16)aHandle, 0 ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SetDateTime( sFileHandle aHandle, DOSTIME * apDateTime )
* ACTION   : gets date & time of specified file
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_SetDateTime( sFileHandle aHandle, sGemDosDateTime * apDateTime )
{
	return( GemDos_Fdatime( apDateTime, (S16)aHandle, 1 ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SetDTA( DTA * apDTA )
* ACTION   : sets default DTA address
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

void	File_SetDTA( sGemDosDTA * apDTA )
{
	gpFileDTA = apDTA;
	GemDos_Fsetdta( apDTA );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_ReadFirst( const char * apFspec, U16 aAttribs )
* ACTION   : reads DTA of first file in directory with file spec apFspec
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_ReadFirst( const char * apFspec, U16 aAttribs )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fsfirst( apFspec, aAttribs ) );
#elif defined(dGODLIB_COMPILER_GCC)
	fprintf( stderr, "File_ReadFirst( %s, %d)\n", apFspec, aAttribs );
	gpFileDirentDIR   = opendir( apFspec );
	fprintf( stderr, "File_ReadFirst: gpFileDirentDIR %p\n", gpFileDirentDIR );
	gpFileDirentEntry = 0;
	if( gpFileDirentDIR )
	{
		struct stat lStat;
		char *	lpFullName;
		gpFileDirentEntry = readdir( gpFileDirentDIR );

		if( gpFileDirentEntry )
		{
			File_Identifier_Init( &gFileFindID );
			File_Identifier_SetPath( &gFileFindID, apFspec );

			File_Identifier_SetFileName( &gFileFindID, gpFileDirentEntry->d_name );
			lpFullName = File_Identifier_ToFullName( &gFileFindID );

			if( stat( lpFullName, &lStat ) >= 0 )
			{
				File_StatToDTA( &lStat, gpFileDTA, gpFileDirentEntry->d_name );
			}
			mMEMFREE( lpFullName );
			fprintf( stderr, "File_ReadFirst: gpFileDirentEntry %p\n", gpFileDirentEntry );
		}
	}
	return( (S32)(gpFileDirentEntry == 0) );
#elif defined(dGODLIB_PLATFORM_WIN)
	S32			lRes;
	_finddata_t	lFindData;

	lFindData.attrib = 0;
	gFileFindAttribs = (U16)(aAttribs | dGEMDOS_FA_ARCHIVE);
	gFileFindHandle  = _findfirst( apFspec, &lFindData );

	if( gFileFindHandle >= 0 )
	{
		lRes = 0;
		while( (lFindData.attrib & (~gFileFindAttribs)) && (!lRes) )
		{
			lRes = _findnext( gFileFindHandle, &lFindData );
		}
		File_FindDataToDTA( &lFindData, gpFileDTA );
	}
	return( gFileFindHandle < 0 );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_ReadNext()
* ACTION   : reads DTA of next file in directory with file spec from File_ReadFirst()
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_ReadNext()
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Fsnext() );
#elif defined(dGODLIB_COMPILER_GCC)
	fprintf( stderr, "File_ReadNext() gpFileDirentDIR %p\n", gpFileDirentDIR );
	gpFileDirentEntry = 0;

	if( gpFileDirentDIR )
	{
		struct stat lStat;
		char *	lpFullName;
		gpFileDirentEntry = readdir( gpFileDirentDIR );

		if( gpFileDirentEntry )
		{
			File_Identifier_SetFileName( &gFileFindID, gpFileDirentEntry->d_name );
			lpFullName = File_Identifier_ToFullName( &gFileFindID );

			if( stat( lpFullName, &lStat ) >= 0 )
			{
				File_StatToDTA( &lStat, gpFileDTA, gpFileDirentEntry->d_name );
			}
			mMEMFREE( lpFullName );
			fprintf( stderr, "File_ReadFirst: gpFileDirentEntry %p\n", gpFileDirentEntry );
		}
		else
		{
			File_Identifier_DeInit( &gFileFindID );
			closedir( gpFileDirentDIR );
		}
	}

	fprintf( stderr, "File_ReadNext() : result: %ld\n", (gpFileDirentEntry!=0));
	return( (S32)(gpFileDirentEntry==0) );

#elif defined(dGODLIB_PLATFORM_WIN)
	S32			lRes;
	_finddata_t	lFindData;

	lRes = 0;
	
	do
	{
		lRes = _findnext( gFileFindHandle, &lFindData );
	}while( (lFindData.attrib & (~gFileFindAttribs)) && (!lRes) );

	if( lRes )
	{
		_findclose( gFileFindHandle );
	}
	else
	{
		File_FindDataToDTA( &lFindData, gpFileDTA );
	}
	return( lRes );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Exists( const char * apFileName )
* ACTION   : checks to see if file exits
* RETURNS  : 1 : file exits
*            0 : file not found
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U8		File_Exists( const char * apFileName )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	sFileHandle	lHandle;

	lHandle = File_Open( apFileName );
	if( lHandle > 0 )
	{
		File_Close( lHandle );
		return( 1 );
	}
	return( 0 );
#else
	U8		lRes;
	FILE *	lpFile;

	lpFile = fopen( apFileName, "r" );
	if( lpFile )
	{	
		lRes = 1;
		fclose( lpFile );
	}
	else
	{
		lRes = 0;
	}
	return( lRes );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_GetSize( const char * apFileName )
* ACTION   : gets size of a file
* RETURNS  : file size (positive )
*            or negative error code
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		File_GetSize( const char * apFileName )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	sFileHandle	lHandle;
	S32				lSize;

	lHandle = File_Open( apFileName );
	if( lHandle <= 0 )
	{
		return( lHandle );
	}

	lSize = File_SeekFromEnd( lHandle, 0 );
	File_Close( lHandle );

	return( lSize );
#else
	FILE *	lpFile;
	S32		lSize;

	lpFile = fopen( apFileName, "rb" );
	if( lpFile )
	{
		fseek( lpFile, 0, SEEK_END );
		lSize  = ftell( lpFile );
		fseek( lpFile, 0, SEEK_SET );
		fclose( lpFile );
	}
	else
	{
		lSize = 0;
	}
	return( lSize );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Load( const char * apFileName )
* ACTION   : allocates memory and loads a file
* RETURNS  : pointer to file in memory
* CREATION : 11.01.99 PNK
*-----------------------------------------------------------------------------------*/

void *	File_Load( const char * apFileName )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	sFileHandle	lHandle;
	S32				lSize;
	void *			lpBuffer;

	lSize = File_GetSize( apFileName );
	if( lSize <= 0 )
	{
		return( 0 );
	}

	lHandle = File_Open( apFileName );
	if( lHandle <= 0 )
	{
		return( 0 );
	}

	lpBuffer = mMEMALLOC( lSize );

	File_Read( lHandle, lSize, lpBuffer );

	File_Close( lHandle );

	return( lpBuffer );
#else
	FILE *	lpFile;
	void *	lpMem;
	S32		lSize;

	lSize  = File_GetSize( apFileName );
	if( lSize )
	{
		lpMem  = mMEMALLOC( lSize );
		lpFile = fopen( apFileName, "rb" );
		fread( lpMem, 1, lSize, lpFile );
		fclose( lpFile );
	}
	else
	{
		lpMem = 0;
	}
	return( lpMem );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_LoadSlowRam( const char * apFileName )
* ACTION   : allocates memory and loads a file
* RETURNS  : pointer to file in memory
* CREATION : 11.01.99 PNK
*-----------------------------------------------------------------------------------*/

void *	File_LoadSlowRam( const char * apFileName )
{
	sFileHandle	lHandle;
	S32				lSize;
	void *			lpBuffer;

	lSize = File_GetSize( apFileName );
	if( lSize <= 0 )
	{
		return( 0 );
	}

	lHandle = File_Open( apFileName );
	if( lHandle <= 0 )
	{
		return( 0 );
	}

	lpBuffer = Memory_ScreenAlloc( lSize );

	File_Read( lHandle, lSize, lpBuffer );

	File_Close( lHandle );

	return( lpBuffer );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_LoadAt( const char * apFileName, void * apBuffer )
* ACTION   : loads a file to apBuffer
* RETURNS  : 1 load successful
*            0 load error
* CREATION:  11.01.99 PNK
*-----------------------------------------------------------------------------------*/

U8	File_LoadAt( const char * apFileName, void * apBuffer )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	sFileHandle	lHandle;
	S32				lSize;

	lSize = File_GetSize( apFileName );
	if( lSize <= 0 )
		return( 0 );

	lHandle = File_Open( apFileName );
	if( lHandle <= 0 )
	{
		return( 0 );
	}

	File_Read( lHandle, lSize, apBuffer );

	File_Close( lHandle );

	return( 1 );
#else
	FILE *	lpFile;
	S32		lSize;
	U8		lRes;

	lSize  = File_GetSize( apFileName );
	if( lSize )
	{
		lpFile = fopen( apFileName, "rb" );
		fread( apBuffer, 1, lSize, lpFile );
		fclose( lpFile );
		lRes = 1;
	}
	else
	{
		lRes = 0;
	}
	return( lRes );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_UnLoad( void * apMem )
* ACTION   : File_UnLoad
* CREATION : 05.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	File_UnLoad( void * apMem )
{
	mMEMFREE( apMem );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FileSave( const char * apFileName, const void * apBuffer, U32 aBytes )
* ACTION   : saves a file from apBuffer
* RETURNS  : 1 load successful
*            0 load error
* CREATION : 11.01.99 PNK
*-----------------------------------------------------------------------------------*/

U8	File_Save( const char * apFileName, const void * apBuffer, U32 aBytes )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	sFileHandle	lHandle;


	lHandle = File_Create( apFileName );
	if( lHandle <= 0 )
	{
		return( 0 );
	}

	File_Write( lHandle, aBytes, apBuffer);
	File_Close( lHandle );

	return( 1 );
#else
	FILE *	lpFile;

	lpFile = fopen( apFileName, "wb" );
	if( !lpFile )
	{
		return( 0 );
	}
	fwrite( apBuffer, 1, aBytes, lpFile );
	fclose( lpFile );
	return( 1 );
#endif
}


#ifdef	dGODLIB_COMPILER_GCC
/*-----------------------------------------------------------------------------------*
* FUNCTION : File_StatToDTA( const struct stat * apStat, sGemDosDTA * apDTA, const char * apFileName )
* ACTION   : File_StatToDTA
* CREATION : 5.1.2009 PNK
*-----------------------------------------------------------------------------------*/

void	File_StatToDTA( const struct stat * apStat, sGemDosDTA * apDTA, const char * apFileName )
{
	struct tm *	lpTime;
	S32			i,j;

/*	apDTA->mAttrib = apStat->st_attr;*/
	apDTA->mAttrib = 0;
	apDTA->mLength = apStat->st_size;
	apDTA->mTime   = 0;
	apDTA->mDate   = 0;
	for( i=0; i<14; i++ )
	{
		apDTA->mFileName[ i ] = 0;
	}


	i = 0;
	while( apFileName[ i ] )
	{
		i++;
	}
	while( (i) && ('/'!=apFileName[i]) && ('\\'!=apFileName[i]) && (':'!=apFileName[i]) )
	{
		i--;
	}
	if( ('/'==apFileName[i]) || ('\\'==apFileName[i]) || (':'==apFileName[i]) )
	{
		i++;
	}

	j = 0;
	while( (j<14) && (apFileName[i]) )
	{
		apDTA->mFileName[ j ] = apFileName[ i ];
		i++;
		j++;
	}


	lpTime = localtime( &apStat->st_mtime );

	apDTA->mTime  = 0;
	apDTA->mTime |= (lpTime->tm_sec>>1) & 31;
	apDTA->mTime |= (lpTime->tm_min & 63) << 5;
	apDTA->mTime |= (lpTime->tm_hour & 31) << 11;

	apDTA->mDate  = 0;
	apDTA->mDate |= lpTime->tm_mday & 31;
	apDTA->mDate |=((lpTime->tm_mon+1) & 15)<<5;
	apDTA->mDate |=((lpTime->tm_year-80) & 127)<<9;
}
#endif


#ifdef	dGODLIB_PLATFORM_WIN
/*-----------------------------------------------------------------------------------*
* FUNCTION : File_FindDataToDTA( const _finddata_t * apFindData,sGemDosDTA * apDTA )
* ACTION   : File_FindDataToDTA
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	File_FindDataToDTA( const _finddata_t * apFindData,sGemDosDTA * apDTA )
{
	U16	i;
	struct tm * lpTime;

	apDTA->mAttrib = (U8)apFindData->attrib;
	apDTA->mLength = apFindData->size;
	apDTA->mTime   = (U16)((apFindData->time_create) & 0xFFFF);
	apDTA->mDate   = (U16)((apFindData->time_create >> 16L) & 0xFFFF);

	for( i=0; i<14; i++ )
	{
		apDTA->mFileName[ i ] = apFindData->name[ i ];
	}

	
	lpTime = localtime( &apFindData->time_write );
	apDTA->mTime  = 0;
	apDTA->mTime |= (lpTime->tm_sec>>1) & 31;
	apDTA->mTime |= (lpTime->tm_min & 63) << 5;
	apDTA->mTime |= (lpTime->tm_hour & 31) << 11;

	apDTA->mDate  = 0;
	apDTA->mDate |= lpTime->tm_mday & 31;
	apDTA->mDate |=((lpTime->tm_mon+1) & 15)<<5;
	apDTA->mDate |=((lpTime->tm_year-80) & 127)<<9;
}
#endif


#ifdef	dGODLIB_SYSTEM_D3D

/*-----------------------------------------------------------------------------------*
* FUNCTION : File_SetWindowHandle( HWND aHandle )
* ACTION   : File_SetWindowHandle
* CREATION : 21.1.2008 PNK
*-----------------------------------------------------------------------------------*/
HWND	gFileWindowHandle;
void	File_SetWindowHandle( HWND aHandle )
{
	gFileWindowHandle = aHandle;
}

#endif


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Selector( const char * apTitle, sFileIdentifier * apID )
* ACTION   : File_Selector
* CREATION : 26.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	File_Selector( const char * apTitle, sFileIdentifier * apID )
{
#ifdef	dGODLIB_SYSTEM_TOS
	U32	lLen;
	U16	lRes;
	lLen = String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_PATH ] );
	if( lLen && !mFILE_CHAR_IS_SLASH(apID->mpStrings[ eFILEIDENTIFIER_PATH ][ lLen-1 ]) )
	{
		String_StrCat( apID->mpStrings[ eFILEIDENTIFIER_PATH ], apID->mpStrings[ eFILEIDENTIFIER_PATH ], "\\" );
	}
	String_StrCat( apID->mpStrings[ eFILEIDENTIFIER_PATH ], apID->mpStrings[ eFILEIDENTIFIER_PATH ], apID->mpStrings[ eFILEIDENTIFIER_MASK ] );

	lRes = File_FileSelectorAES( apTitle, apID->mpStrings[ eFILEIDENTIFIER_PATH ], apID->mpStrings[ eFILEIDENTIFIER_FILENAME ] );

	lLen = String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_PATH ] );
	if( lLen )
	{
		lLen--;
		while( (lLen) && (!mFILE_CHAR_IS_SLASH(apID->mpStrings[ eFILEIDENTIFIER_PATH ][ lLen ])) )
		{
			lLen--;
		}
		apID->mpStrings[ eFILEIDENTIFIER_PATH ][ lLen ] = 0;
	}


	return( lRes );
#elif defined (dGODLIB_SYSTEM_D3D)

/* note: for this to work in vista lpstrfile must point to a file not a folder path */

	OPENFILENAME	lOpenFileName;
	U16				lRes;
	char			lString[ 1024 ];
	char			lPathOld[ 1024 ];
	int				lPathLen = strlen(apID->mpStrings[eFILEIDENTIFIER_PATH]);
	if( lPathLen )
	{
		if( '\\' == apID->mpStrings[eFILEIDENTIFIER_PATH][ lPathLen-1 ] )
		{
			sprintf( lString, "%s%s", apID->mpStrings[eFILEIDENTIFIER_PATH], apID->mpStrings[eFILEIDENTIFIER_FILENAME] );
		}
		else
		{
			sprintf( lString, "%s\\%s", apID->mpStrings[eFILEIDENTIFIER_PATH], apID->mpStrings[eFILEIDENTIFIER_FILENAME] );
		}
	}
	Memory_Clear( sizeof(OPENFILENAME), &lOpenFileName );
	lOpenFileName.lStructSize = sizeof(OPENFILENAME);
	lOpenFileName.lpstrFilter = "All Files\0*.*";
	lOpenFileName.lpstrDefExt = apID->mpStrings[ eFILEIDENTIFIER_MASK ];
	lOpenFileName.nMaxFile    = 1023;
	lOpenFileName.hwndOwner   = gFileWindowHandle;
	lOpenFileName.hInstance   = 0;
	lOpenFileName.lpstrFile   = lString;
	lOpenFileName.lpstrTitle  = apTitle;
	lOpenFileName.Flags       = OFN_SHOWHELP;
	lPathOld[0]=0;
	Drive_GetPath( 0, lPathOld );
	lRes = (U16)GetOpenFileName( &lOpenFileName );
	Drive_SetPath( lPathOld );
//	lOpenFileName.lpstrFile = "C:\\Program Files\\STEEM\\hard_drv\\SOURCE\\RG\\OVERACHR\\PRG\\UNLINK\\GLOBAL\\FONT8X8.BFB";
	File_Identifier_FromFullName( apID, lOpenFileName.lpstrFile );

	return( lRes );
#else
	(void)apTitle;
	(void)apID;
	return( 0 );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_FileName_ToExistingPath( const char * apFileName )
* ACTION   : File_FileName_ToExistingPath
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

char *	File_FileName_ToExistingPath( const char * apFileName )
{
	char *	lpFileName;
	S32		lLen;

	lpFileName = (char*)mMEMCALLOC( String_StrLen(apFileName) + 8 );
	String_StrCpy( lpFileName, apFileName );

	if( File_Exists(lpFileName) )
	{
		lLen = String_StrLen( lpFileName );
		while( (lLen) && !mFILE_CHAR_IS_SLASH( lpFileName[ lLen ] ) )
		{
			lLen--;
		}
		lpFileName[ lLen ] = 0;
	}

	lLen = String_StrLen( lpFileName );
	if( lLen && (mFILE_CHAR_IS_SLASH(lpFileName[lLen-1]) ) )
	{
		lpFileName[ lLen-1 ] = 0;
	}

	while( lLen )
	{
		if( !File_ReadFirst( lpFileName, dGEMDOS_FA_DIR ) )
		{
			return( lpFileName );
		}
		lLen = String_StrLen( lpFileName );
		while( lLen && (!mFILE_CHAR_IS_SLASH(lpFileName[lLen]) ) )
		{
			lLen--;
		}
		lpFileName[ lLen ] = 0;
	}

	mMEMFREE( lpFileName );

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_Init( sFileIdentifier * apID )
* ACTION   : File_Identifier_Init
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_Init( sFileIdentifier * apID )
{
	char	lFileName[ 512 ];
	U32		lLen;
	U16		i;

	Assert( apID );

	Drive_GetPath( 0, lFileName );

	lLen = String_StrLen( lFileName );

	if( lLen < 128 )
	{
		lLen = 128;
	}

	apID->mMallocSizes[ eFILEIDENTIFIER_PATH ]     = lLen;
	apID->mMallocSizes[ eFILEIDENTIFIER_MASK ]     = 14;
	apID->mMallocSizes[ eFILEIDENTIFIER_FILENAME ] = 14;

	for( i=0; i<eFILEIDENTIFIER_LIMIT; i++ )
	{
		apID->mpStrings[ i ] = (char*)mMEMCALLOC( apID->mMallocSizes[ i ] + 1 ); 
	}
	String_StrCpy( apID->mpStrings[ eFILEIDENTIFIER_FILENAME ], "*.*" );
	String_StrCpy( apID->mpStrings[ eFILEIDENTIFIER_MASK ], "*.*" );
	apID->mpStrings[ eFILEIDENTIFIER_PATH ][ 0 ] = (char)(Drive_GetDrive() + 'A');
	apID->mpStrings[ eFILEIDENTIFIER_PATH ][ 1 ] = ':';
	String_StrCpy( &apID->mpStrings[ eFILEIDENTIFIER_PATH ][ 2 ], lFileName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_DeInit( sFileIdentifier * apID )
* ACTION   : File_Identifier_DeInit
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_DeInit( sFileIdentifier * apID )
{
	U16	i;

	Assert( apID );

	for( i=0; i<eFILEIDENTIFIER_LIMIT; i++ )
	{
		mMEMFREE( apID->mpStrings[ i ] );
		apID->mpStrings[ i ] = 0;
		apID->mMallocSizes[ i ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_FileIdentifier_SetString( sFileIdentifier * apID, const U16 aIndex, const char * apString )
* ACTION   : File_FileIdentifier_SetString
* CREATION : 26.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_FileIdentifier_SetString( sFileIdentifier * apID, const U16 aIndex, const char * apString )
{
	U32	lLen;

	Assert( apID );
	Assert( apString );
	Assert( aIndex < eFILEIDENTIFIER_LIMIT );

	lLen = String_StrLen( apString );

	if( apID->mpStrings[ aIndex ] )
	{
		if( String_StrLen( apID->mpStrings[ aIndex ] ) > apID->mMallocSizes[ aIndex ] )
		{
			mMEMFREE( apID->mpStrings[ aIndex ] );
			apID->mpStrings[ aIndex ]    = (char*)mMEMCALLOC( lLen+1 );
			apID->mMallocSizes[ aIndex ] = lLen;
		}
	}
	else
	{
		apID->mpStrings[ aIndex ]    = (char*)mMEMCALLOC( lLen+1 );
		apID->mMallocSizes[ aIndex ] = lLen;
	}
	String_StrCpy( apID->mpStrings[ aIndex ], apString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_SetFileName( sFileIdentifier * apID, const char * apFileName )
* ACTION   : File_Identifier_SetFileName
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_SetFileName( sFileIdentifier * apID, const char * apFileName )
{
	File_FileIdentifier_SetString( apID, eFILEIDENTIFIER_FILENAME, apFileName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_SetPath( sFileIdentifier * apID, const char * apPath )
* ACTION   : File_Identifier_SetPath
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_SetPath( sFileIdentifier * apID, const char * apPath )
{
	U32	lLen;

	File_FileIdentifier_SetString( apID, eFILEIDENTIFIER_PATH, apPath );

	lLen = String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_PATH ] );
	if( lLen )
	{
		if( mFILE_CHAR_IS_SLASH( apID->mpStrings[ eFILEIDENTIFIER_PATH ][ lLen-1 ] ) )
		{
			apID->mpStrings[ eFILEIDENTIFIER_PATH ][ lLen-1 ] = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_SetMask( sFileIdentifier * apID, const char * apMask )
* ACTION   : File_Identifier_SetMask
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_SetMask( sFileIdentifier * apID, const char * apMask )
{
	File_FileIdentifier_SetString( apID, eFILEIDENTIFIER_MASK, apMask );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_FromFullName( sFileIdentifier * apID, const char * apFullName )
* ACTION   : File_Identifier_FromFullName
* CREATION : 25.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	File_Identifier_FromFullName( sFileIdentifier * apID, const char * apFullName )
{
	S32	lLen;
	S32	lFileIndex;
	S32	lFileLen;
	S32	i;
	S32	lDotIndex;
	U16	lMaskFlag;
	char *	lpFileName;
	char *	lpPath;
	char *	lpMask;

	lLen = String_StrLen( apFullName );

	lpPath = (char*)mMEMCALLOC( lLen + 1 );
	String_StrCpy( lpPath, apFullName );

	lFileIndex = lLen-1;
	while( lFileIndex && (!mFILE_CHAR_IS_SLASH( apFullName[lFileIndex] ) ) )
	{
		lFileIndex--;
	}

	lpPath[ lFileIndex ] = 0;

	lFileLen = (lLen - lFileIndex) + 1;

	if( lFileLen < 14 )
	{
		lFileLen = 14;
	}


	lMaskFlag = 0;
	lDotIndex = 0;

	for( i=lFileIndex; i<lLen; i++ )
	{
		if( '*' == apFullName[i] )
		{
			lMaskFlag = 1;
		}
		if( '.' == apFullName[i] )
		{
			lDotIndex = i;
		}
	}

	lpFileName = (char*)mMEMCALLOC( lFileLen );
	lpMask = (char*)mMEMCALLOC( lFileLen );

	String_StrCpy( lpMask, "*.*" );

	if( !lMaskFlag )
	{
		String_StrCpy( &lpMask[ 1 ], &apFullName[ lDotIndex ] );
		String_StrCpy( lpFileName, &apFullName[ lFileIndex+1 ] );
	}
	else
	{
		String_StrCpy( lpMask, &apFullName[ lFileIndex+1 ] );
	}

	File_Identifier_SetFileName( apID, lpFileName );
	File_Identifier_SetPath( apID, lpPath );
	File_Identifier_SetMask( apID, lpMask );

	mMEMFREE( lpFileName );
	mMEMFREE( lpPath );
	mMEMFREE( lpMask );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : File_Identifier_ToFullName( sFileIdentifier * apID )
* ACTION   : File_Identifier_ToFullName
* CREATION : 26.1.2008 PNK
*-----------------------------------------------------------------------------------*/

char *	File_Identifier_ToFullName( sFileIdentifier * apID )
{
	char *	lpFullName;
	U32		lLen;
	U32		lPathLen;

	if( apID )
	{
		lLen  = String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_PATH ] );
		lLen += String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_FILENAME ] );
		lLen += 2;

		lpFullName = (char*)mMEMCALLOC( lLen );
		if( lpFullName )
		{
			lPathLen = String_StrLen( apID->mpStrings[ eFILEIDENTIFIER_PATH ] );
			if( lPathLen )
			{
				if( mFILE_CHAR_IS_SLASH( apID->mpStrings[eFILEIDENTIFIER_PATH][ lPathLen-1 ] ) )
				{
					sprintf( lpFullName, "%s%s", apID->mpStrings[eFILEIDENTIFIER_PATH], apID->mpStrings[eFILEIDENTIFIER_FILENAME] );
				}
				else
				{
					sprintf( lpFullName, "%s\\%s", apID->mpStrings[eFILEIDENTIFIER_PATH], apID->mpStrings[eFILEIDENTIFIER_FILENAME] );
				}
			}
		}
	}
	else
	{
		lpFullName = 0;
	}

	return( lpFullName );
}


/* ################################################################################ */
