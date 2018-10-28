
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: JSON.C
::
:: JSON Parser
::
:: [c] 2018 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"JSON.H"

#include	<GODLIB\ASSERT\ASSERT.H>
#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\LEXER\LEXER.H>
#include	<GODLIB\LINKLIST\GOD_LL.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\REFLECT\REFLECT.H>


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_Parse( sString * apSrc, sElementCollectionJSON  * apCollection )
* ACTION   : parses JSON text file into an array of elements
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_Parse( sString * apSrc, sElementCollectionJSON  * apCollection )
{
	U32				elementCount = 0;
	U32				index = 0;
	U16				depth = 0;
	sElementJSON *	pElements = 0;
	sLexerContext	lexer;

	Lexer_Init( &lexer, apSrc );
	while( Lexer_GetNextToken( &lexer ) )
		elementCount++;
/*	Lexer_DeInit( &lexer );*/

	if( elementCount )
	{
		pElements = mMEMCALLOC( sizeof( sElementJSON ) * elementCount );

		Lexer_Init( &lexer, apSrc );
		while( Lexer_GetNextToken( &lexer ) )
			pElements[ index++ ].mToken = lexer.mToken;
/*		Lexer_DeInit( &lexer );*/

		for( index = 0; index < elementCount;index++ )
		{
			char c = pElements[ index ].mToken.mpChars[ 0 ];
			pElements[ index ].mDepth = depth;
			if( '{' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ObjectBegin;
				depth++;
			}
			else if( '[' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ArrayBegin;
				depth++;

			}
			else if( '}' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ObjectEnd;
				depth--;
				pElements[ index ].mDepth = depth;
			}
			else if( ']' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ArrayEnd;
				depth--;
				pElements[ index ].mDepth = depth;
			}
			else if( ':' == c || ',' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_Seperator;
			}
			else
			{
				U32 i2;
				for( i2 = index + 1; i2 < elementCount; i2++ )
				{
					if( ':' == pElements[ i2 ].mToken.mpChars[ 0 ] || '[' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
					}
					else if( '{' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_ObjectName;
						break;
					}
					else if( ',' == pElements[ i2 ].mToken.mpChars[ 0 ] || '}' == pElements[ i2 ].mToken.mpChars[ 0 ] || ']' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_PropertyValue;
						break;
					}
					else if( ':' != pElements[ i2 ].mToken.mpChars[ 0 ] && '[' != pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_PropertyName;
						break;
					}
				}
			}

		}
	}

	apCollection->mCount = elementCount;
	apCollection->mpElements = pElements;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_Destroy( sElementCollectionJSON * apCollection )
* ACTION   : release memory associated with JSON parsing
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_Destroy( sElementCollectionJSON * apCollection )
{
	mMEMFREE( apCollection->mpElements )
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementsTrim( const sElementCollectionJSON * apCollection )
* ACTION   : removes quotes from JSON keys/values
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementsTrim( const sElementCollectionJSON * apCollection )
{
	U32 i;
	for( i = 0; i < apCollection->mCount; i++ )
	{
		String_QuoteTrim( &apCollection->mpElements[ i ].mToken );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementsToStruct( const sElementCollectionJSON * apCollection, const struct sReflectType * apType, U32 * apIndex, void * apDest )
* ACTION   : convertes JSON elements into a concrete data structure based on TYPE system
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementsToStruct( const sElementCollectionJSON * apCollection, const struct sReflectType * apType, U32 * apIndex, void * apDest )
{
	U16 depth = apCollection->mpElements[ *apIndex ].mDepth;
	sString * lpPropName = 0;

	for( ; *apIndex < apCollection->mCount; (*apIndex)++ )
	{
		sElementJSON * lpE = &apCollection->mpElements[ *apIndex ];
		if( ( eTypeJSON_ObjectEnd == lpE->mTypeFlags ) && ( depth == lpE->mDepth ) )
			break;

		if( eTypeJSON_PropertyName == lpE->mTypeFlags )
			lpPropName = &lpE->mToken;

		if( eTypeJSON_PropertyValue == lpE->mTypeFlags && lpPropName )
			Reflect_SetData( apType, lpPropName, &lpE->mToken, apDest );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection )
* ACTION   : helper function to walk a JSON structure
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection )
{
	apWalker->mIndex = 0;
	String_Init( &apWalker->mObjectName, 0 );
	apWalker->mpCollection = apCollection;
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker )
* ACTION   : retrieves next object from JSON structure
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

U8		JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker )
{
	apWalker->mIndex++;
	for( ;apWalker->mIndex < apWalker->mpCollection->mCount; apWalker->mIndex++ )
	{
		const sElementJSON * lpE = &apWalker->mpCollection->mpElements[ apWalker->mIndex ];
		if( eTypeJSON_ObjectBegin == lpE->mTypeFlags )
			return 1;
		if( eTypeJSON_ObjectName == lpE->mTypeFlags )
			apWalker->mObjectName = lpE->mToken;
	}
	return 0;
}

U32		JSON_GetObjectCount( const sElementCollectionJSON * apCollection, const char * apObjectName )
{
	U32 count = 0;
	U32 i;
	U8	found = 0;

	sString lObjName;
	String_Init( &lObjName, apObjectName );

	for( i = 0; i <apCollection->mCount; i++ )
	{
		const sElementJSON * lpE = &apCollection->mpElements[ i ];
		if( eTypeJSON_ObjectName == lpE->mTypeFlags )
			found = String_IsEqual( &lpE->mToken, &lObjName );
		else if( found && ( eTypeJSON_ObjectBegin == lpE->mTypeFlags ) )
		{
			if( i && eTypeJSON_ArrayBegin == apCollection->mpElements[ i - 1 ].mTypeFlags )
			{
				U32 depth = apCollection->mpElements[ i - 1 ].mDepth;
				for( ; i < apCollection->mCount; i++ )
				{
					if( ( eTypeJSON_ObjectEnd == apCollection->mpElements[ i ].mTypeFlags ) && ( lpE->mDepth == apCollection->mpElements[ i ].mDepth ) )
						count++;
					if( ( eTypeJSON_ArrayEnd == apCollection->mpElements[ i ].mTypeFlags ) && ( depth == apCollection->mpElements[ i ].mDepth ) )
						break;
				}

			}
			else
			{
				for( ; i < apCollection->mCount; i++ )
					if( ( eTypeJSON_ObjectEnd == apCollection->mpElements[ i ].mTypeFlags ) && ( lpE->mDepth == apCollection->mpElements[ i ].mDepth ) )
						break;
				count++;
			}
			found = 0;
		}
	}
	return count;
}

U32		JSON_GetValueCount( const sElementCollectionJSON * apCollection, const char * apObjectName, const char * apPropertyName )
{
	U32 count = 0;

	(void)apCollection;
	(void)apObjectName;
	(void)apPropertyName;

	return count;
}

sObjectJSON	* JSON_TreeCreate( const sElementCollectionJSON * apCollection )
{
	U32 jsonTypeCounts[ eTypeJSON_LIMIT ];
	U32 i;
	U8 * pMem = 0;
	U16 depthMax = 0;
	U32 objSizeBytes = 0;
	U32 propSizeBytes = 0;
	U32 valueSizeBytes = 0;
	sObjectJSON * pTree =0;
	

	Memory_Clear( sizeof( jsonTypeCounts ), jsonTypeCounts );

	for( i=0; i<apCollection->mCount; i++ )
	{
		const sElementJSON * lpE = &apCollection->mpElements[ i ];
		jsonTypeCounts[ lpE->mTypeFlags ]++;
		if( lpE->mDepth > depthMax )
			depthMax = lpE->mDepth;
	}


	objSizeBytes   = (jsonTypeCounts[ eTypeJSON_ObjectBegin ]+1) * sizeof(sObjectJSON);
	propSizeBytes  = jsonTypeCounts[ eTypeJSON_PropertyName ] * sizeof(sPropertyJSON);
	valueSizeBytes = jsonTypeCounts[ eTypeJSON_PropertyValue ] * sizeof(sTagString);

	pMem = mMEMCALLOC(objSizeBytes + propSizeBytes + valueSizeBytes);
	if( pMem )
	{
		U32	objCursor = 0;
		U32 jsonTypeIndex[ eTypeJSON_LIMIT ];
		sObjectJSON * pObjects = (sObjectJSON*)(&pMem[0]);
		sObjectJSON * pObjCurrent = 0;
		sPropertyJSON * pProps = (sPropertyJSON*)(&pMem[objSizeBytes]);
		sPropertyJSON * pPropCurrent = 0;
		sString * pValues = (sString*)(&pMem[objSizeBytes+propSizeBytes]);
		const sString * pObjName = 0;
		sObjectJSON ** ppObjStack = 0;

		Memory_Clear( sizeof( jsonTypeIndex ), jsonTypeIndex );
		ppObjStack = mMEMCALLOC( sizeof(sObjectJSON*) * (depthMax+1) );

		ppObjStack[ 0 ] = pObjects;
		pObjCurrent = pObjects;

		pTree = (sObjectJSON*)pMem;
		for( i=0; i<apCollection->mCount; i++ )
		{
			const sElementJSON * lpE = &apCollection->mpElements[ i ];
			if( eTypeJSON_ObjectName== lpE->mTypeFlags )
			{
				pObjName = &lpE->mToken;
			}
			else if( eTypeJSON_ObjectBegin== lpE->mTypeFlags )
			{
				pObjCurrent = pObjects;
				if( objCursor )
				{
					sObjectJSON * pParent = ppObjStack[ objCursor - 1 ];
					sObjectJSON * sib = pParent->mpChildren;
					if( sib )
					{
						for( ;sib->mpSibling; sib = sib->mpSibling );
						GODLIB_ASSERT( sib != pObjects );
						sib->mpSibling = pObjects;
					}
					else
						pParent->mpChildren = pObjects;
				}
				if( pObjName )
				{
					pObjCurrent->mObjectName = *pObjName;
				}
				else
				{
					String_Init( &pObjCurrent->mObjectName, "");
				}
				ppObjStack[ objCursor++ ] = pObjCurrent;
				pObjects++;
			}
			else if( eTypeJSON_PropertyName== lpE->mTypeFlags )
			{
				sPropertyJSON * pParentProp;
				pObjCurrent->mPropertyCount++;
				pParentProp = pObjCurrent->mpProperties;
				if( pParentProp )
				{
					for( ;pParentProp->mpSibling; pParentProp = pParentProp->mpSibling);
					GODLIB_ASSERT( pParentProp != pProps );
					pParentProp->mpSibling = pProps;
				}
				else
					pObjCurrent->mpProperties = pProps;
				pProps->mPropertyName = lpE->mToken;
				pProps->mpValues = pValues;
				pPropCurrent = pProps;
				pProps++;
			}
			else if( eTypeJSON_PropertyValue== lpE->mTypeFlags )
			{
				if( pPropCurrent )
					pPropCurrent->mValueCount++;
				*pValues = lpE->mToken;
				pValues++;
			}
			else if( eTypeJSON_ObjectEnd== lpE->mTypeFlags )
			{
				objCursor--;
				pObjCurrent = ppObjStack[objCursor];
				pObjName = &pObjCurrent->mObjectName;
			}
		}
		mMEMFREE(ppObjStack);
		GODLIB_ASSERT( (pObjects+1) == (sObjectJSON*)( &pMem[ objSizeBytes ] ) );
		GODLIB_ASSERT( pProps == (sPropertyJSON*)( &pMem[ objSizeBytes + propSizeBytes ] ) );
	}


	return pTree;
}

U32				JSON_Tree_GetObjectCount( const sObjectJSON * apTree, const char * apObjectName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( String_IsEqualNT( &apTree->mObjectName, apObjectName ))
			count++;
		count += JSON_Tree_GetObjectCount( apTree->mpChildren, apObjectName );
	}
	return count;
}


U32				JSON_Tree_GetPropertyCount( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( String_IsEqualNT( &apTree->mObjectName, apObjectName ))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling )
			{
				if( String_IsEqualNT( &prop->mPropertyName, apPropertyName))
					count++;
			}
		}
		count += JSON_Tree_GetPropertyCount( apTree->mpChildren, apObjectName, apPropertyName );
	}
	return count;
}

