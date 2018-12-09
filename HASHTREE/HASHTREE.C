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


/* ###################################################################################
#  STRUCTS
################################################################################### */


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


#ifdef dGODLIB_PLATFORM_ATARI
#define			HashTree_Validate( _a )
#else
void			HashTree_Validate( sHashTree * apTree );
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
	apTree->mVariableCount = 0;
	apTree->mpVars = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_DeInit( sHashTree * apTree )
* ACTION   : HashTree_DeInit
* CREATION : 04.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_DeInit( sHashTree * apTree )
{
	(void)apTree;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_Var_Find( sHashTree * apTree, U32 aKey )
* ACTION   : finds a var based on a hash key
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarClient_Find( sHashTree * apTree, U32 aKey )
* ACTION   : finds a var client based on a hash key
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_Var_Init( sHashTreeVar * apVar, sHashTree * apTree, const char * apName, const U32 aSize, void * apData )
* ACTION   : inits a var and adds to hash tree
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_Var_DeInit( sHashTreeVar * apVar, sHashTree * apTree )
* ACTION   : deinits a var and removes from hash tree
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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
* FUNCTION : HashTree_VarClient_Init( sHashTreeVarClient * apClient, sHashTree * apTree, const char * apName, fHashTreeVarCB aOnWrite )
* ACTION   : inits a varclient, links it to variable if found, otherwise attaches it to unbound client list in hashtree
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarClient_DeInit( sHashTreeVarClient * apClient, sHashTree * apTree )
* ACTION   : deinits a varclient, detaches from variables and tree
* CREATION : 09.12.2018 PNK
*-----------------------------------------------------------------------------------*/

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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_Init( sHashTreeVarBlock * apBlock, sHashTree * apTree, U32 aFilterFlags )
* ACTION   : inits a varblock, sets hashes, datasizes and data
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarBlock_Init( sHashTreeVarBlock * apBlock, sHashTree * apTree, U32 aFilterFlags )
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


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_DeInit( sHashTreeVarBlock * apBlock )
* ACTION   : deinits a varblock, currently does nothing
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarBlock_DeInit( sHashTreeVarBlock * apBlock )
{
	(void)apBlock;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_Delocate( sHashTreeVarBlock * apBlock )
* ACTION   : delocates a varblock
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarBlock_Delocate( sHashTreeVarBlock * apBlock )
{
	Endian_FromBigU32( &apBlock->mID );
	Endian_FromBigU32( &apBlock->mVersion );
	Endian_FromBigU32( &apBlock->mTotalDataSize );
	Endian_FromBigU32( &apBlock->mVarCount );

	*(U32*)&apBlock->mpData -= (U32)apBlock;
	*(U32*)&apBlock->mpDataSizes -= (U32)apBlock;
	*(U32*)&apBlock->mpHashes -= (U32)apBlock;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_Relocate( sHashTreeVarBlock * apBlock )
* ACTION   : relocates a varblock
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

void	HashTree_VarBlock_Relocate( sHashTreeVarBlock * apBlock )
{
	*(U32*)&apBlock->mpData += (U32)apBlock;
	*(U32*)&apBlock->mpDataSizes += (U32)apBlock;
	*(U32*)&apBlock->mpHashes += (U32)apBlock;

	Endian_FromBigU32( &apBlock->mID );
	Endian_FromBigU32( &apBlock->mVersion );
	Endian_FromBigU32( &apBlock->mTotalDataSize );
	Endian_FromBigU32( &apBlock->mVarCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashTree_VarBlock_Apply( sHashTreeVarBlock * apBlock, sHashTree * apTree )
* ACTION   : writes to value of all variables referenced in varblock
* CREATION : 28.03.2018 PNK
*-----------------------------------------------------------------------------------*/

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
