/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: PROFILER.C
::
:: Interrupt based function profiling
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PROFILER.H"

#include	<STRING.H>

#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/PROGRAM/PROGRAM.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dPROFILER_BUFFER_LIMIT	(32L*1024L)
#define	dPROFILER_SAVE_LIMIT	((dPROFILER_BUFFER_LIMIT*3)/4)


/* ###################################################################################
#  DATA
################################################################################### */

U32 *		gpProfilerBuffer = 0;
U32			gProfilerIndex;
U32			gProfilerOldHBL;
U8			gProfilerOnFlag = 0;
sFileHandle	gProfilerHandle;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	void	Profiler_HBL( void );
extern	void	Profiler_VBL( void );
#ifdef __VBCC__
extern void	_etext( void );
#else
extern	void	__text( void );
#endif

/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_Init( const U16 aGameBuildHi, const U16 aGameBuildLo)
* ACTION   : allocates memory for profiler buffer
* CREATION : 11.05.01 PNK
*-----------------------------------------------------------------------------------*/

U8		Profiler_Init( const U16 aGameBuildHi, const U16 aGameBuildLo)
{
	sProfilerHeader	lHeader;	

	gProfilerIndex  = 0;

	gProfilerOldHBL  = *(U32*)0x68;
	gProfilerOnFlag  = 0;
	gpProfilerBuffer = (U32*)mMEMCALLOC( dPROFILER_BUFFER_LIMIT );
	gProfilerHandle  = File_Create( "PROFILE.PRO" );

	if( (!gpProfilerBuffer) || (gProfilerHandle <0) )
	{
		return( 0 );
	}

	lHeader.mID          = dPROFILER_ID;
	lHeader.mVersion     = 0;
	lHeader.mEntryCount  = 0;
#ifdef __VBCC__
	lHeader.mpText       = (U32)_etext;
#else
	lHeader.mpText       = (U32)__text;
#endif
	lHeader.mGameBuildHi = aGameBuildHi;
	lHeader.mGameBuildLo = aGameBuildLo;
	strcpy( lHeader.mDateText, __DATE__ );
	strcpy( lHeader.mTimeText, __TIME__ );

	
	File_Write( gProfilerHandle, sizeof(sProfilerHeader), &lHeader );

	Vbl_AddCall( Profiler_VBL );

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_DeInit( void )
* ACTION   : releases memory allocated for profiler buffer
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void		Profiler_DeInit( void )
{
	if( gpProfilerBuffer )
	{
		Profiler_Disable();
		*(U32*)0x68 = gProfilerOldHBL;
		File_Close( gProfilerHandle );
		mMEMFREE( gpProfilerBuffer );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_Update( void )
* ACTION   : updates profiler
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void		Profiler_Update( void )
{
	if( gProfilerOnFlag )
	{
		if( gProfilerIndex > dPROFILER_SAVE_LIMIT )
		{
			Profiler_Disable();
			File_Write( gProfilerHandle, gProfilerIndex, gpProfilerBuffer );
			Profiler_Enable();
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_Enable( void )
* ACTION   : enables interrupt based profiling
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_Enable( void )
{
	if( gpProfilerBuffer )
	{
		gProfilerIndex  = 0;
		*(U32*)0x68     = (U32)Profiler_HBL;
		gProfilerOnFlag = 1;
		System_SetIML( 1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_Disable( void )
* ACTION   : enables interrupt based profiling
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_Disable( void )
{
	gProfilerOnFlag = 0;
	if( gpProfilerBuffer )
	{
		*(U32*)0x68     = gProfilerOldHBL;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_IsProfiling( void )
* ACTION   : returns true if the profiler has been initialised
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

U8		Profiler_IsProfiling( void )
{
	return( (U8)(gProfilerOnFlag) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_LoadProfile( char * apFileName )
* ACTION   : loads a profile file and fixes up the entry count
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

sProfilerHeader	*	Profiler_LoadProfile( char * apFileName )
{
	sProfilerHeader *	lpHeader;
	S32					lSize;

	lSize = File_GetSize( apFileName );
	if( lSize < (S32)sizeof(sProfilerHeader ) )
	{
		return( 0 );
	}


	lpHeader = (sProfilerHeader*)File_Load( apFileName );
	if( lpHeader )
	{
		Profiler_Relocate( lpHeader );
		lSize -= sizeof(sProfilerHeader);
		lpHeader->mEntryCount = lSize >> 2L;
	}

	return( lpHeader );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_BuildSymbolTable( const sProgramHeader * apProgram, const sProfilerHeader * apProfile )
* ACTION   : creates symbol table of count sorted symbols 
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

sProfilerSymbolTable *	Profiler_BuildSymbolTable( const sProgramHeader * apProgram, const sProfilerHeader * apProfile )
{
	sProfilerSymbolTable *	lpTable;
	sProgramSymbol *	lpSym;
	U32 *				lpEntries;
	U32					lValidCount;
	U32					lSymbolCount;
	U32					lTemp;
	U32					i,j;

	if( !apProgram )
	{
		return( 0 );
	}

	Endian_FromBigU32( &apProgram->mBSSSize );
	Endian_FromBigU32( &apProgram->mTextSize );
	Endian_FromBigU32( &apProgram->mDataSize );
	Endian_FromBigU32( &apProgram->mSymbolTableSize );




	lValidCount = 0;

	lSymbolCount = apProgram->mSymbolTableSize / sizeof(sProgramSymbol);

	lTemp  = (U32)apProgram;
	lTemp += sizeof(sProgramHeader);
	lTemp += apProgram->mTextSize;
	lTemp += apProgram->mDataSize;
	lpSym  = (sProgramSymbol *)lTemp;

	for( i=0; i<lSymbolCount; i++ )
	{
		Endian_FromBigU16( &lpSym[ i ].mType );
		Endian_FromBigU32( &lpSym[ i ].mValue );
		if( ( lpSym[ i ].mType & (dPROGRAM_SYMBOL_GLOBAL | dPROGRAM_SYMBOL_EXTERN | dPROGRAM_SYMBOL_DATA | dPROGRAM_SYMBOL_TEXT | dPROGRAM_SYMBOL_BSS ) )
			&& (lpSym[ i ].mName[ 0 ] != '.' ) )
		{
			lValidCount++;
		}
	}

	printf( "valid count :%ld\n", lValidCount );

	if( !lValidCount )
	{
		return( 0 );
	}

	lTemp  = sizeof(sProfilerSymbol);
	lTemp *= lValidCount+1;
	lTemp += sizeof(sProfilerSymbolHeader);
	lpTable = (sProfilerSymbolTable*)mMEMCALLOC( lTemp );

	lpTable->mHeader.mCount = lValidCount;
	j = 0;
	for( i=0; i<lSymbolCount; i++ )
	{
		if( ( lpSym[ i ].mType & (dPROGRAM_SYMBOL_GLOBAL | dPROGRAM_SYMBOL_EXTERN | dPROGRAM_SYMBOL_DATA | dPROGRAM_SYMBOL_TEXT | dPROGRAM_SYMBOL_BSS ) )
			&& (lpSym[ i ].mName[ 0 ] != '.' ) )
		{
			lpTable->mSymbol[ j ].mSymbol = lpSym[ i ];
			lpTable->mSymbol[ j ].mCount  = 0;
			j++;
		}
	}
	lpTable->mHeader.mSize = apProgram->mTextSize + apProgram->mDataSize + apProgram->mBSSSize;
	strcpy( lpTable->mSymbol[ lValidCount ].mSymbol.mName, "DA_END" );
	lpTable->mSymbol[ lValidCount ].mSymbol.mValue = lpTable->mHeader.mSize;

	
	printf( "program size %ld\n", lpTable->mHeader.mSize );
	printf( "pText %lX\n", apProfile->mpText );

	Profiler_SymbolTableAddressSort( lpTable );


	lTemp  = (U32)apProfile;
	lTemp += sizeof(sProfilerHeader);
	lpEntries = (U32*)lTemp;
	lpTable->mHeader.mHitCount = 0;
	for( i=0; i<apProfile->mEntryCount; i++ )
	{
		Endian_FromBigU32( &lpEntries[ i ] );
		Profiler_AddHit( lpTable, lpEntries[ i ] - (U32)apProfile->mpText );
	}

	Profiler_SymbolTableCountSort( lpTable );

	return( lpTable );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_AddHit( sProfilerSymbolTable * apTable, const U32 aAddress )
* ACTION   : adds a hit of function at aAddress to apTable
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_AddHit( sProfilerSymbolTable * apTable, const U32 aAddress )
{
	U32		i;
	U32		lStep;

	if( aAddress < apTable->mHeader.mSize )
	{
		lStep = (apTable->mHeader.mCount+1) >> 1;
		i     = lStep;

		while( lStep > 1 )
		{
			lStep = (lStep+1) >> 1;

			if( aAddress >= apTable->mSymbol[ i ].mSymbol.mValue )
			{
				if( aAddress < apTable->mSymbol[ i+1 ].mSymbol.mValue )
				{
					apTable->mHeader.mHitCount++;
					apTable->mSymbol[ i ].mCount++;
					return;
				}
				i += lStep;
			}
			else
			{
				i -= lStep;
			}
		}
		if( aAddress >= apTable->mSymbol[ i ].mSymbol.mValue )
		{
			if( aAddress < apTable->mSymbol[ i+1 ].mSymbol.mValue )
			{
				apTable->mHeader.mHitCount++;
				apTable->mSymbol[ i ].mCount++;
				return;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_SymbolAddressCmp( const void * apSym0,  const void * apSym1 )
* ACTION   : address comparison for symbol table qsort
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

int		Profiler_SymbolAddressCmp( const void * apSym0,  const void * apSym1 )
{
	S32	lVal;
	const sProfilerSymbol * lpSym0 = (const sProfilerSymbol *)apSym0;
	const sProfilerSymbol * lpSym1 = (const sProfilerSymbol *)apSym1;

	lVal  = lpSym0->mSymbol.mValue;
	lVal -= lpSym1->mSymbol.mValue;

	if( lVal > 0 )
	{
		return( 1 );
	}
	if( lVal < 0 )
	{
		return( -1 );
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_SymbolCountCmp( const void * apSym0,  const void * apSym1 )
* ACTION   : count comparison for symbol table qsort
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

int		Profiler_SymbolCountCmp( const void * apSym0,  const void * apSym1 )
{
	S32	lVal;
	const sProfilerSymbol * lpSym0 = (const sProfilerSymbol *)apSym0;
	const sProfilerSymbol * lpSym1 = (const sProfilerSymbol *)apSym1;

	lVal  = lpSym0->mCount;
	lVal -= lpSym1->mCount;

	if( lVal > 0 )
	{
		return( -1 );
	}
	if( lVal < 0 )
	{
		return( 1 );
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_SymbolTableAddressSort( sProfilerSymbolTable * apTable )
* ACTION   : sorts symbol table by address
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_SymbolTableAddressSort( sProfilerSymbolTable * apTable )
{
	qsort( &apTable->mSymbol[ 0 ], apTable->mHeader.mCount, sizeof(sProfilerSymbol), Profiler_SymbolAddressCmp );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_SymbolTableCountSort( sProfilerSymbolTable * apTable )
* ACTION   : sorts symbol table by count
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_SymbolTableCountSort( sProfilerSymbolTable * apTable )
{
	qsort( &apTable->mSymbol[ 0 ], apTable->mHeader.mCount, sizeof(sProfilerSymbol), Profiler_SymbolCountCmp );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_IsValid( const sProfilerHeader * apHeader )
* ACTION   : returns true if profiler file format is valid
* CREATION : 01.05.01 PNK
*-----------------------------------------------------------------------------------*/

U8		Profiler_IsValid( const sProfilerHeader * apHeader )
{
	if( !apHeader )
	{
		return( 0 );
	}
	return( (U8)(apHeader->mID == dPROFILER_ID) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profiler_Relocate( sProfilerHeader * apHeader )
* ACTION   : Profiler_Relocate
* CREATION : 07.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Profiler_Relocate( sProfilerHeader * apHeader )
{
	if( apHeader )
	{
		Endian_FromBigU32( &apHeader->mID ); 
		Endian_FromBigU32( &apHeader->mVersion ); 
		Endian_FromBigU16( &apHeader->mGameBuildHi ); 
		Endian_FromBigU16( &apHeader->mGameBuildLo ); 
		Endian_FromBigU32( &apHeader->mpText ); 
		Endian_FromBigU32( &apHeader->mEntryCount ); 
	}
}


/* ################################################################################ */
