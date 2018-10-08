#include	"JSON.H"

#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\LEXER\LEXER.H>
#include	<GODLIB\LINKLIST\GOD_LL.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\REFLECT\REFLECT.H>

/*
trick is building array
value , -> property array ++
} , -> prevobject->sibling = new object
], -> new "row" of array

node [root] 
 node [object]
  node [setting..0]
  node [setting..n]
 node [object]
  node [array?]

{ -> build node (object)
[ -> create node (array)
,

*/

#if 0
sObjectJSON *	JSON_Parse(sString * apSrc)
{
	sObjectJSON *	lpObject = 0;
	sPropertyJSON *	lpProperty = 0;
	sLexerContext	lLexer;
	sString			lName;
	U16				lKeyValueIndex = 0;
	U32				lArrayDim = 0;

	

	lLexer.mSource = *apSrc;
	lLexer.mOffset = 0;

	char d = 0;
	while (Lexer_GetNextToken(&lLexer))
	{
		char c = lLexer.mToken.mpChars[0];

		if ('[' == c)
		{
			lArrayDim++;
			continue;
		}
		lArrayDim = 0;



		if (':' == c)
			lKeyValueIndex = 1;
		else if ('{' == c)
		{
			sObjectJSON *	lpObjNew = (sObjectJSON*)mMEMCALLOC(sizeof(sObjectJSON));
			lpObjNew->mpParent = lpObject;
			if (lpObject)
			{
				GOD_LL_INSERT_TAIL(sObjectJSON, lpObject->mpChildObjects, mpSiblingObject, lpObjNew);
			}
			lpObject = lpObjNew;
		}
		else if ('}' == c)
		{
			if( lpObject )
				lpObject = lpObject->mpParent;
		}
		else if ('[' == c)
		{
		}
		else if (']' == c)
		{
		}
		else if (',' == c)
		{
			lKeyValueIndex = 0;
		}
		else
		{
			if (!lKeyValueIndex)
				lName = lLexer.mToken;
			else
			{
				if (',' == d)
				{
					if (lpProperty)
					{
						sValueJSON * lpValue = (sValueJSON*)mMEMCALLOC(sizeof(sValueJSON));
						lpValue->mValue = lLexer.mToken;
						lpValue->mpValueNext = 0;
						GOD_LL_INSERT_TAIL(sValueJSON, lpProperty->mpValues, mpValueNext, lpValue);
					}
				}
				else
				{
					if (lpObject)
					{
						lpProperty = (sPropertyJSON*)mMEMCALLOC(sizeof(sPropertyJSON));
						lpProperty->mName = lName;
						GOD_LL_INSERT_TAIL(sPropertyJSON, lpObject->mpProperties, mpPropertyNext, lpProperty);
					}
				}
			}
		}
		d = c;
	}
	return(lpObject);
}

void			JSON_Destroy(sObjectJSON * apJSON)
{
	sObjectJSON * lpObject = apJSON;
	while (lpObject)
	{
		sObjectJSON * lpNext = lpObject->mpSiblingObject;
		sPropertyJSON *lpProp = lpObject->mpProperties;

		while (lpProp)
		{
			sPropertyJSON *lpPropNext = lpProp->mpPropertyNext;
			sValueJSON * lpValue = lpProp->mpValues;
			while (lpValue)
			{
				sValueJSON * lpValueNext = lpValue->mpValueNext;
				String_DeInit(&lpValue->mValue);
				mMEMFREE(lpValue);
				lpValue = lpValueNext;
			}
			String_DeInit(&lpProp->mName);
			mMEMFREE(lpProp);
			lpProp = lpPropNext;
		}

		if (lpObject->mpChildObjects)
			JSON_Destroy(lpObject->mpChildObjects);

		mMEMFREE(lpObject);
		lpObject = lpNext;
	}
}

#endif

/*
{              [0] cretea anonymous object[0]
 "field0" :    [1] (child of [0])
  "value",     [2] (child of [1]) , -> go back up to [1]
 "field1" :    [3] (child of [0], sibling of[1])
   "value2",   [4] (child of [3] , ->go back up to [1] 
 "array" :     [5] (child of [1]
  [            [6] (child of [5]
   6,          [7] (child of [6] , stay at this level

 ],

file
 collection of objects
  object:
   collection of properties
    property
     array of objects

comma operator

 "property" : [ "value", (value2) <- subling link
 "property" : "value",  (property2)  <- 
   is parent of prev [ ?
[
 {
 }, 

*/

