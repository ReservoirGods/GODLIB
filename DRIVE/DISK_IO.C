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
#include	<GODLIB\FILE\FILE.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\RANDOM\RANDOM.H>
#include	<GODLIB\STRING\STRPATH.H>


#define	mDISK_IMAGE_U16_READ( _var )          ((((U16)_var[1]) << 8) | _var[0])
#define	mDISK_IMAGE_U16_WRITE( _var, _value ) { _var[0]=(_value>>8)&0xFF; _var[1]=_value&0xFF; }


/* ###################################################################################
#  CODE
################################################################################### */

U16	Disk_Image_ST_GetClusterSize( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U16 secSize = mDISK_IMAGE_U16_READ( boot->mBytesPerSector );

	return secSize * boot->mSectorsPerCluster;
}

sBootSector * Disk_Image_ST_GetpBootSector( const sDiskImageST * apImage )
{
	return (sBootSector *)apImage;
}


U8	* Disk_Image_ST_GetpFAT( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * fat = (U8*)apImage;
	fat += mDISK_IMAGE_U16_READ( boot->mBytesPerSector );

	return fat;
}

sDiskImageDirEntry	* Disk_Image_ST_GetpRootDirectory( const sDiskImageST * apImage )
{
	sBootSector * boot = (sBootSector*)apImage;
	U8 * dir = (U8*)apImage;
	U16 secSize = mDISK_IMAGE_U16_READ( boot->mBytesPerSector );
	U16 fatSize = mDISK_IMAGE_U16_READ( boot->mSectorsPerFAT );
	dir += ((fatSize * boot->mNumberOfFATs + 1) * secSize );

	return (sDiskImageDirEntry*)dir;
}


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


U8	* Disk_Image_ST_GetpDataCluster( const sDiskImageST * apImage, U16 aCluster )
{
	U32 clusterSize = Disk_Image_ST_GetClusterSize( apImage );
	U8 * data = Disk_Image_ST_GetpData( apImage );
	data +=  clusterSize * aCluster;

	return data;
}

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

void	Disk_BootSector_Init( sBootSector * apBoot, const sDiskFormatParameters * apParams )
{
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
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfDirEntries, 0x70 );
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfSectors, sectorCount );
	apBoot->mMediaDescriptor = 0xF9;	/* 3.5-inch double sided, 80 tracks per side, 9 sectors per track (720 KB) */
	mDISK_IMAGE_U16_WRITE( apBoot->mSectorsPerFAT, 5 );
	mDISK_IMAGE_U16_WRITE( apBoot->mSectorsPerTrack, apParams->mSectorsPerTrackCount );
	mDISK_IMAGE_U16_WRITE( apBoot->mNumberOfSides, apParams->mSideCount );
}


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


void			Disk_Image_ST_Save( const sDiskImageST * apImage, const char * apFileName )
{
	U32 size = Disk_Image_ST_GetSizeBytes( apImage );

	File_Save( apFileName, apImage, size );
}


U32				Disk_Image_ST_GetSizeBytes( const sDiskImageST * apImage )
{
	U32	size = mDISK_IMAGE_U16_READ( apImage->mBytesPerSector );

	size *= mDISK_IMAGE_U16_READ( apImage->mNumberOfSectors );

	return size;
}


sDiskImageST *	Disk_Image_ST_Load( const char * apFileName )
{
	sDiskImageST * image = (sDiskImageST*)File_Load( apFileName );
	return image;
}

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


const sDiskImageDirEntry *			Disk_Image_ST_GetDirEntry( const sDiskImageST * apImage, const char * apFileName )
{
	sBootSector * boot = (sBootSector*)apImage;
	U16 rootEntries = mDISK_IMAGE_U16_READ( boot->mNumberOfDirEntries );
	const sDiskImageDirEntry * rootDir = Disk_Image_ST_GetpRootDirectory( apImage );

	const sDiskImageDirEntry * entry = Disk_Image_ST_GetDirEntry_Internal( apImage, rootDir, rootEntries, apFileName );

	return entry;
}

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

void			Disk_Image_ST_FileUnLoad( void * apFile )
{
	mMEMFREE( apFile );
}


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

U8				Disk_Image_ST_FileSave( sDiskImageST * apImage, void * apBuffer, U32 aSize, const char * apFileName ) 
{
	sDiskImageDirEntry * entry = (sDiskImageDirEntry *)Disk_Image_ST_GetDirEntry( apImage, apFileName );

	if( entry )
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

