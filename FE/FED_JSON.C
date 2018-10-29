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
	eFedItem_Asset,
	eFedItem_Box,
	eFedItem_Call,
	eFedItem_Control,
	eFedItem_ControlList,
	eFedItem_FontGroup,
	eFedItem_Lock,
	eFedItem_List,
	eFedItem_ListItem,
	eFedItem_Page,
	eFedItem_PageStyle,
	eFedItem_Sample,
	eFedItem_Size,
	eFedItem_Slider,
	eFedItem_Sprite,
	eFedItem_SpriteList,
	eFedItem_SpriteGroup,
	eFedItem_Text,
	eFedItem_Transition,
	eFedItem_Pos,
	eFedItem_Var,

	eFedItem_LIMIT
};

enum
{
	eFedObject_None,
	eFedObject_Controls,
	eFedObject_Images,
	eFedObject_Layout,
	eFedObject_Pages,
	eFedObject_PageStyle,
	eFedObject_Sprites,
	eFedObject_Vars,

	eFedObject_LIMIT
};

enum
{
	eFedControl_None,
	eFedControl_Action,
	eFedControl_BackLink,
	eFedControl_Link,
	eFedControl_Selector,
	eFedControl_Slider,
	eFedControl_Toggle,
	eFedControl_LIMIT
};

enum
{
	eFedPageStyle_FontNormal,
	eFedPageStyle_FontSelected,
	eFedPageStyle_ImageBG,
	eFedPageStyle_SoundMove,
	eFedPageStyle_SoundSelect,
	eFedPageStyle_SpriteCursor,
	eFedPageStyle_TransitionIn,
	eFedPageStyle_TransitionOut,
	eFedPageStyle_LIMIT
};

sTagValue gFedItemSizeofs[ eFedItem_LIMIT ] =
{
	{ eFedItem_Asset, sizeof(sFedAsset) }, 
	{ eFedItem_Box,sizeof(sFedBox ) },
	{ eFedItem_Call,sizeof(sFedCall ) },
	{ eFedItem_Control,sizeof(sFedControl ) },
	{ eFedItem_ControlList,sizeof(sFedControlList ) },
	{ eFedItem_FontGroup,sizeof(sFedFontGroup ) },
	{ eFedItem_Lock,sizeof(sFedLock ) },
	{ eFedItem_List,sizeof(sFedList ) },
	{ eFedItem_ListItem,sizeof(sFedListItem ) },
	{ eFedItem_Page,sizeof(sFedPage ) },
	{ eFedItem_PageStyle,sizeof(sFedPageStyle ) },
	{ eFedItem_Sample,sizeof(sFedSample ) },
	{ eFedItem_Size,sizeof(sFedSize ) },
	{ eFedItem_Slider,sizeof(sFedSlider ) },
	{ eFedItem_Sprite,sizeof(sFedSprite ) },
	{ eFedItem_SpriteList,sizeof(sFedSpriteList ) },
	{ eFedItem_SpriteGroup,sizeof(sFedSpriteGroup ) },
	{ eFedItem_Text,sizeof(sFedText ) },
	{ eFedItem_Transition,sizeof(sFedTransition ) },
	{ eFedItem_Pos,sizeof(sFedPos ) },
	{ eFedItem_Var,sizeof(sFedVar ) },
};

sTagString	gFedPageStyleTagStrings[ eFedObject_LIMIT ] =
{
	{	eFedPageStyle_FontNormal,		"font_normal"	},
	{	eFedPageStyle_FontSelected,		"font_selected"	},
	{	eFedPageStyle_ImageBG,			"image_bg"		},
	{	eFedPageStyle_SoundMove,		"sound_move"	},
	{	eFedPageStyle_SoundSelect,		"sound_select"	},
	{	eFedPageStyle_SpriteCursor,		"sprite_cursor"	},
	{	eFedPageStyle_TransitionIn,		"transition_in"	},
	{	eFedPageStyle_TransitionOut,	"transition_out"},
};

