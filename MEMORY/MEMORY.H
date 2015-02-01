#ifndef	INCLUDED_MEMORY_H
#define	INCLUDED_MEMORY_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#ifndef dDEBUG

#define	mMEMALLOC( aSize )			Memory_Alloc( aSize );
#define	mMEMCALLOC( aSize )			Memory_Calloc( aSize );
#define	mMEMSCREENCALLOC( aSize )	Memory_ScreenCalloc( aSize );
#define	mMEMFREE( apMem )			_Memory_Release( apMem );
#define	mMEMSCREENFREE( apMem )		Memory_ScreenRelease( apMem );

#else

#define	mMEMALLOC( aSize )			Memory_DbgAlloc( aSize, __FILE__, __LINE__ );
#define	mMEMCALLOC( aSize )			Memory_DbgCalloc( aSize, __FILE__, __LINE__ )
#define	mMEMSCREENCALLOC( aSize )	Memory_DbgScreenCalloc( aSize, __FILE__, __LINE__ );
#define	mMEMFREE( apMem )			Memory_DbgFree( apMem, __FILE__, __LINE__ );
#define	mMEMSCREENFREE( apMem )		Memory_DbgScreenFree( apMem, __FILE__, __LINE__ );

#endif


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void * 	Memory_Alloc(  const U32 aSize );
void *	Memory_Calloc( const U32 aSize );

void *	Memory_ScreenAlloc(  const U32 aSize );
void *	Memory_ScreenCalloc( const U32 aSize );
S32		Memory_ScreenRelease( void * apMem );

S32		_Memory_Release( void * apMem );

void	Memory_Clear( U32 aSize, void * apMem );
void	Memory_Copy(  U32 aSize, const void * apSrc, void * apDst );

U32		Memory_GetAllocCount( void );
U32		Memory_GetAllocatedSize( void );
U32		Memory_GetHighTide( void );
U32		Memory_GetLargestAlloc( void );
U32		Memory_GetSmallestAlloc( void );
U32		Memory_GetFailedSize( void );

U32		Memory_GetSize( void );
U8		Memory_Validate( void );

void *	Memory_DbgScreenCalloc( const U32 aSize, const char * apFile, const U32 aLine );
void *	Memory_DbgAlloc( const U32 aSize, const char * apFile, const U32 aLine );
void *	Memory_DbgCalloc( const U32 aSize, const char * apFile, const U32 aLine );
S32		Memory_DbgFree( void * apMem, const char * apFile, const U32 aLine );
S32		Memory_DbgScreenFree( void * apMem, const char * apFile, const U32 aLine );

void	Memory_ShowCurrentRecords( void );


/* ################################################################################ */

#endif	/*	INCLUDED_MEMORY_H	*/
