/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"STRLIST.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eSTRINGLIST_FLAG_SERIALISED = (1<<0)
};


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_Init( sStringList * apList )
* ACTION   : StringList_Init
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	StringList_Init( sStringList * apList )
{
	apList->mCount  = 0;
	apList->mpItems = 0;
	apList->mFlags  = 0;
	apList->mpMem   = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_DeInit( sStringList * apList )
* ACTION   : StringList_DeInit
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	StringList_DeInit( sStringList * apList )
{
	sStringListItem *	lpItem;
	sStringListItem *	lpItemNext;
	sString *			lpString;

	if( apList )
	{
		lpItem = apList->mpItems;
		while( lpItem )
		{
			lpItemNext = lpItem->mpNext;
			lpString   = lpItem->mpString;
			if( lpString )
			{
				if( apList->mFlags & eSTRINGLIST_FLAG_SERIALISED )
				{
					lpString->mpChars = 0;
				}
				String_Destroy( lpString );
				mMEMFREE( lpItem );
			}
			lpItem     = lpItemNext;
		}

		if( apList->mpMem )
		{
			mMEMFREE( apList->mpMem );
			apList->mpMem = 0;
		}
		apList->mCount  = 0;
		apList->mpItems = 0;
		apList->mFlags  = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_GetItemCount( sStringList * apList )
* ACTION   : StringList_GetItemCount
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	StringList_GetItemCount( sStringList * apList )
{
	return( apList->mCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_GetStringsSize( sStringList * apList )
* ACTION   : StringList_GetStringsSize
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	StringList_GetStringsSize( sStringList * apList )
{
	U32	lSize;
	sStringListItem *	lpItem;

	lSize = 0;
	if( apList )
	{
		lpItem  = apList->mpItems;
		while( lpItem )
		{
			if( lpItem->mpString )
			{
				lSize  += lpItem->mpString->mCharCount + 1;
			}
			lpItem  = lpItem->mpNext;
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_ItemCreate( sStringList * apList,const char * apChars )
* ACTION   : StringList_ItemCreate
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sStringListItem *	StringList_ItemCreate( sStringList * apList,const char * apChars )
{
	sString *			lpString;
	sStringListItem *	lpItem;

	if( apChars )
	{
		lpString         = String_Create( apChars );
		lpItem           = (sStringListItem*)mMEMCALLOC( sizeof(sStringListItem) );
		lpItem->mpNext   = apList->mpItems;
		lpItem->mpString = lpString;
		apList->mpItems  = lpItem;
		apList->mCount++;
	}
	else
	{
		lpItem = 0;
	}

	return( lpItem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_ItemDestroy( sStringListItem * apItem )
* ACTION   : StringList_ItemDestroy
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	StringList_ItemDestroy( sStringList * apList, sStringListItem * apItem )
{
	sStringListItem **	lppItemLast;
	sStringListItem *	lpItem;
	sStringListItem *	lpItemNext;

	lppItemLast = &apList->mpItems;
	lpItem      = apList->mpItems;
	while( lpItem )
	{
		lpItemNext = lpItem->mpNext;
		if( lpItem == apItem )
		{
			*lppItemLast = lpItem->mpNext;
			if( apList->mFlags & eSTRINGLIST_FLAG_SERIALISED )
			{
				lpItem->mpString->mpChars = 0;
			}
			String_Destroy( lpItem->mpString );
			mMEMFREE( lpItem );
			apList->mCount--;
			return;
		}
		lppItemLast =&lpItem->mpNext;
		lpItem      = lpItemNext;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_Serialise( void )
* ACTION   : StringList_Serialise
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	StringList_StringsSerialise( sStringList * apList )
{
	U32				lSize;

	if( apList )
	{
		lSize         = StringList_GetStringsSize( apList );
		if( lSize )
		{
			apList->mpMem = mMEMCALLOC( lSize );
			StringList_StringsSerialiseTo( apList, (U8*)apList->mpMem );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void StringList_StringsSerialiseTo(sStringList * apList,U8 * apDest)
* ACTION   : StringList_StringsSerialiseTo
* CREATION : 23.06.2004 PNK
*-----------------------------------------------------------------------------------*/

void StringList_StringsSerialiseTo(sStringList * apList,U8 * apDest)
{
	U32					lOff;
	sString *			lpString;
	sStringListItem *	lpItem;


	if( apList )
	{
		lOff          = 0;
		lpItem        = apList->mpItems;

		while( lpItem )
		{

			lpString = lpItem->mpString;
			if( lpString )
			{

				String_StrCpy( (char*)&apDest[ lOff ], lpString->mpChars );
				if( !(apList->mFlags & eSTRINGLIST_FLAG_SERIALISED) )
				{
					mMEMFREE( lpString->mpChars );
				}
				lpString->mpChars = (char*)&apDest[ lOff ];
				lOff += ( lpString->mCharCount + 1 );
			}
			lpItem = lpItem->mpNext;
		}
		apList->mFlags |= eSTRINGLIST_FLAG_SERIALISED;
	}
}


/* ################################################################################ */
