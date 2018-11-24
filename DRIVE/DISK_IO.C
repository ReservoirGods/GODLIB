/*
	DISK FORMAT

	EACH SECTOR IS 512 bytes
	The first sector on first track of first side is bootsector
	This is followed by File allocation table
	Each entry is 12 bit and references a cluster
	A "cluster" is 2 sectors (so 1K)
	For a 80x9x2 disk there are 1440 sectors or 720 clusters
	FAT takes 5 sectors, so can address 1706 clusters
	There is a second copy of the FAT (helps corruption recovery i guess) after first FAT
	then you get DIRECTORY

	TRK SEC CLS CNT OFFSET	
	 0   0   0   1  0x0000	Bootector       [ always 1 sector ]
	 0   1   0   5  0x0200	FAT (0)         [ size = mSectorsPerFAT ]
	 0   6   3   5  0x0C00	FAT (1)         [ size = mSectorsPerFAT ]
	 1   7   5   8  0x1600	Root Directory  [ size = (mNumberOfDirEntries*sizeof(sDiskImageDirEntry)) / mBytesPerSector ]
	                0x2400	Data Sectors 

*/



/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"DISK_IO.H"

#include	<GODLIB\ASSERT\ASSERT.H>
#include	<GODLIB\CLOCK\CLOCK.H>
#include	<GODLIB\GEMDOS\GEMDOS.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\RANDOM\RANDOM.H>
#include	<GODLIB\STRING\STRING.H>
#include	<GODLIB\STRING\STRPATH.H>


#define	mDISK_IMAGE_U16_READ( _var )          ((((U16)_var[1]) << 8) | _var[0])
#define	mDISK_IMAGE_U16_WRITE( _var, _value ) { _var[1]=(_value>>8)&0xFF; _var[0]=_value&0xFF; }

void DiskImage_DirInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount );
void DiskImage_DirDeInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount );

sDiskImageDirEntry *	DiskImage_Directory_Create_Internal(sDiskImage * apImage, const char * apFileName, U16 * apEntryCount );

void	DiskImage_Cluster_Write( sDiskImage * apImage, void * apBuffer, U16 aClusterIndex );
void	DiskImage_Clusters_Write( sDiskImage * apImage, void * apBuffer, U16 aClusterIndex, U16 aCount );


void	DiskImageDirEntry_Init( sDiskImageDirEntry * apEntry, const char * apFileName );
U8		DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName );

U8 		DiskImageFuncs_ST_Memory_Init( struct sDiskImage * apImage, const char * apFileName );
void 	DiskImageFuncs_ST_Memory_DeInit(	struct sDiskImage * apImage );
void 	DiskImageFuncs_ST_Memory_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
void 	DiskImageFuncs_ST_Memory_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
U8 		DiskImageFuncs_ST_Memory_Commit(	struct sDiskImage * apImage, const char * apFileName );

sDiskImageFuncs gfDiskImageFuncs_ST_Memory =
{
	&DiskImageFuncs_ST_Memory_Init,
	&DiskImageFuncs_ST_Memory_DeInit,
	&DiskImageFuncs_ST_Memory_SectorsRead,
	&DiskImageFuncs_ST_Memory_SectorsWrite,
	&DiskImageFuncs_ST_Memory_Commit
};

U8		DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName );
void	DiskImageFuncs_ST_Streamed_DeInit(	struct sDiskImage * apImage );
void 	DiskImageFuncs_ST_Streamed_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
void 	DiskImageFuncs_ST_Streamed_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
U8		DiskImageFuncs_ST_Streamed_Commit(	struct sDiskImage * apImage, const char * apFileName );

sDiskImageFuncs gfDiskImageFuncs_ST_Streamed =
{
	&DiskImageFuncs_ST_Streamed_Init,
	&DiskImageFuncs_ST_Streamed_DeInit,
	&DiskImageFuncs_ST_Streamed_SectorsRead,
	&DiskImageFuncs_ST_Streamed_SectorsWrite,
	&DiskImageFuncs_ST_Streamed_Commit
};


/* ###################################################################################
#  CODE
################################################################################### */

void	DiskFormatParameters_Init(sDiskFormatParameters * apParams )
{
	apParams->mRootDirectorySectorCount = 7;
	apParams->mSectorsPerCluster = 2;
	apParams->mSectorByteCount = 512;
	apParams->mSectorsPerFAT = 5;
	apParams->mSectorsPerTrackCount = 9;
	apParams->mSideCount = 2;
	apParams->mTrackCount = 80;
}


