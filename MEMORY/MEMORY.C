/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: MEMORY.C
::
:: Low level memory management routines
::
:: This file contains all the platform specific memory management routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"MEMORY.H"

#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/DEBUG/DEBUG.H>
#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#ifdef dGODLIB_PLATFORM_WIN
#define	dMEMORY_RECORD_LIMIT	65536
#else
#define	dMEMORY_RECORD_LIMIT	4096
#endif
#define	dMEMORY_HEADER_SIZE		16
#define	dMEMORY_TRAILER_SIZE	16


/* ###################################################################################
#  ENUMERATED TYPES
################################################################################### */

enum
{
	eMX_STRAM = 0,
	eMX_TTRAM = 1,
	eMX_PREFSTRAM = 2,
	eMX_PREFTTRAM = 2
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sMemoryTrackRecord
{
	const char *	mpFileName;
	U32				mLine;
	void *			mpMem;
	U32				mSize;
	U32				mIndex;
} sMemoryTrackRecord;


/* ###################################################################################
#  DATA
################################################################################### */

U8	gMemoryHeader[ dMEMORY_HEADER_SIZE ] =
{
	0x11,0xF2,0x36,0x77,
	0xBA,0xA5,0xF6,0x9E,
	0x20,0xCD,0x48,0xE2,
	0x61,0xD3,0x06,0x57,
};

U8	gMemoryTrailer[ dMEMORY_TRAILER_SIZE ] =
{
	0x23,0x84,0x1F,0xB8,
	0x76,0x39,0xC0,0x5B,
	0xA1,0x05,0x9A,0xFC,
	0x62,0xDE,0x4D,0xE7,
};

U32	gMemoryAllocCount    = 0;
U32	gMemoryAllocatedSize = 0;
U32	gMemoryFailedSize    = 0;
U32	gMemoryHighTide      = 0;
U32 gMemoryLargestAlloc  = 0;
U32	gMemorySmallestAlloc = 0x7FFFFFFL;


#ifdef dMEMORY_TRACK

U8	gMemoryTrackInitialised = 0;
U32 gMemoryTrackIndex = 0;
sMemoryTrackRecord	gMemoryRecord[ dMEMORY_RECORD_LIMIT ];

#endif

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void					Memory_TrackInit( void );
void					Memory_TrackAlloc( const void * apMem, const U32 aSize, const char * apFileName, const U32 aLine );
void					Memory_TrackFree( const void * apMem );
sMemoryTrackRecord *	Memory_TrackGetFreeRecord( void );
sMemoryTrackRecord *	Memory_TrackFindRecord( const void * apMem );


/* ###################################################################################
#  CODE
################################################################################### */

#ifdef dMEMORY_TRACK

/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_TrackInit()
* ACTION   : initialises memory tracking
* CREATION : 16.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Memory_TrackInit()
{
	U32	i;

	gMemoryAllocCount    = 0;
	gMemoryAllocatedSize = 0;
	gMemoryHighTide      = 0;
	gMemoryLargestAlloc  = 0;
	gMemoryFailedSize    = 0;
	gMemorySmallestAlloc = 0x7FFFFFFFL;

	for( i=0; i<dMEMORY_RECORD_LIMIT; i++ )
	{
		gMemoryRecord[ i ].mpMem = 0;
		gMemoryRecord[ i ].mSize = 0;
	}

	gMemoryTrackInitialised = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_TrackGetFreeRecord()
* ACTION   : returns a free space in records array
* CREATION : 16.01.01 PNK
*-----------------------------------------------------------------------------------*/

sMemoryTrackRecord *	Memory_TrackGetFreeRecord()
{
	U16	i;

	for( i=0; i<dMEMORY_RECORD_LIMIT; i++ )
	{
		if( !gMemoryRecord[ i ].mpMem )
		{
			return( &gMemoryRecord[ i ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_TrackFindRecord( const void * apMem )
* ACTION   : finds record with specified address
* CREATION : 16.01.01 PNK
*-----------------------------------------------------------------------------------*/

sMemoryTrackRecord *	Memory_TrackFindRecord( const void * apMem )
{
	U32	i;

	for( i=0; i<dMEMORY_RECORD_LIMIT; i++ )
	{
		if( apMem == gMemoryRecord[ i ].mpMem )
		{
			return( &gMemoryRecord[ i ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_TrackAlloc( const void * apMem,const U32 aSize,const char * apFileName,const U32 aLine )
* ACTION   : Memory_TrackAlloc
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Memory_TrackAlloc( const void * apMem,const U32 aSize,const char * apFileName,const U32 aLine )
{
	sMemoryTrackRecord *	lpRecord;

	if( !gMemoryTrackInitialised )
	{
		Memory_TrackInit();
	}

	if( !apMem )
	{
		gMemoryFailedSize += aSize;
		return;
	}

	gMemoryAllocCount++;
	gMemoryAllocatedSize += aSize;
	if( aSize > gMemoryLargestAlloc )
	{
		gMemoryLargestAlloc = aSize;
	}
	if( aSize < gMemorySmallestAlloc )
	{
		gMemorySmallestAlloc = aSize;
	}
	if( gMemoryAllocatedSize > gMemoryHighTide )
	{
		gMemoryHighTide = gMemoryAllocatedSize;
	}

	lpRecord = Memory_TrackGetFreeRecord();
	if( lpRecord )
	{
		lpRecord->mpMem      = (void*)apMem;
		lpRecord->mSize      = aSize;
		lpRecord->mLine      = aLine;
		lpRecord->mpFileName = apFileName;
		lpRecord->mIndex	 = gMemoryTrackIndex;
	}

	gMemoryTrackIndex++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_TrackFree( const void * apMem )
* ACTION   : tracks memory frees
* CREATION : 16.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Memory_TrackFree( const void * apMem )
{
	sMemoryTrackRecord *	lpRecord;

	if( !gMemoryTrackInitialised )
		Memory_TrackInit();

	if( !apMem )
		return;


	gMemoryAllocCount--;

	lpRecord = Memory_TrackFindRecord( apMem );
	if( lpRecord )
	{
		gMemoryAllocatedSize -= lpRecord->mSize;

		lpRecord->mpMem = 0;
		lpRecord->mSize = 0;
	}
	else
	{
		DebugChannel_Printf0( eDEBUGCHANNEL_MEMORY, "WARNING: memory record not found\n" );
	}
}

#endif

/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_Alloc( const U32 aSize )
* ACTION   : allocates memory of specified size
*            FastRAM allocated if available. memory is not cleared.
*            do NOT use this call to allocate memory for the screen!
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_Alloc( const U32 aSize )
{
	void *	lpMem;
	U32	lSize;

	lSize = aSize;

#ifdef	dMEMORY_GUARD
	lSize += (dMEMORY_HEADER_SIZE + dMEMORY_TRAILER_SIZE);
#endif
#if	1
	if( !aSize )
	{
		DebugChannel_Printf0( eDEBUGCHANNEL_MEMORY, "WARNING: 0 byte alloc" );
	}
	lpMem = malloc( lSize );
#else
	if( System_GetTosVersion() > 0x200 )
	{
		lpMem = GemDos_Mxalloc( lSize, eMX_PREFTTRAM );
	}
	else
	{
		lpMem = GemDos_Malloc( lSize );
	}
#endif


#ifdef	dMEMORY_GUARD
	if( lpMem )
	{
		Memory_Copy( dMEMORY_HEADER_SIZE,  gMemoryHeader,   lpMem );
		Memory_Copy( dMEMORY_TRAILER_SIZE, gMemoryTrailer,  (void*)((U32)lpMem + (lSize -= dMEMORY_TRAILER_SIZE)) );
		lpMem = (void*)((U32)lpMem + dMEMORY_HEADER_SIZE);
	}
#endif
	if( !lpMem )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_MEMORY, "WARNING: memory allocation failed: %d bytes\n", aSize );
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_ScreenAlloc( U32 aSize )
* ACTION   : allocates memory of specified size
*            allocates ST RAM for the screen. memory not cleared.
*            only use this call to allocate memory for the screen!
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

void * 	Memory_ScreenAlloc( const U32 aSize )
{
	void *	lpMem;
	U32		lSize;

	lSize = aSize;
#ifdef	dMEMORY_GUARD
	lSize += (dMEMORY_HEADER_SIZE + dMEMORY_TRAILER_SIZE);
#endif

#ifndef	dGODLIB_PLATFORM_ATARI
	lpMem = malloc( lSize );
#else

	if( System_GetTosVersion() > 0x200 )
	{
		lpMem = GemDos_Mxalloc( lSize, eMX_STRAM );
	}
	else
	{
		lpMem = GemDos_Malloc( lSize );
	}

#endif
	if( !lpMem )
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_MEMORY, "Memory_ScreenAlloc() : failed %ld bytes\n", aSize );
	}


/*	Debug_Action( Memory_TrackAlloc( lpMem, aSize ) );*/
#ifdef	dMEMORY_GUARD
	if( lpMem )
	{
		Memory_Copy( dMEMORY_HEADER_SIZE,  gMemoryHeader,   lpMem );
		Memory_Copy( dMEMORY_TRAILER_SIZE, gMemoryTrailer,  (void*)((U32)lpMem + (lSize -= dMEMORY_TRAILER_SIZE)) );
		lpMem = (void*)((U32)lpMem + dMEMORY_HEADER_SIZE);
	}
#endif

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_ScreenRelease( void * apMem )
* ACTION   : Memory_ScreenRelease
* CREATION : 26.02.2005 PNK
*-----------------------------------------------------------------------------------*/

S32	Memory_ScreenRelease( void * apMem )
{
	S32		lRes;
	U8 *	lpMem;

	lRes = 1;
	if( apMem )
	{
		lpMem = (U8*)apMem;
#ifdef	dMEMORY_GUARD
		lpMem -= dMEMORY_HEADER_SIZE;
#endif

#ifndef	dGODLIB_PLATFORM_ATARI
		free( lpMem );
		lRes = 0;
#else
		lRes = (S32)GemDos_Mfree( lpMem );
#endif
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_Calloc( U32 aSize )
* ACTION   : allocates memory of specified size and clears it
*            FastRAM allocated if available.
*            do NOT use this call to allocate memory for the screen!
* CREATION : 11.04.99 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_Calloc( const U32 aSize )
{
	void * lpMem;

	lpMem = Memory_Alloc( aSize );

	if( lpMem )
	{
		Memory_Clear( aSize, lpMem );
	}
	else
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_MEMORY, "Memory_Calloc() : failed %ld bytes\n", aSize );
		DebugChannel_Printf2( eDEBUGCHANNEL_MEMORY, "total allocs %ld size %ld : \n", Memory_GetAllocCount(), Memory_GetAllocatedSize() );
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_ScreenCalloc( U32 aSize )
* ACTION   : allocates screen memory of specified size and clears it
*            ST RAM only allocated.
*            only use this call to allocate memory for the screen!
* CREATION : 11.04.99 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_ScreenCalloc( const U32 aSize )
{
	void * lpMem;

	lpMem = Memory_ScreenAlloc( aSize );

	if( lpMem )
	{
		Memory_Clear( aSize, lpMem );
	}
	else
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_MEMORY, "Memory_ScreenCalloc() : failed %ld bytes\n", aSize );
		DebugChannel_Printf2( eDEBUGCHANNEL_MEMORY, "total allocs %ld size %ld : \n", Memory_GetAllocCount(), Memory_GetAllocatedSize() );
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : _Memory_Release( void * apMem )
* ACTION   : frees memory
*            error handling included. returns error code or 0
* CREATION : 11.04.99 PNK
*-----------------------------------------------------------------------------------*/

S32		_Memory_Release( void * apMem )
{
	U8 *	lpMem;
	if( apMem )
	{
		lpMem = (U8*)apMem;

#ifdef	dMEMORY_GUARD
		lpMem -= dMEMORY_HEADER_SIZE;
#endif

#if	1
		free( lpMem );
		return( 0 );
#else
		return( (S32)GemDos_Mfree( lpMem ) );
#endif
	}
	else
	{
		DebugChannel_Printf1( eDEBUGCHANNEL_MEMORY, "Memory_Release() : failed %p\n", apMem );
		return( -1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_Clear( U32 aSize, void * apMem)
* ACTION   : clear aSize bytes from pMem
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

#ifndef dGODLIB_PLATFORM_ATARI

void	Memory_Clear( U32 aSize, void * apMem )
{
	U8	*	lpMem;

	if( apMem )
	{
		lpMem = (U8 *)apMem;

		while( aSize-- )
		{
			*lpMem++ = 0;
		}
	}
}

#endif

/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_Copy( U32 aSize, void * apSrc, void * apDst )
* ACTION   : copies size bytes from src to dest
*            error handling included
*            this routine needs optimizing!
* CREATION : 11.04.99 PNK
*-----------------------------------------------------------------------------------*/

void	Memory_Copy_Internal( U32 aSize, const void * apSrc, void * apDst )
{
	U8 *	lpSrc;
	U8 *	lpDst;

	lpSrc = (U8 *)apSrc;
	lpDst = (U8 *)apDst;

	if( (lpSrc) && (lpDst) )
	{
		if( lpSrc < lpDst )
		{
			lpSrc += aSize;
			lpDst += aSize;
			while( aSize-- )
			{
				*--lpDst = *--lpSrc;
			}
		}
		else
		{
			while( aSize-- )
			{
				*lpDst++ = *lpSrc++;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetFree( void )
* ACTION   : returns amount of free memory
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32		Memory_GetFree( void )
{
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetAllocCount( void )
* ACTION   : returns number
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetAllocCount( void )
{
	return( gMemoryAllocCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetAllocatedSize( void )
* ACTION   : returns amount of memory allocated
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetAllocatedSize( void )
{
	return( gMemoryAllocatedSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetHighTide( void )
* ACTION   : returns high tide for memory allocations
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetHighTide( void )
{
	return( gMemoryHighTide );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetLargestAlloc( void )
* ACTION   : returns size of largest alloc
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetLargestAlloc( void )
{
	return( gMemoryLargestAlloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetSmallestAlloc( void )
* ACTION   : returns size of smallest alloc
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetSmallestAlloc( void )
{
	return( gMemorySmallestAlloc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetFailedSize( void )
* ACTION   : Memory_GetFailedSize
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetFailedSize( void )
{
	return( gMemoryFailedSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_GetSize( void )
* ACTION   : Memory_GetSize
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Memory_GetSize( void )
{
	U32 *	lpRamTop;

	lpRamTop = (U32*)0x42EL;
	return( *lpRamTop );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 Memory_Validate(void)
* ACTION   : Memory_Validate
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 Memory_Validate(void)
{
#if defined(dMEMORY_GUARD) && defined(dMEMORY_TRACK)
	U8		lRes;
	U8 *	lpMem;
	U16		i,j;

	lRes = 1;

	for( i=0; i<dMEMORY_RECORD_LIMIT; i++ )
	{
		lpMem = gMemoryRecord[ i ].mpMem;
		if( lpMem )
		{
			j = 0;
			lpMem -= dMEMORY_HEADER_SIZE;
			while( (j<dMEMORY_HEADER_SIZE) && (lpMem[ j ] == gMemoryHeader[ j ]) )
			{
				j++;
			}
			if( j<dMEMORY_HEADER_SIZE )
			{
				lRes = 0;
				DebugChannel_Printf2( eDEBUGCHANNEL_MEMORY, "Memory_ValidateError() head %lx %ld", gMemoryRecord[ i ].mpMem, gMemoryRecord[ i ].mSize );
				*(U32*)0 = 0;
			}

			lpMem  = gMemoryRecord[ i ].mpMem;
			lpMem += gMemoryRecord[ i ].mSize;
			j = 0;
			while( (j<dMEMORY_TRAILER_SIZE) && (lpMem[ j ] == gMemoryTrailer[ j ]) )
			{
				j++;
			}
			if( j<dMEMORY_TRAILER_SIZE )
			{
				lRes = 0;
				DebugChannel_Printf2( eDEBUGCHANNEL_MEMORY, "Memory_ValidateError() tail %lx %ld", gMemoryRecord[ i ].mpMem, gMemoryRecord[ i ].mSize );
				*(U32*)0 = 0;
			}

		}
	}
	return( lRes );
#else
	return( 1 );
#endif

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_DbgScreenCalloc( const U32 aSize,const char * apFile,const U32 aLine )
* ACTION   : Memory_DbgScreenCalloc
* CREATION : 13.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_DbgScreenCalloc( const U32 aSize,const char * apFile,const U32 aLine )
{
	void * lpMem;

	(void)apFile;
	(void)aLine;
	Memory_Validate();
	lpMem = Memory_ScreenCalloc( aSize );
#ifdef dMEMORY_TRACK
	Debug_Action( Memory_TrackAlloc( lpMem, aSize, apFile, aLine ) );
#endif
	Memory_Validate();

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_DbgAlloc( const U32 aSize,const char * apFile,const U32 aLine )
* ACTION   : Memory_DbgAlloc
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_DbgAlloc( const U32 aSize,const char * apFile,const U32 aLine )
{
	void * lpMem;

	(void)apFile;
	(void)aLine;
	Memory_Validate();
	lpMem = Memory_Alloc( aSize );
#ifdef dMEMORY_TRACK
	Debug_Action( Memory_TrackAlloc( lpMem, aSize, apFile, aLine ) );
#endif
	Memory_Validate();

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_DbgCalloc( const U32 aSize,const char * apFile,const U32 aLine )
* ACTION   : Memory_DbgCalloc
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void *	Memory_DbgCalloc( const U32 aSize,const char * apFile,const U32 aLine )
{
	void * lpMem;

	(void)apFile;
	(void)aLine;
	Memory_Validate();
	lpMem = Memory_Calloc( aSize );
#ifdef dMEMORY_TRACK
	Debug_Action( Memory_TrackAlloc( lpMem, aSize, apFile, aLine ) );
#endif
	Memory_Validate();

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_DbgFree( void * apMem,const char * apFile,const U32 aLine )
* ACTION   : Memory_DbgFree
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

S32		Memory_DbgFree( void * apMem,const char * apFile,const U32 aLine )
{
	(void)apFile;
	(void)aLine;

	Memory_Validate();
#ifdef dMEMORY_TRACK
	Debug_Action( Memory_TrackFree( apMem ) );
#endif
	return( _Memory_Release( apMem ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_DbgScreenFree( void * apMem,const char * apFile,const U32 aLine )
* ACTION   : Memory_DbgScreenFree
* CREATION : 13.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S32	Memory_DbgScreenFree( void * apMem,const char * apFile,const U32 aLine )
{
	(void)apFile;
	(void)aLine;

	Memory_Validate();
#ifdef dMEMORY_TRACK
	Debug_Action( Memory_TrackFree( apMem ) );
#endif
	return( Memory_ScreenRelease( apMem ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Memory_ShowCurrentRecords( void )
* ACTION   : Memory_ShowCurrentRecords
* CREATION : 02.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Memory_ShowCurrentRecords( void )
{
#ifdef dMEMORY_TRACK
	U32	i;

	for( i=0; i<dMEMORY_RECORD_LIMIT; i++ )
	{
		if( gMemoryRecord[ i ].mpMem )
		{
/*			DebugChannel_Printf4( eDEBUGCHANNEL_MEMORY, "mem %08p : (%ld) %s : %ld\n",*/
			DebugLog_Printf5( "mem [%08ld] %08p : (%ld) %s : %ld\n",
				gMemoryRecord[ i ].mIndex,
				gMemoryRecord[ i ].mpMem,
				gMemoryRecord[ i ].mSize,
				gMemoryRecord[ i ].mpFileName,
				gMemoryRecord[ i ].mLine );
		}
	}
#endif
}

U8		Memory_IsEqual( const void * apMem0, const void * apMem1, U32 aSizeBytes )
{
	const U8 * lpM0 = (const U8*)apMem0;
	const U8 * lpM1 = (const U8*)apMem1;
	for( ;aSizeBytes && ( *lpM0++ == *lpM1++ ); aSizeBytes-- );
	return( 0 == aSizeBytes );
}

/* ################################################################################ */