U32				JSON_Tree_GetObjectPropertyCount( const sObjectJSON * apTree, const char * apObjectName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( String_IsEqualNT( &apTree->mObjectName, apObjectName ))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling)
				count++;
		}
		count += JSON_Tree_GetObjectPropertyCount( apTree->mpChildren, apObjectName );
	}
	return count;
}

U32				JSON_Tree_GetPropertyValueCount( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( String_IsEqualNT( &apTree->mObjectName, apObjectName ))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling )
			{
				if( String_IsEqualNT( &prop->mPropertyName, apPropertyName))
					count += prop->mValueCount;
			}
		}
		count += JSON_Tree_GetPropertyValueCount( apTree->mpChildren, apObjectName, apPropertyName );
	}
	return count;
}

U32				JSON_Tree_GetPropertyTextSize( const sObjectJSON * apTree, const char * apPropertyName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		sPropertyJSON * prop = apTree->mpProperties;
		for( ;prop; prop=prop->mpSibling )
		{
			if( String_IsEqualNT( &prop->mPropertyName, apPropertyName))
			{
				U32 i;
				for( i=0; i<prop->mValueCount; i++ )			
				{
					count += String_GetCharCount( &prop->mpValues[i] );
				}
			}
		}
		count += JSON_Tree_GetPropertyTextSize( apTree->mpChildren, apPropertyName );
	}
	return count;
}

