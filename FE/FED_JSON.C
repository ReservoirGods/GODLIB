/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FED_JSON.H"

#include	"FED.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/LEXER/JSON.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/REFLECT/REFLECT.H>
#include	<GODLIB/STRING/STRPATH.H>

/* ###################################################################################
#  CODE
################################################################################### */

enum
{
	eFedObject_None,
	eFedObject_Controls,
	eFedObject_Layout,
	eFedObject_Page,

	eFedObject_LIMIT
};

enum
{
	eFedControl_None,
	eFedControl_Action,
	eFedControl_BackLink,
	eFedControl_Link,
	eFedControl_Selector,
	eFedControl_Toggle,
	eFedControl_LIMIT
};

sTagString	gFedObjectTagStrings[ eFedObject_LIMIT ] =
{
	{ eFedObject_None, "" },
	{ eFedObject_Controls, "controls" },
	{ eFedObject_Layout, "layout" },
	{ eFedObject_Page, "page" },
};

sTagString gFedControlTagStrings[ eFedControl_LIMIT ] = 
{
	{ eFedControl_None,		""			},
	{ eFedControl_Action,	"action"	},
	{ eFedControl_BackLink,	"backlink"	},
	{ eFedControl_Link,		"link"		},
	{ eFedControl_Selector,	"selector"	},
	{ eFedControl_Toggle,	"toggle"	},
};


typedef struct sFedJSON_Layout
{
	S32	x0;
	S32	y0;
	S32	x1;
	S32	y1;
} sFedJSON_Layout;


sReflectElement		gFedJSON_Layout_Elements[] =
{
	{ "S32", "x0", mOFFSET( sFedJSON_Layout, x0) },
	{ "S32", "y0", mOFFSET( sFedJSON_Layout, y0 ) },
	{ "S32", "x1", mOFFSET( sFedJSON_Layout, x1 ) },
	{ "S32", "y1", mOFFSET( sFedJSON_Layout, y1 ) },
};

sReflectType		gFedJSON_Layout_Type =
{
	"sFedJSON_Layout",
	sizeof( sFedJSON_Layout ),
	mARRAY_COUNT( gFedJSON_Layout_Elements ),
	0,
	gFedJSON_Layout_Elements,
	0
};


sReflectDictionary	gFedJSON_Dictionary = { 1, &gFedJSON_Layout_Type };


/* ###################################################################################
#  CODE
################################################################################### */

#define FEDJSON_ARRANGE( aType ) apHeader->mp##aType##s = (sFed##aType*)&pMem[ offset ]; offset += sizeof(sFed##aType) * apHeader->m##aType##Count;

U32	FedJSON_MemoryArrange( sFedHeader * apHeader, U32 aSize )
{
	U8 * pMem = (U8*)apHeader;
	U32 offset = 0;

	(void)aSize;

	FEDJSON_ARRANGE( Asset );
	FEDJSON_ARRANGE( Call );
	FEDJSON_ARRANGE( ControlList );
	FEDJSON_ARRANGE( FontGroup );
	FEDJSON_ARRANGE( List );
	FEDJSON_ARRANGE( ListItem );
	FEDJSON_ARRANGE( Lock );
	FEDJSON_ARRANGE( Page );
	FEDJSON_ARRANGE( PageStyle );
	FEDJSON_ARRANGE( Sample );
	FEDJSON_ARRANGE( Slider );
	FEDJSON_ARRANGE( Sprite );
	FEDJSON_ARRANGE( SpriteGroup );
	FEDJSON_ARRANGE( SpriteList );
	FEDJSON_ARRANGE( Text );
	FEDJSON_ARRANGE( Transition );
	FEDJSON_ARRANGE( Var );

	return offset;
}

