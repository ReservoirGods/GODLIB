/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"HASHTREE.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define dHASHTREE_BLOCK_ID 		mSTRING_TO_U32( 'H', 'T', 'B', 'K' )
#define dHASHTREE_BLOCK_VERSION 0

#define	dHASHTREE_STR_LIMIT	256
#define	dHASHTREE_TOK_LIMIT	32


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	U16		mTokenCount;
	U32		mGlobalHash[ dHASHTREE_TOK_LIMIT ];
	U32		mLocalHash[ dHASHTREE_TOK_LIMIT ];
} sHashTreeTokeniser;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sHashTreeNode *	HashTree_NodeRegister( sHashTree * apTree, const char * apName );
void			HashTree_NodeUnRegister( sHashTree * apTree, sHashTreeNode * apNode );

void			HashTree_Tokenise( sHashTreeTokeniser * apToken, const char * apString );

void			HashTree_SubNodesDestroy( sHashTree * apTree, sHashTreeNode * apNode );

sHashTreeNode *	HashTree_NodeTokReg( sHashTree * apTree, sHashTreeTokeniser * apToken );

sHashTreeNode *	HashTree_NodeCreate( sHashTree * apTree, const U32 aGlobalID, const U32 aLocalID );
void			HashTree_NodeDestroy( sHashTree * apTree, sHashTreeNode * apNode );

sHashTreeVar *	HashTree_NodeVarReg( sHashTreeNode * apNode, const U32 aGlobalID, const U32 aLocalID );