U32				JSON_Tree_GetObjectPropertyTextSize( const sObjectJSON * apTree, const char * apObjectName )
{
	U32 count = 0;
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( String_IsEqualNT( &apTree->mObjectName, apObjectName ))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling)
			{
				U32 i;
				for( i=0; i<prop->mValueCount; i++ )			
				{
					count += String_GetCharCount( &prop->mpValues[i] );
				}
			}
		}
		count += JSON_Tree_GetObjectPropertyTextSize( apTree->mpChildren, apObjectName );
	}
	return count;
}


sTreeCollectorJSON *	JSON_Tree_Collect_Internal( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName, sTreeCollectorJSON * apCol )
{
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( !apObjectName || (String_IsEqualNT( &apTree->mObjectName, apObjectName)))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling)
			{
				if( !apPropertyName || (String_IsEqualNT( &prop->mPropertyName, apPropertyName)))
				{
					U32 i;
					for( i=0; i<prop->mValueCount; i++ )			
					{
						apCol->mTextSize += String_GetCharCount( &prop->mpValues[i] );
					}
					apCol->mValueCount += prop->mValueCount;
					apCol->mPropertyCount++;
				}
			}
			apCol->mObjectCount++;

		}
		JSON_Tree_Collect_Internal( apTree->mpChildren, apObjectName, apPropertyName, apCol );
	}
	return apCol;
}

