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

#include	<GODLIB\CLOCK\CLOCK.H>
#include	<GODLIB\GEMDOS\GEMDOS.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\RANDOM\RANDOM.H>
#include	<GODLIB\STRING\STRPATH.H>


#define	mDISK_IMAGE_U16_READ( _var )          ((((U16)_var[1]) << 8) | _var[0])
#define	mDISK_IMAGE_U16_WRITE( _var, _value ) { _var[0]=(_value>>8)&0xFF; _var[1]=_value&0xFF; }

void	DiskImageDirEntry_Init( sDiskImageDirEntry * apEntry, const char * apFileName );
U8	DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName );

void DiskImageFuncs_ST_Memory_Init( struct sDiskImage * apImage, const char * apFileName );
void DiskImageFuncs_ST_Memory_DeInit(	struct sDiskImage * apImage );

void DiskImageFuncs_ST_Memory_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
void DiskImageFuncs_ST_Memory_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );

sDiskImageFuncs gfDiskImageFuncs_ST_Memory =
{
	&DiskImageFuncs_ST_Memory_Init,
	&DiskImageFuncs_ST_Memory_DeInit,
	&DiskImageFuncs_ST_Memory_SectorsRead,
	&DiskImageFuncs_ST_Memory_SectorsWrite
};

void DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName );
void DiskImageFuncs_ST_Streamed_DeInit(	struct sDiskImage * apImage );

void DiskImageFuncs_ST_Streamed_SectorsRead(   struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );
void DiskImageFuncs_ST_Streamed_SectorsWrite(  struct sDiskImage * apImage, void * apBuffer, U16 aSectorIndex, U16 aSectorCount );

sDiskImageFuncs gfDiskImageFuncs_ST_Streamed =
{
	&DiskImageFuncs_ST_Streamed_Init,
	&DiskImageFuncs_ST_Streamed_DeInit,
	&DiskImageFuncs_ST_Streamed_SectorsRead,
	&DiskImageFuncs_ST_Streamed_SectorsWrite
};


/* ###################################################################################
#  CODE
################################################################################### */


void DiskImage_DirInit( sDiskImage * apImage, sDiskImageDirEntry * apDir, U16 aEntryCount )
{
	U16 i;

	for( i=0; i<aEntryCount; i++ )
	{
		sDiskImageDirEntry * entry = &apDir[ aEntryCount ];
		if( entry->mAttribute & dGEMDOS_FA_DIR )
		{
			U16	cluster = entry->mFirstCluster;
			U16 clusterCount = DiskImage_FAT_GetLinkedClusterCount( apImage, cluster );
			U32 size = clusterCount * apImage->mClusterSizeBytes;

			entry->mpSubDirectory = mMEMCALLOC( size );
			entry->mSubDirectoryEntryCount = (U16)( size / sizeof(sDiskImageDirEntry) );
			apImage->mfFuncs->SectorsRead( apImage, entry->mpSubDirectory, cluster, (U16)( clusterCount * apImage->mSectorsPerCluster ) );
			DiskImage_DirInit( apImage, entry->mpSubDirectory, entry->mSubDirectoryEntryCount );
		}
	}
}

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

void			DiskImage_Init( sDiskImage * apImage, sDiskImageFuncs * apFuncs, const char * apFileName )
{
	apImage->mfFuncs = apFuncs;
	apImage->mfFuncs->Init( apImage, apFileName );

	apImage->mClusterTotalCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfSectors ) / apImage->mSectorsPerCluster;

	DiskImage_DirInit( apImage, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );
}


void			DiskImage_DeInit( sDiskImage * apImage )
{
	DiskImage_DirDeInit( apImage, apImage->mpRootDirectory, apImage->mRootDirectoryEntryCount );
	apImage->mfFuncs->DeInit( apImage );
}


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


U16						DiskImage_FAT_GetFreeCluster( sDiskImage * apImage )
{
	U16	clusterCount = apImage->mClusterTotalCount;
	U8 * fat = apImage->mpFAT;
	U16 i;

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
			return i;
	}
	return 0;
}

