/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"HASHLIST.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_Init( sHashList * apList,const U32 aItemSize,fHashListItemCB aInitCB,fHashListItemCB aDeInitCB )
* ACTION   : HashList_Init
* CREATION : 10.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	HashList_Init( sHashList * apList,const U32 aItemSize,fHashListItemCB aInitCB,fHashListItemCB aDeInitCB )
{
	apList->mItemCount = 0;
	apList->mItemSize  = aItemSize;
	apList->mpItems    = 0;
	apList->mInitCB    = aInitCB;
	apList->mInitCB    = aDeInitCB;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_DeInit( sHashList * apList )
* ACTION   : HashList_DeInit
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	HashList_DeInit( sHashList * apList )
{
	sHashListItem *	lpItemNext;
	sHashListItem *	lpItem;

	lpItem = apList->mpItems;

	while( lpItem )
	{
		lpItemNext = lpItem->mpNext;
		if( apList->mDeInitCB )
		{
			apList->mDeInitCB( lpItem );
		}
		mMEMFREE( lpItem );
		lpItem     = lpItemNext;
	}

	apList->mItemCount = 0;
	apList->mpItems    = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_BuildHash( const char * apString )
* ACTION   : HashList_BuildHash
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	HashList_BuildHash( const char * apString )
{
	U32		lHash;
	U32		lTemp;
	char	lC;

	lHash = 0;
	while ( *apString )
	{
		lC    = *apString++;
		if( (lC >= 'a') && (lC <='z') )
		{
			lC += ( 'A'-'a' );
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
* FUNCTION : HashList_ItemFind( sHashList * apList,const U32 aID )
* ACTION   : HashList_ItemFind
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

sHashListItem *	HashList_ItemFind( sHashList * apList,const U32 aID )
{
	sHashListItem *	lpItem;

	lpItem = apList->mpItems;
	while( lpItem )
	{
		if( lpItem->mID == aID )
		{
			return( lpItem );
		}
		lpItem = lpItem->mpNext;
	}
	return( lpItem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_ItemRegister( sHashList * apList,const U32 aID )
* ACTION   : HashList_ItemRegister
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

sHashListItem *	HashList_ItemRegister( sHashList * apList,const U32 aID )
{
	sHashListItem *	lpItem;

	lpItem = HashList_ItemFind( apList, aID );

	if( lpItem )
	{
		lpItem->mRefCount++;
	}
	else
	{
		lpItem = (sHashListItem*)mMEMCALLOC( apList->mItemSize );
		if( lpItem )
		{
			lpItem->mID       = aID;
			lpItem->mpNext    = apList->mpItems;
			lpItem->mRefCount = 1;

			apList->mpItems   = lpItem;
			apList->mItemCount++;

			if( apList->mInitCB )
			{
				apList->mInitCB( lpItem );
			}
		}
	}

	return( lpItem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_ItemUnRegister( sHashList * apList,const U32 aID )
* ACTION   : HashList_ItemUnRegister
* CREATION : 09.12.2003 PNK
*-----------------------------------------------------------------------------------*/

void	HashList_ItemUnRegister( sHashList * apList,const U32 aID )
{
	sHashListItem *	lpItemLast;
	sHashListItem *	lpItem;

	lpItem     = apList->mpItems;
	lpItemLast = 0;

	while( lpItem )
	{
		if( lpItem->mID == aID )
		{
			lpItem->mRefCount--;
			if( lpItem->mRefCount <= 0 )
			{
				if( lpItemLast )
				{
					lpItemLast->mpNext = lpItem->mpNext;
				}
				else
				{
					apList->mpItems = lpItem->mpNext;
				}
				if( apList->mDeInitCB )
				{
					apList->mDeInitCB( lpItem );
				}
				mMEMFREE( lpItem );
				apList->mItemCount--;
			}
		}
		lpItemLast = lpItem;
		lpItem     = lpItem->mpNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : HashList_GetItemIndex( sHashList * apList,sHashListItem * apItem )
* ACTION   : HashList_GetItemIndex
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	HashList_GetItemIndex( sHashList * apList,sHashListItem * apItem )
{
	U32				lIndex;
	sHashListItem *	lpItem;

	lIndex = 0;

	if( apList )
	{
		lpItem = apList->mpItems;
		while( (lpItem) && (lpItem != apItem) )
		{
			lIndex++;
			lpItem = lpItem->mpNext;
		}
	}
	return( lIndex );
}


/* ################################################################################ */