sTreeCollectorJSON *	JSON_Tree_Collect( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName, sTreeCollectorJSON * apCol )
{
	apCol->mObjectCount = 0;
	apCol->mPropertyCount = 0;
	apCol->mTextSize = 0;
	apCol->mValueCount = 0;
	JSON_Tree_Collect_Internal( apTree, apObjectName, apPropertyName, apCol );
	return apCol;
}

void	JSON_Tree_Callback( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName, fJSON aCallback, void * apContext )
{
	for( ;apTree; apTree = apTree->mpSibling )
	{
		if( !apObjectName || (String_IsEqualNT( &apTree->mObjectName, apObjectName)))
		{
			sPropertyJSON * prop = apTree->mpProperties;
			for( ;prop; prop=prop->mpSibling)
			{
				if( !apPropertyName || (String_IsEqualNT( &prop->mPropertyName, apPropertyName)))
				{
					aCallback( apTree, prop, apContext );
				}
			}
		}
		JSON_Tree_Callback( apTree->mpChildren, apObjectName, apPropertyName, aCallback, apContext );
	}

}

sPropertyJSON *	JSON_Tree_GetpProperty( const sObjectJSON * apTree, const char * apPropName )
{
	sPropertyJSON * prop = apTree->mpProperties;
	for( ;prop && !String_IsEqualNT(&prop->mPropertyName, apPropName); prop=prop->mpSibling);
	return prop;
}

void		JSON_TreeDestroy( sObjectJSON * apJSON )
{
	mMEMFREE( apJSON );
}


/* ################################################################################ */