U16						DiskImage_FAT_GetFreeClusterCount( sDiskImage * apImage )
{
	U16	clusterCount = apImage->mClusterTotalCount;
	U8 * fat = apImage->mpFAT;
	U16 count =0;
	U16 i;

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


void					DiskImage_FAT_SetNextClusterIndex( sDiskImage * apImage, U16 aClusterSrc, U16 aClusterNext )
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

sDiskImageDirEntry *	DiskImage_GetDirectory( sDiskImage * apImage, const char * apFileName, U16 * apEntryCount )
{
	sStringPath path;
	char * folder;
	sDiskImageDirEntry * entries = apImage->mpRootDirectory;
	*apEntryCount = apImage->mRootDirectoryEntryCount;

	StringPath_SetNT( &path, apFileName );

	folder=StringPath_GetFolderFirst( &path );
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

		folder = StringPath_GetFolderNext( &path, folder );
	}

	return entries;
}

sDiskImageDirEntry *	DiskImage_GetDirEntry( sDiskImage * apImage, const char * apFileName )
{
	sStringPath	path;
	sDiskImageDirEntry * entries;
	U16 entryCount = 0;

	StringPath_GetDirectory( &path, apFileName );
	entries = DiskImage_GetDirectory( apImage, apFileName, &entryCount );
	if( entries )
	{
		U16 i;
		StringPath_SetFileName( &path, apFileName );
		for( i=0; i<entryCount; i++ )
		{
			sDiskImageDirEntry * entry = &entries[i];
			if( DiskImageDirEntry_FileNameEqual(entry,apFileName))
				return entry;
		}
	}
		
	return 0;
}

void	DiskImage_DirWalker_Init( sDiskImage * apImage, const char * apDirName, sDiskImageDirWalker * apWalker )
{
	Memory_Clear( sizeof(sDiskImageDirWalker), apWalker );

	apWalker->mpEntries = DiskImage_GetDirectory( apImage, apDirName, &apWalker->mEntryCount );
	apWalker->mpDiskImage = apImage;
}


void	DiskImage_DirWalker_DeInit( sDiskImageDirWalker * apWalker )
{
	if( apWalker->mpEntries && apWalker->mpDiskImage->mpRootDirectory != apWalker->mpEntries )
	{
		mMEMFREE( apWalker->mpEntries );
		apWalker->mpEntries = 0;
	}
}


sDiskImageDirEntry *	DiskImage_DirWalker_Next( sDiskImageDirWalker * apWalker )
{	
	sDiskImageDirEntry * entry = &apWalker->mpEntries[ apWalker->mEntryIndex ];
	if( apWalker->mEntryIndex > apWalker->mEntryCount )
		return 0;
	apWalker->mEntryIndex++;
	return( entry );
}




U8	DiskImage_File_Exists( sDiskImage * apImage, const char * apFileName )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );

	return entry ? 1 : 0;
}


U32						DiskImage_File_GetSize( sDiskImage * apImage, const char * apFileName )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );
	U32 size = 0;

	if( entry )
	{
		Endian_ReadLittleU32( &size, entry->mSize );
	}

	return size;
}


void			DiskImage_File_Load_Internal( sDiskImage * apImage, sDiskImageDirEntry * apEntry, void * apBuffer )
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


void *					DiskImage_File_Load( sDiskImage * apImage, const char * apFileName )
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


U8						DiskImage_File_LoadAt( sDiskImage * apImage, const char * apFileName, void * apBuffer )
{
	sDiskImageDirEntry * entry = DiskImage_GetDirEntry( apImage, apFileName );

	if( entry )
	{
		DiskImage_File_Load_Internal( apImage, entry, apBuffer );
		return 1;
	}

	return 0;
}


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
		if( 0 == apEntries[i].mFileName[0])
			return( &apEntries[i]);
	}

	return 0;
}