sNodeJSON *	JSON_Parse(sString * apSrc)
{
	sNodeJSON * lpRoot = 0;
	sNodeJSON * lpPrev = 0;
	sLexerContext	lLexer;

	sElementCollectionJSON lCol;
	JSON_Parse2( apSrc, &lCol );
	JSON_Destroy2( &lCol );

	char d = 0;
	char c = 0;
	Lexer_Init( &lLexer, apSrc );
	while (Lexer_GetNextToken(&lLexer))
	{
		d = c;
		c = lLexer.mToken.mpChars[0];

		if( '}' == c || ']' == c )
		{
			d = c - 2;
			for( ;lpPrev; lpPrev=lpPrev->mpParent )
			{
				if( d == lpPrev->mName.mpChars[ 0 ] )
				{
					lpPrev = lpPrev->mpParent;
					break;
				}
			}
			continue;
		}

		if (':' == c || ',' == c)
			continue;

		sNodeJSON * lpNodeNew = (sNodeJSON*)mMEMCALLOC(sizeof(sNodeJSON));
		if (!lpRoot)
			lpRoot = lpNodeNew;

		if( ',' == d )
		{
			if (lpPrev && lpPrev->mpParent)
			{
				if( '[' == lpPrev->mName.mpChars[ 0 ] || '{' == lpPrev->mName.mpChars[ 0 ] )
				{
					lpNodeNew->mpParent = lpPrev;
					GOD_LL_INSERT_TAIL( sNodeJSON, lpPrev->mpChildren->mpSibling, mpSibling, lpNodeNew );
				}
				else if( '[' == lpPrev->mpParent->mName.mpChars[ 0 ]  || '{' == lpPrev->mpParent->mName.mpChars[ 0 ] )
				{
					lpNodeNew->mpParent = lpPrev->mpParent;
					lpPrev->mpSibling = lpNodeNew;
				}
				else 
				{
					lpNodeNew->mpParent = lpPrev->mpParent->mpParent;
					lpPrev->mpParent->mpSibling = lpNodeNew;
				}
			}
		}
		else
		{
			lpNodeNew->mpParent = lpPrev;
			if( lpPrev && !lpPrev->mpChildren )
				lpPrev->mpChildren = lpNodeNew;
		}
		lpPrev = lpNodeNew;

		String_SetStatic( &lpPrev->mName, lLexer.mToken.mpChars, String_GetLength(&lLexer.mToken) );

//		arrayFlag = ( lpPrev && lpPrev->mName.mpChars && lpPrev->mName.mpChars[ 0 ] == '[' ) ? 1 : 0;

	}
	return(lpRoot);
}

void		JSON_Destroy(sNodeJSON * apJSON)
{
	while (apJSON)
	{
		sNodeJSON * lpNext = apJSON->mpSibling;
		String_DeInit(&apJSON->mName);
		JSON_Destroy(apJSON->mpChildren);
		mMEMFREE(apJSON)
		apJSON = lpNext;
	}
}

static const char * gpBlanks = "                                ";

void		ShowMe(const sString * apString )
{
	DebugLog_Printf0( apString->mpChars );
}

void		JSON_DebugNodeShow( const sNodeJSON * apNode, U32 aDepth )
{
	(void)apNode;
	(void)aDepth;
	if( apNode )
	{
		const char * lpB = gpBlanks + 32 - aDepth;
		const sNodeJSON * lpNode = apNode;
		DebugLog_Printf0( "\n" );
		
		while( lpNode )
		{
			DebugLog_Printf0( lpB );
			String_Action( &lpNode->mName, ShowMe );
			DebugLog_Printf0( "\n" );
			JSON_DebugNodeShow( lpNode->mpChildren, aDepth + 1 );
			lpNode = lpNode->mpSibling;
		}
	}
}
void		JSON_DebugShow( const sNodeJSON * apJSON )
{
	JSON_DebugNodeShow( apJSON, 0 );
}


U32			JSON_GetNodeCount( const sNodeJSON * apJSON )
{
	U32 count = 0;
	if( apJSON )
	{
		for( ;apJSON; apJSON=apJSON->mpSibling )
		{
			count += JSON_GetNodeCount( apJSON->mpChildren );
			count++;
		}
	}
	return count;
}