sTagString	gFedObjectTagStrings[ eFedObject_LIMIT ] =
{
	{ eFedObject_None, "" },
	{ eFedObject_Controls, "controls" },
	{ eFedObject_Images, "images" },
	{ eFedObject_Layout, "layout" },
	{ eFedObject_Pages, "pages" },
	{ eFedObject_PageStyle, "pagestyle" },
	{ eFedObject_Sprites, "sprites" },
	{ eFedObject_Vars, "vars" },
};

sTagString gFedControlTagStrings[ eFedControl_LIMIT ] = 
{
	{ eFedControl_None,		""			},
	{ eFedControl_Action,	"action"	},
	{ eFedControl_BackLink,	"backlink"	},
	{ eFedControl_Link,		"link"		},
	{ eFedControl_Selector,	"selector"	},
	{ eFedControl_Slider,	"slider"	},
	{ eFedControl_Toggle,	"toggle"	},
};

sTagString	gFedFadeNames[ eFED_FADE_LIMIT ] =
{
	{	eFED_FADE_NONE,		"NONE"		},
	{	eFED_FADE_BG,		"BG"		},
	{	eFED_FADE_PAL,		"PAL"		},
	{	eFED_FADE_RGB,		"RGB"		},
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

typedef struct sFedJSON_Context
{
	sFedHeader * pHeader;
	sFedHeader * pCalculatedHeader;
	sFedHeader	mCounters;

	U32		mItemSizes[ eFedItem_LIMIT ];
	U32		mItemCounts[ eFedItem_LIMIT ];
	U32		mItemIndices[ eFedItem_LIMIT ];
	U8 *	mpItemBases[ eFedItem_LIMIT ];

	U8*			mpRawBase;
	U32			mRawOffset;
	U32			mRawSize;

	char *		mpTextBase;
	U32			mTextOffset;
	U32			mTextSize;

	U32			mModeCounting;

} sFedJSON_Context;

sReflectDictionary	gFedJSON_Dictionary = { 1, &gFedJSON_Layout_Type };


/* ###################################################################################
#  CODE
################################################################################### */

#define FEDJSON_ARRANGE( aType ) apHeader->mp##aType##s = (sFed##aType*)&pMem[ offset ]; offset += sizeof(sFed##aType) * apHeader->m##aType##Count;

U32	FedJSON_MemoryArrange( sFedHeader * apHeader )
{
	U8 * pMem = (U8*)apHeader;
	U32 offset = 0;

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

void	FedJSON_AssetBuildOld( const sObjectJSON * apObject, const sPropertyJSON * apProperty, void * apContext )
{
	sFedJSON_Context * context = (sFedJSON_Context*)apContext;
	sFedAsset * ass = &context->pHeader->mpAssets[ context->pHeader->mAssetCount ];
	
	(void)apObject;
	(void)apProperty;
	(void)ass;

	context->pHeader->mAssetCount++;
}

void	FedJSON_SetText( sFedText ** appText, const sObjectJSON * apObject, const char * apPropName, U16 aValueIndex, sFedJSON_Context * apContext )
{
	sPropertyJSON * prop = JSON_Tree_GetpProperty(apObject,apPropName);
	if( prop && (prop->mValueCount > aValueIndex))
	{
		U32 i;
		U32 len = String_GetLength( &prop->mpValues[aValueIndex]);

		*appText = &apContext->pHeader->mpTexts[ apContext->mCounters.mTextCount ];
		(*appText)->mpString = &apContext->mpTextBase[ apContext->mTextOffset ];
		
		for(i=0;i<len; i++)
			apContext->mpTextBase[ apContext->mTextOffset++ ] = prop->mpValues[aValueIndex].mpChars[ i ];
		apContext->mpTextBase[ apContext->mTextOffset++ ] = 0;
		apContext->mCounters.mTextCount++;
		GODLIB_ASSERT( apContext->mTextOffset <= apContext->mTextSize );
	}
}


void	FedJSON_GetTextSize(  const sObjectJSON * apObject, const char * apPropName, U16 aValueIndex, sFedJSON_Context * apContext )
{
	sPropertyJSON * prop = JSON_Tree_GetpProperty(apObject,apPropName);
	if( prop && (prop->mValueCount > aValueIndex))
	{
		apContext->mTextSize += (String_GetLength( &prop->mpValues[aValueIndex])) + 1;
	}
}

void *	FedJSON_ItemCreate( sFedJSON_Context * apContext, U16 aFedItemType )
{
	U8 * pMem = apContext->mpItemBases[ aFedItemType ];
	GODLIB_ASSERT( aFedItemType < eFedItem_LIMIT );
	if( pMem )
	{
		pMem += (apContext->mItemSizes[aFedItemType] * apContext->mItemIndices[ aFedItemType ]);
	}
	apContext->mItemIndices[ aFedItemType ]++;
	return pMem;
}

void * FedJSON_ItemGetCurrent( sFedJSON_Context * apContext, U16 aFedItemType )
{
	U8 * pMem = apContext->mpItemBases[ aFedItemType ];
	GODLIB_ASSERT( aFedItemType < eFedItem_LIMIT );
	if( pMem && apContext->mItemIndices[ aFedItemType ] )
	{
		pMem += (apContext->mItemSizes[aFedItemType] * (apContext->mItemIndices[ aFedItemType ]-1));
	}
	return pMem;
}

void *	FedJSON_RawCreate( sFedJSON_Context * apContext, U32 aSizeBytes )
{
	U8 * pMem = apContext->mpRawBase;
	if( pMem )
	{
		pMem += apContext->mRawOffset;
		apContext->mRawOffset += aSizeBytes;
	}
	return pMem;
}

char *	FedJSON_ItemStringCreate( sFedJSON_Context * apContext, sString * apSrc )
{
	char * pDst = apContext->mpTextBase;
	U32 len = String_GetLength(apSrc);
	if( pDst )
	{
		pDst += apContext->mTextOffset;
		String_StrCpy2( pDst, apSrc->mpChars, len );
		pDst[ len ] = 0;
	}
	apContext->mTextOffset += (len+1);
	return pDst;
}

sFedAsset *	FedJSON_AssetBuild( sFedJSON_Context * apContext, sObjectJSON * apObject )
{
	sFedAsset * ass = (sFedAsset*)FedJSON_ItemCreate( apContext, eFedItem_Asset );
	sPropertyJSON * filename = JSON_Tree_GetpProperty( apObject, "filename");
	sPropertyJSON * context = JSON_Tree_GetpProperty( apObject, "context");

	if( context->mValueCount )
	{
		char * contextString = FedJSON_ItemStringCreate( apContext, &context->mpValues[0]);	
		if( ass )
			ass->mpContext = contextString;
	}
	if( filename->mValueCount )
	{
		char * fnameString = FedJSON_ItemStringCreate( apContext, &filename->mpValues[0]);	
		if( ass )
			ass->mpFileName = fnameString;
	}

	return ass;
}


sFedTransition *	FedJSON_TransitionBuild( sFedJSON_Context * apContext, sObjectJSON * apObject )
{
	sFedTransition * trans = FedJSON_ItemCreate( apContext, eFedItem_Transition );
	
	sPropertyJSON * framecount = JSON_Tree_GetpProperty( apObject, "framecount");
	sPropertyJSON * fadetype = JSON_Tree_GetpProperty( apObject, "fadetype");
	sPropertyJSON * fadecolour = JSON_Tree_GetpProperty( apObject, "fadecolour");

	if( !trans )
		return trans;

	if( framecount && framecount->mValueCount )
	{
		trans->mFadeFrameCount = (U16)String_ToS32( &framecount->mpValues[0]);
	}
	if( fadetype && fadetype->mValueCount )
	{
		const sTagString * tag = sTagString_GetFromString( &fadetype->mpValues[0], gFedFadeNames, eFED_FADE_LIMIT );
		if( tag )
			trans->mFadeType = (U16)tag->ID;
	}
	if( fadecolour && fadecolour->mValueCount )
	{
		trans->mFadeColour = (U16)String_ToS32( &fadecolour->mpValues[0]);
	}

	return trans;
}


void	FedJSON_Build( const sObjectJSON * apObject, sFedJSON_Context * apContext )
{
	sString lOnOffStrings[ 2 ];

	String_Init( &lOnOffStrings[ 0 ], "OFF" );
	String_Init( &lOnOffStrings[ 1 ], "ON" );

	for( ;apObject; apObject=apObject->mpSibling )
	{
		const sTagString * tag = sTagString_GetFromString( &apObject->mObjectName, gFedObjectTagStrings, eFedObject_LIMIT );
		if( tag )
		{
			if( eFedObject_Controls == tag->ID )
			{
				sObjectJSON * pControlObj = apObject->mpChildren;
				sFedControlList * pControlList = (sFedControlList *)FedJSON_ItemCreate( apContext, eFedItem_ControlList );
				sFedPage * pPage = (sFedPage*)FedJSON_ItemGetCurrent( apContext, eFedItem_Page );
				if( pPage )
					pPage->mpControlList = pControlList;
				
/*					pControlList->mppControls = &apContext->mppControlsBase[ apContext->mControlsOffset ];*/
				for( ; pControlObj; pControlObj=pControlObj->mpSibling)
				{
					U32 t;
					sPropertyJSON * prop;
					sFedControl * pControl = (sFedControl *)FedJSON_ItemCreate( apContext, eFedItem_Control );
					if( pControlList)
						pControlList->mppControls[ pControlList->mControlCount++ ] = pControl;

					prop = JSON_Tree_GetpProperty( pControlObj, "binding");
					if( prop )
					{
						char * name = FedJSON_ItemStringCreate( apContext, &prop->mpValues[0] );
						sFedVar * var = (sFedVar*)FedJSON_ItemCreate( apContext, eFedItem_Var );
						if( pControl )
						{
							pControl->mpSetVar = var;
							pControl->mpSetVar->mpName =  name;
						}
					}

					prop = JSON_Tree_GetpProperty( pControlObj, "lock");
					if( prop )
					{
						sFedLock * lock = (sFedLock*)FedJSON_ItemCreate( apContext, eFedItem_Lock );
						sFedVar * var = (sFedVar*)FedJSON_ItemCreate( apContext, eFedItem_Var );
						char * name =FedJSON_ItemStringCreate( apContext, &prop->mpValues[0]);
						if( pControl )
						{
							pControl->mpLock = lock;
							pControl->mpLock->mpVisVar = var;
							pControl->mpLock->mpVisVar->mpName = name;
							pControl->mpLock->mVisValue = 1;
							if( prop->mValueCount > 1 )
								pControl->mpLock->mVisValue = String_ToS32( &prop->mpValues[1]);
						}
					}


					for( t=0; t<eFedControl_LIMIT; t++ )
					{
						prop = JSON_Tree_GetpProperty( pControlObj, gFedControlTagStrings[t].pString );
						if( prop )
						{
							FedJSON_SetText( &pControl->mpTitle, pControlObj, gFedControlTagStrings[t].pString, 0, apContext);

							if( eFedControl_Action == t )
							{
								pControl->mControlType = eFED_CONTROL_CALL;
								prop = JSON_Tree_GetpProperty( pControlObj, "binding");
								if( prop )
								{
									sFedCall * call = (sFedCall*)FedJSON_ItemCreate( apContext, eFedItem_Call );
									pControl->mpCall = call;
									if( pControl->mpCall )
										pControl->mpCall->mpCallVar = pControl->mpSetVar;
								}
							}
							else if( eFedControl_BackLink == t )
							{
								/* write to page link matrix */
								pControl->mControlType = eFED_CONTROL_LINK;
							}
							else if( eFedControl_Link == t )
							{
								/* write to page link matrix */
								pControl->mControlType = eFED_CONTROL_LINK;
								/* temp copy name of link page into link pointer, we will resolve these in second pass */
								pControl->mpLink = (sFedPage*)&prop->mpValues[0];
							}
							else if( eFedControl_Selector == t )
							{
								/* read all values and build list */
								sFedList * list = (sFedList*)FedJSON_ItemCreate( apContext, eFedItem_List);

								if( pControl )
								{
									pControl->mControlType = eFED_CONTROL_LIST;
									pControl->mpList = list;
									pControl->mpList->mpVar = pControl->mpSetVar;
								}
								prop = JSON_Tree_GetpProperty( pControlObj, "selections" );
								if( prop )
								{
									U32 it = 0;
									sFedListItem ** ppItems = FedJSON_RawCreate( apContext, prop->mValueCount * sizeof(sFedListItem*));
									if( pControl )
									{
										pControl->mpList->mItemCount = (U16)prop->mValueCount;
										pControl->mpList->mppItems = ppItems;
									}
									for( it=0; it<prop->mValueCount; it++ )
									{
										sFedListItem * item = (sFedListItem*)FedJSON_ItemCreate( apContext, eFedItem_ListItem);
										sFedText * text = (sFedText*)FedJSON_ItemCreate( apContext, eFedItem_Text);
										char * name = FedJSON_ItemStringCreate( apContext, &prop->mpValues[it]);
										if( item )
										{
											item->mpText = text;
											item->mpText->mpString = name;
										}
										if( pControl )
											pControl->mpList->mppItems[it] = item;
									}
								}
							}
							else if( eFedControl_Toggle == t )
							{
								U32 it;
								sFedList * list = (sFedList*)FedJSON_ItemCreate( apContext, eFedItem_List);
								sFedListItem ** ppItems = (sFedListItem**)FedJSON_RawCreate( apContext, 2 * sizeof(sFedListItem*));

								/* build list of two values */
								if( pControl )
								{
									pControl->mControlType = eFED_CONTROL_LIST;

									pControl->mpList = list;
									pControl->mpList->mpVar = pControl->mpSetVar;

									pControl->mpList->mppItems = ppItems;
								}

								for( it=0; it<2; it++ )
								{
									sFedListItem * item = (sFedListItem*)FedJSON_ItemCreate( apContext, eFedItem_ListItem);
									sFedText * text = (sFedText*)FedJSON_ItemCreate( apContext, eFedItem_Text);
									char * name = FedJSON_ItemStringCreate( apContext, &lOnOffStrings[it]);
									if( item )
									{
										item->mpText = text;
										item->mpText->mpString = name;
									}
									if( pControl )
										pControl->mpList->mppItems[it] = item;
								}

							}
							else if( eFedControl_Slider == t )
							{
								sFedSlider * slider = (sFedSlider*)FedJSON_ItemCreate( apContext, eFedItem_Slider );
								/* create slider */
								if( pControl )
								{
									pControl->mControlType = eFED_CONTROL_SLIDER;

									pControl->mpSlider =slider;
									pControl->mpSlider->mpVar = pControl->mpSetVar;
								}
								prop = JSON_Tree_GetpProperty( pControlObj, "range" );
								if( prop )
								{
									if( prop->mValueCount > 0)
										pControl->mpSlider->mValueMin = String_ToS32( &prop->mpValues[ 0 ] );
									if( prop->mValueCount > 1)
										pControl->mpSlider->mValueMax = String_ToS32( &prop->mpValues[ 1 ] );
								}
							}

							break;
						}
					}
				}						
			}
			else if( eFedObject_Layout == tag->ID )
			{

			}
			else if( eFedObject_PageStyle == tag->ID )
			{
				sFedPageStyle * style = (sFedPageStyle*)FedJSON_ItemCreate(apContext, eFedItem_PageStyle);
				sFedFontGroup * fontGroup = (sFedFontGroup*)FedJSON_ItemCreate(apContext, eFedItem_FontGroup);

				if( style )
					style->mpControlFonts = fontGroup;
				
				sObjectJSON * pStyleObj = apObject->mpChildren;				
				for( ; pStyleObj; pStyleObj=pStyleObj->mpSibling)
				{				
					const sTagString * styleTag = sTagString_GetFromString( &pStyleObj->mObjectName, gFedPageStyleTagStrings, eFedPageStyle_LIMIT );
					if( styleTag )
					{
						if( eFedPageStyle_FontNormal == styleTag->ID )
						{
							sFedAsset * ass = (sFedAsset*)FedJSON_AssetBuild( apContext, pStyleObj );
							if( fontGroup )
								fontGroup->mpFontNormal = ass;
						}
						else if( eFedPageStyle_FontSelected == styleTag->ID)
						{
							sFedAsset * ass = (sFedAsset*)FedJSON_AssetBuild( apContext, pStyleObj );
							if( fontGroup )
								fontGroup->mpFontSelected = ass;
						}
						else if( eFedPageStyle_ImageBG == styleTag->ID)
						{
							sFedAsset * ass = (sFedAsset*)FedJSON_AssetBuild( apContext, pStyleObj );
							if( style )
								style->mpPageBG = ass;
						}
						else if( eFedPageStyle_SoundMove == styleTag->ID)
						{
							sFedSample * spl = FedJSON_ItemCreate( apContext, eFedItem_Sample );
							sFedAsset * ass = (sFedAsset*)FedJSON_AssetBuild( apContext, pStyleObj );
							if( spl )
								spl->mpAsset = ass;
							if( style )
								style->mpSampleMove = spl;
						}
						else if( eFedPageStyle_SoundSelect == styleTag->ID)
						{
							sFedSample * spl = FedJSON_ItemCreate( apContext, eFedItem_Sample );
							sFedAsset * ass = (sFedAsset*)FedJSON_AssetBuild( apContext, pStyleObj );
							if( spl )
								spl->mpAsset = ass;
							if( style )
								style->mpSampleSelect = spl;
						}
						else if (eFedPageStyle_SpriteCursor == styleTag->ID )
						{
							sFedSprite * sprite = (sFedSprite*)FedJSON_ItemCreate( apContext, eFedItem_Sprite );
							sFedAsset * ass = FedJSON_AssetBuild( apContext, pStyleObj );
							if( sprite )
								sprite->mpAsset = ass;
							if( style )
								style->mpCursor = sprite;
							
						}
						else if( eFedPageStyle_TransitionIn == styleTag->ID)
						{
							sFedTransition * trans = FedJSON_TransitionBuild( apContext, pStyleObj );
							if( style )
								style->mpIntroTrans = trans;
						}
						else if( eFedPageStyle_TransitionOut == styleTag->ID)
						{
							sFedTransition * trans = FedJSON_TransitionBuild( apContext, pStyleObj );						
							if( style )
								style->mpOutroTrans = trans;
						}
					}
				}
			}
			else if( eFedObject_Pages == tag->ID )
			{
				sFedPage * page = FedJSON_ItemCreate( apContext, eFedItem_Page );
				const sPropertyJSON * prop = JSON_Tree_GetpProperty( apObject, "id" );

				if( prop && prop->mValueCount )
				{
					sFedText * text = (sFedText*)FedJSON_ItemCreate( apContext, eFedItem_Text );
					char * name = FedJSON_ItemStringCreate( apContext, &prop->mpValues[0]);
					if( text )
						text->mpString = name;
					if( page )
						page->mpTitle = text;
				}
				FedJSON_Build( apObject->mpChildren, apContext );
			}
			else
			{
				FedJSON_Build( apObject->mpChildren, apContext );
			}
		}
	}
}

sFedHeader *		FedJSON_ParseText( const char * apText, const U32 aSize )
{
	sFedHeader	lHeader = {0};
	sFedHeader * pHeader = 0;
	sString jsonString;
	sElementCollectionJSON jsonElements;
/*	
	const sTagString * pObjectTag = 0;
	const sTagString * pControlTag = 0;
*/	
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
		sFedJSON_Context context;
		sObjectJSON * pTree = JSON_TreeCreate( &jsonElements );
		sTreeCollectorJSON collector;
		U32 textSize = 0;

		Memory_Clear( sizeof(context), &context );

		/* pass 1 - calculate sizes */
		FedJSON_Build( pTree, &context);

		size = 0;

		/* pass 2 - build */
		FedJSON_Build( pTree, &context);



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


		size = FedJSON_MemoryArrange( &lHeader );
		size += textSize;

		pMem = (U8*)mMEMALLOC(size);
		pHeader = (sFedHeader*)pMem;
		*pHeader = lHeader;
		FedJSON_MemoryArrange( pHeader );

		/* layout assets */

		context.pHeader = pHeader;
		context.pCalculatedHeader = &lHeader;
		FedJSON_Build( pTree, &context );



		JSON_TreeDestroy( pTree );
	}
#if 0
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
#endif


	return pHeader;
}


/* ################################################################################ */
