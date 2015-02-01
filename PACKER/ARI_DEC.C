/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16	GodPack_Ari_SymbolDecode( sGodPackAriTables * apTables,sGodPackBitStream * apStream );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_Decode( const void * apSrc,void * apDst,const U32 aDstSize )
* ACTION   : GodPack_Ari_Decode
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Ari_Decode( const void * apSrc,void * apDst,const U32 aDstSize )
{
	U32					i;
	U32					lOffset;
	U8 *				lpDst;
	sGodPackBitStream	lStream;
	sGodPackAriTables	lTables;
	U16					lSymbol;

	lpDst = (U8*)apDst;

	GodPack_BitStream_Init( &lStream, (void*)apSrc, aDstSize );
	GodPack_Ari_TablesInit( &lTables );

	lTables.mValue = 0;
	for( i=0; i<16; i++ )
	{
		lTables.mValue <<= 1L;
		lTables.mValue  += GodPack_BitStream_BitRead( &lStream );
	}

	lOffset = 0;
	lSymbol = GodPack_Ari_SymbolDecode( &lTables, &lStream );
	while( lSymbol != 257 )
	{
		lpDst[ lOffset ] = lTables.mIndexToChar[ lSymbol ];
		GodPack_Ari_ModelUpdate( &lTables, lSymbol );
		lOffset++;
		lSymbol          = GodPack_Ari_SymbolDecode( &lTables, &lStream );
	}

	return( lOffset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Ari_SymbolDecode( sGodPackAriTables * apTables,sGodPackBitStream * apStream )
* ACTION   : GodPack_Ari_SymbolDecode
* CREATION : 20.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	GodPack_Ari_SymbolDecode( sGodPackAriTables * apTables,sGodPackBitStream * apStream )
{
	U32	lRange;
	U16	lCum;
	U16	lSymbol;
	U16	lTrue;

	lRange =  (apTables->mHigh  - apTables->mLow)+1;
	lCum   =(U16)((((apTables->mValue - apTables->mLow + 1) * apTables->mCumFreq[ 0 ]) - 1) / lRange);
	lSymbol= 1;

	while( apTables->mCumFreq[ lSymbol ] > lCum )
	{
		lSymbol++;
	}

	apTables->mHigh = apTables->mLow + ((lRange * apTables->mCumFreq[lSymbol-1])/ apTables->mCumFreq[0]) - 1;
	apTables->mLow += ((lRange * apTables->mCumFreq[ lSymbol ])/apTables->mCumFreq[ 0 ]);

	lTrue = 1;
	while( lTrue )
	{
		if( apTables->mHigh < dGODPACK_ARI_HALF )
		{
		}
		else if( apTables->mLow >= dGODPACK_ARI_HALF )
		{
			apTables->mValue -= dGODPACK_ARI_HALF;
			apTables->mLow   -= dGODPACK_ARI_HALF;
			apTables->mHigh  -= dGODPACK_ARI_HALF;
		}
		else if( ( apTables->mLow  >= dGODPACK_ARI_FIRST_QTR ) &&
			     ( apTables->mHigh <  dGODPACK_ARI_THIRD_QTR ) )
		{
			apTables->mValue -= dGODPACK_ARI_FIRST_QTR;
			apTables->mLow   -= dGODPACK_ARI_FIRST_QTR;
			apTables->mHigh  -= dGODPACK_ARI_FIRST_QTR;
		}
		else
		{
			return( lSymbol );
		}
		apTables->mLow  <<= 1L;
		apTables->mHigh <<= 1L;
		apTables->mHigh++;
		apTables->mValue <<= 1L;
		apTables->mValue += GodPack_BitStream_BitRead( apStream );
	}
	return( lSymbol );
}


/* ################################################################################ */
