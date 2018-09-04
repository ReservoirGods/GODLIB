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
#include	"../FILE/FILE.H"
#include	"../MEMORY/MEMORY.H"


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

#if defined(dGODLIB_PLATFORM_ATARI)
extern sBasePage * _BasPag;
#else
sBasePage	gBasePageWin;
sBasePage * _BasPag = &gBasePageWin;
#endif

U8	gProgramArgvSpace[ 1024 ];

void	Program_Execute_Internal( const sBasePage * apHeader, const char * apCmdLine );


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

	U32		lTextSize;
	U32		lDataSize;
	U32		lSymbolTableSize;

/*	if( apHeader->mRelocationFlag )*/
	{

		Endian_ReadBigU32( &apHeader->mTextSize, lTextSize );
		Endian_ReadBigU32( &apHeader->mDataSize, lDataSize );
		Endian_ReadBigU32( &apHeader->mSymbolTableSize, lSymbolTableSize );

		if( !apHeader )
		{
			return;
		}

		lpText  = (U8*)apHeader;
		lpText += sizeof( sProgramHeader );

		lpRelocTable  = lpText;
		lpRelocTable += lTextSize;
		lpRelocTable += lDataSize;
		lpRelocTable += lSymbolTableSize;

	/*-	lOffset = *(U32*)lpRelocTable;*/
		Endian_ReadBigU32( ((U32*)lpRelocTable), lOffset );
		lpRelocTable += 4;

		lMax = lTextSize + lDataSize;

		if( lOffset )
		{
			U32 lAdr;
			Endian_ReadBigU32( ((U32*)&lpText[lOffset]), lAdr );
			lAdr += (U32)lpText;
			Endian_WriteBigU32( ((U32*)&lpText[lOffset]), lAdr );
	/*		*(U32*)&lpText[ lOffset ] += (U32)lpText; */
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
						Endian_ReadBigU32( ((U32*)&lpText[lOffset]), lAdr );
						lAdr += (U32)lpText;
						Endian_WriteBigU32( ((U32*)&lpText[lOffset]), lAdr );
					}
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
	U16 lMagic;
	if( !apHeader )
	{
		return( 0 );
	}
	Endian_ReadBigU16( &apHeader->mMagic, lMagic);
	return( (U8)( lMagic == dPROGRAM_MAGIC) );
}

sBasePage *	Program_Load( const char * apFileName )
{
	U8 * lpData = 0;
	sProgramHeader lHeader;
	sFileHandle lHandle;
	U32 lLen;
	U32 lBSSSize;

	U32 lFileSize = File_GetSize(apFileName);

	if( lFileSize < sizeof(sProgramHeader ))
		return(0);

	lHandle = File_Open(apFileName);
	lLen = File_Read(lHandle,sizeof(sProgramHeader),&lHeader);

	if( lLen == sizeof(sProgramHeader) && Program_IsValid(&lHeader) )
	{
		lFileSize -= sizeof(sProgramHeader);
		Endian_ReadBigU32( &lHeader.mBSSSize, lBSSSize );
		lLen = lFileSize + lBSSSize + sizeof(sBasePage);
		lpData = (U8 *)mMEMALLOC( lLen );
		if( lpData )
		{
			sProgramHeader * lpHead = (sProgramHeader *)&lpData[ sizeof(sBasePage) - sizeof(sProgramHeader)];
			Memory_Copy( sizeof(sProgramHeader), &lHeader, lpHead);
			lLen = File_Read( lHandle, lFileSize, &lpHead[1] );
			if( lFileSize == lLen )
			{
				Program_Init( (sBasePage*)lpData );
			}
		}
	}

	File_Close(lHandle);

	return( (sBasePage*)lpData );
}


void		Program_Init( sBasePage * apPage )
{
	U32		lTextSize;
	U32		lDataSize;
	U32		lBSSSize;
	U8 *	lpData = (U8*)apPage;

	sBasePage * lpParent = (sBasePage*)_BasPag;
	sProgramHeader * lpHead = (sProgramHeader *)&lpData[ sizeof(sBasePage) - sizeof(sProgramHeader)];

	Endian_ReadBigU32( &lpHead->mTextSize, lTextSize );
	Endian_ReadBigU32( &lpHead->mDataSize, lDataSize );
	Endian_ReadBigU32( &lpHead->mBSSSize, lBSSSize );

	Program_Relocate( lpHead );

	apPage->mpLowTPA = apPage;
	apPage->mpHiTPA = lpParent->mpHiTPA;
	apPage->mpText = (U8*)&apPage[1];
	apPage->mTextLength = lTextSize;
	apPage->mpData = ((U8*)apPage->mpText) + lTextSize;
	apPage->mDataLength = lDataSize;
	apPage->mpBSS = ((U8*)apPage->mpData) + lDataSize;
	apPage->mBSSLength = lBSSSize;
	apPage->mpParentBasepage = lpParent;
	apPage->mpDTA = &apPage->mReserved1;
	apPage->mpEnvironment = lpParent->mpEnvironment;
	apPage->mCommandLine[0] = 0;
	apPage->mCommandLine[1] = 0;
}


void				Program_UnLoad( sBasePage * apHeader )
{
	mMEMFREE( apHeader );
}

void				Program_Execute( sBasePage * apPage, const char * apCmdLine, const char * apParentApplicationName )
{
	int i;
	if( !apPage ) return;
	if( apCmdLine )
	{
		for( i=0; (i<125) && apCmdLine[i]; apPage->mCommandLine[i+1] = apCmdLine[i], i++ );
		apPage->mCommandLine[i+1]=0;
		apPage->mCommandLine[0]=(U8)i;
		if( (i==125) && apCmdLine[i-1] )
		{
			const char * lpARGV="ARGV=";
			U16 a = 0;
			if( _BasPag && _BasPag->mpEnvironment )
			{
				for( a = 0; _BasPag->mpEnvironment[ a ] && a < sizeof( gProgramArgvSpace ) - 1; gProgramArgvSpace[a++] = _BasPag->mpEnvironment[ a ] );
			}
			gProgramArgvSpace[ a++ ] = 0;
			for( i=0; lpARGV[i] && a<sizeof(gProgramArgvSpace)-1; gProgramArgvSpace[ a++ ] = lpARGV[ i++] );
			gProgramArgvSpace[ a++ ] =0;
			for( i=0; apParentApplicationName[i] && a<sizeof(gProgramArgvSpace)-1; gProgramArgvSpace[ a++ ] = apParentApplicationName[ i++ ] );
			gProgramArgvSpace[ a++ ] =0;
			for( i=0; apCmdLine[i] && a<sizeof(gProgramArgvSpace)-2; gProgramArgvSpace[ a++ ] = ' ' == apCmdLine[ i ] ? 0 : apCmdLine [ i ], i++ );
			gProgramArgvSpace[ a++ ] =0;
			gProgramArgvSpace[ a++ ] =0;
			apPage->mpEnvironment = (char*)&gProgramArgvSpace[0];
			apPage->mCommandLine[ 0 ] = 127;
		}
	}

	Memory_Clear( apPage->mBSSLength, apPage->mpBSS );
#if defined(dGODLIB_PLATFORM_ATARI)
	Program_Execute_Internal( apPage, apCmdLine );
#else
	(void)apCmdLine;
#endif
}


/* ################################################################################ */
