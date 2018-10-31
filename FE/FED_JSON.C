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

#define FEDX_ELEMENTS		\
	FEDX( Asset )			\
	FEDX( Call )			\
	FEDX( Control )			\
	FEDX( ControlList )		\
	FEDX( FontGroup )		\
	FEDX( Lock )			\
	FEDX( List )			\
	FEDX( ListItem )		\
	FEDX( Page )			\
	FEDX( PageStyle )		\
	FEDX( Sample )			\
	FEDX( Slider )			\
	FEDX( Sprite )			\
	FEDX( SpriteList )		\
	FEDX( SpriteGroup )		\
	FEDX( Text )			\
	FEDX( Transition )		\
	FEDX( Var )				

/*
	FEDX( Box )				\
	FEDX( Pos )				\
	FEDX( Size )			\
*/

enum	
{
#define FEDX( fi )	eFedItem_##fi,
FEDX_ELEMENTS
#undef FEDX
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

U32	gFedItemSizeofs[ eFedItem_LIMIT ] =
{
#define FEDX( fi )	sizeof(sFed##fi),
FEDX_ELEMENTS
#undef FEDX
};

/*
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
*/

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

	U32		mItemCounts[ eFedItem_LIMIT ];
	U32		mItemIndices[ eFedItem_LIMIT ];
	U8 *	mpItemBases[ eFedItem_LIMIT ];
	U8 ** 	mppItemBases[ eFedItem_LIMIT];

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
		pMem += (gFedItemSizeofs[aFedItemType] * apContext->mItemIndices[ aFedItemType ]);
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
		pMem += (gFedItemSizeofs[ aFedItemType ] * (apContext->mItemIndices[ aFedItemType ]-1));
	}
	return pMem;
}

