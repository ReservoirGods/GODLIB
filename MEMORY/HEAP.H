#ifndef	INCLUDED_HEAP_H
#define	INCLUDED_HEAP_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32		Heap_Init( struct sHeap * apHeap, const U32 aSize );
U32		Heap_DeInit( struct sHeap * apHeap );

U32		Heap_Reset( struct sHeap * apHeap );

void *	Heap_Alloc( struct sHeap * apHeap, const U32 aSize );
void	Heap_Free( struct sHeap * apHeap, void * apMem );


/* ################################################################################ */

#endif	/* INCLUDED_HEAP_H */
