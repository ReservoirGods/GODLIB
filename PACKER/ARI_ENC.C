/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	GodPack_Ari_SymbolEncode( sGodPackAriTables * apTables, sGodPackBitStream * apStream, const U16 aSymbol );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_Encode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Ari_Encode
* CREATION : 19.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Ari_Encode( const void * apSrc,void * apDst,const U32 aSize )
{
	U32					i;
	U8 *				lpSrc;
	U8 *				lpDst;
	U16					lSymbol;
	sGodPackBitStream	lStream;
	sGodPackAriTables	lTables;

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;

	GodPack_BitStream_Init( &lStream, lpDst, aSize );
	GodPack_Ari_TablesInit( &lTables );

	for( i=0; i<aSize; i++ )
	{
		lSymbol = lTables.mCharToIndex[ lpSrc[i] ];
		GodPack_Ari_SymbolEncode(  &lTables, &lStream, lSymbol );
		GodPack_Ari_ModelUpdate( &lTables, lSymbol );
	}
	GodPack_Ari_SymbolEncode( &lTables, &lStream, 257 );

	lStream.mSwapBitsLeft++;
	if( lTables.mLow < dGODPACK_ARI_FIRST_QTR )
	{
		GodPack_BitStream_MultiBitsWrite( &lStream, 0 );
	}
	else
	{
		GodPack_BitStream_MultiBitsWrite( &lStream, 1 );
	}

	GodPack_BitStream_DeInit( &lStream );

	

	return( lStream.mOffset );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_TablesInit( sGodPackAriTables * apTables )
* ACTION   : GodPack_Ari_TablesInit
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_Ari_TablesInit( sGodPackAriTables * apTables )
{
	U16	i;
	
	apTables->mIndexToChar[ 0 ] = 0;
	for( i=0; i<256; i++ )
	{
		apTables->mCharToIndex[ i   ] = (U16)(i+1);
		apTables->mIndexToChar[ i+1 ] = (U8)i;
	}
	for( i=0; i<258; i++ )
	{
		apTables->mSymbolFreq[ i ] = 1;
		apTables->mCumFreq[ i ]    = (U16)(257-i);
	}
	apTables->mSymbolFreq[ 0 ] = 0;
	apTables->mLow             = 0;
	apTables->mHigh            = dGODPACK_ARI_VALUE_LIMIT;
	apTables->mValue           = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_SymbolEncode( sGodPackAriTables * apTables,sGodPackBitStream * apStream,const U16 aSymbol )
* ACTION   : GodPack_Ari_SymbolEncode
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_Ari_SymbolEncode( sGodPackAriTables * apTables,sGodPackBitStream * apStream,const U16 aSymbol )
{
	U32	lRange;
	U16	lTrue;
	
	lRange  = apTables->mHigh - apTables->mLow + 1;

	apTables->mHigh = apTables->mLow + ((lRange*apTables->mCumFreq[aSymbol-1])/apTables->mCumFreq[0]) - 1;
	apTables->mLow += ((lRange*apTables->mCumFreq[aSymbol])/apTables->mCumFreq[0]);
	
	lTrue = 1;
	while( lTrue )
	{
		if( apTables->mHigh < dGODPACK_ARI_HALF )
		{
			GodPack_BitStream_MultiBitsWrite( apStream, 0 );
		}
		else if( apTables->mLow >= dGODPACK_ARI_HALF )
		{
			GodPack_BitStream_MultiBitsWrite( apStream, 1 );
			apTables->mLow  -= dGODPACK_ARI_HALF;
			apTables->mHigh -= dGODPACK_ARI_HALF;
		}
		else if( ( apTables->mLow  >= dGODPACK_ARI_FIRST_QTR ) &&
			     ( apTables->mHigh <  dGODPACK_ARI_THIRD_QTR ) )
		{
			apStream->mSwapBitsLeft++;
			apTables->mLow  -= dGODPACK_ARI_FIRST_QTR;
			apTables->mHigh -= dGODPACK_ARI_FIRST_QTR;
		}
		else
		{
			return;
		}

		apTables->mLow  <<= 1L;
		apTables->mHigh <<= 1L;
		apTables->mHigh++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_ModelUpdate( sGodPackAriTables * apTables,U16 aSymbol )
* ACTION   : GodPack_Ari_ModelUpdate
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	GodPack_Ari_ModelUpdate( sGodPackAriTables * apTables,U16 aSymbol )
{
	U16	i;
	U16	lCum;
	U16	lIndex;
	U16	lSymbol;
	
	if( apTables->mCumFreq[ 0 ] == dGODPACK_ARI_FREQ_LIMIT )
	{
		lCum = 0;
		i    = 258;
		while( i )
		{
			i--;
			apTables->mSymbolFreq[ i ] = (U16)((apTables->mSymbolFreq[ i ] + 1) >> 1);
			apTables->mCumFreq[ i ]    = lCum;
			lCum                       = (S16)(lCum + apTables->mSymbolFreq[ i ]);
		}
	}

	i =  aSymbol;
	while( apTables->mSymbolFreq[ i ] == apTables->mSymbolFreq[ i - 1 ] )
	{
		i--;
	}

	if( i < aSymbol )
	{
		lIndex  = apTables->mIndexToChar[ i ];
		lSymbol = apTables->mIndexToChar[ aSymbol ];

		apTables->mIndexToChar[ i ]       = (U8)lSymbol;
		apTables->mIndexToChar[ aSymbol ] = (U8)lIndex;

		apTables->mCharToIndex[ lIndex  ] = aSymbol;
		apTables->mCharToIndex[ lSymbol ] = i;
	}

	apTables->mSymbolFreq[ i ] += 1;

	while( i )
	{
		i--;
		apTables->mCumFreq[ i ]++;
	}
}


/* ################################################################################ */
