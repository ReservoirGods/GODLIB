#ifndef	INCLUDED_HASHTREE_H
#define	INCLUDED_HASHTREE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

struct	sHashTreeVarClient;

typedef	void	(*fHashTreeVarCB)(  struct sHashTreeVarClient  * apClient );


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sHashTreeVarClient
{
	fHashTreeVarCB				mfOnWrite;
	fHashTreeVarCB				mfOnInit;
	fHashTreeVarCB				mfOnDeInit;
	U32							mUserData;
	U32							mHashKey;
	struct	sHashTreeVar *		mpVar;
	struct sHashTreeVarClient *	mpNext;
} sHashTreeVarClient;


typedef	struct	sHashTreeVar
{
	U32								mHashKey;
	U32								mFilterFlags;
	U32								mDataSize;
	void *							mpData;
	U32								mDataSmall;
	struct	sHashTreeVarClient *	mpClients;
	struct	sHashTreeVar *			mpVarNext;
} sHashTreeVar;


typedef	struct sHashTree
{
	U32						mVariableCount;
	sHashTreeVar *			mpVars;
	sHashTreeVarClient *	mpUnboundClients;
} sHashTree;


typedef struct sHashTreeVarBlock
{
	U32		mID;
	U32 	mVersion;
	U32		mVarCount;
	U32		mTotalDataSize;
	U32 *	mpHashes;
	U16 *	mpDataSizes;
	U8 *	mpData;
} sHashTreeVarBlock;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32						HashTree_BuildHash( const char * apName );

void					HashTree_Init( sHashTree * apTree );
void					HashTree_DeInit( sHashTree * apTree );

void					HashTree_Var_Init( sHashTreeVar * apVar, sHashTree * apTree, const char * apName, const U32 aSize, void * apData );
void					HashTree_Var_DeInit( sHashTreeVar * apVar, sHashTree * apTree );

#if 0
sHashTreeVar *			HashTree_Var_Create( sHashTree * apTree, const char * apName, const U32 aSize, void * apData );
void					HashTree_Var_Destroy( sHashTree * apTree, sHashTreeVar * apVar );
#endif

void					HashTree_VarClient_Init( sHashTreeVarClient * apClient, sHashTree * apTree, const char * apName, fHashTreeVarCB aOnWrite );
void					HashTree_VarClient_DeInit( sHashTreeVarClient * apClient, sHashTree * apTree );

void					HashTree_VarWrite( sHashTreeVar * apVar, void * apData );
void					HashTree_VarRead( const sHashTreeVar * apVar, void * apDest, const U32 aSize );

U32						HashTree_VarBlock_GetSize( sHashTree * apTree, U32 aFilterFlags );
void					HashTree_VarBlock_Init( sHashTreeVarBlock * apBlock, sHashTree * apTree, U32 aFilterFlags );
void					HashTree_VarBlock_DeInit( sHashTreeVarBlock * apBlock );

void					HashTree_VarBlock_Delocate( sHashTreeVarBlock * apBlock );
void					HashTree_VarBlock_Relocate( sHashTreeVarBlock * apBlock );

void					HashTree_VarBlock_Apply( sHashTreeVarBlock * apBlock, sHashTree * apTree );


/* ################################################################################ */

#endif	/* INCLUDED_HASHTREE_H */
