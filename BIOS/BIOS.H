#ifndef	INCLUDED_BIOS_H
#define	INCLUDED_BIOS_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dBIOS_DEV_PRINTER	0
#define	dBIOS_DEV_AUX		1
#define	dBIOS_DEV_CONSOLE	2
#define	dBIOS_DEV_MIDI		3
#define	dBIOS_DEV_IKBD		4
#define	dBIOS_DEV_RAW		5


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sBiosBPB
{
	U16	mSectorSize;
	U16	mSectorsPerCluster;
	U16	mClusterSize;
	U16	mRootDirectorySize;
	U16	mSectorsPerFAT;
	U16	mStartSecondFAT;
	U16	mStartDataFAT;
	U16	mClusterCount;
	U16	mFlags;
} sBiosBPB;

typedef	struct sBiosBasePage
{
	void *	mpStartTPA;
	void *	mpEndTPA;
	void *	mpText;
	U32		mTextLength;
	void *	mpData;
	U32		mDataLength;
	void *	mpBSS;
	U32		mBSSLength;
	void *	mpDTA;
	void *	mpParent;
	U32		mReserved0;
	char *	mpEnv;
	char	mReserved1[ 80 ];
	char	mCmdLine[ 128 ];
} sBiosBasePage;

typedef	struct sBiosMD
{
	struct	sBiosMD *	mpNext;
	void *				mpStart;
	S32					mLength;
	sBiosBasePage *		mpBasePage;
} sBiosMD;

typedef	struct sBiosMPB
{
	sBiosMD *	mpFreeList;
	sBiosMD *	mpAllocatedList;
	sBiosMD *	mpRoving;
} sBiosMPB;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

S32				Bios_Bconin( const S16 aDev );
S32				Bios_Bconout( const S16 aDev, const U16 aChar );
S32				Bios_Bconstat( const S16 aDev );
S32				Bios_Bcostat( const S16 aDev );

U32				Bios_Drvmap( void );

sBiosBPB *		Bios_Getbpb( const S16 aDev );
void			Bios_Getmpb( sBiosMPB * apMPB );

S32				Bios_Kbshift( const S16 aMode );

S32				Bios_Mediach( const S16 aDev );

S32				Bios_Rwabs( const S16 aMode, void * apBuffer, const S16 aCount, const S16 aRecNo, const S16 aDv, const U32 aLongRecNo );

void			(*Bios_Setexec( const U16 aNum, void (* apNewVec)(void) ))(void);

S32				Bios_Tickcal( void );


/* ################################################################################ */

#endif	/*	INCLUDED_BIOS_H */
