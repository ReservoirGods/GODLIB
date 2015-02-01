#ifndef	INCLUDED_THREAD_H
#define	INCLUDED_THREAD_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB\BASE\BASE.H>
#include	<GODLIB\ASSERT\ASSERT.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sThread
{
	U16	mPC;
	U16	mSleepTicks;
	U16	mState;
	U16	mFlags;
} sThread;

typedef	struct sThreadSemaphore
{
	U32	mCount;
} sThreadSemaphore;

typedef	U8	(*fThread)( sThread * apThread );

typedef struct sThreadObject
{
	sThread	mThread;
	fThread	mfFunc;
} sThreadObject;


/* ###################################################################################
#  ENUMS
################################################################################### */

typedef enum eTHREAD_STATE
{
	eTHREAD_STATE_NONE,
	eTHREAD_STATE_RUNNING,
	eTHREAD_STATE_WAITING,
	eTHREAD_STATE_YIELDED,
	eTHREAD_STATE_SUSPENDED,
	eTHREAD_STATE_SLEEPING,
	eTHREAD_STATE_ENDED
}eTHREAD_STATE;

enum
{
	eTHREAD_FLAG_INITED  = (1<<0),
	eTHREAD_FLAG_RUNNING = (1<<1),
};

/* ###################################################################################
#  MACROS
################################################################################### */

/*	private internal functions */

#define	_miTHREAD_CONTEXT_SAVE(_a)		(_a)->mPC = __LINE__; case __LINE__:
#define	_miTHREAD_CONTEXT_RESTORE(_a)	switch((_a)->mPC) { case 0:
#define _miTHREAD_RETURN(_a,_aState )	(_a)->mState = _aState; (_a)->mFlags &= ~eTHREAD_FLAG_RUNNING; return(_aState)
#define	_miTHREAD_ASSERT( _Cond )		Assert( _Cond )
#define	_miTHREAD_VERIFY_INIT(_a)		_miTHREAD_ASSERT(  ( (_a)->mFlags & (eTHREAD_FLAG_INITED|eTHREAD_FLAG_RUNNING) ) == eTHREAD_FLAG_INITED  )
#define	_miTHREAD_VERIFY_IN(_a)			_miTHREAD_ASSERT( _lpThread == _a )

/* public interface */

#define	mTHREAD_INIT( _a )		(_a)->mPC = 0; (_a)->mSleepTicks=0; (_a)->mState=eTHREAD_STATE_NONE; (_a)->mFlags |= eTHREAD_FLAG_INITED; 
#define	mTHREAD_DEINIT( _a )	(_a)->mPC = 0; (_a)->mSleepTicks=0; (_a)->mState=eTHREAD_STATE_NONE; (_a)->mFlags &= ~eTHREAD_FLAG_INITED; 
#define	mTHREAD_RESTART( _a )	_miTHREAD_VERIFY_IN(_a); mTHREAD_INIT(_a); _miTHREAD_RETURN(_a,eTHREAD_STATE_YIELDED)

#define	mTHREAD_BEGIN( _a )	{  U8 _lThreadYieldFlag=0; sThread * _lpThread = (_a); _miTHREAD_VERIFY_INIT(_a); _miTHREAD_CONTEXT_RESTORE(_a); 
#define	mTHREAD_END( _a )	 _miTHREAD_VERIFY_IN(_a); (void)_lThreadYieldFlag; (void)_lpThread; } ((_a)->mPC = 0); _miTHREAD_RETURN(_a,eTHREAD_STATE_ENDED); } 

#define	mTHREAD_WAIT_UNTIL( _a, _aCond ) _miTHREAD_VERIFY_IN(_a); _miTHREAD_CONTEXT_SAVE( _a ); if( !(_aCond)) {_miTHREAD_RETURN(_a,eTHREAD_STATE_WAITING);}
#define	mTHREAD_WAIT_WHILE( _a, _aCond ) mTHREAD_WAIT_UNTIL( _a, !(_aCond) )

#define	mTHREAD_YIELD( _a )				  _miTHREAD_VERIFY_IN(_a); _lThreadYieldFlag=1; _miTHREAD_CONTEXT_SAVE(_a); if(_lThreadYieldFlag){ _miTHREAD_RETURN(_a,eTHREAD_STATE_YIELDED); }
#define	mTHREAD_YIELD_WHILE( _a, _aCond ) _miTHREAD_VERIFY_IN(_a); _lThreadYieldFlag=1; _miTHREAD_CONTEXT_SAVE(_a); if((_lThreadYieldFlag)||(_aCond)){ _miTHREAD_RETURN(_a,eTHREAD_STATE_YIELDED); }
#define	mTHREAD_YIELD_UNTIL( _a, _aCond ) mTHREAD_YIELD_WHILE( _a, !(_aCond) )

#define	mTHREAD_SUSPEND( _a )	_miTHREAD_VERIFY_IN(_a); (_a)->mState = eTHREAD_STATE_SUSPENDED; _miTHREAD_CONTEXT_SAVE(_a); if((_a)->mState==eTHREAD_STATE_SUSPENDED){ _miTHREAD_RETURN(_a,eTHREAD_STATE_SUSPENDED); }
#define	mTHREAD_RESUME( _a )	(_a)->mState = eTHREAD_STATE_YIELDED

#define	mTHREAD_SLEEP( _a, _aTicks )		_miTHREAD_VERIFY_IN(_a); (_a)->mSleepTicks=_aTicks; _miTHREAD_CONTEXT_SAVE(_a); if( (_a)->mSleepTicks ){ (_a)->mSleepTicks--; _miTHREAD_RETURN(_a,eTHREAD_STATE_SLEEPING); }
#define	mTHREAD_WAKE( _a )					(_a)->mSleepTicks=0

#define mTHREAD_SPAWN( _a, _aChild, _aFunc )	_miTHREAD_VERIFY_IN(_a); mTHREAD_INIT( _aChild ); mTHREAD_WAIT_WHILE( _a, _aFunc(_aChild) < eTHREAD_STATE_ENDED )

#define	mTHREAD_HASFINISHED( _a )			(_a)->mState >= aTHREAD_STATE_EXITED

#define	mTHREAD_SEMAPHORE_INIT( _a, _aSema, _aValue )	(_aSema)->mCount = aValue;
#define	mTHREAD_SEMAPHORE_WAIT( _a, _aSema )			mTHREAD_WAIT_UNTIL( _a, (_aSema)->mCount > 0 ); (_aSema)->mCount--
#define	mTHREAD_SEMAPHORE_SIGNAL( _a, _aSema )			(_aSema)->mCount++;


/* ################################################################################ */

#endif	/*	INCLUDED_SCREEN_H	*/
