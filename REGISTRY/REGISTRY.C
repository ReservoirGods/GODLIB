/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"REGISTRY.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dREGISTRY_STR_LIMIT	256
#define	dREGISTRY_TOK_LIMIT	32


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	U16		mTokenCount;
	U32		mGlobalHash[ dREGISTRY_TOK_LIMIT ];
	U32		mLocalHash[ dREGISTRY_TOK_LIMIT ];
} sRegistryTokeniser;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sRegistryNode *	Registry_NodeRegister( sRegistry * apTree, const char * apName );
void			Registry_NodeUnRegister( sRegistry * apTree, sRegistryNode * apNode );

void			Registry_Tokenise( sRegistryTokeniser * apToken, const char * apString );

void			Registry_SubNodesDestroy( sRegistry * apTree, sRegistryNode * apNode );

sRegistryNode *	Registry_NodeTokReg( sRegistry * apTree, sRegistryTokeniser * apToken );

sRegistryNode *	Registry_NodeCreate( sRegistry * apTree, const U32 aGlobalID, const U32 aLocalID );
void			Registry_NodeDestroy( sRegistry * apTree, sRegistryNode * apNode );

sRegistryVar *	Registry_NodeVarReg( sRegistryNode * apNode, const U32 aGlobalID, const U32 aLocalID );

U32				Registry_NodeGetSaveSize( sRegistryNode * apNode );
U32				Registry_VarGetSaveSize( sRegistryVar * apVar );
U8 *			Registry_NodeSave( const sRegistryNode * apNode, sRegistrySaveNode * apLastNode, U8 * apMem );
U8 *			Registry_NodesSave( const sRegistryNode * apNode, U8 * apMem );