const sNodeJSON *	JSON_GetNode( const sNodeJSON * apJSON, const char * apPath )
{
	const sNodeJSON * lpJSON = apJSON;
	sString nodeName;
	U32 len;

	for( len = 0; apPath[ len ] && '/' != apPath[ len ] && '\\' != apPath[ len ]; len++ );
	String_Init( &nodeName, 0 );
	String_SetStatic( &nodeName, apPath, len );

	for( ; lpJSON; lpJSON = lpJSON->mpSibling )
	{
		if( '{' == lpJSON->mName.mpChars[ 0 ] || '[' == lpJSON->mName.mpChars[0] )
		{
			const sNodeJSON * lpRes = JSON_GetNode( apJSON, apPath );
			if( lpRes )
				return lpRes;
		}
		if( String_IsEqual( &nodeName, &lpJSON->mName ) )
		{
			if( '/' != apPath[ len ] || '\\' != apPath[ len ] )
				len++;
			if( 0 == apPath[ len ] )
				return( lpJSON );
			return( JSON_GetNode( apJSON, apPath + len ) );
		}
	}
	return( 0 );
}

U32		JSON_NodeGetStringSize( const sNodeJSON * apJSON )
{
	U32 size = 0;
	if( apJSON )
	{
		size += String_GetLength( &apJSON->mName ) + 1;
		if( '{' == apJSON->mName.mpChars[ 0 ] || '[' == apJSON->mName.mpChars[ 0 ] )
			size += 2;
		for( ;apJSON; apJSON = apJSON->mpSibling )
		{
			size += JSON_NodeGetStringSize( apJSON->mpChildren );
		}
	}
	return size;
}

void	JSON_NodeSerialiseStringAppend( const sString * apSrc, sString * apDst, const U32 * apOffset )
{
	U32 len = String_GetLength( apSrc );
	U32 i;

	if( *apOffset && *apOffset < String_GetLength( apDst ) )
		apDst->mpChars[ *apOffset++ ] = ' ';

	for( i = 0; i < len; i++ )
	{
		if( *apOffset < String_GetLength(apDst) )
			apDst->mpChars[ *apOffset++ ] = apSrc->mpChars[ i ];
	}
	
}

void	JSON_NodeSerialise( const sNodeJSON * apJSON, sString * apDst, const U32 * apOffset )
{
	if( apJSON )
	{
		JSON_NodeSerialiseStringAppend( &apJSON->mName, apDst, apOffset );

		for( ;apJSON; apJSON = apJSON->mpSibling )
		{
			JSON_NodeSerialiseStringAppend( &apJSON->mName, apDst, apOffset );
			JSON_NodeSerialise( apJSON->mpChildren, apDst, apOffset );
		}
	}
}


void				JSON_NodesToString( const sNodeJSON * apJSON, sString * apSrc )
{
	U32 size = JSON_NodeGetStringSize( apJSON );
	String_Init( apSrc, 0 );
	if( size )
	{
		String_Alloc( apSrc, size );
	}
}

char * gpTestStrings[] =
{
	"blah",
	"bloo",
	"blee"
};
typedef struct sBuffer2
{
	void *	mpData;
	U32		mElementSizeof;
	U32		mElementCount;
}sBuffer2;

#define	mGOD_BUFFER( aType, aName )				\
	aType gpGodBuffer##aName##Data[];			\
	sBuffer2 aName= { gpGodBuffer##aName##Data, sizeof(aType), mARRAY_COUNT(gpGodBuffer##aName##Data) };		\
	aType gpGodBuffer##aName##Data[] =


void	JSON_Parse2( sString * apSrc, sElementCollectionJSON  * apCollection )
{
	U32				elementCount = 0;
	U32				index = 0;
	U16				depth = 0;
	sElementJSON *	pElements = 0;
	sLexerContext	lexer;

	Lexer_Init( &lexer, apSrc );
	while( Lexer_GetNextToken( &lexer ) )
		elementCount++;
//	Lexer_DeInit( &lexer );

	if( elementCount )
	{
		pElements = mMEMCALLOC( sizeof( sElementJSON ) * elementCount );

		Lexer_Init( &lexer, apSrc );
		while( Lexer_GetNextToken( &lexer ) )
			pElements[ index++ ].mToken = lexer.mToken;
//		Lexer_DeInit( &lexer );

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
					else if( ',' == pElements[ i2 ].mToken.mpChars[ 0 ] || '}' == pElements[ i2 ].mToken.mpChars[ 0 ] )
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

void	JSON_Destroy2( sElementCollectionJSON * apCollection )
{
	mMEMFREE( apCollection->mpElements )
}

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

void	JSON_ElementsTrim( const sElementCollectionJSON * apCollection )
{
	U32 i;
	for( i = 0; i < apCollection->mCount; i++ )
	{
		String_QuoteTrim( &apCollection->mpElements[ i ].mToken );
	}
}


void	JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection )
{
	apWalker->mIndex = 0;
	String_Init( &apWalker->mObjectName, 0 );
	apWalker->mpCollection = apCollection;
}
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