sFedHeader *		FedJSON_ParseText( const char * apText, const U32 aSize )
{
	sFedHeader	lHeader = {0};
	sFedHeader * pHeader = 0;
	sString jsonString;
	sElementCollectionJSON jsonElements;
	const sTagString * pObjectTag = 0;
	const sTagString * pControlTag = 0;
	U32 objectTypeCounts[ eFedObject_LIMIT ];
	U32 controlTypeCounts[ eFedControl_LIMIT ];
	U32 controlContentsCounts[ eFedControl_LIMIT ];
	U32 i;
	U8 * pMem = 0;
	U32 size = 0;

	Memory_Clear( sizeof(objectTypeCounts), objectTypeCounts );
	Memory_Clear( sizeof(controlTypeCounts), controlTypeCounts );
	Memory_Clear( sizeof(controlContentsCounts), controlContentsCounts );

	String_Init( &jsonString, 0 );
	String_SetStatic( &jsonString, apText, aSize );
	JSON_Parse( &jsonString, &jsonElements );
	JSON_ElementsTrim( &jsonElements );

	for( i = 1; i < eFedObject_LIMIT; i++ )
	{
/*		objectTypeCounts[ i ] = JSON_GetObjectCount( &jsonElements, gFedObjectTagStrings[ i ].pString );*/

	}

	{
		sObjectJSON * pTree = JSON_TreeCreate( &jsonElements );
		sTreeCollectorJSON collector;
		U32 textSize = 0;

		lHeader.mAssetCount = (U16)JSON_Tree_Collect( pTree, "images", 0, &collector )->mPropertyCount,
							+ (U16)JSON_Tree_Collect( pTree, "fonts", 0, &collector )->mPropertyCount,
							+ (U16)JSON_Tree_Collect( pTree, "sprites", 0, &collector )->mPropertyCount,
							+ (U16)JSON_Tree_Collect( pTree, "sounds", 0, &collector )->mPropertyCount;

		lHeader.mCallCount = (U16)JSON_Tree_Collect( pTree, 0, "action", &collector )->mPropertyCount;
		lHeader.mControlCount = (U16)JSON_Tree_Collect( pTree, "controls", 0, &collector )->mObjectCount;
		lHeader.mListCount = (U16)JSON_Tree_Collect( pTree, "controls", "selector", &collector )->mPropertyCount;
		lHeader.mListItemCount = (U16)JSON_Tree_Collect( pTree, "controls", "selections", &collector )->mValueCount;
		lHeader.mLockCount = (U16)JSON_Tree_Collect( pTree, "controls", "lock", &collector )->mPropertyCount;
		lHeader.mPageCount = (U16)JSON_Tree_Collect( pTree, "page", 0, &collector )->mObjectCount;
		lHeader.mSliderCount = (U16)JSON_Tree_Collect( pTree, "controls", "slider", &collector )->mPropertyCount;
		lHeader.mSpriteCount = (U16)JSON_Tree_Collect( pTree, "sprites", 0, &collector )->mObjectCount;
		lHeader.mVarCount = (U16)JSON_Tree_Collect( pTree, "vars", 0, &collector )->mPropertyCount;

		if( JSON_Tree_GetObjectCount( pTree, "fonts") )
			lHeader.mFontGroupCount = 1;
		lHeader.mPageCount = lHeader.mControlListCount;
		lHeader.mTextCount = lHeader.mControlCount + lHeader.mControlListCount + lHeader.mPageCount;
		

		
		textSize = (U16)JSON_Tree_Collect( pTree, "vars", 0, &collector )->mTextSize
				  +(U16)JSON_Tree_Collect( pTree, 0, "filename", &collector )->mTextSize
				  +(U16)JSON_Tree_Collect( pTree, 0, "context", &collector )->mTextSize;

		for( i=0; i<eFedControl_LIMIT; i++ )
			textSize += (U16)JSON_Tree_Collect( pTree, "controls", gFedControlTagStrings[i].pString, &collector )->mTextSize;


		JSON_TreeDestroy( pTree );
	}

	for( i = 0; i < jsonElements.mCount; i++ )
	{
		if( eTypeJSON_ObjectName == jsonElements.mpElements[ i ].mTypeFlags )
		{
			pObjectTag = sTagString_GetFromString(&jsonElements.mpElements[ i ].mToken, gFedObjectTagStrings, eFedObject_LIMIT );
		}
		else if( eTypeJSON_ObjectBegin == jsonElements.mpElements[ i ].mTypeFlags )
		{
			if( pObjectTag )
				objectTypeCounts[ pObjectTag->ID ]++;
		}
		else if( eTypeJSON_PropertyName == jsonElements.mpElements[ i ].mTypeFlags )
		{
			if( pObjectTag && eFedObject_Controls==pObjectTag->ID)
			{
				pControlTag = sTagString_GetFromString(&jsonElements.mpElements[i].mToken, gFedControlTagStrings, eFedControl_LIMIT);
				if( pControlTag )
					controlTypeCounts[ pControlTag->ID ]++;
			}
		}
		else if( eTypeJSON_PropertyValue == jsonElements.mpElements[ i ].mTypeFlags )
		{
			if( pControlTag )
				controlContentsCounts[ pControlTag->ID ]++;
		}
	}

	size = sizeof(sFedHeader);
	pMem = (U8*)mMEMALLOC(size);
	pHeader = (sFedHeader*)pMem;

	if(!pHeader)
		return pHeader;

	{
		sElementCollectionWalkerJSON walker;
		sString layoutString;
		sFedJSON_Layout	layout;

		layout.x0 = 0;
		layout.y0 = 0;
		layout.x1 = 320;
		layout.y1 = 200;

		String_Init( &layoutString, 0 );
		String_SetStaticNT( &layoutString, "layout" );

		FedJSON_MemoryArrange( pHeader, size );
		JSON_ElementWalker_Init( &walker, &jsonElements );
		while( JSON_ElementWalker_GetNextObject( &walker ) )
		{
			if( String_IsEqual( &layoutString, &walker.mObjectName ) )
			{
				Memory_Clear( sizeof( layout ), &layout );
				JSON_ElementsToStruct( &jsonElements, &gFedJSON_Layout_Type, &walker.mIndex, &layout );
			}
		}
	}



	return pHeader;
}


/* ################################################################################ */