void			Registry_NodeLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc );
void			Registry_NodeUnLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc );
void			Registry_NodesLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_BuildHash( const char * apName )
* ACTION   : Registry_BuildHash
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Registry_BuildHash( const char * apName )
{
	U32		lHash;
	U32		lTemp;
	char	lC;

	lHash = 0;
	while( *apName )
	{
		lC = *apName++;
		if( ( lC >= 'a' ) && ( lC <= 'z' ) )
		{
			lC += ( 'A' - 'a' );
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
* FUNCTION : Registry_Init( sRegistry * apTree )
* ACTION   : Registry_Init
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_Init( sRegistry * apTree )
{
	apTree->mNodeCount = 0;
	apTree->mVariableCount = 0;
	apTree->mpNodes = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_DeInit( sRegistry * apTree )
* ACTION   : Registry_DeInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_DeInit( sRegistry * apTree )
{
	if( apTree->mpNodes )
	{
		Registry_NodeUnRegister( apTree, apTree->mpNodes );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeCreate( sRegistry * apTree, const U32 aGlobalID,const U32 aLocalID )
* ACTION   : Registry_NodeCreate
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryNode *	Registry_NodeCreate( sRegistry * apTree, const U32 aGlobalID, const U32 aLocalID )
{
	sRegistryNode *	lpNode;

	lpNode = (sRegistryNode*)mMEMCALLOC( sizeof( sRegistryNode ) );

	if( lpNode )
	{
		lpNode->mGlobalID = aGlobalID;
		lpNode->mLocalID = aLocalID;
		if( apTree )
		{
			apTree->mNodeCount++;
		}
	}

	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeDestroy( sRegistry * apTree, sRegistryNode * apNode )
* ACTION   : Registry_NodeDestroy
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_NodeDestroy( sRegistry * apTree, sRegistryNode * apNode )
{
	if( apTree )
	{
		if( apTree->mpNodes == apNode )
		{
			apTree->mpNodes = 0;
		}
		apTree->mNodeCount--;
	}
	mMEMFREE( apNode );
	/*
	sRegistryVar *	lpVar;

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
* FUNCTION : Registry_NodeRegister( sRegistry * apTree,const char * apName )
* ACTION   : Registry_NodeRegister
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryNode *	Registry_NodeRegister( sRegistry * apTree, const char * apName )
{
	sRegistryTokeniser	lTokeniser;
	sRegistryNode *		lpNode;

	Registry_Tokenise( &lTokeniser, apName );

	lpNode = Registry_NodeTokReg( apTree, &lTokeniser );

	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeTokReg( sRegistry * apTree,sRegistryTokeniser * apToken )
* ACTION   : Registry_NodeTokReg
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryNode *	Registry_NodeTokReg( sRegistry * apTree, sRegistryTokeniser * apToken )
{
	sRegistryNode *	lpNode;
	sRegistryNode *	lpParent;
	U16				i;

	lpNode = 0;

	if( apToken->mTokenCount )
	{
		if( !apTree->mpNodes )
		{
			apTree->mpNodes = Registry_NodeCreate( apTree, apToken->mGlobalHash[ 0 ], apToken->mLocalHash[ 0 ] );
		}

		i = 0;
		lpNode = apTree->mpNodes;

		while( i < apToken->mTokenCount )
		{
			lpParent = lpNode;
			while( ( lpNode ) && ( i < apToken->mTokenCount ) )
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
							lpParent = lpNode;
							lpNode = Registry_NodeCreate( apTree, apToken->mGlobalHash[ i ], apToken->mLocalHash[ i ] );
							lpNode->mpParent = lpParent;
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
				lpNode = Registry_NodeCreate( apTree, apToken->mGlobalHash[ i ], apToken->mLocalHash[ i ] );
				lpNode->mpNext = lpParent->mpNext;
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
* FUNCTION : Registry_NodeUnRegister( sRegistry * apTree, sRegistryNode * apNode )
* ACTION   : Registry_NodeUnRegister
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_NodeUnRegister( sRegistry * apTree, sRegistryNode * apNode )
{
	sRegistryNode *			lpNode;


	if( apNode )
	{
		apNode->mRefCount--;
		if( apNode->mRefCount <= 0 )
		{
			Registry_SubNodesDestroy( apTree, apNode->mpChild );

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
						while( ( lpNode->mpNext ) && ( lpNode->mpNext != apNode ) )
						{
							lpNode = lpNode->mpNext;
						}
						if( lpNode->mpNext == apNode )
						{
							lpNode->mpNext = apNode->mpNext;
						}
					}
				}
				Registry_NodeUnRegister( apTree, apNode->mpParent );
			}
			Registry_NodeDestroy( apTree, apNode );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SubNodesDestroy( sRegistry * apTree, sRegistryNode * apNode )
* ACTION   : Registry_SubNodesDestroy
* CREATION : 05.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SubNodesDestroy( sRegistry * apTree, sRegistryNode * apNode )
{
	sRegistryNode *			lpNode;
	sRegistryNode *			lpNodeNext;

	lpNode = apNode;
	while( lpNode )
	{
		Registry_SubNodesDestroy( apTree, lpNode->mpChild );

		lpNodeNext = lpNode->mpNext;
		Registry_NodeDestroy( apTree, lpNode );
		lpNode = lpNodeNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarInit( sRegistry * apTree,const char * apName,const U32 aSize,void * apData )
* ACTION   : Registry_VarInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryVar *	Registry_VarInit( sRegistry * apTree, const char * apName, const U32 aSize, void * apData )
{
	sRegistryVar *			lpVar;
	sRegistryVarClient *	lpClient;


	lpVar = Registry_VarRegister( apTree, apName );

	if( lpVar )
	{
		lpVar->mDataSize = aSize;
		if( aSize <= 4 )
		{
			lpVar->mpData = &lpVar->mDataSmall;
		}
		else
		{
			/*			DebugLog_Printf2( "Registry_VarInit() : %s : %s", apName, apData );*/
			lpVar->mpData = mMEMCALLOC( aSize );
		}
		if( lpVar->mpData )
		{
			Memory_Copy( aSize, apData, lpVar->mpData );
		}

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
* FUNCTION : Registry_VarDeInit( sRegistry * apTree, sRegistryVar * apVar )
* ACTION   : Registry_VarDeInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_VarDeInit( sRegistry * apTree, sRegistryVar * apVar )
{
	sRegistryVarClient *	lpClient;


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
			if( apVar->mDataSize > 4 )
			{
				mMEMFREE( apVar->mpData );
			}
			apVar->mpData = 0;
			apVar->mDataSize = 0;
		}
	}

	Registry_VarUnRegister( apTree, apVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeVarReg( sRegistryNode * apNode,const U32 aGlobalID,const U32 aLocalID )
* ACTION   : Registry_NodeVarReg
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryVar *	Registry_NodeVarReg( sRegistryNode * apNode, const U32 aGlobalID, const U32 aLocalID )
{
	sRegistryVar *		lpVar;

	lpVar = apNode->mpVars;

	while( ( lpVar ) && ( lpVar->mLocalID != aLocalID ) )
	{
		lpVar = lpVar->mpNext;
	}

	if( !lpVar )
	{
		lpVar = (sRegistryVar*)mMEMCALLOC( sizeof( sRegistryVar ) );
		if( lpVar )
		{
			lpVar->mGlobalID = aGlobalID;
			lpVar->mLocalID = aLocalID;
			lpVar->mpNode = apNode;
			lpVar->mpNext = apNode->mpVars;
			apNode->mpVars = lpVar;
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


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarRegister( sRegistry * apTree,const char * apName )
* ACTION   : Registry_VarRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryVar *	Registry_VarRegister( sRegistry * apTree, const char * apName )
{
	sRegistryTokeniser	lTokeniser;
	sRegistryVar *		lpVar;
	sRegistryNode *		lpNode;
	U32					lLocalHash;


	Registry_Tokenise( &lTokeniser, apName );

	lpVar = 0;

	if( lTokeniser.mTokenCount > 1 )
	{
		lTokeniser.mTokenCount--;
		lpNode = Registry_NodeTokReg( apTree, &lTokeniser );
		if( lpNode )
		{
			lpVar = lpNode->mpVars;

			lLocalHash = lTokeniser.mLocalHash[ lTokeniser.mTokenCount ];


			while( ( lpVar ) && ( lpVar->mLocalID != lLocalHash ) )
			{
				lpVar = lpVar->mpNext;
			}
			if( !lpVar )
			{
				lpVar = (sRegistryVar*)mMEMCALLOC( sizeof( sRegistryVar ) );
				if( lpVar )
				{
					lpVar->mGlobalID = lTokeniser.mGlobalHash[ lTokeniser.mTokenCount ];
					lpVar->mLocalID = lLocalHash;
					lpVar->mpNode = lpNode;
					lpVar->mpNext = lpNode->mpVars;
					lpNode->mpVars = lpVar;
				}
			}
			if( lpVar )
			{
				lpVar->mRefCount++;
			}
		}

	}
	/*	DebugLog_Printf3( "Registry_VarRegister() tree:%lX %s %lx", apTree, apName, lpVar );*/

	return( lpVar );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarUnRegister( sRegistry * apTree, sRegistryVar * apVar )
* ACTION   : Registry_VarUnRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_VarUnRegister( sRegistry * apTree, sRegistryVar * apVar )
{
	if( apVar )
	{
		sRegistryNode *	lpNode;

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
				sRegistryVar *	lpVar;

				lpVar = apVar->mpNode->mpVars;
				while( lpVar->mpNext && ( lpVar->mpNext != apVar ) )
				{
					lpVar = lpVar->mpNext;
				}
				lpVar->mpNext = apVar->mpNext;
			}

			mMEMFREE( apVar );
		}

		if( lpNode )
		{
			Registry_NodeUnRegister( apTree, lpNode );
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarClientRegister( sRegistry * apTree,const * apName,fRegistryVarCB aOnWrite,fRegistryVarCB aOnInit,fRegistryVarCB onDeInit )
* ACTION   : Registry_VarClientRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistryVarClient *	Registry_VarClientRegister( sRegistry * apTree, const char * apName, fRegistryVarCB aOnWrite, fRegistryVarCB aOnInit, fRegistryVarCB aOnDeInit, const U32 aUserData )
{
	sRegistryVarClient *	lpClient;
	sRegistryVar *			lpVar;


	lpClient = 0;
	lpVar = Registry_VarRegister( apTree, apName );

	if( lpVar )
	{
		lpClient = (sRegistryVarClient*)mMEMCALLOC( sizeof( sRegistryVarClient ) );
		if( lpClient )
		{
			lpClient->mfOnDeInit = aOnDeInit;
			lpClient->mfOnInit = aOnInit;
			lpClient->mfOnWrite = aOnWrite;
			lpClient->mpVar = lpVar;
			lpClient->mpNext = lpVar->mpClients;
			lpClient->mUserData = aUserData;
			lpVar->mpClients = lpClient;
		}
	}
	return( lpClient );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarClientUnRegister( sRegistry * apTree, sRegistryVarClient * apClient )
* ACTION   : Registry_VarClientUnRegister
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_VarClientUnRegister( sRegistry * apTree, sRegistryVarClient * apClient )
{
	sRegistryVarClient * lpClient;

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
				while( ( lpClient->mpNext ) && ( lpClient->mpNext != apClient ) )
				{
					lpClient = lpClient->mpNext;
				}
				if( lpClient->mpNext == apClient )
				{
					lpClient->mpNext = apClient->mpNext;
				}
			}
			Registry_VarUnRegister( apTree, apClient->mpVar );
		}
		mMEMFREE( apClient );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarWrite( sRegistryVar * apVar,void * apData )
* ACTION   : Registry_VarWrite
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_VarWrite( sRegistryVar * apVar, void * apData )
{
	sRegistryVarClient *	lpClient;

	/*	DebugLog_Printf2( "Registry_VarWrite: %lx %s", apVar, apData );*/
	if( apVar )
	{
		if( ( apVar->mDataSize ) && ( apVar->mpData ) )
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
* FUNCTION : Registry_VarRead( sRegistryVar * apVar, void * apDest, const U32 aSize )
* ACTION   : Registry_VarRead
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void		Registry_VarRead( const sRegistryVar * apVar, void * apDest, const U32 aSize )
{
	if( apDest )
	{
		if( ( apVar ) && ( apVar->mpData ) && ( apVar->mDataSize == aSize ) )
		{
			Memory_Copy( aSize, apVar->mpData, apDest );
		}
		else
		{
			/*			if( apVar )
			{
			DebugLog_Printf4( "Registry_VarRead() err : %lX pData %lx varSize:%ld  readSize:%ld ", apVar, apVar->mpData, apVar->mDataSize, aSize );
			}
			else
			{
			DebugLog_Printf0( "Registry_VarRead() err : null pointer" );
			}*/
			Memory_Clear( aSize, apDest );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_Tokenise( sRegistryTokeniser * apToken,const char * apString )
* ACTION   : Registry_Tokenise
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_Tokenise( sRegistryTokeniser * apToken, const char * apString )
{
	U16	lSrcIndex;
	U16	lDstIndex;
	U16	lCurLen;
	U16	lExitFlag;
	char	lSrcString[ dREGISTRY_STR_LIMIT + 1 ];
	char	lString[ dREGISTRY_STR_LIMIT + 1 ];

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
		if( lDstIndex >= dREGISTRY_STR_LIMIT )
		{
			lExitFlag = 1;
		}
		lSrcIndex++;
	}

	if( lDstIndex )
	{
		if( lSrcString[ lDstIndex - 1 ] == '\\' )
		{
			lSrcString[ lDstIndex - 1 ] = 0;
		}
	}
	lSrcString[ lDstIndex ] = 0;


	lExitFlag = 0;
	lDstIndex = 0;
	lSrcIndex = 0;
	lCurLen = 0;

	apToken->mTokenCount = 0;

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

				apToken->mLocalHash[ apToken->mTokenCount ] = Registry_BuildHash( lString );
				apToken->mTokenCount++;

				lDstIndex = 0;
			}
			break;

		case	0:
			lExitFlag = 1;
			break;

		default:
			if( lDstIndex < dREGISTRY_STR_LIMIT )
			{
				if( !lCurLen )
				{
					apToken->mGlobalHash[ apToken->mTokenCount ] = Registry_BuildHash( &lSrcString[ lSrcIndex ] );
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
		apToken->mLocalHash[ apToken->mTokenCount ] = Registry_BuildHash( lString );
		apToken->mTokenCount++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeBuild( sRegistry * apTree,const char * apNodeName )
* ACTION   : Registry_SaveNodeBuild
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sRegistrySaveNode *	Registry_SaveNodeBuild( sRegistry * apTree, const char * apNodeName )
{
	sRegistrySaveNode * lpSaveNode;
	sRegistryNode *		lpNode;
	U8 *				lpMem;
	U32					lSize;

	/*	DebugLog_Printf2( "Registry_SaveNodeBuild() apTree %lX apNodeName %s", apTree, apNodeName );*/

	lpSaveNode = 0;
	if( apTree )
	{
		lpNode = Registry_NodeRegister( apTree, apNodeName );
		if( lpNode )
		{
			lSize = Registry_NodeGetSaveSize( lpNode );
			lpMem = (U8*)mMEMCALLOC( lSize );
			lpSaveNode = (sRegistrySaveNode*)lpMem;
			if( lpMem )
			{
				Registry_NodeSave( lpNode, 0, lpMem );
			}
		}
	}

	return( lpSaveNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeDestroy( ssRegistry * apTree, const char * apNodeName, RegistrySaveNode * apSaveNode )
* ACTION   : Registry_SaveNodeDestroy
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodeDestroy( sRegistry * apTree, const char * apNodeName, sRegistrySaveNode * apSaveNode )
{
#if	0
	sRegistrySaveNode *	lpNode;
	sRegistrySaveNode *	lpNodeNext;


	lpNode = apNode;

	while( lpNode )
	{
		lpNodeNext = lpNode->mpNext;
		Registry_SaveNodeDestroy( lpNode->mpChild );
		/*		mMEMFREE( lpNode->mpVars );*/
		mMEMFREE( lpNode );
		lpNode = lpNodeNext;
	}
#endif
	sRegistryNode *	lpNode;

	lpNode = Registry_NodeRegister( apTree, apNodeName );
	Registry_NodeUnRegister( apTree, lpNode );
	Registry_NodeUnRegister( apTree, lpNode );
	mMEMFREE( apSaveNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeLoad( sRegistry * apTree,const char * apNodeName,const sRegistrySaveNode * apSaveNode )
* ACTION   : Registry_SaveNodeLoad
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodeLoad( sRegistry * apTree, const char * apNodeName, const sRegistrySaveNode * apSaveNode )
{
	sRegistryNode *	lpNode;

	/*	DebugLog_Printf3( "Registry_SaveNodeLoad() tree %lX name %s node %lx", apTree, apNodeName, apSaveNode );*/

	if( apTree && apSaveNode )
	{
		lpNode = Registry_NodeRegister( apTree, apNodeName );
		if( lpNode )
		{
			Registry_NodeLoad( apTree, lpNode, apSaveNode );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeUnLoad( sRegistry * apTree, const char * apNodeName, const sRegistrySaveNode * apSaveNode )
* ACTION   : Registry_SaveNodeUnLoad
* CREATION : 16.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodeUnLoad( sRegistry * apTree, const char * apNodeName, const sRegistrySaveNode * apSaveNode )
{
	sRegistryNode *	lpNode;

	/*	DebugLog_Printf3( "Registry_SaveNodeLoad() tree %lX name %s node %lx", apTree, apNodeName, apSaveNode );*/

	if( apTree && apSaveNode )
	{
		lpNode = Registry_NodeRegister( apTree, apNodeName );
		if( lpNode )
		{
			Registry_NodeUnLoad( apTree, lpNode, apSaveNode );
		}
		Registry_NodeUnRegister( apTree, lpNode );
		Registry_NodeUnRegister( apTree, lpNode );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeGetSize( const sRegistrySaveNode * apSaveNode )
* ACTION   : Registry_SaveNodeGetSize
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Registry_SaveNodeGetSize( const sRegistrySaveNode * apSaveNode )
{
	U32							lSize;
	U16							i;
	const sRegistrySaveNode *	lpNode;

	/*	DebugLog_Printf1( "Registry_SaveNodeGetSize() size %lX", apSaveNode );*/

	lSize = 0;
	lpNode = apSaveNode;

	while( lpNode )
	{
		/*		DebugLog_Printf1( "Node LID      %lX\n", lpNode->mLocalID );
		DebugLog_Printf1( "Node GID      %lX\n", lpNode->mGlobalID );
		DebugLog_Printf1( "Node mpChild  %lX\n", lpNode->mpChild );
		DebugLog_Printf1( "Node mpNext   %lX\n", lpNode->mpNext );
		DebugLog_Printf1( "Node varcount %ld\n", lpNode->mVarCount );
		DebugLog_Printf1( "Node mpVars   %lX\n", lpNode->mpVars );
		*/

		lSize += sizeof( sRegistrySaveNode );

		for( i = 0; i< lpNode->mVarCount; i++ )
		{
			/*			DebugLog_Printf1( "Var          %lX\n", &lpNode->mpVars[ i ]  );
			DebugLog_Printf1( "Var LID      %lX\n", lpNode->mpVars[ i ].mLocalID   );
			DebugLog_Printf1( "Var GID      %lX\n", lpNode->mpVars[ i ].mGlobalID );
			DebugLog_Printf1( "Var DataSize %ld\n", lpNode->mpVars[ i ].mDataSize );
			DebugLog_Printf1( "Var mpData   %lX\n", lpNode->mpVars[ i ].mpData    );
			*/
			lSize += sizeof( sRegistrySaveVar );
			lSize += ( ( lpNode->mpVars[ i ].mDataSize + 1 ) & 0xFFFFFFFEL );
			/*			DebugLog_Printf2( "size %ld varsize %ld\n", lSize, lpNode->mpVars[ i ].mDataSize );*/
		}

		Registry_SaveNodeGetSize( lpNode->mpChild );

		lpNode = lpNode->mpNext;
	}

	/*	DebugLog_Printf2( "Registry_SaveNodeGetSize() %lx size %ld\n", apSaveNode, lSize );*/

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeGetSaveSize( sRegistryNode * apNode )
* ACTION   : Registry_NodeGetSaveSize
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Registry_NodeGetSaveSize( sRegistryNode * apNode )
{
	U32				lSize;
	sRegistryNode *	lpNode;

	/*	DebugLog_Printf1( "Registry_NodeGetSaveSize() %lX", apNode );*/

	lSize = 0;
	lpNode = apNode;

	while( lpNode )
	{
		lSize = sizeof( sRegistrySaveNode );
		lSize += Registry_VarGetSaveSize( lpNode->mpVars );
		lSize += Registry_NodeGetSaveSize( lpNode->mpChild );
		lpNode = lpNode->mpNext;
	}

	/*	DebugLog_Printf2( "Registry_NodeGetSaveSize() %lx size %ld", apNode, lSize );*/

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_VarGetSaveSize( sRegistryVar * apVar )
* ACTION   : Registry_VarGetSaveSize
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Registry_VarGetSaveSize( sRegistryVar * apVar )
{
	U32				lSize;
	sRegistryVar *	lpVar;

	/*	DebugLog_Printf1( "Registry_VarGetSaveSize() %lX", apVar );*/

	lSize = 0;
	lpVar = apVar;

	while( lpVar )
	{
		lSize += sizeof( sRegistrySaveVar );
		lSize += ( lpVar->mDataSize + 1 ) & 0xFFFFFFFEL;
		lpVar = lpVar->mpNext;
	}

	/*	DebugLog_Printf1( "Registry_VarGetSaveSize() size %ld", lSize );*/

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeSave( const sRegistryNode * apNode,const sRegistrySaveNode * apLastNode,U8 * apMem )
* ACTION   : Registry_NodeSave
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 *	Registry_NodeSave( const sRegistryNode * apNode, sRegistrySaveNode * apLastNode, U8 * apMem )
{
	U8 *				lpMem;
	sRegistrySaveNode *	lpSaveNode;
	sRegistrySaveVar *	lpSaveVar;
	sRegistryVar *		lpVar;

	/*	DebugLog_Printf3( "Registry_NodeSave() apNode:%lX apLastNode:%lx apMem %lx", apNode, apLastNode, apMem );*/

	lpMem = apMem;
	if( apNode )
	{
		lpSaveNode = (sRegistrySaveNode*)lpMem;
		lpMem += sizeof( sRegistrySaveNode );

		if( apLastNode )
		{
			apLastNode->mpNext = lpSaveNode;
		}

		lpSaveNode->mGlobalID = apNode->mGlobalID;
		lpSaveNode->mLocalID = apNode->mLocalID;
		lpSaveNode->mpVars = (sRegistrySaveVar*)lpMem;
		lpSaveNode->mpNext = 0;

		lpSaveNode->mVarCount = 0;

		lpSaveVar = (sRegistrySaveVar*)lpMem;

		lpVar = apNode->mpVars;
		while( lpVar )
		{
			lpSaveNode->mVarCount++;
			lpMem += sizeof( sRegistrySaveVar );
			lpVar = lpVar->mpNext;
		}

		lpVar = apNode->mpVars;
		while( lpVar )
		{
			/*			DebugLog_Printf1( "Registry_NodeSave() var %lx", lpVar );*/

			lpSaveVar->mDataSize = lpVar->mDataSize;
			lpSaveVar->mGlobalID = lpVar->mGlobalID;
			lpSaveVar->mLocalID = lpVar->mLocalID;
			lpSaveVar->mpData = lpMem;

			/*			DebugLog_Printf3( "Registry_NodeSave() varcpy sz:%ld src:%lx dst:%lx", lpVar->mDataSize, lpVar->mpData, lpMem );*/
			Memory_Copy( lpVar->mDataSize, lpVar->mpData, lpMem );
			lpMem += ( ( lpVar->mDataSize + 1L ) & 0xFFFFFFFEL );

			lpVar = lpVar->mpNext;
			lpSaveVar++;
		}

		if( apNode->mpChild )
		{
			lpMem = Registry_NodesSave( apNode->mpChild, lpMem );
			lpSaveNode->mpChild = (sRegistrySaveNode*)lpMem;
		}
	}


	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodesSave( const sRegistryNode * apNode,U8 * apMem )
* ACTION   : Registry_NodesSave
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 *	Registry_NodesSave( const sRegistryNode * apNode, U8 * apMem )
{
	const sRegistryNode *	lpNode;
	sRegistrySaveNode *		lpLastMem;
	sRegistrySaveNode *		lpLast;
	U8 *					lpMem;

	/*	DebugLog_Printf2( "Registry_NodesSave() apNode:%lX apMem:%lx", apNode, apMem );*/

	lpNode = apNode;
	lpMem = apMem;
	lpLastMem = 0;
	lpLast = 0;

	while( lpNode )
	{
		lpLast = lpLastMem;
		lpLastMem = (sRegistrySaveNode*)lpMem;
		lpMem = Registry_NodeSave( lpNode, lpLast, lpMem );
		lpNode = lpNode->mpNext;
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeLoad( sRegistry * apTree, sRegistryNode * apNode,const sRegistrySaveNode * apSrc )
* ACTION   : Registry_NodeLoad
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_NodeLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc )
{
	U16				i;
	sRegistryVar *	lpVar;
	sRegistrySaveVar *	lpSrcVar;

	/*	DebugLog_Printf2( "Registry_SaveNodeLoad() apDst:%lX apSrc:%lx", apNode, apSrc );*/
	(void)apTree;

	if( apNode && apSrc )
	{
		apNode->mGlobalID = apSrc->mGlobalID;
		apNode->mLocalID = apSrc->mLocalID;

		for( i = 0; i<apSrc->mVarCount; i++ )
		{
			lpSrcVar = &apSrc->mpVars[ i ];
			lpVar = Registry_NodeVarReg( apNode, lpSrcVar->mGlobalID, lpSrcVar->mLocalID );
			if( !lpVar->mpData )
			{
				lpVar->mDataSize = lpSrcVar->mDataSize;
				if( lpVar->mDataSize <= 4 )
				{
					lpVar->mpData = &lpVar->mDataSmall;
				}
				else
				{
					lpVar->mpData = mMEMCALLOC( lpVar->mDataSize );
				}
			}
			Registry_VarWrite( lpVar, lpSrcVar->mpData );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodeUnLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc )
* ACTION   : Registry_NodeUnLoad
* CREATION : 16.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_NodeUnLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc )
{
	U16				i;
	sRegistryVar *	lpVar;
	sRegistrySaveVar *	lpSrcVar;

	if( apNode && apSrc )
	{
		apNode->mGlobalID = apSrc->mGlobalID;
		apNode->mLocalID = apSrc->mLocalID;

		for( i = 0; i<apSrc->mVarCount; i++ )
		{
			lpSrcVar = &apSrc->mpVars[ i ];
			lpVar = Registry_NodeVarReg( apNode, lpSrcVar->mGlobalID, lpSrcVar->mLocalID );
			Registry_VarUnRegister( apTree, lpVar );
			Registry_VarUnRegister( apTree, lpVar );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_NodesLoad( sRegistry * apTree, sRegistryNode * apNode,const sRegistrySaveNode * apSrc )
* ACTION   : Registry_NodesLoad
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_NodesLoad( sRegistry * apTree, sRegistryNode * apNode, const sRegistrySaveNode * apSrc )
{
	sRegistryNode *		lpNode;
	sRegistryNode *		lpNode2;
	const sRegistrySaveNode *	lpSrcNode;

	/*	DebugLog_Printf2( "Registry_SaveNodesLoad() apDst:%lX apSrc:%lx", apNode, apSrc );*/

	if( apNode )
	{
		lpSrcNode = apSrc;

		while( lpSrcNode )
		{
			lpNode = apNode;
			while( ( lpNode ) && ( lpNode->mLocalID != lpSrcNode->mLocalID ) )
			{
				lpNode = lpNode->mpNext;
			}
			if( !lpNode )
			{
				lpNode = Registry_NodeCreate( apTree, lpSrcNode->mGlobalID, lpSrcNode->mLocalID );
				lpNode->mpParent = apNode->mpParent;
				lpNode->mpNext = apNode->mpNext;
				apNode->mpNext = lpNode;
			}

			Registry_NodeLoad( apTree, lpNode, lpSrcNode );

			if( lpSrcNode->mpChild )
			{
				if( !lpNode->mpChild )
				{
					lpNode2 = Registry_NodeCreate( apTree, lpSrcNode->mGlobalID, lpSrcNode->mLocalID );
					lpNode2->mpParent = lpNode;
					lpNode->mpChild = lpNode2;
				}
				Registry_NodesLoad( apTree, lpNode->mpChild, lpSrcNode->mpChild );
			}

			lpSrcNode = lpSrcNode->mpNext;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodesDelocate( sRegistrySaveNode * apNode )
* ACTION   : Registry_SaveNodesDelocate
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodesDelocate( sRegistrySaveNode * apNode )
{
	sRegistrySaveNode *	lpNode;
	sRegistrySaveNode *	lpNodeNext;

	/*	DebugLog_Printf1( "Registry_SaveNodesRelocate() %lX", apNode );*/

	lpNode = apNode;
	while( lpNode )
	{
		lpNodeNext = lpNode->mpNext;
		Registry_SaveNodeDelocate( lpNode );
		*(U32*)&lpNode->mpNext -= (U32)lpNode;
		Endian_FromBigU32( &lpNode->mpNext );
		lpNode = lpNodeNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeDelocate( sRegistrySaveNode * apNode )
* ACTION   : Registry_SaveNodeDelocate
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodeDelocate( sRegistrySaveNode * apNode )
{
	U16	i;

	/*	DebugLog_Printf1( "Registry_SaveNodeDelocate() %lX", apNode );*/

	if( apNode )
	{
		if( apNode->mpChild )
		{
			Registry_SaveNodesDelocate( apNode->mpChild );
			*(U32*)&apNode->mpChild -= (U32)apNode;
			Endian_FromBigU32( &apNode->mpChild );
		}
		if( apNode->mpVars )
		{
			for( i = 0; i<apNode->mVarCount; i++ )
			{
				if( 4 == apNode->mpVars[ i ].mDataSize )
				{
					Endian_FromBigU32( apNode->mpVars[ i ].mpData );
				}
				else if( 2 == apNode->mpVars[ i ].mDataSize )
				{
					Endian_FromBigU16( apNode->mpVars[ i ].mpData );
				}
				*(U32*)&apNode->mpVars[ i ].mpData -= (U32)apNode;
				Endian_FromBigU32( &apNode->mpVars[ i ].mpData );
				Endian_FromBigU32( &apNode->mpVars[ i ].mDataSize );
				Endian_FromBigU32( &apNode->mpVars[ i ].mGlobalID );
				Endian_FromBigU32( &apNode->mpVars[ i ].mLocalID );
			}
			*(U32*)&apNode->mpVars -= (U32)apNode;
			Endian_FromBigU32( &apNode->mpVars );
		}
		Endian_FromBigU32( &apNode->mGlobalID );
		Endian_FromBigU32( &apNode->mLocalID );
		Endian_FromBigU16( &apNode->mVarCount );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodesRelocate( sRegistrySaveNode * apNode )
* ACTION   : Registry_SaveNodesRelocate
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodesRelocate( sRegistrySaveNode * apNode )
{
	sRegistrySaveNode *	lpNode;
	sRegistrySaveNode *	lpNodeNext;

	/*	DebugLog_Printf1( "Registry_SaveNodesRelocate() %lX", apNode );*/

	lpNode = apNode;
	while( lpNode )
	{
		Endian_FromBigU32( &lpNode->mpNext );
		lpNodeNext = lpNode->mpNext;
		Registry_SaveNodeRelocate( lpNode );
		Endian_FromBigU32( &lpNode->mpNext );
		*(U32*)&lpNode->mpNext += (U32)lpNode;
		lpNode = lpNodeNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Registry_SaveNodeRelocate( sRegistrySaveNode * apNode )
* ACTION   : Registry_SaveNodeRelocate
* CREATION : 28.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Registry_SaveNodeRelocate( sRegistrySaveNode * apNode )
{
	U16	i;

	/*	DebugLog_Printf1( "Registry_SaveNodeRelocate() %lX", apNode );*/

	if( apNode )
	{
		Endian_FromBigU32( &apNode->mGlobalID );
		Endian_FromBigU32( &apNode->mLocalID );
		Endian_FromBigU16( &apNode->mVarCount );

		if( apNode->mpChild )
		{
			Endian_FromBigU32( &apNode->mpChild );
			*(U32*)&apNode->mpChild += (U32)apNode;
			Registry_SaveNodesRelocate( apNode->mpChild );
		}
		if( apNode->mpVars )
		{
			Endian_FromBigU32( &apNode->mpVars );
			*(U32*)&apNode->mpVars += (U32)apNode;
			for( i = 0; i<apNode->mVarCount; i++ )
			{
				Endian_FromBigU32( &apNode->mpVars[ i ].mDataSize );
				Endian_FromBigU32( &apNode->mpVars[ i ].mGlobalID );
				Endian_FromBigU32( &apNode->mpVars[ i ].mLocalID );
				Endian_FromBigU32( &apNode->mpVars[ i ].mpData );
				*(U32*)&apNode->mpVars[ i ].mpData += (U32)apNode;
				if( 4 == apNode->mpVars[ i ].mDataSize )
				{
					Endian_FromBigU32( apNode->mpVars[ i ].mpData );
				}
				else if( 2 == apNode->mpVars[ i ].mDataSize )
				{
					Endian_FromBigU16( apNode->mpVars[ i ].mpData );
				}
			}
		}
	}
}


/* ################################################################################ */