sDiskImageDirEntry *	DiskImage_Directory_Create(sDiskImage * apImage, const char * apFileName, U16 * apEntryCount )
{
	sStringPath path;
	char * folder;
	sDiskImageDirEntry * entries = apImage->mpRootDirectory;
	*apEntryCount = apImage->mRootDirectoryEntryCount;

	StringPath_SetNT( &path, apFileName );

	folder=StringPath_GetFolderFirst( &path );
	for
	( 
		folder=StringPath_GetFolderFirst( &path );		
		folder;
		folder = StringPath_GetFolderNext( &path, folder )
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
					continue;
				}
			}
		}

		/* if we get here we didn't find directory */
		for( i=0;i<cnt;i++)
		{
			sDiskImageDirEntry * entry = &entries[ i ];
			if( 0 == entry->mFileName[0] )
				break;
		}

		if( i < cnt )
		{
			
		}
		else
		{
		/* 2D0 - handle case when we are out of dir entries */
			return 0;
		}
	}

	return entries;
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

	directory = DiskImage_Directory_Create( apImage, path.mChars, &entryCount );
	if( directory )
	{
		sDiskImageDirEntry * entry;
		const char * fileName = StringPath_GetpFileName(apFileName);

		entry = DiskImage_DirEntry_GetFree( directory, fileName, entryCount );

		if( entry )
		{
			U8 * src = (U8*)apBuffer;
			U32	size = aBytes;
			U16 clusterNext = 0;
			U16	clusterIndex = DiskImage_FAT_GetFreeCluster( apImage );

			DiskImageDirEntry_Init( entry, fileName );
			Endian_WriteLittleU32( &entry->mSize, aBytes );


			while( clusterIndex && size )
			{
				U32 writeSize = apImage->mClusterSizeBytes;

				if( size >= writeSize )
				{
					apImage->mfFuncs->SectorsWrite( apImage, src, clusterIndex, 1 );
				}
				else
				{
					U8 * sector = mMEMCALLOC( apImage->mSectorSizeBytes );

					Memory_Copy( size, src, sector );
					apImage->mfFuncs->SectorsWrite( apImage, src, clusterIndex, 1 );
					mMEMFREE( sector );
					writeSize = size;
				}
				clusterNext = DiskImage_FAT_GetFreeCluster( apImage );
				DiskImage_FAT_SetNextClusterIndex( apImage, clusterIndex, clusterNext );
				clusterIndex = clusterNext;

				size -= writeSize;
			}

			DiskImage_FAT_SetNextClusterIndex( apImage, clusterIndex, 0xFF7 );
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


void DiskImageFuncs_ST_Memory_Init( struct sDiskImage * apImage, const char * apFileName )
{
	apImage->mpBootSector = File_Load( apFileName );
	if( apImage->mpBootSector )
	{
		U8 * mem = (U8*)apImage->mpBootSector;
		U16 fatSize = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mSectorsPerFAT );
		apImage->mSectorSizeBytes = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mBytesPerSector );
		apImage->mClusterSizeBytes = apImage->mSectorSizeBytes * apImage->mpBootSector->mSectorsPerCluster;
		apImage->mpFAT = mem + apImage->mSectorSizeBytes;
		apImage->mRootDirectoryEntryCount = mDISK_IMAGE_U16_READ( apImage->mpBootSector->mNumberOfDirEntries );

		fatSize *= apImage->mSectorSizeBytes;
		fatSize *= apImage->mpBootSector->mNumberOfFATs;

		apImage->mpRootDirectory = (sDiskImageDirEntry*)( mem + apImage->mSectorSizeBytes + fatSize );
	}
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
	U32 	size = apImage->mClusterSizeBytes;
	U32 	offset = apImage->mClusterSizeBytes;
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
	U32 	size = apImage->mClusterSizeBytes;
	U32 	offset = apImage->mClusterSizeBytes;
	offset *= aSectorIndex;
	offset += apImage->mDataSectorOffset;
	size *= aSectorCount;
	mem += offset;
	Memory_Copy( size, apBuffer, mem );
}


