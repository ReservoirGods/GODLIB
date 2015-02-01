#ifndef	INCLUDED_HEAP_H
#define	INCLUDED_HEAP_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32		Heap_Init( sHeap * apHeap, const U32 aSize );
U32		Heap_DeInit( sHeap * apHeap );

U32		Heap_Reset( sHeap * apHeap );

void *	Heap_Alloc( sHeap * apHeap, const U32 aSize );
void	Heap_Free( sHeap * apHeap, void * apMem );


/* ################################################################################ */

#endif	/* INCLUDED_HEAP_H */
