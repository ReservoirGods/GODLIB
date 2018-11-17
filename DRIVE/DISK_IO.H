#ifndef INCLUDED_DISK_IO_H
#define INCLUDED_DISK_IO_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB\FILE\FILE.H>

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sBootSector
{
	U8	mBranch[2];					/* 0x00 */
	U8	mOEM[6];					/* 0x02 */
	U8	mSerialPattern[ 3 ];		/* 0x08 */
	U8	mBytesPerSector[2];			/* 0x0B */
	U8	mSectorsPerCluster;			/* 0x0D */
	U8	mReservedSectors[2];		/* 0x0E */
	U8	mNumberOfFATs;				/* 0x10 */
	U8	mNumberOfDirEntries[ 2 ];	/* 0x11 */
	U8	mNumberOfSectors[ 2 ];		/* 0x13 */
	U8	mMediaDescriptor;			/* 0x15 */
	U8	mSectorsPerFAT[2];			/* 0x16 */
	U8	mSectorsPerTrack[2];		/* 0x18 */
	U8	mNumberOfSides[2];			/* 0x1A */
	U8	mNumberOfHiddenSectors[2];	/* 0x1C */
	U8	mExecFlag[2];				/* 0x1E */
	U8	mLoadMode[ 2 ];				/* 0x20 */
	U8	mLoadSectorStart[ 2 ];		/* 0x22 */
	U8	mLoadSectorCount[ 2 ];		/* 0x24 */
	U8	mLoadAddress[ 4 ];			/* 0x26 */
	U8	mFATAddress[ 4 ];			/* 0x2A */
	U8	mFileName[ 2 ];				/* 0x2E */
	U8	mReserved;					/* 0x39 */
	U8	mSpace[ 0x200 - 0x42 ];		/* 0x40 */
	U8	mChecksum[2];
} sBootSector;

typedef struct sDiskFormatParameters
{
	U16	mSideCount;
	U16	mTrackCount;
	U16	mSectorsPerTrackCount;
	U16	mSectorByteCount;
}sDiskFormatParameters;

typedef struct sDiskImageDirEntry
{
	char	mFileName[ 8 ];
	char	mExtension[ 3 ];
	U8		mAttribute;
	struct sDiskImageDirEntry	*	mpSubDirectory;
	U16						mSubDirectoryEntryCount;
	U8		mReserved[ 4 ];
	U16		mTime;
	U16		mDate;
	U16		mFirstCluster;
	U32		mSize;
} sDiskImageDirEntry;


typedef struct sDiskImage
{
	U16							mSectorsPerCluster;
	U16							mSectorSizeBytes;
	U16							mClusterSizeBytes;
	U16							mClusterTotalCount;
	U16							mRootDirectoryEntryCount;
	U16							mDataSectorOffset;
	sBootSector *				mpBootSector;
	U8 *						mpFAT;
	sDiskImageDirEntry *		mpRootDirectory;
	sFileHandle					mFileHandle;
	struct sDiskImageFuncs *	mfFuncs;
} sDiskImage;

typedef struct sDiskImageFuncs
{
	void (* Init )( 		struct sDiskImage * apImage, const char * apFileName );
	void (* DeInit )( 		struct sDiskImage * apImage );

	void (*	SectorsRead )(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
	void (*	SectorsWrite )(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
} sDiskImageFuncs;


typedef struct sDiskImageDirWalker
{
	U16						mEntryIndex;
	U16						mEntryCount;
	sDiskImageDirEntry *	mpEntries;
	sDiskImage *			mpDiskImage;
} sDiskImageDirWalker;


extern sDiskImageFuncs	gfDiskImageFuncs_ST_Memory;
extern sDiskImageFuncs	gfDiskImageFuncs_ST_Streamed;


typedef struct sBootSector sDiskImageST;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void					DiskImage_Init( sDiskImage * apImage, sDiskImageFuncs * apFuncs, const char * apFileName );
void					DiskImage_DeInit( sDiskImage * apImage );

U8						DiskImage_File_Exists( sDiskImage * apImage, const char * apFileName );
U32						DiskImage_File_GetSize( sDiskImage * apImage, const char * apFileName );
void *					DiskImage_File_Load( sDiskImage * apImage, const char * apFileName );
U8						DiskImage_File_LoadAt( sDiskImage * apImage, const char * apFileName, void * apBuffer );

sDiskImageDirEntry *	DiskImage_Directory_Create(sDiskImage * apImage, const char * apFileName, U16 * apEntryCount );
U8						DiskImage_File_Save( sDiskImage * apImage, const char * apFileName, void * apBuffer, U32 aBytes );

void					DiskImage_DirWalker_Init( sDiskImage * apImage, const char * apDirName, sDiskImageDirWalker * apWalker );
void					DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker );

sDiskImageDirEntry *	DiskImage_DirWalker_Next( sDiskImageDirWalker * apWalker );

U16						DiskImage_FAT_GetLinkedClusterNext( sDiskImage * apImage, U16 aClusterIndex );
U16						DiskImage_FAT_GetLinkedClusterCount(  sDiskImage * apImage, U16 aClusterIndex );
U16						DiskImage_FAT_GetFreeCluster( sDiskImage * apImage );
U16						DiskImage_FAT_GetFreeClusterCount( sDiskImage * apImage );
void					DiskImage_FAT_SetNextClusterIndex( sDiskImage * apImage, U16 aClusterSrc, U16 aClusterNext );

/* old api */


void			Disk_BootSector_Init( sBootSector * apBoot, const sDiskFormatParameters * apParams );

sDiskImageST *	Disk_Image_ST_Create( const sDiskFormatParameters * apParams );
U32				Disk_Image_ST_GetSizeBytes( const sDiskImageST * apImage );

void			Disk_Image_ST_Save( const sDiskImageST * apImage, const char * apFileName );
sDiskImageST *	Disk_Image_ST_Load( const char * apFileName );
void			Disk_Image_ST_UnLoad( sDiskImageST * );

void *			Disk_Image_ST_FileLoad( const sDiskImageST * apImage, const char * apFileName );
void			Disk_Image_ST_FileUnLoad( void * apFile );

void 			Disk_Image_ST_FileDelete( sDiskImageST * apImage, const char * apFileName );
U8				Disk_Image_ST_FileSave( sDiskImageST * apImage, void * apBuffer, U32 aSize, const char * apFileName );

/* ################################################################################ */

#endif	/* INCLUDED_DISK_IO_H */