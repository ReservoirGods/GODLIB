#ifndef	INCLUDED_LINKFILE_H
#define	INCLUDED_LINKFILE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/FILE/FILE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dLINKFILE_SENTINEL	0x10FFEB0DL


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct sLinkFileFile
{
	U32		mSize;
	U32		mUnPackedSize;
	U32		mOffset;
	U16		mPackedFlag;
	U16		mLoadedFlag;
	char *	mpFileName;
} sLinkFileFile;


typedef	struct sLinkFileFolder
{
	U16							mFileCount;
	U16							mFolderCount;
	char *						mpFolderName;
	sLinkFileFile *				mpFiles;
	struct sLinkFileFolder *	mpFolders;
} sLinkFileFolder;


typedef	struct sLinkFile
{
	U32					mID;
	U32					mVersion;
	U32					mFatSize;
	U32					mTotalFileCount;
	U16					mInRamFlag;
	U16					mTotalFolderCount;
	sFileHandle			mFileHandle;
	sLinkFileFolder *	mpRoot;
} sLinkFile;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sLinkFile *	LinkFile_Init( char * apLinkFileName );
sLinkFile *	LinkFile_InitToRAM( char * apLinkFileName );
void		LinkFile_DeInit(       sLinkFile * apLinkFile );

U8			LinkFile_FileExists(   sLinkFile * apLinkFile, char * apFileName );
void *		LinkFile_FileLoad(     sLinkFile * apLinkFile, char * apFileName, const U16 aDepackFlag, const U16 aSlowRamFlag );
U8			LinkFile_FileLoadAt(   sLinkFile * apLinkFile, char * apFileName, void * apBuffer, const U16 aDepackFlag );
S32			LinkFile_FileGetSize(  sLinkFile * apLinkFile, char * apFileName, const U16 aUnpackedSizeFlag );

sLinkFile *	LinkFile_Create( void );
U8			LinkFile_FileCreate( sLinkFileFolder * apFolder, char * apFileName );
S32			LinkFile_Save( sLinkFile * apLinkFile, char * apFileName );
void		LinkFile_BuildFromDirectory( char * apDirectoryName, char * apLinkFileName );
void		LinkFile_BuildFromFile( char * apFileName, char * apLinkFileName );
void		LinkFile_Dump( sLinkFile * apLinkFile,char * apFileName,char * apSrcDirectory );


extern	void *	gpLinkFileLoadingBuffer;
extern	S32		gLinkFileLoadingSize;
extern	U8		gLinkFileLoadingFlag;

/* ################################################################################ */

#endif	/* INCLUDED_LINKFILE_H */
