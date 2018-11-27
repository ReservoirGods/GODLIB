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
struct	sHashTreeNode;

typedef	void	(*fHashTreeVarCB)(  struct sHashTreeVarClient  * apClient );
typedef	void	(*fHashTreeNodeCB)( struct sHashTreeNode * apNode );


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sHashTreeVarClient
{
	fHashTreeVarCB				mfOnWrite;
	fHashTreeVarCB				mfOnInit;
	fHashTreeVarCB				mfOnDeInit;
	U32							mUserData;
	struct	sHashTreeVar *		mpVar;
	struct sHashTreeVarClient *	mpNext;
} sHashTreeVarClient;

typedef	struct	sHashTreeVar
{
	U32								mGlobalID;
	U32								mLocalID;
	S32								mRefCount;
	U32								mDataSize;
	void *							mpData;
	U32								mDataSmall;
	struct	sHashTreeNode *			mpNode;
	struct	sHashTreeVarClient *	mpClients;
	struct	sHashTreeVar *			mpNext;
} sHashTreeVar;

typedef	struct	sHashTreeNode
{
	U32						mGlobalID;
	U32						mLocalID;
	S32						mRefCount;
	sHashTreeVar *			mpVars;
	struct	sHashTreeNode *	mpParent;
	struct	sHashTreeNode *	mpChild;
	struct	sHashTreeNode *	mpNext;
} sHashTreeNode;

typedef	struct sHashTree
{
	sHashTreeNode *	mpNodes;
	U32				mNodeCount;
	U32				mVariableCount;
} sHashTree;


typedef	struct	sHashTreeSaveVar
{
	U32		mGlobalID;
	U32		mLocalID;
	U32		mDataSize;
	void *	mpData;
} sHashTreeSaveVar;

typedef	struct	sHashTreeSaveNode
{
	U32							mGlobalID;
	U32							mLocalID;
	U16							mVarCount;
	U16							mPad;
	sHashTreeSaveVar *			mpVars;
	struct sHashTreeSaveNode *	mpChild;
	struct sHashTreeSaveNode *	mpNext;
} sHashTreeSaveNode;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32						HashTree_BuildHash( const char * apName );

void					HashTree_Init( sHashTree * apTree );
void					HashTree_DeInit( sHashTree * apTree );

sHashTreeVar *			HashTree_VarInit( sHashTree * apTree, const char * apName, const U32 aSize, void * apData );
void					HashTree_VarDeInit( sHashTree * apTree, sHashTreeVar * apVar );

sHashTreeVar *			HashTree_VarRegister(   sHashTree * apTree, const char * apName );
void					HashTree_VarUnRegister( sHashTree * apTree, sHashTreeVar * apVar );

sHashTreeVarClient *	HashTree_VarClientRegister( sHashTree * apTree, const char * apName, fHashTreeVarCB aOnWrite, fHashTreeVarCB aOnInit, fHashTreeVarCB onDeInit, const U32 aUserData );
void					HashTree_VarClientUnRegister( sHashTree * apTree, sHashTreeVarClient * apClient );

void					HashTree_VarClient_Init( sHashTreeVarClient * apClient, sHashTree * apTree, const char * apName, fHashTreeVarCB aOnWrite );
void					HashTree_VarClient_DeInit( sHashTreeVarClient * apClient, sHashTree * apTree );

void					HashTree_VarWrite( sHashTreeVar * apVar, void * apData );
void					HashTree_VarRead( const sHashTreeVar * apVar, void * apDest, const U32 aSize );

sHashTreeSaveNode *		HashTree_SaveNodeBuild( sHashTree * apTree, const char * apNodeName );
void					HashTree_SaveNodeDestroy( sHashTree * apTree, const char * apNodeName, sHashTreeSaveNode * apSaveNode );
void					HashTree_SaveNodeLoad( sHashTree * apTree, const char * apNodeName, const sHashTreeSaveNode * apSaveNode );
void					HashTree_SaveNodeUnLoad( sHashTree * apTree, const char * apNodeName, const sHashTreeSaveNode * apSaveNode );
U32						HashTree_SaveNodeGetSize( const sHashTreeSaveNode * apSaveNode );
void					HashTree_SaveNodeDelocate( sHashTreeSaveNode * apNode );
void					HashTree_SaveNodeRelocate( sHashTreeSaveNode * apNode );


/* ################################################################################ */

#endif	/* INCLUDED_HASHTREE_H */