U8	DiskImage_Create( const sDiskFormatParameters * apParams, const char * apFileName )
{
	/* get a pseudo random unique number for disk. used by TOS to detect media changes */
	U32 size = sizeof(sBootSector);
	U32	pattern = Time_GetAbsTime();
	U16 rootDirEntries = (apParams->mSectorByteCount * apParams->mRootDirectorySectorCount) / sizeof(sDiskImageDirEntry);
	U16 sectorCount = apParams->mSectorsPerTrackCount * apParams->mTrackCount * apParams->mSideCount;
	U8 * sector;
	U16 i;
	sFileHandle handle;
	sBootSector * boot;

	if( size < apParams->mSectorByteCount )
		size = apParams->mSectorByteCount;

	sector = mMEMCALLOC( size );
	boot = (sBootSector*)sector;

	Memory_Clear( sizeof(boot), boot );
	
	boot->mBranch[0] = 0xEB;
	boot->mBranch[1] = 0x30;

	boot->mSerialPattern[ 0 ] = ( pattern >> 0 ) & 0xFF;
	boot->mSerialPattern[ 1 ] = ( pattern >> 8 ) & 0xFF;
	boot->mSerialPattern[ 2 ] = ( pattern >> 16 ) & 0xFF;

	mDISK_IMAGE_U16_WRITE( boot->mBytesPerSector, apParams->mSectorByteCount );
	boot->mSectorsPerCluster = (U8)apParams->mSectorsPerCluster;
	mDISK_IMAGE_U16_WRITE( boot->mReservedSectors, 1 );
	boot->mNumberOfFATs = 2;
	mDISK_IMAGE_U16_WRITE( boot->mNumberOfDirEntries, rootDirEntries );
	mDISK_IMAGE_U16_WRITE( boot->mNumberOfSectors, sectorCount );

	boot->mMediaDescriptor = 0xF9;	/* 3.5-inch double sided, 80 tracks per side, 9 sectors per track (720 KB) */
	mDISK_IMAGE_U16_WRITE( boot->mSectorsPerFAT, apParams->mSectorsPerFAT );
	mDISK_IMAGE_U16_WRITE( boot->mSectorsPerTrack, apParams->mSectorsPerTrackCount );
	mDISK_IMAGE_U16_WRITE( boot->mNumberOfSides, apParams->mSideCount );

	handle = File_Create( apFileName );
	if( !File_HandleIsValid(handle))
		return 0;

	File_Write( handle, sizeof(sBootSector), boot );
	Memory_Clear( apParams->mSectorByteCount, sector );
	sector[0] = 0xF0;
	sector[1] = 0xFF;
	sector[2] = 0xFF;
	File_Write( handle, apParams->mSectorByteCount, sector );

	sector[0] = sector[1] = sector[2] = 0;
	for( i=2; i<sectorCount; i++ )
		File_Write( handle, apParams->mSectorByteCount, sector );

	File_Close( handle );

	mMEMFREE( sector );

	return 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_Load( sDiskImage * apImage, sDiskImageFuncs * apFuncs, const char * apFileName )
* ACTION   : inits a disk image. for memory type, whole image is loaded into memory
			 for streaming type, just bootsector, FATs and directory details are cached
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8			DiskImage_Load( sDiskImage * apImage, sDiskImageFuncs * apFuncs, const char * apFileName )
{
	Memory_Clear( sizeof(sDiskImage), apImage );

	apImage->mfFuncs = apFuncs;
	apImage->mfFuncs->Init( apImage, apFileName );

	if( !apImage->mpBootSector )
		return 0;

	apImage->mSectorsPerCluster = apImage->mpBootSector->mSectorsPerCluster;
	apImage->mSectorSizeBytes = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mBytesPerSector );
	apImage->mClusterSizeBytes = apImage->mSectorSizeBytes * apImage->mpBootSector->mSectorsPerCluster;
	apImage->mRootDirectoryEntryCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfDirEntries );
	apImage->mClusterTotalCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfSectors ) / apImage->mSectorsPerCluster;
	
	apImage->mTotalSizeBytes  = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfSectors );
	apImage->mTotalSizeBytes *= apImage->mSectorSizeBytes;

	DiskImage_DirInit( apImage, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );

	return 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_UnLoad( sDiskImage * apImage )
* ACTION   : releases memory associated with the diskimage
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void			DiskImage_UnLoad( sDiskImage * apImage )
{
	DiskImage_DirDeInit( apImage, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );
	apImage->mfFuncs->DeInit( apImage );
}