#ifdef dGODLIB_PLATFORM_ATARI
#define			HashTree_Validate( _a )
#else
void			HashTree_Validate( sHashTree * apTree );
#endif
#if 1
sHashTreeVar *			HashTree_VarRegister(   sHashTree * apTree, const char * apName );
void					HashTree_VarUnRegister( sHashTree * apTree, sHashTreeVar * apVar );
#endif

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_BuildHash( const char * apName )
* ACTION   : HashTree_BuildHash
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	HashTree_BuildHash( const char * apName )
{
	U32		lHash;
	U32		lTemp;
	char	lC;

	lHash = 0;
	while ( *apName )
	{
		lC    = *apName++;
		if( (lC >= 'a') && (lC <='z') )
		{
			lC += ( 'A'-'a' );
		}
		if( '\\' == lC )
		{
			lC = '/';
		}
		lHash = ( lHash << 4L ) + lC;
		lTemp = lHash & 0xF0000000L;
		if( lTemp )
		{
			lHash ^= lTemp >> 24L;
		}
		lHash &= ~lTemp;
	}

	return( lHash );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_Init( sHashTree * apTree )
* ACTION   : HashTree_Init
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_Init( sHashTree * apTree )
{
	apTree->mNodeCount     = 0;
	apTree->mVariableCount = 0;
	apTree->mpNodes        = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_DeInit( sHashTree * apTree )
* ACTION   : HashTree_DeInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_DeInit( sHashTree * apTree )
{
	if( apTree->mpNodes )
	{
		HashTree_NodeUnRegister( apTree, apTree->mpNodes );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeCreate( sHashTree * apTree, const U32 aGlobalID,const U32 aLocalID )
* ACTION   : HashTree_NodeCreate
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeNode *	HashTree_NodeCreate( sHashTree * apTree, const U32 aGlobalID,const U32 aLocalID )
{
	sHashTreeNode *	lpNode;

	lpNode = (sHashTreeNode*)mMEMCALLOC( sizeof(sHashTreeNode) );

	if( lpNode )
	{
		lpNode->mGlobalID = aGlobalID;
		lpNode->mLocalID  = aLocalID;
		if( apTree )
		{
			apTree->mNodeCount++;
		}
	}

	HashTree_Validate( apTree );


	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeDestroy( sHashTree * apTree, sHashTreeNode * apNode )
* ACTION   : HashTree_NodeDestroy
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_NodeDestroy( sHashTree * apTree, sHashTreeNode * apNode )
{
	if( apTree )
	{
		if( apNode->mpParent )
		{
			GOD_LL_REMOVE( sHashTreeNode, apNode->mpParent->mpChild, mpNext, apNode );
		}
		else
		{
			GOD_LL_REMOVE( sHashTreeNode, apTree->mpNodes, mpNext, apNode );
		}
/*
		if( apTree->mpNodes == apNode )
		{
			apTree->mpNodes = 0;
		}
*/
		apTree->mNodeCount--;
	}
	mMEMFREE( apNode );
/*
	sHashTreeVar *	lpVar;

	if( apNode )
	{
		lpVar = apNode->mpVars;
		while( lpVar )
		{
			lpVar->mpNode = 0;
			lpVar         = lpVar->mpNext;
		}
		mMEMFREE( apNode );
	}
*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeRegister( sHashTree * apTree,const char * apName )
* ACTION   : HashTree_NodeRegister
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeNode *	HashTree_NodeRegister( sHashTree * apTree,const char * apName )
{
	sHashTreeTokeniser	lTokeniser;
	sHashTreeNode *		lpNode;

	HashTree_Validate( apTree );

	HashTree_Tokenise( &lTokeniser, apName );

	lpNode = HashTree_NodeTokReg( apTree, &lTokeniser );

	HashTree_Validate( apTree );

	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeTokReg( sHashTree * apTree,sHashTreeTokeniser * apToken )
* ACTION   : HashTree_NodeTokReg
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeNode *	HashTree_NodeTokReg( sHashTree * apTree,sHashTreeTokeniser * apToken )
{
	sHashTreeNode *	lpNode;
	sHashTreeNode *	lpParent;
	U16				i;

	lpNode = 0;

	if( apToken->mTokenCount )
	{
		if( !apTree->mpNodes )
		{
			apTree->mpNodes = HashTree_NodeCreate( apTree, apToken->mGlobalHash[ 0 ], apToken->mLocalHash[ 0 ] );
		}

		i = 0;
		lpNode = apTree->mpNodes;

		while( i < apToken->mTokenCount )
		{
			lpParent = lpNode;
			while( (lpNode) && (i < apToken->mTokenCount) )
			{
				if( lpNode->mLocalID == apToken->mLocalHash[ i ] )
				{
					i++;
					if( i < apToken->mTokenCount )
					{
						if( lpNode->mpChild )
						{
							lpNode = lpNode->mpChild;
						}
						else
						{
							lpParent          = lpNode;
							lpNode            = HashTree_NodeCreate( apTree, apToken->mGlobalHash[ i ], apToken->mLocalHash[ i ] );
							lpNode->mpParent  = lpParent;
							lpParent->mpChild = lpNode;
							lpParent->mRefCount++;
						}
						lpParent = lpNode;
					}
				}
				else
				{
					lpNode = lpNode->mpNext;
				}
			}
			if( !lpNode )
			{
				lpNode = HashTree_NodeCreate( apTree, apToken->mGlobalHash[ i ], apToken->mLocalHash[ i ] );
				lpNode->mpNext   = lpParent->mpNext;
				lpParent->mpNext = lpNode;
				lpNode->mpParent = lpParent->mpParent;
			}
		}
	}

	if( lpNode )
	{
		lpNode->mRefCount++;
	}

	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeUnRegister( sHashTree * apTree, sHashTreeNode * apNode )
* ACTION   : HashTree_NodeUnRegister
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_NodeUnRegister( sHashTree * apTree, sHashTreeNode * apNode )
{
	sHashTreeNode *			lpNode;


	HashTree_Validate( apTree );

	if( apNode )
	{
		apNode->mRefCount--;
		if( apNode->mRefCount <= 0 )
		{
			HashTree_SubNodesDestroy( apTree, apNode->mpChild );

			if( apNode->mpParent )
			{
				lpNode = apNode->mpParent->mpChild;
				if( lpNode )
				{
					if( lpNode == apNode )
					{
						apNode->mpParent->mpChild = apNode->mpNext;
					}
					else
					{
						while( (lpNode->mpNext) && (lpNode->mpNext != apNode) )
						{
							lpNode = lpNode->mpNext;
						}
						if( lpNode->mpNext == apNode )
						{
							lpNode->mpNext = apNode->mpNext;
						}
					}
				}
				HashTree_NodeUnRegister( apTree, apNode->mpParent );
			}
			HashTree_NodeDestroy( apTree, apNode );
		}
	}

	HashTree_Validate( apTree );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_SubNodesDestroy( sHashTree * apTree, sHashTreeNode * apNode )
* ACTION   : HashTree_SubNodesDestroy
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_SubNodesDestroy( sHashTree * apTree, sHashTreeNode * apNode )
{
	sHashTreeNode *			lpNode;
	sHashTreeNode *			lpNodeNext;

	HashTree_Validate( apTree );

	lpNode = apNode;
	while( lpNode )
	{
		HashTree_SubNodesDestroy( apTree, lpNode->mpChild );

		lpNodeNext = lpNode->mpNext;
		HashTree_NodeDestroy( apTree, lpNode );
		lpNode     = lpNodeNext;
	}

	HashTree_Validate( apTree );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarInit( sHashTree * apTree,const char * apName,const U32 aSize,void * apData )
* ACTION   : HashTree_VarInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeVar *	HashTree_Var_Create( sHashTree * apTree,const char * apName,const U32 aSize,void * apData )
{
	sHashTreeVar *			lpVar;
	sHashTreeVarClient *	lpClient;


	lpVar = HashTree_VarRegister( apTree, apName );

	if( lpVar )
	{
		lpVar->mDataSize = aSize;
		if( aSize <= 4 )
		{
			lpVar->mpData = &lpVar->mDataSmall;
			if( lpVar->mpData )
			{
				Memory_Copy( aSize, apData, lpVar->mpData );
			}
		}
		else
		{
/*			DebugLog_Printf2( "HashTree_VarInit() : %s : %s", apName, apData );*/
/*			lpVar->mpData    = mMEMCALLOC( aSize );*/
/* optimised hashtree - doesn't memory allocate, assumes clients provides memory */

			lpVar->mpData = apData;
			GODLIB_ASSERT( apData );
			
		}
	#if 0
		if( lpVar->mpData )
		{
			Memory_Copy( aSize, apData, lpVar->mpData );
		}
	#endif
		lpClient = lpVar->mpClients;
		while( lpClient )
		{
			if( lpClient->mfOnInit )
			{
				lpClient->mfOnInit( lpClient );
			}
			lpClient = lpClient->mpNext;
		}
	}

	return( lpVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarDeInit( sHashTree * apTree, sHashTreeVar * apVar )
* ACTION   : HashTree_VarDeInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_Var_Destroy( sHashTree * apTree, sHashTreeVar * apVar )
{
	sHashTreeVarClient *	lpClient;


	if( apVar )
	{
		lpClient = apVar->mpClients;
		while( lpClient )
		{
			if( lpClient->mfOnDeInit )
			{
				lpClient->mfOnDeInit( lpClient );
			}
			lpClient = lpClient->mpNext;
		}
		if( apVar->mpData )
		{
/*			
			if( apVar->mDataSize > 4 )
			{
				mMEMFREE( apVar->mpData );
			}
*/			
			apVar->mpData    = 0;
			apVar->mDataSize = 0;
		}
	}

	HashTree_VarUnRegister( apTree, apVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_NodeVarReg( sHashTreeNode * apNode,const U32 aGlobalID,const U32 aLocalID )
* ACTION   : HashTree_NodeVarReg
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeVar *	HashTree_NodeVarReg( sHashTreeNode * apNode,const U32 aGlobalID,const U32 aLocalID )
{
	sHashTreeVar *		lpVar;

	lpVar = apNode->mpVars;

	while( (lpVar) && (lpVar->mLocalID != aLocalID) )
	{
		lpVar = lpVar->mpNext;
	}

	if( !lpVar )
	{
		lpVar = (sHashTreeVar*)mMEMCALLOC( sizeof(sHashTreeVar) );
		if( lpVar )
		{
			lpVar->mGlobalID = aGlobalID;
			lpVar->mLocalID  = aLocalID;
			lpVar->mpNode    = apNode;
			lpVar->mpNext    = apNode->mpVars;
			apNode->mpVars   = lpVar;
		}
	}

	if( apNode )
	{
		apNode->mRefCount++;
	}

	if( lpVar )
	{
		lpVar->mRefCount++;
	}

	return( lpVar );
}

sHashTreeVar * HashTree_Var_Find( sHashTree * apTree, U32 aKey )
{
	sHashTreeVar * var = apTree->mpVars;

	for( var=apTree->mpVars;var;var=var->mpVarNext)
	{
		if( var->mHashKey == aKey )
			return var;
	}

	return 0;
}

sHashTreeVarClient * HashTree_VarClient_Find( sHashTree * apTree, U32 aKey )
{
	sHashTreeVarClient * client;

	for( client=apTree->mpUnboundClients; client; client=client->mpNext )
	{
		if( aKey == client->mHashKey )
			return client;
	}

	return 0;
}


void	HashTree_Var_Init( sHashTreeVar * apVar, sHashTree * apTree, const char * apName, const U32 aSize, void * apData )
{
	sHashTreeVarClient * client;

	apVar->mHashKey = HashTree_BuildHash( apName );
	apVar->mDataSize = aSize;
	apVar->mpData = apData;
	if( apVar->mDataSize <= 4 )
	{
		apVar->mpData = &apVar->mDataSmall;
	}

	client = HashTree_VarClient_Find( apTree, apVar->mHashKey );
	if( client )
	{
		GOD_LL_REMOVE( sHashTreeVarClient, apTree->mpUnboundClients, mpNext, client );
		apVar->mpClients = client;
		for( ;client;client=client->mpNext)
		{
			client->mfOnInit( client );
		}
	}

	GOD_LL_INSERT( apTree->mpVars, mpVarNext, apVar );

}

void	HashTree_Var_DeInit( sHashTreeVar * apVar, sHashTree * apTree )
{
	sHashTreeVarClient * client;
	sHashTreeVarClient * next;
	for( client = apVar->mpClients; client; client=next)
	{
		next = client->mpNext;
		client->mpNext = apTree->mpUnboundClients;
		apTree->mpUnboundClients = client;
	}

	GOD_LL_REMOVE( sHashTreeVar, apTree->mpVars, mpVarNext, apVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarRegister( sHashTree * apTree,const char * apName )
* ACTION   : HashTree_VarRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeVar *	HashTree_VarRegister( sHashTree * apTree,const char * apName )
{
	sHashTreeTokeniser	lTokeniser;
	sHashTreeVar *		lpVar;
	sHashTreeNode *		lpNode;
	U32					lLocalHash;


	HashTree_Tokenise( &lTokeniser, apName );

	lpVar = 0;

	if( lTokeniser.mTokenCount > 1 )
	{
		lTokeniser.mTokenCount--;
		lpNode = HashTree_NodeTokReg( apTree, &lTokeniser );
		if( lpNode )
		{
			lpVar = lpNode->mpVars;

			lLocalHash = lTokeniser.mLocalHash[ lTokeniser.mTokenCount ];


			while( (lpVar) && (lpVar->mLocalID != lLocalHash) )
			{
				lpVar = lpVar->mpNext;
			}
			if( !lpVar )
			{
				lpVar = (sHashTreeVar*)mMEMCALLOC( sizeof(sHashTreeVar) );
				if( lpVar )
				{
					lpVar->mGlobalID = lTokeniser.mGlobalHash[ lTokeniser.mTokenCount ];
					lpVar->mLocalID  = lLocalHash;
					lpVar->mpNode    = lpNode;
					lpVar->mpNext    = lpNode->mpVars;
					lpNode->mpVars   = lpVar;

					GOD_LL_INSERT( apTree->mpVars, mpVarNext, lpVar );
				}
			}
			if( lpVar )
			{
				lpVar->mRefCount++;
			}
		}

	}
/*	DebugLog_Printf3( "HashTree_VarRegister() tree:%lX %s %lx", apTree, apName, lpVar );*/

	return( lpVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarUnRegister( sHashTree * apTree, sHashTreeVar * apVar )
* ACTION   : HashTree_VarUnRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarUnRegister( sHashTree * apTree, sHashTreeVar * apVar )
{
	if( apVar )
	{
		sHashTreeNode *	lpNode;

		lpNode = apVar->mpNode;
		apVar->mRefCount--;

		if( apVar->mRefCount <= 0 )
		{
			if( apVar->mpNode->mpVars == apVar )
			{
				apVar->mpNode->mpVars = apVar->mpNext;
			}
			else
			{
				sHashTreeVar *	lpVar;

				lpVar = apVar->mpNode->mpVars;
				while( lpVar->mpNext && (lpVar->mpNext!=apVar) )
				{
					lpVar = lpVar->mpNext;
				}
				lpVar->mpNext = apVar->mpNext;
			}

			GOD_LL_REMOVE( sHashTreeVar, apTree->mpVars, mpVarNext, apVar );

			mMEMFREE( apVar );
		}

		if( lpNode )
		{
			HashTree_NodeUnRegister( apTree, lpNode );
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarClientRegister( sHashTree * apTree,const * apName,fHashTreeVarCB aOnWrite,fHashTreeVarCB aOnInit,fHashTreeVarCB onDeInit )
* ACTION   : HashTree_VarClientRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sHashTreeVarClient *	HashTree_VarClientRegister( sHashTree * apTree,const char * apName,fHashTreeVarCB aOnWrite,fHashTreeVarCB aOnInit,fHashTreeVarCB aOnDeInit, const U32 aUserData )
{
/*	
	sHashTreeVarClient *	lpClient;
	sHashTreeVar *			lpVar;


	lpClient = 0;
	lpVar    = HashTree_VarRegister( apTree, apName );

	if( lpVar )
	{
		lpClient = (sHashTreeVarClient*)mMEMCALLOC( sizeof(sHashTreeVarClient) );
		if( lpClient )
		{			
			lpClient->mfOnDeInit = aOnDeInit;
			lpClient->mfOnInit   = aOnInit;
			lpClient->mUserData  = aUserData;


			lpClient->mfOnWrite  = aOnWrite;
			lpClient->mpVar      = lpVar;
			lpClient->mpNext     = lpVar->mpClients;
			lpVar->mpClients     = lpClient;
		}
	}
	return( lpClient );
*/

	sHashTreeVarClient *	lpClient;
	lpClient = (sHashTreeVarClient*)mMEMCALLOC( sizeof(sHashTreeVarClient) );
	if( lpClient )
	{			
		lpClient->mfOnDeInit = aOnDeInit;
		lpClient->mfOnInit   = aOnInit;
		lpClient->mUserData  = aUserData;
		HashTree_VarClient_Init( lpClient, apTree, apName, aOnWrite );
	}
	return lpClient;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarClientUnRegister( sHashTree * apTree, sHashTreeVarClient * apClient )
* ACTION   : HashTree_VarClientUnRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarClientUnRegister( sHashTree * apTree, sHashTreeVarClient * apClient )
{
	sHashTreeVarClient * lpClient;

	if( apClient )
	{
		if( apClient->mpVar )
		{
			lpClient = apClient->mpVar->mpClients;

			if( lpClient == apClient )
			{
				apClient->mpVar->mpClients = apClient->mpNext;
			}
			else
			{
				while( (lpClient->mpNext) && (lpClient->mpNext != apClient) )
				{
					lpClient = lpClient->mpNext;
				}
				if( lpClient->mpNext == apClient )
				{
					lpClient->mpNext = apClient->mpNext;
				}
			}
			HashTree_VarUnRegister( apTree, apClient->mpVar );
		}
		mMEMFREE( apClient );
	}
}


void	HashTree_VarClient_Init( sHashTreeVarClient * apClient, sHashTree * apTree, const char * apName, fHashTreeVarCB aOnWrite )
{
	apClient->mHashKey  = HashTree_BuildHash( apName );
	apClient->mfOnWrite = aOnWrite;
	apClient->mpNext    = 0;
	apClient->mpVar     = HashTree_Var_Find( apTree, apClient->mHashKey );

	if( apClient->mpVar )
	{
		apClient->mpNext           = apClient->mpVar->mpClients;
		apClient->mpVar->mpClients = apClient;
	}
	else
	{
		GOD_LL_INSERT( apTree->mpUnboundClients, mpNext, apClient );
	}
}


void	HashTree_VarClient_DeInit( sHashTreeVarClient * apClient, sHashTree * apTree )
{
	if( apClient )
	{
		if( apClient->mpVar )
		{
			GOD_LL_REMOVE( sHashTreeVarClient, apClient->mpVar->mpClients, mpNext, apClient );
		}
		else
		{
			GOD_LL_REMOVE( sHashTreeVarClient, apTree->mpUnboundClients, mpNext, apClient );
		}
/*		HashTree_VarUnRegister( apTree, apClient->mpVar );*/
		apClient->mpVar = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarWrite( sHashTreeVar * apVar,void * apData )
* ACTION   : HashTree_VarWrite
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarWrite( sHashTreeVar * apVar,void * apData )
{
	sHashTreeVarClient *	lpClient;

/*	DebugLog_Printf2( "HashTree_VarWrite: %lx %s", apVar, apData );*/
	GODLIB_ASSERT( apVar );
	if( apVar )
	{
		GODLIB_ASSERT( apVar->mDataSize && apVar->mpData );
		if( (apVar->mDataSize) && (apVar->mpData) )
		{
			Memory_Copy( apVar->mDataSize, apData, apVar->mpData );

			lpClient = apVar->mpClients;
			while( lpClient )
			{
				if( lpClient->mfOnWrite )
				{
					lpClient->mfOnWrite( lpClient );
				}
				lpClient = lpClient->mpNext;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarRead( sHashTreeVar * apVar, void * apDest, const U32 aSize )
* ACTION   : HashTree_VarRead
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void		HashTree_VarRead( const sHashTreeVar * apVar, void * apDest, const U32 aSize )
{
	if( apDest )
	{
		if( (apVar) && (apVar->mpData) && (apVar->mDataSize == aSize) )
		{
			Memory_Copy( aSize, apVar->mpData, apDest );
		}
		else
		{
/*			if( apVar )
			{
				DebugLog_Printf4( "HashTree_VarRead() err : %lX pData %lx varSize:%ld  readSize:%ld ", apVar, apVar->mpData, apVar->mDataSize, aSize );
			}
			else
			{
				DebugLog_Printf0( "HashTree_VarRead() err : null pointer" );
			}*/
			Memory_Clear( aSize, apDest );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_Tokenise( sHashTreeTokeniser * apToken,const char * apString )
* ACTION   : HashTree_Tokenise
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_Tokenise( sHashTreeTokeniser * apToken,const char * apString )
{
	U16	lSrcIndex;
	U16	lDstIndex;
	U16	lCurLen;
	U16	lExitFlag;
	char	lSrcString[ dHASHTREE_STR_LIMIT+1 ];
	char	lString[ dHASHTREE_STR_LIMIT+1 ];

	lSrcIndex = 0;

	while( apString[ lSrcIndex ] == '/' || apString[ lSrcIndex ] == '\\' )
	{
		lSrcIndex++;
	}

	lDstIndex = 0;
	lExitFlag = 0;

	while( !lExitFlag )
	{
		switch( apString[ lSrcIndex ] )
		{
		case	'\\':
		case	'/':
			lSrcString[ lDstIndex ] = '\\';
			lDstIndex++;
			break;
		case	0:
			lExitFlag = 1;

		default:
			lSrcString[ lDstIndex ] = apString[ lSrcIndex ];
			lDstIndex++;
			break;
		}
		if( lDstIndex >= dHASHTREE_STR_LIMIT )
		{
			lExitFlag = 1;
		}
		lSrcIndex++;
	}

	if( lDstIndex )
	{
		if( lSrcString[ lDstIndex-1 ] == '\\' )
		{
			lSrcString[ lDstIndex-1 ] = 0;
		}
	}
	lSrcString[ lDstIndex ] =0;


	lExitFlag = 0;
	lDstIndex = 0;
	lSrcIndex = 0;
	lCurLen   = 0;

	apToken->mTokenCount   = 0;

	while( !lExitFlag )
	{
		switch( lSrcString[ lSrcIndex ] )
		{
		case	'\\':
		case	'/':
			if( lCurLen )
			{
				lCurLen = 0;
				lString[ lDstIndex ] = 0;

				apToken->mLocalHash[ apToken->mTokenCount ] = HashTree_BuildHash( lString );
				apToken->mTokenCount++;

				lDstIndex = 0;
			}
			break;

		case	0:
			lExitFlag=1;
			break;

		default:
			if( lDstIndex < dHASHTREE_STR_LIMIT )
			{
				if( !lCurLen )
				{
					apToken->mGlobalHash[ apToken->mTokenCount ] = HashTree_BuildHash( &lSrcString[ lSrcIndex ] );
				}
				lCurLen++;
				lString[ lDstIndex ] = lSrcString[ lSrcIndex ];
				lDstIndex++;
			}
			else
			{
				lExitFlag = 1;
			}
			break;
		}

		lSrcIndex++;
	}

	if( lCurLen )
	{
		lString[ lDstIndex ] = 0;
		apToken->mLocalHash[ apToken->mTokenCount ] = HashTree_BuildHash( lString );
		apToken->mTokenCount++;
	}
}

void		HashTree_NodeValidate( sHashTreeNode * apNode )
{
	sHashTreeNode * node;

	for( node=apNode; node; node =node->mpNext )
	{
		sHashTreeVar * var;

		for( var=node->mpVars; var; var=var->mpNext)
		{
			GODLIB_ASSERT( var->mpNode == node );
		}
		HashTree_NodeValidate( node->mpChild);
	}
}

#ifndef dGODLIB_PLATFORM_ATARI
void		HashTree_Validate( sHashTree * apTree )
{
	HashTree_NodeValidate( apTree->mpNodes );
}
#endif


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_GetSize( sHashTree * apTree, U32 aFilterFlags )
* ACTION   : determine size of varblock that contains all filtered variables
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

U32						HashTree_VarBlock_GetSize( sHashTree * apTree, U32 aFilterFlags )
{
	U32 size = sizeof(sHashTreeVarBlock);
	U32 elementSize = sizeof(U32) + sizeof(U16);
	sHashTreeVar * var;

	for( var = apTree->mpVars; var; var = var->mpVarNext )
	{
		if( var->mFilterFlags & aFilterFlags )
			size += ( elementSize + var->mDataSize );
	}

	return size;
}


void					HashTree_VarBlock_Init( sHashTreeVarBlock * apBlock, sHashTree * apTree, U32 aFilterFlags )
{
	U8 * mem = (U8*)apBlock;
	U16 index = 0;
	sHashTreeVar * var;

	apBlock->mID = dHASHTREE_BLOCK_ID;
	apBlock->mVersion = dHASHTREE_BLOCK_VERSION;

	apBlock->mVarCount = 0;
	for( var = apTree->mpVars; var; var = var->mpVarNext )
	{
		if( var->mFilterFlags & aFilterFlags )
			apBlock->mVarCount++;
	}

	mem += sizeof(sHashTreeVarBlock);
	apBlock->mpHashes = (U32*)mem;
	mem += sizeof(U32) * apBlock->mVarCount;
	apBlock->mpDataSizes = (U16*)mem;
	mem += sizeof(U16) * apBlock->mVarCount;
	apBlock->mpData = mem;

	apBlock->mTotalDataSize = 0;

	for( var = apTree->mpVars; var; var = var->mpVarNext )
	{
		if( var->mFilterFlags & aFilterFlags )
		{
			apBlock->mpHashes[ index ] = var->mHashKey;
			apBlock->mpDataSizes[ index ] = (U16)var->mDataSize;
			apBlock->mTotalDataSize += var->mDataSize;

			Memory_Copy( var->mDataSize, var->mpData, mem );
			mem += var->mDataSize;

			index++;
		}
	}
}

void					HashTree_VarBlock_DeInit( sHashTreeVarBlock * apBlock )
{
	(void)apBlock;
}

void					HashTree_VarBlock_Delocate( sHashTreeVarBlock * apBlock )
{
	Endian_FromBigU32( &apBlock->mID );
	Endian_FromBigU32( &apBlock->mVersion );
	Endian_FromBigU32( &apBlock->mTotalDataSize );
	Endian_FromBigU32( &apBlock->mVarCount );

	*(U32*)&apBlock->mpData -= (U32)apBlock;
	*(U32*)&apBlock->mpDataSizes -= (U32)apBlock;
	*(U32*)&apBlock->mpHashes -= (U32)apBlock;
}

void					HashTree_VarBlock_Relocate( sHashTreeVarBlock * apBlock )
{
	*(U32*)&apBlock->mpData += (U32)apBlock;
	*(U32*)&apBlock->mpDataSizes += (U32)apBlock;
	*(U32*)&apBlock->mpHashes += (U32)apBlock;

	Endian_FromBigU32( &apBlock->mID );
	Endian_FromBigU32( &apBlock->mVersion );
	Endian_FromBigU32( &apBlock->mTotalDataSize );
	Endian_FromBigU32( &apBlock->mVarCount );
}

void	HashTree_VarBlock_Apply( sHashTreeVarBlock * apBlock, sHashTree * apTree )
{
	U32 count = apBlock->mVarCount;
	U32 * pHashes = apBlock->mpHashes;
	U16 * pSizes = apBlock->mpDataSizes;
	U8 * pData = apBlock->mpData;

	if( (apBlock->mVersion != dHASHTREE_BLOCK_VERSION) || (apBlock->mID != dHASHTREE_BLOCK_ID) )
		return;

	while( count )
	{
		sHashTreeVar * var = HashTree_Var_Find( apTree, *pHashes );
		GODLIB_ASSERT( var );
		if( var )
		{
			HashTree_VarWrite( var, pData );
		}
		pData += *pSizes;
		pHashes++;
		pSizes++;
		count--;
	}
}

/* ################################################################################ */
