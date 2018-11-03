/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: LINKFILE.C
::
:: LinkFile routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"LINKFILE.H"

#include	<STRING.H>
#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/DRIVE/DRIVE.H>
#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/PACKER/GODPACK.H>
#include	<GODLIB/PACKER/PACKER.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/STRING/STRPATH.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dLINKFILE_ID		0x12345678L
#define	dLINKFILE_VERSION	0x0L


/* ###################################################################################
#  DATA
################################################################################### */

sGemDosDTA	gLinkFileDTA;

void *	gpLinkFileLoadingBuffer;
S32		gLinkFileLoadingSize;
U8		gLinkFileLoadingFlag;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sLinkFile *			LinkFile_Serialise( sLinkFile * apLinkFile );
void				LinkFile_RelocateFolder( sLinkFileFolder * apFolder, sLinkFile * apLinkFile);
void				LinkFile_Relocate(   sLinkFile * apLinkFile );
sLinkFileFile *		LinkFile_GetpFile(   sLinkFile * apLinkFile, char * apFileName );
sLinkFileFolder *	LinkFile_GetpFolder( sLinkFileFolder * apFolder, char * apFileName );
U32					LinkFile_GrabDirectory( char * apDst, char * apFileName );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Init( char * apLinkFileName )
* ACTION   : opens a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFile *	LinkFile_Init( char * apLinkFileName )
{
	sLinkFile 	lLinkFile;
	sLinkFile *	lpLinkFile;
	sFileHandle	lHandle;
	U32			lFatSize;

/*	printf( "LinkFile_Init() : %s\n", apLinkFileName );*/
	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init(): %s", apLinkFileName );

	lHandle = File_Open( apLinkFileName );
	if( !File_HandleIsValid(lHandle) )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init() : ERROR : couldn't open linkfile %s\n", apLinkFileName );
		return( (sLinkFile*)0 );
	}

	if( sizeof(sLinkFile) != File_Read( lHandle, sizeof(sLinkFile), &lLinkFile ) )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init() : ERROR : couldn't read header for %s \n", apLinkFileName );
		return( (sLinkFile*)0 );
	}

	Endian_ReadBigU32( &lLinkFile.mFatSize, lFatSize );
	lpLinkFile = (sLinkFile*)mMEMCALLOC( lFatSize );
	if( !lpLinkFile )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init() : ERROR : couldn't allocate %ld bytes for FAT\n", lLinkFile.mFatSize );
		return( (sLinkFile*)0 );
	}

	File_SeekFromStart( lHandle, 0 );
	if( (S32)lFatSize != File_Read( lHandle, lFatSize, lpLinkFile ) )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init() : ERROR : couldn't read FAT for %s\n", apLinkFileName );
		mMEMFREE( lpLinkFile );
		return( (sLinkFile*)0 );
	}

	LinkFile_Relocate( lpLinkFile );
	lpLinkFile->mFileHandle = lHandle;
	lpLinkFile->mInRamFlag  = 0;

	return( lpLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Init( char * apLinkFileName )
* ACTION   : opens a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFile *	LinkFile_InitToRAM( char * apLinkFileName )
{
	sLinkFile *		lpLinkFile;
	sPackerHeader	lPackHead;
	sFileHandle		lHandle;
	S32				lSize;
	U32 *			lpBuffer;
	U8 *			lpLoadBuffer;
/*	S32				lOffset;*/

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_InitToRAM(): %s", apLinkFileName );

	lHandle    = File_Open( apLinkFileName );
	lpLinkFile = 0;

	if( File_HandleIsValid(lHandle) )
	{
		File_Read( lHandle, sizeof(sPackerHeader), &lPackHead );
		File_Close( lHandle );

		if( Packer_IsPacked( &lPackHead ) )
		{
			lSize      = Packer_GetDepackSize( &lPackHead ) + dGODPACK_OVERFLOW;
			lpLinkFile = (sLinkFile*)mMEMALLOC( lSize );

			lpBuffer = (U32*)lpLinkFile;
/*
			for( lOffset=0; lOffset<(lSize>>2); lOffset++ )
			{
				lpBuffer[ lOffset ] = dLINKFILE_SENTINEL;
			}
*/
			gpLinkFileLoadingBuffer = lpBuffer;
			gLinkFileLoadingSize    = lSize;
			gLinkFileLoadingFlag    = 1;

			lpLoadBuffer = (U8*)lpLinkFile;
			lpLoadBuffer += Packer_GetLoadOffset( &lPackHead );
			File_LoadAt( apLinkFileName, lpLoadBuffer );

			gLinkFileLoadingFlag    = 0;
		}
		else
		{

			lSize    = File_GetSize( apLinkFileName );
			lpBuffer = 0;
			if( lSize > 0 )
			{

				lHandle = File_Open( apLinkFileName );
				if( File_HandleIsValid( lHandle ) )
				{
					lpBuffer = (U32*)mMEMALLOC( lSize );
/*
					for( lOffset=0; lOffset<(lSize>>2); lOffset++ )
					{
						lpBuffer[ lOffset ] = dLINKFILE_SENTINEL;
					}
*/
					gpLinkFileLoadingBuffer = lpBuffer;
					gLinkFileLoadingSize    = lSize;
					gLinkFileLoadingFlag    = 1;


					File_Read( lHandle, lSize, lpBuffer );

					File_Close( lHandle );

					gLinkFileLoadingFlag = 0;
				}
			}

			lpLinkFile = (sLinkFile*)lpBuffer;
			lpLoadBuffer = (U8*)lpLinkFile;
		}

		if( !lpLinkFile )
		{
			DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_Init() : ERROR : couldn't open linkfile %s\n", apLinkFileName );
		}
		else
		{
			Packer_Depack( lpLoadBuffer, lpLinkFile );
			lpLinkFile->mInRamFlag = 1;
			LinkFile_Relocate( lpLinkFile );
		}
	}

	return( lpLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_DeInit( sLinkFile * apLinkFile )
* ACTION   : closes a linkfile, frees memory
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void		LinkFile_DeInit(       sLinkFile * apLinkFile )
{
	if( apLinkFile )
	{
		if( !apLinkFile->mInRamFlag )
		{
			File_Close( apLinkFile->mFileHandle );
		}
		mMEMFREE( apLinkFile );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FileExists(   sLinkFile * apLinkFile, char * apFileName )
* ACTION   : returns non zero result if apFileName exists in apLinkFile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8			LinkFile_FileExists(   sLinkFile * apLinkFile, char * apFileName )
{
	return( (U8)(0 != LinkFile_GetpFile(apLinkFile,apFileName)));
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FileLoad(     sLinkFile * apLinkFile, char * apFileName, const U16 aDepackFlag )
* ACTION   : allocates memory for and loads apFileName from apLinkFile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void *		LinkFile_FileLoad(     sLinkFile * apLinkFile, char * apFileName, const U16 aDepackFlag, const U16 aSlowRamFlag )
{
	sLinkFileFile *	lpFile;
	U32				lSize;
	void *			lpMem;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_FileLoad(): %s", apFileName );

	lpMem = 0;
	if( !apLinkFile )
	{
		return( 0 );
	}
/*
	if( !File_HandleIsValid( apLinkFile->mFileHandle ) )
	{
		return( 0 );
	}
*/

	lpFile = LinkFile_GetpFile( apLinkFile, apFileName );
	if( !lpFile )
	{
		return( 0 );
	}

	if( apLinkFile->mInRamFlag )
	{
		lpMem = (void*)lpFile->mOffset;
	}
	else
	{
		if( !File_HandleIsValid( apLinkFile->mFileHandle ) )
		{
			return( 0 );
		}
		if( aDepackFlag )
		{
			lSize = lpFile->mUnPackedSize + dGODPACK_OVERFLOW;
		}
		else
		{
			lSize = lpFile->mSize;
		}

		if( lSize )
		{
			if( aSlowRamFlag )
			{
				lpMem = mMEMSCREENCALLOC( lSize );
			}
			else
			{
				lpMem = mMEMALLOC( lSize );
			}
			if( !lpMem )
			{
				DebugChannel_Printf2( eDEBUGCHANNEL_ASSET, "LinkFile_FileLoad() : ERROR : alloc failed file %s size %ld", apFileName, lSize );
				return( 0 );
			}

			File_SeekFromStart( apLinkFile->mFileHandle, lpFile->mOffset );
			File_Read( apLinkFile->mFileHandle, lpFile->mSize, lpMem );

			if( aDepackFlag )
			{
				Packer_Depack( lpMem, lpMem );
			}
		}
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FileLoad(     sLinkFile * apLinkFile, char * apFileName, const U16 aDepackFlag )
* ACTION   : loads apFileName to apBuffer from apLinkFile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8		LinkFile_FileLoadAt(   sLinkFile * apLinkFile, char * apFileName, void * apBuffer, const U16 aDepackFlag )
{
	sLinkFileFile *	lpFile;

/*	printf( "LinkFile_FileLoadAt() : %lX %s\n", apLinkFile, apFileName );*/
	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_FileLoadAt(): %s", apFileName );

	if( !apLinkFile )
	{
		return( 0 );
	}

	lpFile = LinkFile_GetpFile( apLinkFile, apFileName );
	if( !lpFile )
	{
		return( 0 );
	}

	if( apLinkFile->mInRamFlag )
	{
		Memory_Copy( lpFile->mSize, (void*)lpFile->mOffset, apBuffer );
	}
	else
	{
		if( !File_HandleIsValid(apLinkFile->mFileHandle) )
		{
			return( 0 );
		}
		File_SeekFromStart( apLinkFile->mFileHandle, lpFile->mOffset );
		File_Read( apLinkFile->mFileHandle, lpFile->mSize, apBuffer );
	}

	if( aDepackFlag )
	{
		Packer_Depack( apBuffer, apBuffer );
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FileGetSize(  sLinkFile * apLinkFile, char * apFileName, const U16 aUnpackedSizeFlag )
* ACTION   : loads apFileName to apBuffer from apLinkFile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

S32			LinkFile_FileGetSize(  sLinkFile * apLinkFile, char * apFileName, const U16 aUnpackedSizeFlag )
{
	sLinkFileFile *	lpFile;
	S32				lSize;

	if( !apLinkFile )
	{
		return( 0 );
	}
	lpFile = LinkFile_GetpFile( apLinkFile, apFileName );
	if( !lpFile )
	{
		return( 0 );
	}
	if( aUnpackedSizeFlag )
	{
		lSize = (S32)lpFile->mUnPackedSize;
	}
	else
	{
		lSize = (S32)lpFile->mSize;
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Create( void )
* ACTION   : creates a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFile *	LinkFile_Create( void )
{
	sLinkFile *			lpLinkFile;
	sLinkFileFolder *	lpFolder;
	char *				lpFolderName;

/*	mLinkFilePrintf( "LinkFile_Create()\n" );*/

	lpLinkFile = (sLinkFile*)mMEMCALLOC( sizeof(sLinkFile) );
	if( !lpLinkFile )
	{
		return( (sLinkFile*)0 );
	}
	lpFolder = (sLinkFileFolder*)mMEMCALLOC( sizeof(sLinkFileFolder) );
	if( !lpFolder )
	{
		mMEMFREE( lpLinkFile );
		return( (sLinkFile*)0 );
	}

	lpFolderName = (char*)mMEMCALLOC( 1 );
	if( !lpFolderName )
	{
		mMEMFREE( lpLinkFile );
		mMEMFREE( lpFolder   );
		return( (sLinkFile*)0 );
	}

	lpLinkFile->mFatSize    =  0;
	lpLinkFile->mID         = dLINKFILE_ID;
	lpLinkFile->mVersion    = dLINKFILE_VERSION;
	lpLinkFile->mpRoot      = lpFolder;

	lpFolder->mFileCount   = 0;
	lpFolder->mFolderCount = 0;
	lpFolder->mpFolderName = lpFolderName;

	lpFolderName[ 0 ] = 0;

	return( lpLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FileCreate( sLinkFile * apLinkFile, char * apFileName, char * apSrcDirectory )
* ACTION   : adds a file into a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U8		LinkFile_FileCreate( sLinkFileFolder * apFolder, char * apFileName )
{
	char				lString[ 256 ];
	sLinkFileFolder *	lpFolder;
	sLinkFileFile *		lpFiles;
	char *				lpFileName;
	U32					lDirLen;
	U32					lSize;
	U16					i;

/*	mLinkFilePrintf( "LinkFile_FileCreate() : %lX %s\n", apFolder, apFileName );*/

	lDirLen = LinkFile_GrabDirectory( lString, apFileName );

	if( lDirLen )
	{
		lpFolder = LinkFile_GetpFolder( apFolder, lString );
		if( !lpFolder )
		{
			lSize  = apFolder->mFolderCount+1;
			lSize *= sizeof(sLinkFileFolder);

			lpFolder = (sLinkFileFolder*)mMEMCALLOC( lSize );
			if( !lpFolder )
			{
				return( 0 );
			}
			lpFileName = (char*)mMEMCALLOC( lDirLen+1 );
			if( !lpFileName )
			{
				mMEMFREE( lpFolder );
				return( 0 );
			}
			strcpy( lpFileName, lString );
			for( i=0; i<apFolder->mFileCount; i++ )
			{
				lpFolder->mpFolders[ i ] = apFolder->mpFolders[ i ];
			}
			mMEMFREE( apFolder->mpFolders );
			apFolder->mpFolders = lpFolder;

			lpFolder[ apFolder->mFolderCount ].mFileCount   = 0;
			lpFolder[ apFolder->mFolderCount ].mFolderCount = 0;
			lpFolder[ apFolder->mFolderCount ].mpFiles      = 0;
			lpFolder[ apFolder->mFolderCount ].mpFolderName = lpFileName;
			lpFolder[ apFolder->mFolderCount ].mpFolders    = 0;

			lpFolder = &apFolder->mpFolders[ apFolder->mFolderCount ];

			apFolder->mFolderCount++;
		}
		strcpy( lString, &apFileName[ lDirLen+1 ] );
		return( LinkFile_FileCreate( lpFolder, lString ) );
	}


	lSize  = apFolder->mFileCount+1;
	lSize *= sizeof(sLinkFileFile);
	lpFiles = (sLinkFileFile*)mMEMCALLOC( lSize );
	if( !lpFiles )
	{
		return( 0 );
	}
	lpFileName = (char*)mMEMCALLOC( strlen(apFileName)+1 );
	if( !lpFileName )
	{
		mMEMFREE( lpFiles );
		return( 0 );
	}
	strcpy( lpFileName, apFileName );

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		lpFiles[ i ] = apFolder->mpFiles[ i ];
	}
	if( apFolder->mpFiles )
	{
		mMEMFREE( apFolder->mpFiles );
	}
	apFolder->mpFiles = lpFiles;


	lpFiles[ apFolder->mFileCount ].mLoadedFlag   = 0;
	lpFiles[ apFolder->mFileCount ].mOffset       = 0;
	lpFiles[ apFolder->mFileCount ].mPackedFlag   = 0;
	lpFiles[ apFolder->mFileCount ].mpFileName    = lpFileName;
	lpFiles[ apFolder->mFileCount ].mSize         = 0;
	lpFiles[ apFolder->mFileCount ].mUnPackedSize = 0;
	apFolder->mFileCount++;

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_RelocateFolder( sLinkFileFolder * apFolder, U32 aBase )
* ACTION   : relocates a folder and all subfolders it contains
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_RelocateFolder( sLinkFileFolder * apFolder, sLinkFile * apLinkFile )
{
	U16	i;

	Endian_FromBigU32( &apFolder->mpFolders    );
	Endian_FromBigU32( &apFolder->mpFiles      );
	Endian_FromBigU32( &apFolder->mpFolderName );

	*(U32*)&apFolder->mpFolders    += (U32)apLinkFile;
	*(U32*)&apFolder->mpFiles      += (U32)apLinkFile;
	*(U32*)&apFolder->mpFolderName += (U32)apLinkFile;

	Endian_FromBigU16( &apFolder->mFileCount   );
	Endian_FromBigU16( &apFolder->mFolderCount );

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		LinkFile_RelocateFolder( &apFolder->mpFolders[ i ], apLinkFile );
	}
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		Endian_FromBigU32( &apFolder->mpFiles[ i ].mOffset       );
		Endian_FromBigU32( &apFolder->mpFiles[ i ].mSize         );
		Endian_FromBigU32( &apFolder->mpFiles[ i ].mUnPackedSize );
		Endian_FromBigU16( &apFolder->mpFiles[ i ].mPackedFlag   );
		Endian_FromBigU16( &apFolder->mpFiles[ i ].mLoadedFlag   );

		Endian_FromBigU32( &apFolder->mpFiles[ i ].mpFileName );
		*(U32*)&apFolder->mpFiles[ i ].mpFileName += (U32)apLinkFile;

		if( apLinkFile->mInRamFlag )
		{
			*(U32*)&apFolder->mpFiles[ i ].mOffset += (U32)apLinkFile;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_DelocateFolder( sLinkFileFolder * apFolder, U32 aBase )
* ACTION   : Delocates a folder and all subfolders it contains
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_DelocateFolder( sLinkFileFolder * apFolder, U32 aBase )
{
	U16	i;

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		LinkFile_DelocateFolder( &apFolder->mpFolders[ i ], aBase );
	}
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		*(U32*)&apFolder->mpFiles[ i ].mpFileName -= aBase;

		Endian_FromBigU32( &apFolder->mpFiles[ i ].mOffset       );
		Endian_FromBigU32( &apFolder->mpFiles[ i ].mSize         );
		Endian_FromBigU32( &apFolder->mpFiles[ i ].mUnPackedSize );
		Endian_FromBigU16( &apFolder->mpFiles[ i ].mPackedFlag   );
		Endian_FromBigU16( &apFolder->mpFiles[ i ].mLoadedFlag   );

		Endian_FromBigU32( &apFolder->mpFiles[ i ].mpFileName );
	}
	*(U32*)&apFolder->mpFolders    -= aBase;
	*(U32*)&apFolder->mpFiles      -= aBase;
	*(U32*)&apFolder->mpFolderName -= aBase;

	Endian_FromBigU32( &apFolder->mpFolders    );
	Endian_FromBigU32( &apFolder->mpFiles      );
	Endian_FromBigU32( &apFolder->mpFolderName );

	Endian_FromBigU16( &apFolder->mFileCount   );
	Endian_FromBigU16( &apFolder->mFolderCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Relocate( sLinkFile * apLinkFile )
* ACTION   : relocates a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_Relocate( sLinkFile * apLinkFile )
{
	Endian_FromBigU32( &apLinkFile->mFatSize );
	Endian_FromBigU32( &apLinkFile->mVersion );
	Endian_FromBigU16( &apLinkFile->mInRamFlag );
	Endian_FromBigU16( &apLinkFile->mTotalFolderCount );
	Endian_FromBigU32( &apLinkFile->mTotalFileCount );
	Endian_FromBigU32( &apLinkFile->mID );
	Endian_FromBigU32( &apLinkFile->mpRoot );
	*(U32*)&apLinkFile->mpRoot += (U32)apLinkFile;
	LinkFile_RelocateFolder( apLinkFile->mpRoot, apLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Delocate( sLinkFile * apLinkFile )
* ACTION   : delocates a linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_Delocate( sLinkFile * apLinkFile )
{
	LinkFile_DelocateFolder( apLinkFile->mpRoot, (U32)apLinkFile );
	*(U32*)&apLinkFile->mpRoot -= (U32)apLinkFile;
	Endian_FromBigU32( &apLinkFile->mFatSize );
	Endian_FromBigU32( &apLinkFile->mVersion );
	Endian_FromBigU16( &apLinkFile->mInRamFlag );
	Endian_FromBigU16( &apLinkFile->mTotalFolderCount );
	Endian_FromBigU32( &apLinkFile->mTotalFileCount );
	Endian_FromBigU32( &apLinkFile->mID );
	Endian_FromBigU32( &apLinkFile->mpRoot );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetpFolder( sLinkFileFolder * apFolder, char * apFolderName )
* ACTION   : returns a pointer to folder apFolderName if it exists, else 0
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFileFolder *	LinkFile_GetpFolder( sLinkFileFolder * apFolder, char * apFolderName )
{
	U16	i;

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		if( !strcmp( apFolder->mpFolders[ i ].mpFolderName, apFolderName ) )
		{
			return( &apFolder->mpFolders[ i ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetpFile( sLinkFile * apLinkFile, char * apFileName )
* ACTION   : returns a pointer to a sLinkFileFile struct for file in linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFileFile *	LinkFile_GetpFile( sLinkFile * apLinkFile, char * apFileName )
{
	char				lString[ 256 ];
	char *				lpFileName;
	U8					lChar;
	sLinkFileFolder *	lpFolder;
	U16					i;

	DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "LinkFile_GetpFile(): %s", apFileName );

	lpFolder = apLinkFile->mpRoot;

	i=0;

	lpFileName = apFileName;
	while( (i<256) && (*lpFileName) )
	{
		if( (*lpFileName == '/') || (*lpFileName=='\\') )
		{
			if( i )
			{
				lString[ i ] = 0;

				lpFolder = LinkFile_GetpFolder( lpFolder, lString );

				if( !lpFolder )
				{
					DebugChannel_Printf0( eDEBUGCHANNEL_ASSET, "Error: folder not found" );
					return( (sLinkFileFile*)0 );
				}
			}
		}
		else
		{
			lChar = *lpFileName;
			if( (lChar >= 'a') && ( lChar <= 'z' ) )
			{
				lChar += (U8)('A'-'a');
			}
			lString[ i ] = lChar;
			lpFileName++;
			i++;
		}
	}
	lString[ i ] = 0;

	if( i )
	{
		for( i=0; i<lpFolder->mFileCount; i++ )
		{
/*			DebugChannel_Printf1( eDEBUGCHANNEL_ASSET, "file: %s", lpFolder->mpFiles[ i ].mpFileName );*/
			if( !String_StrCmpi( lString, lpFolder->mpFiles[ i ].mpFileName ) )
			{
				DebugChannel_Printf3( eDEBUGCHANNEL_ASSET, "file found %s off %ld size %ld",
					lpFolder->mpFiles[ i ].mpFileName,
					lpFolder->mpFiles[ i ].mOffset,
					lpFolder->mpFiles[ i ].mSize );
				return( &lpFolder->mpFiles[ i ] );
			}
		}
	}

	return( (sLinkFileFile*)0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetStringSize( sLinkFileFolder * apFolder )
* ACTION   : returns total size of all strings in FAT
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_GetStringSize( sLinkFileFolder * apFolder )
{
	U32	lSize;
	U16	i;

	lSize = 0;

	lSize += (strlen(apFolder->mpFolderName) + 1L);

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		lSize += LinkFile_GetStringSize( &apFolder->mpFolders[ i ] );
	}
	for( i=0; i<apFolder->mFileCount; i++ )
	{
		lSize += (strlen(apFolder->mpFiles[ i ].mpFileName)+1L);
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetFileCount( sLinkFileFolder * apFolder )
* ACTION   : returns total number of files in linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_GetFileCount( sLinkFileFolder * apFolder )
{
	U32	lCount;
	U16	i;

	lCount = apFolder->mFileCount;
	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		lCount += LinkFile_GetFileCount( &apFolder->mpFolders[ i ] );
	}

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetDataSize( sLinkFileFolder * apFolder )
* ACTION   : returns size of all data in linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_GetDataSize( sLinkFileFolder * apFolder )
{
	U32	lSize = 0;
	U16	i;

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		lSize += apFolder->mpFiles[ i ].mSize;
	}
	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		lSize += LinkFile_GetDataSize( &apFolder->mpFolders[ i ] );
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GetFolderCount( sLinkFileFolder * apFolder )
* ACTION   : returns count of folders in linkfile
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_GetFolderCount( sLinkFileFolder * apFolder )
{
	U32	lCount;
	U16	i;


	lCount = 1;
	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		lCount += LinkFile_GetFolderCount( &apFolder->mpFolders[ i ] );
	}

	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_SerialiseFolders( sLinkFileFolder * apDstFolder, sLinkFileFolder * apSrcFolder )
* ACTION   : serialises folders
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFileFolder *	LinkFile_SerialiseFolders( sLinkFileFolder * apDstFolder, sLinkFileFolder * apSrcFolder )
{
	sLinkFileFolder *	lpFolder;
	U16					i;

	lpFolder = apDstFolder;

	*lpFolder = *apSrcFolder;
	apDstFolder++;

	lpFolder->mpFolders  = apDstFolder;
	apDstFolder         += lpFolder->mFolderCount;

	for( i=0; i<lpFolder->mFolderCount; i++ )
	{
		apDstFolder = LinkFile_SerialiseFolders( apDstFolder, &apSrcFolder->mpFolders[ i ] );
	}

	return( apDstFolder );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_SerialiseFiles( sLinkFileFolder * apFolder, sLinkFileFile * apFile )
* ACTION   : serialises files
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFileFile *	LinkFile_SerialiseFiles( sLinkFileFolder * apFolder, sLinkFileFile * apFile )
{
	U16				i;


	apFolder->mpFiles = apFile;
	apFile           += apFolder->mFileCount;

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		apFile = LinkFile_SerialiseFiles( &apFolder[ i ], apFile );
	}

	return( apFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_StringCopy( char * apDst, char * apSrc )
* ACTION   : string copy, returns pointer to end of string
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

char *	LinkFile_StringCopy( char * apDst, char * apSrc )
{

	while( *apSrc )
	{
		*apDst++ = *apSrc++;
	}
	*apDst++ = 0;

	return( apDst );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_SerialiseStrings( sLinkFileFolder * apDstFolder, sLinkFileFolder * apSrcFolder, char * apMem )
* ACTION   : serialises strings
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

char *	LinkFile_SerialiseStrings( sLinkFileFolder * apDstFolder, sLinkFileFolder * apSrcFolder, char * apMem )
{
	U16				i;

	apDstFolder->mpFolderName = apMem;
	apMem                     = LinkFile_StringCopy( apMem, apSrcFolder->mpFolderName );

	for( i=0; i<apDstFolder->mFileCount; i++ )
	{
		apDstFolder->mpFiles[ i ].mpFileName = apMem;
		apMem = LinkFile_StringCopy( apMem, apSrcFolder->mpFiles[ i ].mpFileName );
	}

	for( i=0; i<apDstFolder->mFolderCount; i++ )
	{
		apMem = LinkFile_SerialiseStrings( &apDstFolder->mpFolders[ i ], &apSrcFolder->mpFolders[ i ], apMem );
	}

	return( apMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_FolderFree( sLinkFileFolder * apFolder )
* ACTION   : releases memory allocated for folders, files & strings
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_FolderFree( sLinkFileFolder * apFolder )
{
	U16	i;

	mMEMFREE( apFolder->mpFolderName );

	if( apFolder->mFileCount )
	{
		for( i=0; i<apFolder->mFileCount; i++ )
		{
			mMEMFREE( apFolder->mpFiles[ i ].mpFileName );
		}
		mMEMFREE( apFolder->mpFiles );
		apFolder->mpFiles = 0;
	}

	if( apFolder->mFolderCount )
	{
		for( i=0; i<apFolder->mFolderCount; i++ )
		{
			LinkFile_FolderFree( apFolder );
		}
		mMEMFREE( apFolder->mpFolders );
		apFolder->mpFolders = 0;
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Serialise( sLinkFile * apLinkFile )
* ACTION   : serialises the linkfile FAT
* CREATION : 02.09.01 PNK
*-----------------------------------------------------------------------------------*/

sLinkFile *		LinkFile_SerialiseFAT( sLinkFile * apLinkFile )
{
	sLinkFile *			lpLinkFile;
	U8 *				lpMem;
	U32					lStringSize;
	U32					lFileCount;
	U32					lFolderCount;
	U32					lSize;

	lStringSize  = LinkFile_GetStringSize(  apLinkFile->mpRoot );
	lFolderCount = LinkFile_GetFolderCount( apLinkFile->mpRoot );
	lFileCount   = LinkFile_GetFileCount(   apLinkFile->mpRoot );


	apLinkFile->mTotalFileCount   = lFileCount;
	apLinkFile->mTotalFolderCount = (U16)lFolderCount;

	lFileCount   *= sizeof( sLinkFileFile   );
	lFolderCount *= sizeof( sLinkFileFolder );

	lSize = lFileCount + lFolderCount + lStringSize + sizeof(sLinkFile);

	lpLinkFile = (sLinkFile*)mMEMCALLOC( lSize );
	if( !lpLinkFile )
	{
		return( (sLinkFile*)0 );
	}

	*lpLinkFile = *apLinkFile;


	lpLinkFile->mFatSize = (lSize+3L)&0xFFFFFFFCL;


	lpMem  = (U8*)lpLinkFile;
	lpMem += sizeof(sLinkFile);
	lpLinkFile->mpRoot = (sLinkFileFolder*)lpMem;

	lpMem = (U8*)LinkFile_SerialiseFolders( lpLinkFile->mpRoot, apLinkFile->mpRoot     );
	lpMem = (U8*)LinkFile_SerialiseFiles(   lpLinkFile->mpRoot, (sLinkFileFile*)lpMem );
	lpMem = (U8*)LinkFile_SerialiseStrings( lpLinkFile->mpRoot, apLinkFile->mpRoot, (char*)lpMem );

	LinkFile_FolderFree( apLinkFile->mpRoot );
	mMEMFREE( apLinkFile );

	return( lpLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_SetFileOffsets( sLinkFileFolder * apFolder,U32 aOffset,char * apSrcDirectory )
* ACTION   : LinkFile_SetFileOffsets
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_SetFileOffsets( sLinkFileFolder * apFolder,U32 aOffset,char * apSrcDirectory )
{
	sPackerHeader	lHeader;
	sFileHandle		lHandle;
	char			lString[ 128 ];
	S32				lSize;
	U16				i;


	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		sprintf( lString, "%s%s\\", apSrcDirectory, apFolder->mpFolders[ i ].mpFolderName );
		aOffset = LinkFile_SetFileOffsets( &apFolder->mpFolders[ i ], aOffset, lString );
	}

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sprintf( lString, "%s%s", apSrcDirectory, apFolder->mpFiles[ i ].mpFileName );
		lSize = File_GetSize( lString );
		if( lSize < 0 )
		{
			lSize = 0;
		}

		apFolder->mpFiles[ i ].mSize         = lSize;
		apFolder->mpFiles[ i ].mOffset       = aOffset;
		apFolder->mpFiles[ i ].mLoadedFlag   = 0;
		apFolder->mpFiles[ i ].mPackedFlag   = 0;
		apFolder->mpFiles[ i ].mUnPackedSize = lSize;

		lHeader.m0 = 0;
		lHeader.m1 = 0;
		lHeader.m2 = 0;

		lHandle = File_Open( lString );
		if( lHandle > 0 )
		{
			File_Read( lHandle, sizeof(sPackerHeader), &lHeader );
			File_Close( lHandle );
			if( Packer_IsPacked(&lHeader) )
			{
				apFolder->mpFiles[ i ].mPackedFlag   = 1;
				apFolder->mpFiles[ i ].mUnPackedSize = Packer_GetDepackSize( &lHeader );
			}
		}

		aOffset += ((lSize+3L)&0xFFFFFFFCL);
	}

	return( aOffset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_SaveData( sLinkFileFolder * apFolder,sFileHandle aHandle,char * apSrcDirectory )
* ACTION   : LinkFile_SaveData
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_SaveData( sLinkFileFolder * apFolder,sFileHandle aHandle,char * apSrcDirectory )
{
	char	lString[ 128 ];
	void *	lpFile;
	U16		i;

	for( i=0; i<apFolder->mFolderCount; i++ )
	{
		sprintf( lString, "%s%s\\", apSrcDirectory, apFolder->mpFolders[ i ].mpFolderName );
		LinkFile_SaveData( &apFolder->mpFolders[ i ], aHandle, lString );
	}

	for( i=0; i<apFolder->mFileCount; i++ )
	{
		sprintf( lString, "%s%s", apSrcDirectory, apFolder->mpFiles[ i ].mpFileName );
		lpFile = File_Load( lString );
		if( lpFile )
		{
			File_Write( aHandle, ((apFolder->mpFiles[ i ].mSize+3L)&0xFFFFFFFCL), lpFile );
			mMEMFREE( lpFile );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_Dump( sLinkFile * apLinkFile,char * apFileName,char * apSrcDirectory )
* ACTION   : LinkFile_Dump
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_Dump( sLinkFile * apLinkFile,char * apFileName,char * apSrcDirectory )
{
	sStringPath		lDir;
	sFileHandle		lHandle;
	sLinkFile *		lpLinkFile;
	S32				lSize;

	StringPath_GetDirectory( &lDir, apFileName );
	Drive_CreateDirectory( lDir.mChars );

	lHandle = File_Create( apFileName );

	if( !File_HandleIsValid(lHandle) )
		return;

	lpLinkFile = LinkFile_SerialiseFAT( apLinkFile );
	LinkFile_SetFileOffsets( lpLinkFile->mpRoot, lpLinkFile->mFatSize, apSrcDirectory );

	lSize = lpLinkFile->mFatSize;
	LinkFile_Delocate( lpLinkFile );
	File_Write( lHandle, lSize, lpLinkFile );
	LinkFile_Relocate( lpLinkFile );

	LinkFile_SaveData( lpLinkFile->mpRoot, lHandle, apSrcDirectory );

	File_Close( lHandle );

	mMEMFREE( lpLinkFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_GrabDirectory( char * apDst,char * apFileName )
* ACTION   : LinkFile_GrabDirectory
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	LinkFile_GrabDirectory( char * apDst,char * apFileName )
{
	U32	aIndex;
	U32	i;

	aIndex = 0;

	while( apFileName[ aIndex ] )
	{
		if( (apFileName[ aIndex ] == '/') || (apFileName[ aIndex ] =='\\') )
		{
			for( i=0; i<aIndex; i++ )
			{
				apDst[ i ] = apFileName[ i ];
			}
			apDst[ aIndex ] = 0;
			return( aIndex );
		}

		aIndex++;
	}
	apDst[ 0 ] = 0;

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_BuildFromDirectory( char * apDirectoryName,char * apLinkFileName )
* ACTION   : LinkFile_BuildFromDirectory
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_BuildFromDirectory( char * apDirectoryName,char * apLinkFileName )
{
	sGemDosDTA		lDta;
	sGemDosDTA *	lpOldDTA;
	sLinkFile *		lpLinkFile;
	S32				lLen;
	char *			lpDirectory;

	lpOldDTA    = GemDos_Fgetdta();

	lpDirectory = (char*)mMEMCALLOC( strlen(apDirectoryName) + 8 );
	if( lpDirectory )
	{
		strcpy( lpDirectory, apDirectoryName );
		lLen = strlen(apDirectoryName);
		if( lLen )
		{
			if( lpDirectory[ lLen-1 ] != '\\' )
			{
				lpDirectory[ lLen   ] = '\\';
				lpDirectory[ lLen+1 ] = 0;
				lLen++;
			}
		}

		strcat( lpDirectory, "*.*" );
		GemDos_Fsetdta( &lDta );

		if( 0 == GemDos_Fsfirst( lpDirectory, dGEMDOS_FA_READONLY | dGEMDOS_FA_ARCHIVE ) )
		{
			lpLinkFile = LinkFile_Create();
			if( lpLinkFile )
			{
				do
				{
					if(
						(lDta.mFileName[0] != '.') &&
						(lDta.mLength)
						)
					{
						LinkFile_FileCreate( lpLinkFile->mpRoot, &lDta.mFileName[0] );
					}
				} while( 0==GemDos_Fsnext() );
				lpDirectory[ lLen ] = 0;
				LinkFile_Dump( lpLinkFile, apLinkFileName, lpDirectory );
			}
		}

		GemDos_Fsetdta( lpOldDTA );
		mMEMFREE( lpDirectory );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : LinkFile_BuildFromFile( char * apFileName,char * apLinkFileName )
* ACTION   : LinkFile_BuildFromFile
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	LinkFile_BuildFromFile( char * apFileName,char * apLinkFileName )
{
	char		lDirectory[ 256 ];
	char		lFileName[ 256 ];
	sLinkFile *	lpLinkFile;
	char *		lpFile;
	S32			lFileSize;
	S32			lIndex;
	U32			i;

	lFileSize = File_GetSize( apFileName );

	if( lFileSize <= 0 )
	{
		return;
	}
	lpFile = (char*)File_Load( apFileName );
	if( !lpFile )
	{
		return;
	}


	lIndex     = 0;
	while( (lIndex<lFileSize) && (lpFile[ lIndex ] != 10) && (lpFile[ lIndex ] != 13) )
	{
		lDirectory[ lIndex ] = lpFile[ lIndex ];
		lIndex++;
	}
	lDirectory[ lIndex ] = 0;

	if( !lIndex )
	{
		return;
	}

	while( (lIndex<lFileSize) && ((lpFile[ lIndex ] == 10) || (lpFile[ lIndex ] == 13)) )
	{
		lIndex++;
	}


	lpLinkFile = LinkFile_Create();
	if( !lpLinkFile )
	{
		return;
	}

	while( lIndex < lFileSize )
	{
		i      = 0;
		while( (lIndex<lFileSize) && (lpFile[ lIndex ] != 10) && (lpFile[ lIndex ] != 13) )
		{
			lFileName[ i ] = lpFile[ lIndex ];
			lIndex++;
			i++;
		}
		lFileName[ i ] = 0;

		while( (lIndex<lFileSize) && ((lpFile[ lIndex ] == 10) || (lpFile[ lIndex ] == 13)) )
		{
			lIndex++;
		}

		if( i )
		{
			LinkFile_FileCreate( lpLinkFile->mpRoot, lFileName );
		}
	}

	LinkFile_Dump( lpLinkFile, apLinkFileName, lDirectory );
}


/* ################################################################################ */
