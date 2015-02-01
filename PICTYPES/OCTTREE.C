/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: OCTTREE.C
::
:: Routines for octtree colour quantisation
::
:: based on paper 'Octtree Colour Quantisation' by Ian Ashdown
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"OCTTREE.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTreeNode_Init( sOctTreeNode * apNode, U16 aLevel, U8 aLeafFlag )
* DESCRIPTION : initialises a node
* AUTHOR      : 24.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTreeNode_Init( sOctTreeNode * apNode, U16 aLevel, U8 aLeafFlag )
{
	U16	i;

	apNode->mRedSum        = 0;
	apNode->mBlueSum       = 0;
	apNode->mGreenSum      = 0;

	apNode->mChildrenCount = 0;
	apNode->mIndex         = 0;
	apNode->mPixelCount    = 0;
	apNode->mLeafFlag      = aLeafFlag;
	apNode->mLevel         = aLevel;
	apNode->mpNext         = 0;
	apNode->mpPrev         = 0;

	for( i=0; i<dOCTTREE_CHILD_LIMIT; i++ )
	{
		apNode->mpChild[ i ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTreeNode_AddColour( sOctTreeNode * apNode, uCanvasPixel aColour )
* DESCRIPTION : adds a colour to an octtree node
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	OctTreeNode_AddColour( sOctTreeNode * apNode, uCanvasPixel aColour )
{
	apNode->mRedSum   += aColour.b.r;
	apNode->mGreenSum += aColour.b.g;
	apNode->mBlueSum  += aColour.b.b;

	apNode->mPixelCount++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTreeNode_FindChild( sOctTreeNode * apNode, uCanvasPixel aColour )
* DESCRIPTION : returns index for a child
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

U16		OctTreeNode_FindChild( sOctTreeNode * apNode, uCanvasPixel aColour )
{
	U16	lIndex;
	U16	lShift;

	lShift = (U16)((dOCTTREE_DEPTH_LIMIT - apNode->mLevel));

	lIndex  = (U16)(( (aColour.b.r >> lShift) & 1 ) << 2);
	lIndex |= ( (aColour.b.g >> lShift) & 1 ) << 1;
	lIndex |= ( (aColour.b.b >> lShift) & 1 );

	return( lIndex );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTreeNode_GetColour( sOctTreeNode * apNode )
* DESCRIPTION : returns averaged colour for a node
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

uCanvasPixel	OctTreeNode_GetColour( sOctTreeNode * apNode )
{
	uCanvasPixel	lColour;

	lColour.b.r = (U8)( apNode->mRedSum   / apNode->mPixelCount );
	lColour.b.g = (U8)( apNode->mGreenSum / apNode->mPixelCount );
	lColour.b.b = (U8)( apNode->mBlueSum  / apNode->mPixelCount );

	return( lColour );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_Init( sOctTree * apTree )
* DESCRIPTION : initialises an octree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_Init( sOctTree * apTree, U16 aColourLimit )
{
	U16	i;

	apTree->mColourMax = aColourLimit;
	apTree->mHeight    = 0;
	apTree->mLeafCount = 0;
	apTree->mLeafLevel = dOCTTREE_DEPTH_LIMIT + 1;
	apTree->mpRoot     = 0;
	apTree->mpPalette  = 0;
	apTree->mWidth     = 0;

	apTree->mNodeCount = 0;

	apTree->mppReducibles = (sOctTreeNode**)mMEMCALLOC( (dOCTTREE_DEPTH_LIMIT+1) * 4 );

	for( i=0; i<apTree->mLeafLevel; i++ )
	{
		apTree->mppReducibles[ i ] = 0;
	}

	apTree->mpPalette = (uCanvasPixel*)mMEMALLOC( apTree->mColourMax * sizeof(uCanvasPixel) );

	for( i=0; i<apTree->mColourMax; i++ )
	{
		apTree->mpPalette[ i ].l = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_CreateNode( sOctTree * apTree, U16 aLevel )
* DESCRIPTION : creates a node in an octree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

sOctTreeNode *	OctTree_CreateNode( sOctTree * apTree, U16 aLevel )
{
	sOctTreeNode *	lpNode;
	U8				lLeafFlag;

	apTree->mNodeCount++;

	if( aLevel >= apTree->mLeafLevel )
	{
		lLeafFlag = 1;
		apTree->mLeafCount++;
	}
	else
	{
		lLeafFlag = 0;
	}

	lpNode = (sOctTreeNode*)mMEMALLOC( sizeof(sOctTreeNode) );

	if( lpNode )
		OctTreeNode_Init( lpNode, aLevel, lLeafFlag );

	return( lpNode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_DeleteNode( sOctTree * apTree, sOctTreeNode * apNode )
* DESCRIPTION : deletes a node from an octtree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_DeleteNode( sOctTree * apTree, sOctTreeNode * apNode )
{
	U16	i;

	apTree->mNodeCount--;

	if( apNode )
	{	
		if( !apNode->mLeafFlag )
		{
			for( i=0; i<dOCTTREE_CHILD_LIMIT; i++ )
			{
				if( apNode->mpChild[ i ] )
				{
					OctTree_DeleteNode( apTree, apNode->mpChild[ i ] );
					apNode->mpChild[ i ] = 0;
					apNode->mChildrenCount--;
				}
			}
		}
		else
		{
			apTree->mLeafCount--;
		}

		mMEMFREE( apNode );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_QuantizeColour( sOctTree * apTree, sOctTreeNode * apNode, uCanvasPixel aColour )
* DESCRIPTION : returns index for colour aColour
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

U16				OctTree_QuantizeColour( sOctTree * apTree, sOctTreeNode * apNode, uCanvasPixel aColour )
{
	U16	lIndex;

	if( (apNode->mLeafFlag) || (apNode->mLevel == apTree->mLeafLevel) )
	{
		lIndex = apNode->mIndex;
	}
	else
	{
		lIndex = OctTreeNode_FindChild( apNode, aColour );
		lIndex = OctTree_QuantizeColour( apTree, apNode->mpChild[ lIndex ], aColour );
	}

	return( lIndex );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_MakeReducible( sOctTree * apTree, sOctTreeNode * apNode )
* DESCRIPTION : returns index for colour aColour
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_MakeReducible( sOctTree * apTree, sOctTreeNode * apNode )
{
	U16				lLevel;
	sOctTreeNode *	lpHead;
	
	lLevel         = apNode->mLevel;
	lpHead         = apTree->mppReducibles[ lLevel ];
	apNode->mpNext = lpHead;

	if( lpHead )
	{
		lpHead->mpPrev = apNode;
	}

	apTree->mppReducibles[ lLevel ] = apNode;
	apNode->mMarkFlag               = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_ReduceTree( sOctTree * apTree )
* DESCRIPTION : returns index for colour aColour
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_ReduceTree( sOctTree * apTree )
{
	sOctTreeNode *	lpNode;
	sOctTreeNode *	lpChild;
	U16				i;

	lpNode = OctTree_GetReducible( apTree );

	if( lpNode )
	{
		for( i=0; i<dOCTTREE_CHILD_LIMIT; i++ )
		{
			lpChild = lpNode->mpChild[ i ];

			if( lpChild )
			{
				OctTree_DeleteNode( apTree, lpChild );
				lpNode->mpChild[ i ] = 0;
				lpNode->mChildrenCount--;
			}
		}

		lpNode->mLeafFlag = 1;
		apTree->mLeafCount++;

		if( lpNode->mLevel < (apTree->mLeafLevel-1) )
		{
			apTree->mLeafLevel = (U16)(lpNode->mLevel + 1);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_InsertNode( sOctTree * apTree, OctNode * apNode, uCanvasPixel aColour )
* DESCRIPTION : inserts a node into an octree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

U8				OctTree_InsertNode( sOctTree * apTree, sOctTreeNode * apNode, uCanvasPixel aColour )
{
	sOctTreeNode *	lpChild;
	U16				lLevel;
	U16				lIndex;

	lLevel = apNode->mLevel;

	OctTreeNode_AddColour( apNode, aColour );

	if( (!apNode->mLeafFlag) && (lLevel < apTree->mLeafLevel) )
	{
		lIndex = OctTreeNode_FindChild( apNode, aColour );

		lpChild = apNode->mpChild[ lIndex ];

		if( !lpChild )
		{
			lpChild = OctTree_CreateNode( apTree, (U16)(lLevel+1) );

			if( !lpChild )
			{
				return( 0 );
			}

			apNode->mpChild[ lIndex ] = lpChild;
			apNode->mChildrenCount++;
		}

		if( ( apNode->mChildrenCount > 1 ) && ( !apNode->mMarkFlag ) )
		{
			OctTree_MakeReducible( apTree, apNode );
		}

		return( OctTree_InsertNode( apTree, lpChild, aColour ) );
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_ReduceTree( sOctTree * apTree )
* DESCRIPTION : returns index for colour aColour
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_BuildPalette( sOctTree * apTree, sOctTreeNode * apNode, U16 * apIndex )
{
	U16	i;

	if( apNode )
	{
		if( (apNode->mLeafFlag) || (apNode->mLevel == apTree->mLeafLevel) )
		{
			apTree->mpPalette[ *apIndex ] = OctTreeNode_GetColour( apNode );
			apNode->mIndex                = *apIndex;
			*apIndex                      = (U16)((*apIndex) + 1);
		}
		else
		{
			for( i=0; i<dOCTTREE_CHILD_LIMIT; i++ )
			{
				OctTree_BuildPalette( apTree, apNode->mpChild[ i ], apIndex );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_GetReducible( sOctTree * apTree )
* DESCRIPTION : returns reducible node
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

sOctTreeNode *	OctTree_GetReducible( sOctTree * apTree )
{
	sOctTreeNode *	lpReduc;
	sOctTreeNode *	lpMostPixels;
	sOctTreeNode *	lpNext;
	sOctTreeNode *	lpPrev;
	U16				lReducIndex;

	lReducIndex = (U16)(apTree->mLeafLevel - 1);

	while(( !apTree->mppReducibles[ lReducIndex ] ) && (lReducIndex) )
	{
		lReducIndex--;
	}

	lpReduc = apTree->mppReducibles[ lReducIndex ];

	if( !lpReduc )
	{
		return( 0 );
	}

	lpMostPixels = lpReduc;
	while( lpReduc )
	{
		if( lpReduc->mPixelCount > lpMostPixels->mPixelCount )
		{
			lpMostPixels = lpReduc;
		}

		lpReduc = lpReduc->mpNext;
	}

	lpNext = lpMostPixels->mpNext;
	lpPrev = lpMostPixels->mpPrev;

	if( !lpPrev )
	{
		apTree->mppReducibles[ lReducIndex ] = lpNext;
	}
	else
	{
		lpPrev->mpNext = lpNext;
	}

	if( lpNext )
	{
		lpNext->mpPrev = lpPrev;
	}

	lpMostPixels->mpPrev    = 0;
	lpMostPixels->mpNext    = 0;
	lpMostPixels->mMarkFlag = 0;

	return( lpMostPixels );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_BuildTree( sOctTree * apTree, sCanvas * apCanvas )
* DESCRIPTION : builds octree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

U8		OctTree_BuildTree( sOctTree * apTree, sCanvas * apCanvas )
{
	uCanvasPixel	lColour;
	U16				x,y;

	apTree->mpRoot = OctTree_CreateNode( apTree, 0 );

	if( !apTree->mpRoot )
	{
		return( 0 );
	}

	for( y=0; y<apCanvas->mHeight; y++ )
	{
		for( x=0; x<apCanvas->mWidth; x++ )
		{
			lColour = Canvas_GetPixel( apCanvas, x, y );

			if( !OctTree_InsertNode( apTree, apTree->mpRoot, lColour ) )
			{
				OctTree_DeleteNode( apTree, apTree->mpRoot );
				return( 0 );
			}

			if( apTree->mLeafCount > apTree->mColourMax )
			{
				OctTree_ReduceTree( apTree );
			}
		}
	}

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : OctTree_DeleteTree( sOctTree * apTree )
* DESCRIPTION : builds octree
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

void			OctTree_DeleteTree( sOctTree * apTree )
{
	OctTree_DeleteNode( apTree, apTree->mpRoot );

	if( apTree->mpPalette )
	{
		mMEMFREE( apTree->mpPalette );
		apTree->mpPalette = 0;
	}

	if( apTree->mppReducibles )
	{
		mMEMFREE( apTree->mppReducibles );
		apTree->mppReducibles = 0;
	}
}


/* ################################################################################ */
