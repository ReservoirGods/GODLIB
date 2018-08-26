#ifndef	INCLUDED_PROGRAM_H
#define	INCLUDED_PROGRAM_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dPROGRAM_SYMBOL_DEFINED		0x8000
#define	dPROGRAM_SYMBOL_EQUATED		0x4000
#define	dPROGRAM_SYMBOL_GLOBAL		0x2000
#define	dPROGRAM_SYMBOL_REGISTER	0x1000
#define	dPROGRAM_SYMBOL_EXTERN		0x0800
#define	dPROGRAM_SYMBOL_DATA		0x0400
#define	dPROGRAM_SYMBOL_TEXT		0x0200
#define	dPROGRAM_SYMBOL_BSS			0x0100

#define	dPROGRAM_MAGIC				0x601A

#define dPROGRAM_FLAG_FASTLOAD		(1<<0)
#define dPROGRAM_FLAG_PRG_FASTRAM	(1<<1)
#define dPROGRAM_FLAG_MEM_FASTRAM	(1<<2)
#define dPROGRAM_FLAGMASK_MEMPRT	(3<<3)
#define dPROGRAM_FLAG_MEM_PRIVATE	(0<<3)
#define dPROGRAM_FLAG_MEM_GLOBAL	(1<<3)
#define dPROGRAM_FLAG_MEM_SUPER		(2<<3)
#define dPROGRAM_FLAG_MEM_READONLY	(3<<3)
#define dPROGRAM_FLAG_SHAREDTEXT	(12<<3)
#define dPROGRAM_FLAGMASK_TPA		(7<<28)


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16	mMagic;
	U32	mTextSize;
	U32	mDataSize;
	U32	mBSSSize;
	U32	mSymbolTableSize;
	U32 mReserved1;
	U32 mFlags;
	U16	mRelocationFlag;
} sProgramHeader;

typedef	struct
{
	char	mName[ 8 ];
	U16		mType;
	U32		mValue;
} sProgramSymbol;

typedef struct
{
	void *	mpLowTPA;
	void *	mpHiTPA;
	void *	mpText;
	U32		mTextLength;
	void *	mpData;
	U32		mDataLength;
	void *	mpBSS;
	U32		mBSSLength;
	void *	mpDTA;
	void *	mpParentBasepage;
	U32		mReserved0;
	char *	mpEnvironment;
	char	mReserved1[80];
	char	mCommandLine[128];
} sBasePage;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Program_Relocate( const sProgramHeader * apHeader );

U8				Program_IsValid( const sProgramHeader * apHeader );

sBasePage *		Program_Load( const char * apFileName );
void			Program_Init( sBasePage * apPage );
void			Program_UnLoad( sBasePage * apPage );
void			Program_Execute( sBasePage * apPage, const char * apCmdLine );

/* ################################################################################ */

#endif	/*	INCLUDED_PROGRAM_H */
