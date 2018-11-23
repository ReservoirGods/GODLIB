#include	<GODLIB\UNITTEST\UNITTEST.H>

#include	<GODLIB\DRIVE\DISK_IO.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\STRING\STRING.H>

char * gTextText ="this is some test text that we are going to save to a disk image";
char * gTextText2 ="this is another file onthe disk";
char * gTextText3 ="this gonna be file 3";

GOD_UNIT_TEST( Disk )
{
	sDiskImage	image;
	sDiskFormatParameters format;
	U32 size;
	U8 result;
	const char * fileName="POO/TEST.TXT";
	const char * fileName2="ROOT.TXT";
	const char * fileName3="POO/SECOND/HIDE.TXT";

	DiskFormatParameters_Init( &format );
	result = DiskImage_Create( &format, "BLANK.ST");
	GOD_UNIT_TEST_ASSERT( result, "couldn't create .ST file ");

	result = DiskImage_Load( &image, &gfDiskImageFuncs_ST_Memory, "BLANK.ST" );

	{
		U16 i;
		U16 cluster = DiskImage_FAT_GetFreeCluster( &image, 0 );
		GOD_UNIT_TEST_EXPECT( 2==cluster, "couldn't grab default free cluster");

		for( i=2; i<image.mClusterTotalCount-1; i++ )
		{
			cluster = DiskImage_FAT_GetFreeCluster( &image, 0 );
			GOD_UNIT_TEST_EXPECT( i==cluster, "couldn't grab next free cluster");

			DiskImage_FAT_SetNextClusterIndex( &image, cluster, cluster );

			if( i > 2 )
			{
				U16 empty;
				DiskImage_FAT_SetNextClusterIndex( &image, cluster-1, 0 );
				empty = DiskImage_FAT_GetFreeCluster( &image, 0 );
				GOD_UNIT_TEST_EXPECT( i-1==empty, "couldn't grab recently freed free cluster");
				DiskImage_FAT_SetNextClusterIndex( &image, cluster-1, cluster-1 );
			}
		}

		for( i=2; i<image.mClusterTotalCount; i++ )
		{
			DiskImage_FAT_SetNextClusterIndex( &image, i, 0 );
		}
		cluster = DiskImage_FAT_GetFreeCluster( &image, 0 );
		GOD_UNIT_TEST_EXPECT( 2==cluster, "couldn't grab default free cluster");

		for( i = 2; i < image.mClusterTotalCount; i++ )
		{
			DiskImage_FAT_SetNextClusterIndex( &image, i, 1 );
		}

		for( i = 2; i < image.mClusterTotalCount; i++ )
		{
			U16 target = GOD_UNIT_TEST_RAND( image.mClusterTotalCount - 3 ) + 2;
			DiskImage_FAT_SetNextClusterIndex( &image, target, 0 );

			cluster = DiskImage_FAT_GetFreeCluster( &image, 0 );
			GOD_UNIT_TEST_EXPECT( target == cluster, "couldn't grab default free cluster" );
			DiskImage_FAT_SetNextClusterIndex( &image, target, 1 );
		}
	}


	GOD_UNIT_TEST_ASSERT( result, "couldn't load .ST file ");

	size = String_StrLen(gTextText);
	result = DiskImage_File_Save( &image, fileName, gTextText, size );
	GOD_UNIT_TEST_ASSERT( result, "couldn't save file ");

	result = DiskImage_File_Save( &image, fileName2, gTextText2, String_StrLen(gTextText2) );
	result = DiskImage_File_Save( &image, fileName3, gTextText3, String_StrLen(gTextText3) );

	{
		U32 	blockSize = 512*8;
		U32 i;
		char * block = mMEMCALLOC( blockSize );
		for( i=0; i<blockSize; i++ )
		{
			if( 62 == ( i & 63 ) )
				block[ i ] = 10;
			else if( 63 == ( i & 63 ) )
				block[ i ] = 13;
			else
				block[ i ] = 'A'+( ((i/64)%26));
		}
		DiskImage_File_Save( &image, "BLOCK.TXT", block, blockSize );
		mMEMFREE( block );
	}

	GOD_UNIT_TEST_ASSERT( DiskImage_File_Exists(&image, fileName), "file exists error");

	{
		sDiskImageDirWalker walker;
		U16 count;
		DiskImage_DirWalker_Init( &image, 0, &walker );

		for( count=0; DiskImage_DirWalker_Next(&walker); count++ );

/*		GOD_UNIT_TEST_EXPECT( 1==count, "couldn't walk .ST file ");*/
	}
	DiskImage_Save( &image, "BLANK.ST" );
	DiskImage_UnLoad( &image );

}