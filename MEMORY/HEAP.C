/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"HEAP.H"

#include	"MEMORY.H"


/* ###################################################################################
#  DATA
################################################################################### */

#define	dHEAP_ALIGN_SIZE	3L
#define	dHEAP_HEADER_SIZE	3L
#define	dHEAP_PREV_BIT		1L
#define	dHEAP_FASTBIN_SIZE	64L
#define	dHEAP_BIN_LIMIT		128L
#define	dHEAP_FASTBIN_LIMIT		128L

#define dHEAP_LARGE_SIZE	(1024*128)
#define mHEAP_GET_SMALLBIN_INDEX( a )	0	
#define mHEAP_GET_LARGEBIN_INDEX( a )	0	


#define	mHEAP_GET_FASTBIN_INDEX( aSize ) \
	( aSize >> 3L )

#define	mHEAP_CHUNK2MEM( apChunk ) \
	( (void*)( ((U32)(apChunk)) + 8L ) )

#define	mHEAP_MEM2CHUNK( apMem ) \
	( (sHeapChunk*)( ((U32)(apMem)) - 8L ) )

#define	mHEAP_PREV_ACTIVE( apChunk ) \
	( !((apChunk->mPrevSize) & dHEAP_PREV_BIT) )

#define	mHEAP_pCHUNK_BUILD( apChunk, aOffset ) \
	( (sHeapChunk*)( ((U8*)(apChunk)) + (aOffset)  ) )

#define	mHEAP_CHUNK_GET_SIZE( apChunk ) \
	( ((apChunk)->mSize) & (~dHEAP_PREV_BIT) )

#define	mHEAP_CHUNK_UNLINK( apChunk )					\
	apChunk->mpPrev->mpNext = apChunk->mpNext;	\
	apChunk->mpNext->mpPrev = apChunk->mpPrev;

#define	mHEAP_CHUNK_SET_FOOT( apChunk ,aSize )	\
	( (sHeapChunk*)( ((U8*)(apChunk)) + (aSize) ))->mPrevSize = (aSize)


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

typedef struct	sHeapChunk
{
	U32		mPrevSize;
	U32		mSize;
	struct	sHeapChunk *	mpPrev;
	struct	sHeapChunk *	mpNext;
} sHeapChunk;

typedef struct	sHeap
{
	U32				mLockedFlag;
	U32				mSize;

	void *			mpBase;
	void *			mpLastRemainder;

	sHeapChunk *	mpBins[ dHEAP_BIN_LIMIT ];
	sHeapChunk *	mpFastBins[ dHEAP_FASTBIN_LIMIT ];

	sHeapChunk *	mpLastChunk;
	sHeapChunk *	mpTopChunk;
	sHeapChunk *	mpUnsortedChunks;
}sHeap;

