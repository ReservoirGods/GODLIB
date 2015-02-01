/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: PROGRAM.C
::
:: Atari PRG/TOS file functions
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PROGRAM.H"


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Program_Relocate(   const sProgramHeader * apHeader )
* ACTION   : relocates a program
* CREATION : 02.05.01 PNK
*-----------------------------------------------------------------------------------*/

void				Program_Relocate(   const sProgramHeader * apHeader )
{
	U8 *	lpText;
	U8 *	lpRelocTable;
	U8		lFix;
	U32		lOffset;
	U32		lMax;

	if( !apHeader )
	{
		return;
	}

	lpText  = (U8*)apHeader;
	lpText += sizeof( sProgramHeader );

	lpRelocTable  = lpText;
	lpRelocTable += apHeader->mTextSize;
	lpRelocTable += apHeader->mDataSize;
	lpRelocTable += apHeader->mSymbolTableSize;

	lOffset = *(U32*)lpRelocTable;
	lpRelocTable += 4;
	lpText       += lOffset;

	lMax = apHeader->mTextSize + apHeader->mDataSize;

	if( lOffset )
	{
		*(U32*)&lpText[ lOffset ] += (U32)lpText;
		while( *lpRelocTable )
		{
			lFix = *lpRelocTable++;
			if( 1 == lFix )
			{
				lOffset += 254;
			}
			else
			{
				lOffset += lFix;
				if( lOffset < lMax )
				{
					*(U32*)&lpText[ lOffset ] += (U32)lpText;
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Program_GetpSymbol( const sProgramHeader * apHeader, const U32 aOffset )
* ACTION   : returns pointer to symbol at aOffset
* CREATION : 02.05.01 PNK
*-----------------------------------------------------------------------------------*/

U8					Program_IsValid( const sProgramHeader * apHeader )
{
	if( !apHeader )
	{
		return( 0 );
	}
	return( (U8)(apHeader->mMagic == dPROGRAM_MAGIC) );
}


/* ################################################################################ */
