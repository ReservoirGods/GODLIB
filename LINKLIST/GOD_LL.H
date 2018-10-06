#ifndef	INCLUDED_GOD_LL_H
#define	INCLUDED_GOD_LL_H


/* ###################################################################################
#  MACROS
################################################################################### */

#define	GOD_LL_INSERT( _apHead, _apNext, _apItem )	\
{							 						\
	if( _apItem )									\
	{												\
		(_apItem)->_apNext	 = _apHead;				\
		_apHead				 = _apItem;				\
	}												\
}

#define	GOD_LL_INSERT_TAIL( aType, aHead, apNext, apItem )	\
{															\
	aType *		lpItem;										\
	aType **	lppLast;									\
															\
	lppLast = &aHead;										\
	lpItem  = *lppLast;										\
															\
	while( lpItem )											\
	{														\
		lppLast = &lpItem->apNext;							\
		lpItem =  *lppLast;									\
	}														\
															\
	*lppLast = apItem;										\
	(apItem)->apNext = 0;									\
}

#define	GOD_LL_REMOVE( aType, aHead, apNext, aItem )	\
{														\
	aType *		lpItem;									\
	aType **	lppLast;								\
														\
	if( aHead && aItem )								\
	{													\
		lppLast = &aHead;								\
		lpItem  = *lppLast;								\
														\
		while( (lpItem) && (lpItem!=aItem) )			\
		{												\
			lppLast = &lpItem->apNext;					\
			lpItem =  *lppLast;							\
		}												\
														\
		if( lpItem )									\
		{												\
			*lppLast = (lpItem)->apNext;				\
			lpItem->apNext = 0;							\
		}												\
	}													\
}

#define	GOD_LL_FIND( aHead, apNext, aField, aValue, aRes )	\
{															\
	aRes = aHead;											\
	while( (aRes) && (aRes->aField!=aValue) )				\
	{														\
		aRes = aRes->apNext;								\
	}														\
}

#define	GOD_LL_MOVE_FORWARD( aType, aHead, apNext, apItem )	\
{															\
	aType *		lpPrev;										\
	aType **	lppStart;									\
															\
	if( apItem )											\
	{														\
		lppStart = &aHead;									\
		lpPrev   = *lppStart;								\
															\
		while( (lpPrev) && (lpPrev->apNext!=apItem) )		\
		{													\
			lppStart = &lpPrev->apNext;						\
			lpPrev   = *lppStart;							\
		}													\
															\
		if( lpPrev )										\
		{													\
			*lppStart      = apItem;						\
			lpPrev->apNext = apItem->mpNext;				\
			apItem->mpNext = lpPrev;						\
		}													\
	}														\
}

#define	GOD_LL_MOVE_BACK( aType, aHead, apNext, apItem )		\
{															\
	aType *		lpItem;										\
	aType **	lppLast;									\
															\
	if( apItem && apItem->apNext )							\
	{														\
		lppLast = &aHead;									\
		lpItem   = *lppLast;								\
															\
		while( (lpItem) && (lpItem!=apItem) )				\
		{													\
			lppLast = &lpItem->apNext;						\
			lpItem   = *lppLast;							\
		}													\
															\
		if( lpItem )										\
		{													\
			lpItem         = apItem->apNext;				\
			*lppLast       = lpItem;						\
			apItem->apNext = lpItem->apNext;				\
			lpItem->apNext = apItem;						\
		}													\
	}														\
}

#define	GOD_LL_MOVE_UP( aType, apParent, apNext, apItem )	\
{															\
	if( apParent && apItem )								\
	{														\
		GOD_LL_REMOVE( aType, apParent, apNext, apItem );	\
		apItem->apNext   = apParent->apNext;				\
		apParent->apNext = apItem;							\
	}														\
}

#define	GOD_LL_MOVE_DOWN( aType, apParent, apChild, apNext, apItem )		\
{																		\
	if( apParent && apItem )											\
	{																	\
		GOD_LL_REMOVE( aType, apParent, apNext, apItem );				\
		GOD_LL_INSERT( aType, apParent->apChild, apNext, apItem );		\
	}																	\
}


/* ################################################################################ */


#endif	//	INCLUDED_GOD_LL_H
