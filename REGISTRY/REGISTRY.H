#ifndef	INCLUDED_REGISTRY_H
#define	INCLUDED_REGISTRY_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

struct	sRegistryVarClient;
struct	sRegistryNode;

typedef	void( *fRegistryVarCB )( struct sRegistryVarClient  * apClient );
typedef	void( *fRegistryNodeCB )( struct sRegistryNode * apNode );


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sRegistryVarClient
{
	fRegistryVarCB				mfOnWrite;
	fRegistryVarCB				mfOnInit;
	fRegistryVarCB				mfOnDeInit;
	U32							mUserData;
	struct	sRegistryVar *		mpVar;
	struct sRegistryVarClient *	mpNext;
} sRegistryVarClient;

typedef	struct	sRegistryVar
{
	U32								mGlobalID;
	U32								mLocalID;
	S32								mRefCount;
	U32								mDataSize;
	void *							mpData;
	U32								mDataSmall;
	struct	sRegistryNode *			mpNode;
	struct	sRegistryVarClient *	mpClients;
	struct	sRegistryVar *			mpNext;
} sRegistryVar;

typedef	struct	sRegistryNode
{
	U32						mGlobalID;
	U32						mLocalID;
	S32						mRefCount;
	sRegistryVar *			mpVars;
	struct	sRegistryNode *	mpParent;
	struct	sRegistryNode *	mpChild;
	struct	sRegistryNode *	mpNext;
} sRegistryNode;

typedef	struct sRegistry
{
	sRegistryNode *	mpNodes;
	U32				mNodeCount;
	U32				mVariableCount;
} sRegistry;


typedef	struct	sRegistrySaveVar
{
	U32		mGlobalID;
	U32		mLocalID;
	U32		mDataSize;
	void *	mpData;
} sRegistrySaveVar;

typedef	struct	sRegistrySaveNode
{
	U32							mGlobalID;
	U32							mLocalID;
	U16							mVarCount;
	U16							mPad;
	sRegistrySaveVar *			mpVars;
	struct sRegistrySaveNode *	mpChild;
	struct sRegistrySaveNode *	mpNext;
} sRegistrySaveNode;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32						Registry_BuildHash( const char * apName );

void					Registry_Init( sRegistry * apTree );
void					Registry_DeInit( sRegistry * apTree );

sRegistryVar *			Registry_VarInit( sRegistry * apTree, const char * apName, const U32 aSize, void * apData );
void					Registry_VarDeInit( sRegistry * apTree, sRegistryVar * apVar );

sRegistryVar *			Registry_VarRegister( sRegistry * apTree, const char * apName );
void					Registry_VarUnRegister( sRegistry * apTree, sRegistryVar * apVar );

sRegistryVarClient *	Registry_VarClientRegister( sRegistry * apTree, const char * apName, fRegistryVarCB aOnWrite, fRegistryVarCB aOnInit, fRegistryVarCB onDeInit, const U32 aUserData );
void					Registry_VarClientUnRegister( sRegistry * apTree, sRegistryVarClient * apClient );

void					Registry_VarWrite( sRegistryVar * apVar, void * apData );
void					Registry_VarRead( const sRegistryVar * apVar, void * apDest, const U32 aSize );

sRegistrySaveNode *		Registry_SaveNodeBuild( sRegistry * apTree, const char * apNodeName );
void					Registry_SaveNodeDestroy( sRegistry * apTree, const char * apNodeName, sRegistrySaveNode * apSaveNode );
void					Registry_SaveNodeLoad( sRegistry * apTree, const char * apNodeName, const sRegistrySaveNode * apSaveNode );
void					Registry_SaveNodeUnLoad( sRegistry * apTree, const char * apNodeName, const sRegistrySaveNode * apSaveNode );
U32						Registry_SaveNodeGetSize( const sRegistrySaveNode * apSaveNode );
void					Registry_SaveNodeDelocate( sRegistrySaveNode * apNode );
void					Registry_SaveNodeRelocate( sRegistrySaveNode * apNode );


/* ################################################################################ */

#endif	/* INCLUDED_REGISTRY_H */