char *	FedJSON_ItemStringCreate( sFedJSON_Context * apContext, sString * apSrc )
{
	char * pDst = apContext->mpTextBase;
	U32 len = String_GetLength(apSrc);
	if( pDst )
	{
		char * src = apSrc->mpChars;
		char * end = &src[ len ];
		char * dst = pDst + apContext->mTextOffset;
		pDst = dst;

		for( ;src < end; *dst++ = *src++ );
		*dst = 0;
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

	String_Init( &lOnOffStrings[ 0 ], 0 );
	String_Init( &lOnOffStrings[ 1 ], 0 );
	String_SetStaticNT( &lOnOffStrings[ 0 ], "OFF" );
	String_SetStaticNT( &lOnOffStrings[ 1 ], "ON" );

	for( ;apObject; apObject=apObject->mpSibling )
	{
		const sTagString * tag = sTagString_GetFromString( &apObject->mObjectName, gFedObjectTagStrings, eFedObject_LIMIT );
		if( tag )
		{
			if( eFedObject_Controls == tag->ID )
			{
				sFedControl ** ppControls = (sFedControl**)apContext->mppItemBases[ eFedItem_Control ];
				const sObjectJSON * pControlObj = apObject;
				sFedControlList * pControlList = (sFedControlList *)FedJSON_ItemCreate( apContext, eFedItem_ControlList );
				sFedPage * pPage = (sFedPage*)FedJSON_ItemGetCurrent( apContext, eFedItem_Page );
				if( pPage )
					pPage->mpControlList = pControlList;

				if( ppControls )
					ppControls += apContext->mItemIndices[ eFedItem_Control ];

				if( pControlList )
					pControlList->mppControls = ppControls;

				for( ; pControlObj; pControlObj=pControlObj->mpSibling)
				{
					U32 t;
					sPropertyJSON * prop;
					sFedControl * pControl = (sFedControl *)FedJSON_ItemCreate( apContext, eFedItem_Control );
					if( ppControls)
						*ppControls++ = pControl;

					if( pControlList )
						pControlList->mControlCount++;

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
							sFedText * titText = (sFedText*)FedJSON_ItemCreate( apContext, eFedItem_Text);
							char * titName = FedJSON_ItemStringCreate( apContext, &prop->mpValues[0]);

							if( titText )
								titText->mpString = titName;
							if( pControl )
								pControl->mpTitle = titText;

/*							FedJSON_SetText( &pControl->mpTitle, pControlObj, gFedControlTagStrings[t].pString, 0, apContext);*/

							if( eFedControl_Action == t )
							{
								if( pControl )
									pControl->mControlType = eFED_CONTROL_CALL;
								prop = JSON_Tree_GetpProperty( pControlObj, "binding");
								if( prop )
								{
									sFedCall * call = (sFedCall*)FedJSON_ItemCreate( apContext, eFedItem_Call );

									if( call )
										call->mpCallVar = pControl->mpSetVar;

									if( pControl )
										pControl->mpCall = call;
								}
							}
							else if( eFedControl_BackLink == t )
							{
								/* write to page link matrix */
								if( pControl )
									pControl->mControlType = eFED_CONTROL_LINK;
							}
							else if( eFedControl_Link == t )
							{
								if( pControl )
								{
									/* write to page link matrix */
									pControl->mControlType = eFED_CONTROL_LINK;
									/* temp copy name of link page into link pointer, we will resolve these in second pass */
									pControl->mpLink = (sFedPage*)&prop->mpValues[0];
								}
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
									sFedListItem ** ppItems = (sFedListItem**)apContext->mppItemBases[ eFedItem_ListItem ];
									if( ppItems )
										ppItems += apContext->mItemIndices[ eFedItem_ListItem];
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
										if( ppItems )
											*ppItems++ = item;
									}
								}
							}
							else if( eFedControl_Toggle == t )
							{
								U32 it;
								sFedList * list = (sFedList*)FedJSON_ItemCreate( apContext, eFedItem_List);
								sFedListItem ** ppItems = (sFedListItem**)apContext->mppItemBases[ eFedItem_ListItem ];
								if( ppItems )
									ppItems += apContext->mItemIndices[ eFedItem_ListItem];

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
									if( ppItems )
										*ppItems++ = item;
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
								if( prop && pControl )
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
		U32 off =0;

		Memory_Clear( sizeof(context), &context );

		/* pass 1 - calculate sizes */
		FedJSON_Build( pTree, &context);

		size = sizeof( sFedHeader );

		for( i=0; i<eFedItem_LIMIT; i++ )
		{
			size += context.mItemIndices[i] * gFedItemSizeofs[i];
		}
		size += (context.mItemIndices[ eFedItem_Control ] * sizeof(sFedControl*));
		size += (context.mItemIndices[ eFedItem_ListItem ] * sizeof(sFedListItem*));
		size += (context.mItemIndices[ eFedItem_Sprite ] * sizeof(sFedSprite*));
		size += context.mTextOffset + 1;

		pMem = mMEMCALLOC(size);
		pHeader = (sFedHeader*)pMem;
		off = sizeof(sFedHeader);
		for( i=0; i<eFedItem_LIMIT; i++ )
		{
			if( context.mItemIndices[i] )
			{
				context.mpItemBases[i] = &pMem[ off ];
				off += context.mItemIndices[i] * gFedItemSizeofs[i];
			}
		}

		context.mppItemBases[ eFedItem_Control] = (U8**)&pMem[ off ];
		off += (context.mItemIndices[ eFedItem_Control ] * sizeof(sFedControl*));

		context.mppItemBases[ eFedItem_ListItem] = (U8**)&pMem[ off ];
		off += (context.mItemIndices[ eFedItem_ListItem ] * sizeof(sFedListItem*));

		context.mppItemBases[ eFedItem_Sprite] = (U8**)&pMem[ off ];
		off += (context.mItemIndices[ eFedItem_Sprite ] * sizeof(sFedSprite*));

		context.mpTextBase = (char*)&pMem[ off ];

		GODLIB_ASSERT( ( off + context.mTextOffset + 1) == size );

		#define FEDX( fi )	pHeader->m##fi##Count = (U16)context.mItemIndices[ eFedItem_##fi ]; pHeader->mp##fi##s = (sFed##fi*)context.mpItemBases[ eFedItem_##fi ];
		FEDX_ELEMENTS
		#undef FEDX

		for( i=0; i<eFedItem_LIMIT; i++ )
		{
			context.mItemCounts [ i ] = context.mItemIndices [ i ];
			context.mItemIndices[ i ] = 0;
		}
		context.mTextOffset = 0;

		/* pass 2 - build */
		FedJSON_Build( pTree, &context);


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
