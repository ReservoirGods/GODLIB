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
	U8	mFileName[ 11 ];			/* 0x2E */
	U8	mReserved;					/* 0x39 */
	U8	mSpace[ 0x200 - 0x3C ];		/* 0x3A */
	U8	mChecksum[2];
} sBootSector;

typedef struct sDiskFormatParameters
{
	U16	mRootDirectorySectorCount;
	U16	mSectorsPerCluster;
	U16	mSectorsPerFAT;
	U16	mSectorsPerTrackCount;
	U16	mSectorByteCount;
	U16	mSideCount;
	U16	mTrackCount;
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
	U16							mClusterSizeBytes;
	U16							mClusterTotalCount;
	U16							mDataSectorOffset;
	U16							mRootDirectoryEntryCount;
	U16							mSectorsPerCluster;
	U16							mSectorSizeBytes;
	U32							mTotalSizeBytes;
	sFileHandle					mFileHandle;
	struct sDiskImageFuncs *	mfFuncs;
	sBootSector *				mpBootSector;
	U8 *						mpFAT;
	sDiskImageDirEntry *		mpRootDirectory;
} sDiskImage;

typedef struct sDiskImageFuncs
{
	U8   (* Init )( 		struct sDiskImage * apImage, const char * apFileName );
	void (* DeInit )( 		struct sDiskImage * apImage );

	void (*	SectorsRead )(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
	void (*	SectorsWrite )(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );

	U8 (* Commit )( 		struct sDiskImage * apImage, const char * apFileName );

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

						/* sets default parameters for disk format */
void					DiskFormatParameters_Init(sDiskFormatParameters * apParams );

U8						DiskImage_Create( const sDiskFormatParameters * apParams, const char * apFileName );

U8						DiskImage_Load( sDiskImage * apImage, sDiskImageFuncs * apFuncs, const char * apFileName );
void					DiskImage_UnLoad( sDiskImage * apImage );
U8						DiskImage_Save( sDiskImage * apImage, const char * apFileName );

U8						DiskImage_File_Exists( sDiskImage * apImage, const char * apFileName );
U32						DiskImage_File_GetSize( sDiskImage * apImage, const char * apFileName );
void *					DiskImage_File_Load( sDiskImage * apImage, const char * apFileName );
U8						DiskImage_File_LoadAt( sDiskImage * apImage, const char * apFileName, void * apBuffer );

U8						DiskImage_File_Save( sDiskImage * apImage, const char * apFileName, void * apBuffer, U32 aBytes );
U8						DiskImage_File_Delete( sDiskImage * apImage, const char * apFileName, void * apBuffer, U32 aBytes );

U8						DiskImage_Directory_Create( sDiskImage * apImage, const char * apDirName );

void					DiskImage_DirWalker_Init( sDiskImage * apImage, const char * apDirName, sDiskImageDirWalker * apWalker );
void					DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker );

sDiskImageDirEntry *	DiskImage_DirWalker_Next( sDiskImageDirWalker * apWalker );

U16						DiskImage_FAT_GetLinkedClusterNext( sDiskImage * apImage, U16 aClusterIndex );
U16						DiskImage_FAT_GetLinkedClusterCount(  sDiskImage * apImage, U16 aClusterIndex );
U16						DiskImage_FAT_GetFreeCluster( sDiskImage * apImage, U16 aStartCluster );
U16						DiskImage_FAT_GetFreeClusterCount( sDiskImage * apImage );
void					DiskImage_FAT_SetNextClusterIndex( sDiskImage * apImage, U16 aClusterSrc, U16 aClusterNext );

#define					DiskImage_DirEntry_IsSpecialDirectory( apEntry )	( '.' == apEntry->mFileName[0] && (( apEntry->mFileName[1] <= ' ' ) || (('.'==apEntry->mFileName[1])  && (apEntry->mFileName[2]<=' ' )) ) )
#define					DiskImage_DirEntry_IsNormal( apEntry )				( 0xE5 != apEntry->mFileName[0] && !DiskImage_DirEntry_IsSpecialDirectory(apEntry))

/* ################################################################################ */

#endif	/* INCLUDED_DISK_IO_H */