void	Heap_FastBinsMerge( sHeap * apHeap );

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_Init( sHeap * apHeap,const U32 aSize )
* ACTION   : Heap_Init
* CREATION : 18.09.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Heap_Init( sHeap * apHeap,const U32 aSize )
{
	U32	i;

	apHeap->mSize           = aSize;	
	apHeap->mLockedFlag     = 0;
	apHeap->mpBase          = Memory_Alloc( aSize );
	apHeap->mpLastRemainder = 0;

	for( i=0; i<dHEAP_BIN_LIMIT; i++ )
	{
		apHeap->mpBins[ i ] = 0;
	}

	for( i=0; i<dHEAP_BIN_LIMIT; i++ )
	{
		apHeap->mpFastBins[ i ] = 0;
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_DeInit( sHeap * apHeap )
* ACTION   : Heap_DeInit
* CREATION : 18.09.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Heap_DeInit( sHeap * apHeap )
{
	mMEMFREE( apHeap->mpBase );
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_Reset( sHeap * apHeap )
* ACTION   : Heap_Reset
* CREATION : 18.09.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Heap_Reset( sHeap * apHeap )
{
	(void)	apHeap;
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_Alloc( sHeap * apHeap,const U32 aSize )
* ACTION   : Heap_Alloc
* CREATION : 18.09.2003 PNK
*-----------------------------------------------------------------------------------*/

void *	Heap_Alloc( sHeap * apHeap,const U32 aSize )
{
	U32					lSize;
	U32					lIndex;
	sHeapChunk *		lpChunk;
	void *				lpMem;
	
	lSize = (aSize + sizeof(sHeapChunk) + dHEAP_ALIGN_SIZE) & (~dHEAP_ALIGN_SIZE);

/*	if( allocatedstuff )*/
	{
		if( lSize <= dHEAP_FASTBIN_SIZE )
		{
			lIndex  = mHEAP_GET_FASTBIN_INDEX( lSize );
			lpChunk = apHeap->mpFastBins[ lIndex ];
			if( lpChunk )
			{
				apHeap->mpFastBins[ lIndex ] = lpChunk->mpNext;
				return( mHEAP_CHUNK2MEM( lpChunk ) );
			}
		}
		if( lSize < dHEAP_LARGE_SIZE )
		{
			lIndex  = mHEAP_GET_SMALLBIN_INDEX( lSize );
			lpChunk = apHeap->mpBins[ lIndex ];
			if( lpChunk )
			{
				apHeap->mpBins[ lIndex ] = lpChunk->mpPrev;
				if( apHeap->mpBins[ lIndex ] )
				{
					apHeap->mpBins[ lIndex ]->mpNext = lpChunk->mpNext;
				}
				return( mHEAP_CHUNK2MEM( lpChunk ) );
			}
		}
		else
		{
			lIndex = mHEAP_GET_LARGEBIN_INDEX( lSize );
			Heap_FastBinsMerge( apHeap );
		}
	}

	lpMem = 0;

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_Free( sHeap * apHeap,void * apMem )
* ACTION   : Heap_Free
* CREATION : 20.09.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Heap_Free( sHeap * apHeap,void * apMem )
{
	U32				lSize;
	sHeapChunk *	lpChunk;
	(void)apHeap;
	if( apMem )
	{
		lpChunk = mHEAP_MEM2CHUNK( apMem );
		lSize   = mHEAP_CHUNK_GET_SIZE( lpChunk );
	}
	
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : Heap_FastBinsMerge( sHeap * apHeap )
* ACTION   : Heap_FastBinsMerge
* CREATION : 19.09.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Heap_FastBinsMerge( sHeap * apHeap )
{
	U32				i;
	U32				lSize;
	U32				lAfterSize;
	sHeapChunk **	lpBin;
	sHeapChunk *	lpChunk;
	sHeapChunk *	lpChunkNext;
	sHeapChunk *	lpChunkAfter;

	i     = dHEAP_FASTBIN_LIMIT;
	lpBin = &apHeap->mpFastBins[ 0 ];

	while( i-- )
	{
		lpChunk = *lpBin;

		while( lpChunk )
		{
			lpChunkNext  = lpChunk->mpNext;
			lSize        = mHEAP_CHUNK_GET_SIZE( lpChunk );
			lpChunkAfter = mHEAP_pCHUNK_BUILD( lpChunk, lSize );
			lAfterSize   = mHEAP_CHUNK_GET_SIZE( lpChunkAfter );

			if( mHEAP_PREV_ACTIVE( lpChunk ) )
			{
				lSize  += lpChunk->mPrevSize;
				lpChunk = mHEAP_pCHUNK_BUILD( lpChunk, 0-lpChunk->mPrevSize );
				mHEAP_CHUNK_UNLINK( lpChunk );
			}

			if( lpChunkAfter == apHeap->mpTopChunk )
			{
				lSize              += lAfterSize;
				lpChunk->mSize      = lSize | dHEAP_PREV_BIT;
				apHeap->mpTopChunk  = lpChunk;
			}
			else
			{

				if( mHEAP_PREV_ACTIVE( lpChunkAfter ) )
				{
					lSize += lAfterSize;
					mHEAP_CHUNK_UNLINK( lpChunkAfter );
				}
				lpChunkAfter->mSize              = lAfterSize;

				lpChunkAfter                     = apHeap->mpUnsortedChunks->mpNext;
				lpChunkAfter->mpPrev             = lpChunk;
				apHeap->mpUnsortedChunks->mpNext = lpChunk;
				lpChunk->mpPrev                  = apHeap->mpUnsortedChunks;
				lpChunk->mpNext                  = lpChunkAfter;
				lpChunk->mSize                   = lSize | dHEAP_PREV_BIT;

				lpChunk->mSize      = lSize | dHEAP_PREV_BIT;
				mHEAP_CHUNK_SET_FOOT( lpChunk, lSize );
			}

			lpChunk     = lpChunkNext;
		}
		*lpBin++ = 0;
	}
}




/* ################################################################################ */