U8		DiskImage_Save( sDiskImage * apImage, const char * apFileName )
{
	return apImage->mfFuncs->Commit( apImage, apFileName );
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount )
* ACTION   : caches all data in a directory. 
			 stores pointers and entry counts in reserved fields of directory structure
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImage_DirInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount )
{
	U16 i;

	for( i=0; i<aEntryCount; i++ )
	{
		sDiskImageDirEntry * entry = &apDir[ i ];

		entry->mpSubDirectory = 0;

		if( '.' == entry->mFileName[0] && ( entry->mFileName[1] <= ' ' || ('.' == entry->mFileName[1] && entry->mFileName[2] <= ' ') ) )
			continue;

		if( entry->mAttribute & dGEMDOS_FA_DIR )
		{
			U16	cluster = entry->mFirstCluster;
			U16 clusterCount = DiskImage_FAT_GetLinkedClusterCount( apImage, cluster );
			U32 size = clusterCount * apImage->mClusterSizeBytes;

			entry->mpSubDirectory = mMEMCALLOC( size );
			entry->mSubDirectoryEntryCount = (U16)( size / sizeof(sDiskImageDirEntry) );
			apImage->mfFuncs->SectorsRead( apImage, entry->mpSubDirectory, cluster * apImage->mSectorsPerCluster, (U16)( clusterCount * apImage->mSectorsPerCluster ) );
			DiskImage_DirInit( apImage, entry->mpSubDirectory, entry->mSubDirectoryEntryCount );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirDeInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount )
* ACTION   : deallocates memory associated with a directory
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImage_DirDeInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount )
{
	U16 i;

	for( i=0; i<aEntryCount; i++ )
	{
		sDiskImageDirEntry * entry = &apDir[ aEntryCount ];
		if( entry->mAttribute & dGEMDOS_FA_DIR )
		{
			if( entry->mpSubDirectory )
			{
				DiskImage_DirDeInit( apImage, entry->mpSubDirectory, entry->mSubDirectoryEntryCount );
				mMEMFREE( entry->mpSubDirectory );
				entry->mpSubDirectory = 0;
			}
		}
	}
}

void	DiskImage_Cluster_Write( sDiskImage * apImage, void * apBuffer, U16 aClusterIndex )
{
	apImage->mfFuncs->SectorsWrite( apImage, apBuffer, aClusterIndex * apImage->mSectorsPerCluster, apImage->mSectorsPerCluster );
}


void	DiskImage_Clusters_Write( sDiskImage * apImage, void * apBuffer, U16 aClusterIndex, U16 aCount )
{
	apImage->mfFuncs->SectorsWrite( apImage, apBuffer, aClusterIndex * apImage->mSectorsPerCluster, aCount * apImage->mSectorsPerCluster );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_FAT_GetLinkedClusterNext( sDiskImage * apImage, U16 aClusterIndex )
* ACTION   : gets the cluster pointed to by current cluster in FAT
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16				DiskImage_FAT_GetLinkedClusterNext( sDiskImage * apImage, U16 aClusterIndex )
{
	U8 * fat = apImage->mpFAT;
	U16 next = 0;

	fat += (aClusterIndex>>1)*3;

	if( aClusterIndex & 1 )
	{
		next = fat[ 2 ];
		next <<= 4;
		next |= ( ( fat[ 1 ] >> 4 ) & 0xF );
	}
	else
	{
		next = fat[ 1 ] & 0x0F;
		next <<= 8;
		next |= fat[ 0 ];
	}

	return next;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_FAT_GetLinkedClusterCount(  sDiskImage * apImage, U16 aClusterIndex )
* ACTION   : gets the count of clusters linked together in a chain
             speaks to the size of file/directory they contain
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16				DiskImage_FAT_GetLinkedClusterCount(  sDiskImage * apImage, U16 aClusterIndex )
{
	U16	count = 0;
	U16 clusterIndex = aClusterIndex;

	while( clusterIndex < 0xFF7 )
	{
		clusterIndex = DiskImage_FAT_GetLinkedClusterNext( apImage, aClusterIndex );
		count++;
	}

	return count;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_FAT_GetFreeCluster( sDiskImage * apImage )
* ACTION   : returns index the next free cluster (zero value) in FAT
			 this should be 2-number of clusters on disks
			 zero is returned if no clusters are free in the FAT
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16						DiskImage_FAT_GetFreeCluster( sDiskImage * apImage, U16 aStartIndex )
{
	U16	clusterCount = apImage->mClusterTotalCount - 1;
	U8 * fat = apImage->mpFAT;
	U16 i;

	fat += 3;

	(void)aStartIndex;

	for( i = 2; i < clusterCount; i += 2 )
	{
		if( !fat[ 0 ] && !(fat[ 1 ] & 0x0F) )
			return i;
		if( !fat[ 2 ] && !(fat[ 1 ] & 0xF0) )
			return i+1;
		fat += 3;
	}

	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_FAT_GetFreeClusterCount( sDiskImage * apImage )
* ACTION   : gets the count of free clusters in the FAT
			 useful for knowing if there is space to save a file
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16						DiskImage_FAT_GetFreeClusterCount( sDiskImage * apImage )
{
	U16	clusterCount = apImage->mClusterTotalCount;
	U8 * fat = apImage->mpFAT;
	U16 count =0;
	U16 i;

	fat += 3;
	for( i = 2; i < clusterCount; i++ )
	{
		U8	val = *fat++;
		if( i & 1 )
		{
			val &= 0xF0;
			val |= *fat++;
		}
		else if( i & 1 )
		{
			val |= ( *fat ) & 0xF;
		}
		if( !val )
			count++;
	}
	return count;

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_FAT_SetNextClusterIndex( sDiskImage * apImage, U16 aClusterSrc, U16 aClusterNext )
* ACTION   : sets the next cluster in a chain
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void		DiskImage_FAT_SetNextClusterIndex( sDiskImage * apImage, U16 aClusterSrc, U16 aClusterNext )
{
	U8 * fat = apImage->mpFAT;

	fat += (aClusterSrc>>1)*3;

	if( aClusterSrc & 1 )
	{
		fat[ 2 ] = (U8)((aClusterNext>>4) & 0xFF);
		fat[ 1 ] &= 0xF;
		fat[ 1 ] |= ( aClusterNext & 0xF ) << 4;
	}
	else
	{
		fat[ 1 ] &= 0xF0;
		fat[ 1 ] |= (( aClusterNext >> 8 ) & 0xF);
		fat[ 0 ] = (U8)(aClusterNext & 0xFF);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_GetDirectory( sDiskImage * apImage, const char * apDirName, U16 * apEntryCount )
* ACTION   : gets directory referenced by apDirName
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	DiskImage_GetDirectory( sDiskImage * apImage, const char * apDirName, U16 * apEntryCount )
{
	sStringPathSplitter splitter;
	const char * folder;
	sDiskImageDirEntry * entries = apImage->mpRootDirectory;
	*apEntryCount = apImage->mRootDirectoryEntryCount;

	if( !apDirName )
		return entries;

	StringPathSplitter_Init( &splitter, apDirName );

	folder=StringPathSplitter_Next( &splitter );
	while( folder )
	{
		U16 i;
		U16 cnt = *apEntryCount;
		for( i=0; i<cnt; i++ )
		{
			sDiskImageDirEntry * entry = &entries[ i ];

			if( DiskImageDirEntry_FileNameEqual( entry, folder))
			{
				if( entry->mAttribute & dGEMDOS_FA_DIR )
				{
					entries = entry->mpSubDirectory;
					*apEntryCount = entry->mSubDirectoryEntryCount;
					break;
				}
			}
		}
		if( i == cnt )
			return 0;

		folder = StringPathSplitter_Next( &splitter );
	}

	return entries;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_GetDirEntry( sDiskImage * apImage, const char * apFileName )
* ACTION   : gets file entry referenced by apFileName
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	DiskImage_GetDirEntry( sDiskImage * apImage, const char * apFileName )
{
	sStringPath	path;
	sDiskImageDirEntry * entries;
	U16 entryCount = 0;

	StringPath_GetDirectory( &path, apFileName );
	entries = DiskImage_GetDirectory( apImage, path.mChars, &entryCount );
	if( entries )
	{
		U16 i;
		const char * fileName = StringPath_GetpFileName( apFileName );
		for( i=0; i<entryCount; i++ )
		{
			sDiskImageDirEntry * entry = &entries[i];
			if( DiskImageDirEntry_FileNameEqual(entry,fileName))
				return entry;
		}
	}
		
	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirWalker_Init( sDiskImage * apImage, const char * apDirName, sDiskImageDirWalker * apWalker )
* ACTION   : inits a directory walker. dirname should specify directory to work (null means root directory)
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void	DiskImage_DirWalker_Init( sDiskImage * apImage, const char * apDirName, sDiskImageDirWalker * apWalker )
{
	Memory_Clear( sizeof(sDiskImageDirWalker), apWalker );

	apWalker->mpEntries = DiskImage_GetDirectory( apImage, apDirName, &apWalker->mEntryCount );
	apWalker->mpDiskImage = apImage;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker )
* ACTION   : deinits a directory walker
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void	DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker )
{
	apWalker->mpEntries = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker )
* ACTION   : moves to next entry in directory walker
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	DiskImage_DirWalker_Next( sDiskImageDirWalker * apWalker )
{	
	sDiskImageDirEntry * entry;
	for( ; (apWalker->mEntryIndex < apWalker->mEntryCount) && ( !apWalker->mpEntries[ apWalker->mEntryIndex ].mFileName[ 0 ] ); apWalker->mEntryIndex++ );
	if( apWalker->mEntryIndex >= apWalker->mEntryCount )
		return 0;
	entry = &apWalker->mpEntries[ apWalker->mEntryIndex ];
	apWalker->mEntryIndex++;
	return( entry );
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_File_Exists( sDiskImage * apImage, const char * apFileName )
* ACTION   : determines if a file exists
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	DiskImage_File_Exists( sDiskImage * apImage, const char * apFileName )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );

	return entry ? 1 : 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_File_GetSize( sDiskImage * apImage, const char * apFileName )
* ACTION   : returns the size of a file
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U32		DiskImage_File_GetSize( sDiskImage * apImage, const char * apFileName )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );
	U32 size = 0;

	if( entry )
	{
		Endian_ReadLittleU32( &size, entry->mSize );
	}

	return size;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_File_Load_Internal( sDiskImage * apImage, sDiskImageDirEntry * apEntry, void * apBuffer )
* ACTION   : loads a file to a specified address
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void		DiskImage_File_Load_Internal( sDiskImage * apImage, sDiskImageDirEntry * apEntry, void * apBuffer )
{
	U32 size;
	U16 clusterIndex;

	U8 * dst = (U8*)apBuffer;

	Endian_ReadLittleU16( &apEntry->mFirstCluster, clusterIndex );
	Endian_ReadLittleU32( &apEntry->mSize, size );

	while( size )
	{
		U32 readSize = apImage->mClusterSizeBytes;

		if( size >= readSize )
		{
			apImage->mfFuncs->SectorsRead( apImage, dst, clusterIndex, apImage->mSectorsPerCluster );
		}
		else
		{
			U8 * sector = mMEMCALLOC( apImage->mClusterSizeBytes );

			apImage->mfFuncs->SectorsRead( apImage, sector, clusterIndex, apImage->mSectorsPerCluster );
			readSize = size;
			Memory_Copy( size, sector, dst );
			mMEMFREE( sector );
		}

		dst += readSize;
		size -= readSize;

		clusterIndex = DiskImage_FAT_GetLinkedClusterNext( apImage, clusterIndex );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_File_Load( sDiskImage * apImage, const char * apFileName )
* ACTION   : allocates memory for and loads a file
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

void *		DiskImage_File_Load( sDiskImage * apImage, const char * apFileName )
{
	U8 * mem = 0;
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );

	if( entry )
	{
		U32 size = 0;
		
		Endian_ReadLittleU32( &size, entry->mSize );
		mem = mMEMCALLOC( size );
		DiskImage_File_Load_Internal( apImage, entry, mem );
	}

	return mem;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_File_LoadAt( sDiskImage * apImage, const char * apFileName, void * apBuffer )
* ACTION   : loads a file to a specific address
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8			DiskImage_File_LoadAt( sDiskImage * apImage, const char * apFileName, void * apBuffer )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );

	if( entry )
	{
		DiskImage_File_Load_Internal( apImage, entry, apBuffer );
		return 1;
	}

	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_DirEntry_GetFree(sDiskImageDirEntry * apEntries, const char * apFileName, U16 aEntryCount )
* ACTION   : gets the next free entry in a directory
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	DiskImage_DirEntry_GetFree(sDiskImageDirEntry * apEntries, const char * apFileName, U16 aEntryCount )
{
	U16 i;
	for( i=0; i<aEntryCount; i++ )
	{
		if( DiskImageDirEntry_FileNameEqual(&apEntries[i],apFileName))
			return( &apEntries[i]);
	}

	for( i=0; i<aEntryCount; i++ )
	{
		if( apEntries[i].mFileName[0] <= ' ' )
			return( &apEntries[i]);
	}

	for( i=0; i<aEntryCount; i++ )
	{
		if( (U8)apEntries[i].mFileName[0] == 0xE5 )
			return( &apEntries[i]);
	}

	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImage_Directory_Create_Internal(sDiskImage * apImage, const char * apDirName, U16 * apEntryCount )
* ACTION   : creates a directory
* CREATION : 17-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	DiskImage_Directory_Create_Internal(sDiskImage * apImage, const char * apDirName, U16 * apEntryCount )
{
	sStringPathSplitter splitter;
	const char * folder;
	sDiskImageDirEntry * entries = apImage->mpRootDirectory;
	*apEntryCount = apImage->mRootDirectoryEntryCount;

	StringPathSplitter_Init( &splitter, apDirName );

	for
	( 
		folder=StringPathSplitter_Next(&splitter);
		folder;
		folder = StringPathSplitter_Next( &splitter )
	)
	{
		U16 i;
		U16 cnt = *apEntryCount;
		for( i=0; i<cnt; i++ )
		{
			sDiskImageDirEntry * entry = &entries[ i ];

			if( DiskImageDirEntry_FileNameEqual( entry, folder))
			{
				if( entry->mAttribute & dGEMDOS_FA_DIR )
				{
					entries = entry->mpSubDirectory;
					*apEntryCount = entry->mSubDirectoryEntryCount;
					break;
				}
			}
		}

		if( i<cnt)
			continue;

		/* if we get here we didn't find directory */
		for( i=0;i<cnt;i++)
		{
			sDiskImageDirEntry * entry = &entries[ i ];
			if( 0 == entry->mFileName[0] )
				break;
		}

		if( i < cnt )
		{
			U16 cluster = DiskImage_FAT_GetFreeCluster(apImage,0);
			if( cluster )
			{
				sDiskImageDirEntry * subDir;
				sDiskImageDirEntry * entry = &entries[ i ];
				DiskImageDirEntry_Init( entry, folder );
				entry->mAttribute = dGEMDOS_FA_DIR;
				entry->mFirstCluster = cluster;
				DiskImage_FAT_SetNextClusterIndex( apImage, entry->mFirstCluster, 0xFFF );
				entry->mSubDirectoryEntryCount = apImage->mClusterSizeBytes / sizeof(sDiskImageDirEntry);
				entry->mpSubDirectory = mMEMCALLOC( apImage->mClusterSizeBytes );

				subDir = entry->mpSubDirectory;

				DiskImageDirEntry_Init( &subDir[0], "-" );
				DiskImageDirEntry_Init( &subDir[1], "--" );

				subDir[0].mFileName[0]='.';
				subDir[1].mFileName[0]='.';
				subDir[1].mFileName[1]='.';

				subDir[0].mAttribute = dGEMDOS_FA_DIR;
				subDir[1].mAttribute = dGEMDOS_FA_DIR;

				Endian_WriteLittleU16( &subDir[0].mFirstCluster, cluster );
				/* ".." should  point to parent directory cluster */
				subDir[1].mFirstCluster = entries[0].mFirstCluster;

				entries = entry->mpSubDirectory;
				*apEntryCount = entry->mSubDirectoryEntryCount;
			}
		}
		else
		{
#if 0			
			sDiskImageDirEntry * pParent = entries[1].mpSubDirectory;
			if( pParent )
			{
				U8 * dir = (U8*)entries;

				U16	dirCluster;
				U16 nextCluster;
				U16 cluster = DiskImage_FAT_GetFreeCluster();
				Endian_ReadLittleU16( &entries[0].mFirstCluster, dirCluster );
				nextCluster = DiskImage_FAT_GetLinkedClusterNext( apImage, dirCluster );
				while( nextCluster < 0xFF7 )
				{
					apImage->mfFuncs->SectorsWrite( apImage, dir, dirCluster>>1, apImage->mSectorsPerCluster );
					dir += apImage->mSectorByteCount;
					dirCluster = nextCluster;
					nextCluster = DiskImage_FAT_GetLinkedClusterNext( apImage, dirCluster );
				}

				DiskImage_FAT_SetNextClusterIndex( apImage, dirCluster, cluster );
				DiskImage_FAT_SetNextClusterIndex( apImage, cluster, 0xFF7 );

				mMEMFREE( pParent->mpSubDirectory );


				apImage->mfFuncs->SectorsWrite()
			}
			else
			{
				/* can't expand the root directory */
				return 0;
			}
#endif			
		}
	}

	return entries;
}


U8	DiskImage_Directory_Create( sDiskImage * apImage, const char * apDirName )
{
	U16 count = 0;
	sDiskImageDirEntry * entry = DiskImage_Directory_Create_Internal( apImage, apDirName, &count );

	return entry ? 1 : 0;
}


U8		DiskImage_File_Save( sDiskImage * apImage, const char * apFileName, void * apBuffer, U32 aBytes )
{
	sStringPath	path;
	sDiskImageDirEntry * directory;
	U16 entryCount = 0;
	U32 freeSpace = DiskImage_FAT_GetFreeClusterCount( apImage );
	freeSpace *= apImage->mClusterSizeBytes;

	if( freeSpace < aBytes )
		return 0;

	StringPath_GetDirectory( &path, apFileName );

	directory = DiskImage_Directory_Create_Internal( apImage, path.mChars, &entryCount );
	if( directory )
	{
		sDiskImageDirEntry * entry;
		const char * fileName = StringPath_GetpFileName(apFileName);

		entry = DiskImage_DirEntry_GetFree( directory, fileName, entryCount );

		if( entry )
		{
			U8 * src = (U8*)apBuffer;
			U32	size = aBytes;
			U16 clusterLast = 0;
			U16 clusterNext = 0;
			U16	clusterIndex = DiskImage_FAT_GetFreeCluster( apImage, 0 );

			DiskImageDirEntry_Init( entry, fileName );
			Endian_WriteLittleU32( (&entry->mSize), aBytes );
			Endian_WriteLittleU16( &entry->mFirstCluster, clusterIndex );

			while( clusterIndex && size )
			{
				U32 writeSize = apImage->mClusterSizeBytes;

				if( size >= writeSize )
				{
					DiskImage_Cluster_Write( apImage, src, clusterIndex );
				}
				else
				{
					U8 * block = mMEMCALLOC( apImage->mClusterSizeBytes );

					Memory_Copy( size, src, block );
					DiskImage_Cluster_Write( apImage, block, clusterIndex );
					mMEMFREE( block );
					writeSize = size;
				}
				DiskImage_FAT_SetNextClusterIndex( apImage, clusterIndex, 0xFFF );

				clusterNext = DiskImage_FAT_GetFreeCluster( apImage, clusterIndex );
				if( clusterLast )
					DiskImage_FAT_SetNextClusterIndex( apImage, clusterLast, clusterIndex );

				clusterLast = clusterIndex;
				clusterIndex = clusterNext;

				src += writeSize;

				size -= writeSize;
			}
		}
	}
	return 1;
}


/*
	Functions for memory based ST disk image
*/

/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Memory_Init( struct sDiskImage * apImage, const char * apFileName )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/


U8  DiskImageFuncs_ST_Memory_Init( struct sDiskImage * apImage, const char * apFileName )
{
	apImage->mpBootSector = File_Load( apFileName );

	if( !apImage->mpBootSector )
		return 0;
	else
	{
		U8 * mem = (U8*)apImage->mpBootSector;
		U32 offset = 0;
		U16 fatSize = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mSectorsPerFAT );
		U16 secSize = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mBytesPerSector );
		U16 fileCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfDirEntries );
		fatSize *= secSize;
		fatSize *= apImage->mpBootSector->mNumberOfFATs;

		offset = secSize;

		apImage->mpFAT = mem + offset;
		offset += fatSize;
		apImage->mpRootDirectory = (sDiskImageDirEntry*)( mem + offset );

		offset += fileCount * sizeof(sDiskImageDirEntry);

		/* as addressing effectively starts at 2 lets move this ptr back 2 sectors */
		offset -= (secSize * 2) * apImage->mpBootSector->mSectorsPerCluster;
		apImage->mDataSectorOffset = (U16)offset;
	}

	return 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Memory_DeInit( struct sDiskImage * apImage )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Memory_DeInit(	struct sDiskImage * apImage )
{
	if(apImage->mpBootSector )
	{
		File_UnLoad( apImage->mpBootSector );
		apImage->mpBootSector = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Memory_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Memory_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
{
	U8 *	mem = (U8*)apImage->mpBootSector;
	U32 	size = apImage->mSectorSizeBytes;
	U32 	offset = apImage->mSectorSizeBytes;
	offset *= aSectorIndex;
	offset += apImage->mDataSectorOffset;
	size *= aSectorCount;
	mem += offset;
	Memory_Copy( size, mem, apBuffer );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Memory_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Memory_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
{
	U8 *	mem = (U8*)apImage->mpBootSector;
	U32 	size = apImage->mSectorSizeBytes;
	U32 	offset = apImage->mSectorSizeBytes;
	offset *= aSectorIndex;
	offset += apImage->mDataSectorOffset;
	size *= aSectorCount;
	mem += offset;
	Memory_Copy( size, apBuffer, mem );
}

void DiskImageFuncs_ST_Memory_Dir_Commit( sDiskImage * apImage, sDiskImageDirEntry * apEntries, U16 aEntryCount )
{
	U16 i;
	for( i=0; i<aEntryCount; i++ )	
	{
		sDiskImageDirEntry * entry = &apEntries[ i ];

		if( '.' == entry->mFileName[0] && ( entry->mFileName[1] <= ' ' || ('.' == entry->mFileName[1] && entry->mFileName[2] <= ' ') ) )
			continue;

		if( (entry->mAttribute & dGEMDOS_FA_DIR) && (entry->mpSubDirectory) && (entry->mSubDirectoryEntryCount) )
		{
			U8 * dst = (U8*)apImage->mpBootSector;
			U16 cluster;
			U32 offset;
			Endian_ReadLittleU16( &entry->mFirstCluster, cluster );
			while( cluster < 0xFF7 )
			{
				offset  = cluster;
				offset *= apImage->mClusterSizeBytes;
				offset += apImage->mDataSectorOffset;

				Memory_Copy( apImage->mSectorSizeBytes, entry->mpSubDirectory, dst+offset );
				cluster = DiskImage_FAT_GetLinkedClusterNext( apImage, cluster );
			}

			DiskImageFuncs_ST_Memory_Dir_Commit( apImage, entry->mpSubDirectory, entry->mSubDirectoryEntryCount );
		}
	}
}


U8 DiskImageFuncs_ST_Memory_Commit(  struct sDiskImage * apImage, const char * apFileName )
{
	U32 fatSize = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mSectorsPerFAT );
	fatSize *= apImage->mSectorSizeBytes;

	Memory_Copy( fatSize, apImage->mpFAT, apImage->mpFAT + fatSize );

	DiskImageFuncs_ST_Memory_Dir_Commit( apImage, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );

	return File_Save( apFileName, apImage->mpBootSector, apImage->mTotalSizeBytes );
}


/*
	Functions for streaming ST disk images
*/

/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8 DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName )
{
	apImage->mFileHandle = File_Open( apFileName );

	if( !File_HandleIsValid(apImage->mFileHandle))
		return 0;
	else
	{
		U16 fatSectors;
		U32 fatSize;
		U32 dirSize;
		apImage->mpBootSector = mMEMCALLOC( 512 );
		File_Read( apImage->mFileHandle, 512, apImage->mpBootSector );

		fatSectors = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mSectorsPerFAT );
		apImage->mSectorSizeBytes = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mBytesPerSector );
		apImage->mClusterSizeBytes = apImage->mSectorSizeBytes * apImage->mpBootSector->mSectorsPerCluster;
		apImage->mRootDirectoryEntryCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfDirEntries );

		fatSize  = fatSectors;
		fatSize *= apImage->mSectorSizeBytes;
		apImage->mpFAT = mMEMCALLOC( fatSize );
		File_Read( apImage->mFileHandle, fatSize, apImage->mpFAT );
		File_SeekFromCurrent( apImage->mFileHandle, fatSize );
		dirSize = apImage->mRootDirectoryEntryCount;
		dirSize *= sizeof(sDiskImageDirEntry);
		apImage->mpRootDirectory = mMEMCALLOC( dirSize );
		File_Read( apImage->mFileHandle, dirSize, apImage->mpRootDirectory );
	}
	return 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Streamed_DeInit( struct sDiskImage * apImage )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Streamed_DeInit(	struct sDiskImage * apImage )
{
	if(apImage->mpBootSector )
	{
		mMEMFREE( apImage->mpBootSector );
		apImage->mpBootSector = 0;
	}
	if(apImage->mpFAT )
	{
		mMEMFREE( apImage->mpFAT );
		apImage->mpFAT = 0;
	}
	if(apImage->mpRootDirectory )
	{
		mMEMFREE( apImage->mpRootDirectory );
		apImage->mpRootDirectory = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Streamed_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Streamed_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
{
	if( File_HandleIsValid(apImage->mFileHandle))
	{
		U32 size = apImage->mSectorSizeBytes;
		U32 offset = apImage->mSectorSizeBytes;
		size *= aSectorCount;
		offset *= aSectorIndex;
		File_SeekFromStart( apImage->mFileHandle, apImage->mDataSectorOffset + offset );
		File_Read( apImage->mFileHandle, size, apBuffer);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Streamed_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Streamed_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount )
{
	if( File_HandleIsValid(apImage->mFileHandle))
	{
		U32 size = apImage->mSectorSizeBytes;
		U32 offset = apImage->mSectorSizeBytes;
		size *= aSectorCount;
		offset *= aSectorIndex;
		File_SeekFromStart( apImage->mFileHandle, apImage->mDataSectorOffset + offset );
		File_Write( apImage->mFileHandle, size, apBuffer);
	}
}


void DiskImageFuncs_ST_Streamed_Dir_Commit( sDiskImage * apImage, sFileHandle aHandle, sDiskImageDirEntry * apEntries, U16 aEntryCount )
{
	U16 i;
	for( i=0; i<aEntryCount; i++ )	
	{
		sDiskImageDirEntry * entry = &apEntries[ i ];

		if( '.' == entry->mFileName[0] && ( entry->mFileName[1] <= ' ' || ('.' == entry->mFileName[1] && entry->mFileName[2] <= ' ') ) )
			continue;

		if( (entry->mAttribute & dGEMDOS_FA_DIR) && (entry->mpSubDirectory) && (entry->mSubDirectoryEntryCount) )
		{
			U32 offset;
			Endian_ReadLittleU16( &entry->mFirstCluster, offset );
			offset *= apImage->mClusterSizeBytes;
			offset += apImage->mDataSectorOffset;
			File_Write( aHandle, aEntryCount * sizeof(sDiskImageDirEntry), apEntries );

			DiskImageFuncs_ST_Streamed_Dir_Commit( apImage, aHandle, entry->mpSubDirectory, entry->mSubDirectoryEntryCount );
		}
	}
}


U8 DiskImageFuncs_ST_Streamed_Commit( sDiskImage * apImage, const char * apFileName )
{
	sFileHandle handle = apImage->mFileHandle;	
	U32 size;

	if( !File_HandleIsValid(apFileName) )
		handle = File_Open(apFileName);

	File_SeekFromStart( handle, 0 );
	File_Write( handle, sizeof(sBootSector), apImage->mpBootSector );


	size  = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mSectorsPerFAT );
	size *= apImage->mSectorSizeBytes;

	/* FAT0 */
	File_Write( handle, size, apImage->mpFAT );
	/* FAT1 */
	File_Write( handle, size, apImage->mpFAT );

	File_Write( handle, apImage->mRootDirectoryEntryCount * sizeof(sDiskImageDirEntry), apImage->mpRootDirectory );

	DiskImageFuncs_ST_Streamed_Dir_Commit( apImage, handle, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );

	if( !File_HandleIsValid(apFileName) )
		File_Close(handle);

	return 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageDirEntry_Init( sDiskImageDirEntry * apEntry, const char * apFileName )
* ACTION   : inits a diskimagedirentry
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void	DiskImageDirEntry_Init( sDiskImageDirEntry * apEntry, const char * apFileName )
{
	U16 i;
	Memory_Clear( sizeof(sDiskImageDirEntry), apEntry );

	for( i=0; (i<8) && ('.'!=*apFileName) && (*apFileName); i++ )
		apEntry->mFileName[i] = *apFileName++;

	for( ;i<8;i++)
		apEntry->mFileName[i] = ' ';

	i=0;
	if( '.' == *apFileName )
	{
		apFileName++;
		for( ; (i<3) && *apFileName; i++)
			apEntry->mExtension[i] = *apFileName++;
	}
	for( ;i<3;i++)
		apEntry->mExtension[i] = ' ';
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName )
* ACTION   : check to see if entry in directory has same filename as one passed in
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName )
{
	U16 i;
	U8 fileName[11];
	const char * src = &entry->mFileName[0];

	for( i=0; i<8 && *apFileName && ('.' != *apFileName); i++ )
		fileName[i]=*apFileName++;
	
	for( ;i<8; i++)
		fileName[i] = ' ';

	if( '.' == *apFileName )
		apFileName++;
	
	for( ; i<11 && *apFileName; i++ )
		fileName[i] = *apFileName++;

	for( ;i<11; i++ )
		fileName[i]=' ';
	
	for( i=0; i<11; i++ )
		if( src[i] != fileName[i])
			return 0;

	return 1;

#if 0

	for( i = 0; (i < 8) && (apFileName[ i ]) && ('.' != apFileName[i]) && (!StringPath_IsSeperator(apFileName[i])); i++ )
	{
		if( entry->mFileName[ i ] != apFileName[ i ] )
			return 0;
	}

	if( i < 8 && entry->mFileName[ i ] )
		return 0;

	if( '.' != apFileName[ i ] )
		return 0 == entry->mExtension[ 0 ] ? 1 : 0;

	apFileName = &apFileName[ i + 1 ];

	for( i = 0; i < 3 && apFileName[ i ] && '.' != apFileName[ i ] && !StringPath_IsSeperator( apFileName[ i ] ); i++ )
	{
		if( entry->mExtension[ i ] != apFileName[ i ] )
			return 0;
	}

	return( 1 );
#endif	
}




#if 0
/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_CreateDirectory( sDiskImageST * apImage, sDiskImageDirEntry * apDir, U16 aDirCount, const char * apDirName )
* ACTION   : creates a directory on an image
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry *	Disk_Image_ST_CreateDirectory( sDiskImageST * apImage, sDiskImageDirEntry * apDir, U16 aDirCount, const char * apDirName )
{
	U16 i;
	for( i=0; i<aDirCount; i++ )
	{
		sDiskImageDirEntry * entry = &apDir[ i ];
		if( DiskImageDirEntry_FileNameEqual( entry, apDirName ))
			return entry;

		if( 0 == entry->mFileName[0] )
		{
			sDiskImageDirEntry * subDir;

			DiskImageDirEntry_Init( entry, apDirName );
			entry->mAttribute = dGEMDOS_FA_DIR;
			entry->mFirstCluster = Disk_Image_GetSectorFree( apImage );

			subDir = (sDiskImageDirEntry*)Disk_Image_ST_GetpDataCluster( apImage, entry->mFirstCluster );

			DiskImageDirEntry_Init( &subDir[0], "-" );
			DiskImageDirEntry_Init( &subDir[1], "--" );

			subDir[0].mFileName[0] = '.';
			subDir[1].mFileName[0] = '.';
			subDir[1].mFileName[1] = '.';
		}
	}
	return 0;
}

#endif


/* ################################################################################ */

