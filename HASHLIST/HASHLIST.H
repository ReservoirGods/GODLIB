#ifndef	INCLUDED_HASHLIST_H
#define	INCLUDED_HASHLIST_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	void	(*fHashListItemCB)( void * apItem );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sHashListItem
{
	U32						mID;
	S32						mRefCount;
	struct sHashListItem *	mpNext;
} sHashListItem;


typedef	struct
{
	U32					mItemCount;
	U32					mItemSize;
	sHashListItem *		mpItems;
	fHashListItemCB		mInitCB;
	fHashListItemCB		mDeInitCB;
} sHashList;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			HashList_Init( sHashList * apList, const U32 aItemSize, fHashListItemCB aInitCB, fHashListItemCB aDeInitCB );
void			HashList_DeInit( sHashList * apList );

U32				HashList_BuildHash( const char * apString );

sHashListItem *	HashList_ItemFind( sHashList * apList, const U32 aID );

sHashListItem *	HashList_ItemRegister(   sHashList * apList, const U32 aID );
void			HashList_ItemUnRegister( sHashList * apList, const U32 aID );

U32				HashList_GetItemIndex( sHashList * apList, sHashListItem * apItem );


/* ################################################################################ */

#endif	/* INCLUDED_HASHLIST_H */