/*
	Functions for streaming ST disk images
*/

/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName )
* ACTION   : 
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void DiskImageFuncs_ST_Streamed_Init( struct sDiskImage * apImage, const char * apFileName )
{
	apImage->mFileHandle = File_Open( apFileName );

	if( File_HandleIsValid(apImage->mFileHandle))
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



/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetpBootSector( const sDiskImageST * apImage )
* ACTION   : currently sDiskImageST is just a raw disk image, so first bytes are bootsector
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

sBootSector * Disk_Image_ST_GetpBootSector( const sDiskImageST * apImage )
{
	return (sBootSector *)apImage;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetClusterSize( const sDiskImageST * apImage )
* ACTION   : retrieves the cluster size as defined in the bootsector
			 typically clusters are 2 sectors, and sectors are 512 bytes
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16	Disk_Image_ST_GetClusterSize( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U16 secSize = mDISK_IMAGE_U16_READ( boot->mBytesPerSector );

	return secSize * boot->mSectorsPerCluster;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8	* Disk_Image_ST_GetpFAT( const sDiskImageST * apImage )
* ACTION   : gets the FAT from a disk image
			 typically disks have 2 File Allocation Tables
			 the first one begins immediately after the bootsector
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	* Disk_Image_ST_GetpFAT( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * fat = (U8*)apImage;
	fat += mDISK_IMAGE_U16_READ( boot->mBytesPerSector );

	return fat;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetpRootDirectory( const sDiskImageST * apImage )
* ACTION   : gets the root directory from a disk image
			 the root directory follows the bootsector and FAT(s)
			 typically a bootsector holds 112 files (7 sectors)
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageDirEntry	* Disk_Image_ST_GetpRootDirectory( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * dir = (U8*)apImage;
	U16 secSize = mDISK_IMAGE_U16_READ( boot->mBytesPerSector );
	U16 fatSize = mDISK_IMAGE_U16_READ( boot->mSectorsPerFAT );
	dir += ((fatSize * boot->mNumberOfFATs + 1) * secSize );

	return (sDiskImageDirEntry*)dir;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetpData( const sDiskImageST * apImage )
* ACTION   : gets the first data sector from disk
			 disk structure:
			 bootsector(1)->FAT0(5)->FAT1(5)->Root(7)->DATA at sector 18
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	* Disk_Image_ST_GetpData( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * data = (U8*)apImage;
	U16 secSize = mDISK_IMAGE_U16_READ( boot->mBytesPerSector );
	U16 fatSize = mDISK_IMAGE_U16_READ( boot->mSectorsPerFAT );
	U16 rootEntries = mDISK_IMAGE_U16_READ( boot->mNumberOfDirEntries );
	data += ( ( fatSize * boot->mNumberOfFATs + 1 ) * secSize );
	data += rootEntries * sizeof( sDiskImageDirEntry );

	return data;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetpDataCluster( const sDiskImageST * apImage, U16 aCluster )
* ACTION   : get pointer to a specific data cluster
			 bootsector(1)->FAT0(5)->FAT1(5)->Root(7)->DATA + (aCluster * sectorSize * sectorsPerCluster)
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	* Disk_Image_ST_GetpDataCluster( const sDiskImageST * apImage, U16 aCluster )
{
	U32 clusterSize = Disk_Image_ST_GetClusterSize( apImage );
	U8 * data = Disk_Image_ST_GetpData( apImage );
	data +=  clusterSize * aCluster;

	return data;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName )
* ACTION   : check to see if entry in directory has same filename as one passed in
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U8	DiskImageDirEntry_FileNameEqual( const sDiskImageDirEntry * entry, const char * apFileName )
{
	U16 i;

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

	for( i=0; (i<8) && ('.'!=apFileName[i]) && (apFileName[i]); i++ )
		apEntry->mFileName[i] = apFileName[i];
	if( '.' == apFileName[i ])
	{
		apFileName = &apFileName[ i + 1 ];
		for( i=0; (i<3) && apFileName[i]; i++)
			apEntry->mExtension[i] = apFileName[i];
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetClusterNext( const sDiskImageST * apImage, U16 aSector )
* ACTION   : retrieves the next cluster in a FAT12 table
			 each byte is encoded in little endian 12 bit format
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16		Disk_Image_ST_GetClusterNext( const sDiskImageST * apImage, U16 aCluster )
{
	U16	next = 0;
	sBootSector * boot = (sBootSector*)apImage;
	U8 * mem = (U8*)apImage;
	mem += mDISK_IMAGE_U16_READ( boot->mBytesPerSector );
	mem += ( aCluster >> 1 ) * 3;

	if( aCluster & 1 )
	{
		next = mem[ 2 ];
		next <<= 4;
		next |= ( ( mem[ 1 ] >> 4 ) & 0xF );
	}
	else
	{
		next = mem[ 1 ] & 0x0F;
		next <<= 8;
		next |= mem[ 0 ];
	}
	return next;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_SetCluster( const sDiskImageST * apImage, U16 aCluster, U16 aValue )
* ACTION   : sets a cluster index in FAT to specific value, used to link clusters of a file
			 FAT[ aCluster ] = aValue
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void		Disk_Image_ST_SetCluster( const sDiskImageST * apImage, U16 aCluster, U16 aValue )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * mem = (U8*)apImage;
	mem += mDISK_IMAGE_U16_READ( boot->mBytesPerSector );
	mem += ( aCluster >> 1 ) * 3;

	if( aCluster & 1 )
	{
		mem[ 2 ] = (U8)((aValue>>4) & 0xFF);
		mem[ 1 ] &= 0xF;
		mem[ 1 ] |= ( aValue & 0xF ) << 4;
	}
	else
	{
		mem[ 1 ] &= 0xF0;
		mem[ 1 ] |= (( aValue >> 8 ) & 0xF);
		mem[ 0 ] = (U8)(aValue & 0xFF);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_GetSectorFree( const sDiskImageST * apImage )
* ACTION   : walks FAT looking for first free cluster (zero value)
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U16		Disk_Image_GetSectorFree( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U16	clusterCount = mDISK_IMAGE_U16_READ( boot->mBytesPerSector ) / boot->mSectorsPerCluster;
	U16 i;
	U8 * mem = (U8*)apImage;
	mem += mDISK_IMAGE_U16_READ( boot->mBytesPerSector );

	for( i = 2; i < clusterCount; i++ )
	{
		U8	val = *mem++;
		if( i & 1 )
		{
			val &= 0xF0;
			val |= *mem++;
		}
		else if( i & 1 )
		{
			val |= ( *mem ) & 0xF;
		}
		if( !val )
			return i;
	}
	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_BootSector_Init( sBootSector * apBoot, const sDiskFormatParameters * apParams )
* ACTION   : sets some initial values in the bootsector
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void	Disk_BootSector_Init( sBootSector * apBoot, const sDiskFormatParameters * apParams )
{
	/* get a pseudo random unique number for disk. used by TOS to detect media changes */
	U32	pattern = Time_GetAbsTime();
	U16 sectorCount = apParams->mSectorsPerTrackCount * apParams->mTrackCount * apParams->mSideCount;
	Memory_Clear( sizeof( sBootSector ), apBoot );
	
	apBoot->mBranch[ 0 ] = 0xEB;
	apBoot->mBranch[ 1 ] = 0x30;

	apBoot->mSerialPattern[ 0 ] = ( pattern >> 0 ) & 0xFF;
	apBoot->mSerialPattern[ 1 ] = ( pattern >> 8 ) & 0xFF;
	apBoot->mSerialPattern[ 2 ] = ( pattern >> 16 ) & 0xFF;

	mDISK_IMAGE_U16_WRITE( apBoot->mBytesPerSector, apParams->mSectorByteCount );
	apBoot->mSectorsPerCluster = 2;
	mDISK_IMAGE_U16_WRITE( apBoot->mReservedSectors, 1 );
	apBoot->mNumberOfFATs = 2;
	/* typical disk uses 7 sectors for root directory, so 112 entries on 512 byte/sector disks */
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfDirEntries, 0x70 );
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfSectors, sectorCount );
	apBoot->mMediaDescriptor = 0xF9;	/* 3.5-inch double sided, 80 tracks per side, 9 sectors per track (720 KB) */
	mDISK_IMAGE_U16_WRITE( apBoot->mSectorsPerFAT, 5 );
	mDISK_IMAGE_U16_WRITE( apBoot->mSectorsPerTrack, apParams->mSectorsPerTrackCount );
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfSides, apParams->mSideCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_Create( const sDiskFormatParameters * apParams )
* ACTION   : creates a disk image based on parameters
			allocates memory and inits bootsector
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageST *	Disk_Image_ST_Create( const sDiskFormatParameters * apParams )
{
	U8 *	pMem;
	U32		size;

	size  = apParams->mTrackCount;
	size *= apParams->mSectorsPerTrackCount;
	size *= apParams->mSectorByteCount;
	size *= apParams->mSideCount;

	pMem = mMEMCALLOC( size );
	if( !pMem )
		return 0;

	Disk_BootSector_Init( (sBootSector*)pMem, apParams );

	return (sDiskImageST*)pMem;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_Save( const sDiskImageST * apImage, const char * apFileName )
* ACTION   : saves disk image to disk under specified filename
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void			Disk_Image_ST_Save( const sDiskImageST * apImage, const char * apFileName )
{
	U32 size = Disk_Image_ST_GetSizeBytes( apImage );

	File_Save( apFileName, apImage, size );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetSizeBytes( const sDiskImageST * apImage )
* ACTION   : gets the size in bytes of the disk images (sector size * number of sectors)
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

U32				Disk_Image_ST_GetSizeBytes( const sDiskImageST * apImage )
{
	U32	size = mDISK_IMAGE_U16_READ( apImage->mBytesPerSector );

	size *= mDISK_IMAGE_U16_READ( apImage->mNumberOfSectors );

	return size;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_Load( const char * apFileName )
* ACTION   : loads a disk image from disk
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

sDiskImageST *	Disk_Image_ST_Load( const char * apFileName )
{
	sDiskImageST * image = (sDiskImageST*)File_Load( apFileName );
	return image;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetDirEntry_Internal( const sDiskImageST * apImage, const sDiskImageDirEntry * apDir, U16 aEntryCount, const char * apFileName )
* ACTION   : retrieves an entry from a directory, if it exists and matches filename
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

const sDiskImageDirEntry *			Disk_Image_ST_GetDirEntry_Internal( const sDiskImageST * apImage, const sDiskImageDirEntry * apDir, U16 aEntryCount, const char * apFileName )
{
	U16 i;

	for( i = 0; i < aEntryCount; i++ )
	{
		const sDiskImageDirEntry * entry = &apDir[ i ];
		if( DiskImageDirEntry_FileNameEqual( entry, apFileName ) )
		{
			for( ;*apFileName && !StringPath_IsSeperator( *apFileName ); apFileName++ );
			if( *apFileName )
			{
				U32 clusterSize = Disk_Image_ST_GetClusterSize( apImage );
				U32	dirSize = clusterSize;
				U16	cluster;
				sDiskImageDirEntry * lpDir;

				Endian_ReadLittleU16( &entry->mFirstCluster, cluster );

				lpDir = (sDiskImageDirEntry*)Disk_Image_ST_GetpDataCluster( apImage, cluster );

				while( cluster < 0xFF7 )
				{
					dirSize += clusterSize;
					cluster = Disk_Image_ST_GetClusterNext( apImage, cluster );
				}
				apFileName++;
				return Disk_Image_ST_GetDirEntry_Internal( apImage, lpDir, (U16)(dirSize / sizeof(sDiskImageDirEntry)), apFileName );
			}
			return( entry );
		}
	}
	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_GetDirEntry( const sDiskImageST * apImage, const char * apFileName )
* ACTION   : retrieves an entry from a directory, if it exists and matches filename
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

const sDiskImageDirEntry *			Disk_Image_ST_GetDirEntry( const sDiskImageST * apImage, const char * apFileName )
{
	sBootSector * boot = (sBootSector*)apImage;
	U16 rootEntries = mDISK_IMAGE_U16_READ( boot->mNumberOfDirEntries );
	const sDiskImageDirEntry * rootDir = Disk_Image_ST_GetpRootDirectory( apImage );

	const sDiskImageDirEntry * entry = Disk_Image_ST_GetDirEntry_Internal( apImage, rootDir, rootEntries, apFileName );

	return entry;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_FileLoad( const sDiskImageST * apImage, const char * apFileName )
* ACTION   : loads a file from the disk image
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void *			Disk_Image_ST_FileLoad( const sDiskImageST * apImage, const char * apFileName )
{
	U8 * mem = 0;
	U8 * dst = 0;
	U32	clusterSize = Disk_Image_ST_GetClusterSize( apImage );
	U16 cluster;
	U32 size;

	const sDiskImageDirEntry * entry = Disk_Image_ST_GetDirEntry( apImage, apFileName );

	if( !entry )
		return 0;

	Endian_ReadLittleU32( &entry->mSize, size );
	Endian_ReadLittleU16( &entry->mFirstCluster, cluster );

	mem = mMEMCALLOC( size );
	if( !mem )
		return mem;

	dst = mem;
	while( size )
	{
		U8 * src = Disk_Image_ST_GetpDataCluster( apImage, cluster );

		if( size < clusterSize )
			clusterSize = size;

		Memory_Copy( clusterSize, src, dst );

		size -= clusterSize;
		dst += clusterSize;

		cluster = Disk_Image_ST_GetClusterNext( apImage, cluster );
	}

	return mem;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_FileUnLoad( void * apFile )
* ACTION   : frees memory associated with a file
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void			Disk_Image_ST_FileUnLoad( void * apFile )
{
	mMEMFREE( apFile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Disk_Image_ST_FileDelete( sDiskImageST * apImage, const char * apFileName )
* ACTION   : deletes a file from an image
* CREATION : 12-11-08 PNK
*-----------------------------------------------------------------------------------*/

void 			Disk_Image_ST_FileDelete( sDiskImageST * apImage, const char * apFileName )
{
	sDiskImageDirEntry * entry = (sDiskImageDirEntry *)Disk_Image_ST_GetDirEntry( apImage, apFileName );
	U16 cluster;
	U16 clusterNext;

	if( !entry )
		return;

	entry->mFileName[ 0 ] = 0xE5;
	entry->mSize = 0;

	Endian_ReadLittleU16( &entry->mFirstCluster, cluster );

	while( cluster < 0xFF7 )
	{
		clusterNext = Disk_Image_ST_GetClusterNext( apImage, cluster );
		Disk_Image_ST_SetCluster( apImage, cluster, 0 );
		cluster = clusterNext;
	}
}

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


U8				Disk_Image_ST_FileSave( sDiskImageST * apImage, void * apBuffer, U32 aSize, const char * apFileName ) 
{
	sDiskImageDirEntry * entry = (sDiskImageDirEntry *)Disk_Image_ST_GetDirEntry( apImage, apFileName );

	if( entry )
	{

	}
	else
	{

	}


	(void)apImage;
	(void)apFileName;
	(void)aSize;
	(void)apBuffer;

	return  0;
}


void			Disk_Image_ST_UnLoad( sDiskImageST * apImage )
{
	mMEMFREE( apImage );
}


/* ################################################################################ */

