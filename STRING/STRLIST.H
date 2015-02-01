#ifndef	INCLUDED_STRLIST_H
#define	INCLUDED_STRLIST_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sStringListItem
{
	sString *					mpString;
	struct	sStringListItem *	mpNext;
} sStringListItem;

typedef	struct	sStringList
{
	U32					mCount;
	U32					mFlags;
	sStringListItem *	mpItems;
	void *				mpMem;
} sStringList;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void				StringList_Init( sStringList * apList );
void				StringList_DeInit( sStringList * apList );
U32					StringList_GetItemCount( sStringList * apList );
U32					StringList_GetStringsSize( sStringList * apList );

sStringListItem *	StringList_ItemCreate( sStringList * apList, const char * apChars );
void				StringList_ItemDestroy( sStringList * apList, sStringListItem * apItem );

void				StringList_StringsSerialise( sStringList * apList );
void				StringList_StringsSerialiseTo( sStringList * apList, U8 * apDest );


/* ################################################################################ */

#endif	/* INCLUDED_STRLIST_H */
