#ifndef	INCLUDED_GEMDOS_H
#define	INCLUDED_GEMDOS_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGEMDOS_DEFAULT_DRIVE	0

#define	dGEMDOS_DEV_READY		-1
#define	dGEMDOS_DEV_BUSY		0

#define	dGEMDOS_DEV_INSTALL		0xDE02
#define	dGEMDOS_DEV_NEWTTY		0xDE00
#define	dGEMDOS_DEV_NEWBIOS		0xDE01

#define	dGEMDOS_DP_INQUIRE		-1
#define	dGEMDOS_DP_IOPEN		0
#define	dGEMDOS_DP_MAXLINKS		1
#define	dGEMDOS_DP_PATHMAX		2
#define	dGEMDOS_DP_NAMEMAX		3
#define	dGEMDOS_DP_ATOMIC		4
#define	dGEMDOS_DP_TRUNC		5
#define	dGEMDOS_DP_CASE			6

#define	dGEMDOS_F_DUPFD			0
#define	dGEMDOS_F_GETFD			1
#define	dGEMDOS_F_SETFD			2
#define	dGEMDOS_F_GETFL			3
#define	dGEMDOS_F_SETFL			4
#define	dGEMDOS_F_GETLK			5
#define	dGEMDOS_F_SETLK			6
#define	dGEMDOS_F_SETLKW		6
#define	dGEMDOS_FSTAT			0x4600
#define	dGEMDOS_FIONREAD		0x4601
#define	dGEMDOS_FIONWRITE		0x4602
#define	dGEMDOS_SHMGETBLK		0x4D01
#define	dGEMDOS_PPROCADDR		0x5001
#define	dGEMDOS_PBASEADDR		0x5002
#define	dGEMDOS_PCTXSIZE		0x5003
#define	dGEMDOS_PSETFLAGS		0x5004
#define	dGEMDOS_PGETFLAGS		0x5005
#define	dGEMDOS_PTRACEGFLAGS	0x5006
#define	dGEMDOS_PTRACESFLAGS	0x5006
#define	dGEMDOS_PTRACEGO		0x5008
#define	dGEMDOS_PTRACEFLOW		0x5009
#define	dGEMDOS_PTRACESTEP		0x500A
#define	dGEMDOS_PLOADINFO		0x500C
#define	dGEMDOS_TIOCGETP		0x5400
#define	dGEMDOS_TIOCSETP		0x5401
#define	dGEMDOS_TIOCGETC		0x5402
#define	dGEMDOS_TIOCSETC		0x5403
#define	dGEMDOS_TIOCGLTC		0x5404
#define	dGEMDOS_TIOCSLTC		0x5405
#define	dGEMDOS_TIOCGPGRP		0x5406
#define	dGEMDOS_TIOCSPGRP		0x5407
#define	dGEMDOS_TIOCSTOP		0x5409
#define	dGEMDOS_TIOCSTART		0x540A
#define	dGEMDOS_TIOCGWINSZ		0x540B
#define	dGEMDOS_TIOCWINSZ		0x540C
#define	dGEMDOS_TIOCGXKEY		0x540D
#define	dGEMDOS_TIOCSXKEY		0x540E
#define	dGEMDOS_TIOCIBAUD		0x5412
#define	dGEMDOS_TIOCOBAUD		0x5413
#define	dGEMDOS_TIOCCBRK		0x5415
#define	dGEMDOS_TIOCGFLAGS		0x5416
#define	dGEMDOS_TIOCSFLAGS		0x5417
#define	dGEMDOS_TCURSOFF		0x6300
#define	dGEMDOS_TCURSON			0x6301
#define	dGEMDOS_TCURSBLINK		0x6302
#define	dGEMDOS_TCURSSTEADY		0x6303
#define	dGEMDOS_TCURSSRATE		0x6304
#define	dGEMDOS_TCURSGRATE		0x6305

#define	dGEMDOS_FA_READONLY		1
#define	dGEMDOS_FA_HIDDEN		2
#define	dGEMDOS_FA_SYSTEM		4
#define	dGEMDOS_FA_VOLUME		8
#define	dGEMDOS_FA_DIR			16
#define	dGEMDOS_FA_ARCHIVE		32

#define	dGEMDOS_FS_INSTALL		0xF001
#define	dGEMDOS_FS_MOUNT		0xF002
#define	dGEMDOS_FS_UNMOUNT		0xF003
#define	dGEMDOS_FS_UNINSTALL	0xF004

#define	dGEMDOS_GSH_CONIN		0
#define	dGEMDOS_GSH_CONOUT		1
#define	dGEMDOS_GSH_AUX			2
#define	dGEMDOS_GSH_PRN			3
#define	dGEMDOS_GSH_BIOSCON		-1
#define	dGEMDOS_GSH_BIOSAUX		-2
#define	dGEMDOS_GSH_BIOSPRN		-3
#define	dGEMDOS_GSH_MIDIIN		-4
#define	dGEMDOS_GSH_MIDIOUT		-5

#define	dGEMDOS_LIM_MAXTIME		1
#define	dGEMDOS_LIM_MAXMEM		2
#define	dGEMDOS_LIM_MAXMALLOC	3

#define	dGEMDOS_MSG_READ		0
#define	dGEMDOS_MSG_WRITE		1
#define	dGEMDOS_MSG_READWRITE	2

#define	dGEMDOS_MX_STRAM		0
#define	dGEMDOS_MX_TTRAM		1
#define	dGEMDOS_MX_PREFSTRAM	2
#define	dGEMDOS_MX_PREFTTRAM	3
#define	dGEMDOS_MX_HEADER		0
#define	dGEMDOS_MX_PRIVATE		1
#define	dGEMDOS_MX_GLOBAL		2
#define	dGEMDOS_MX_SUPERVISOR	3
#define	dGEMDOS_MX_READABLE		4

#define	dGEMDOS_PE_LOADGO		0
#define	dGEMDOS_PE_LOAD			3
#define	dGEMDOS_PE_GO			4
#define	dGEMDOS_PE_BASEPAGE		5
#define	dGEMDOS_PE_GOTHENFREE	6
#define	dGEMDOS_PE_CLOADGO		100
#define	dGEMDOS_PE_CGO			104
#define	dGEMDOS_PE_NOSHARE		106
#define	dGEMDOS_PE_REPLACE		200

#define	dGEMDOS_PRU_KERNELTIME			0
#define	dGEMDOS_PRU_PROCESSTIME			1
#define	dGEMDOS_PRU_CHILDKERNELTIME		2
#define	dGEMDOS_PRU_CHILDPROCESSTIME	3
#define	dGEMDOS_PRU_MEMORY				4

#define	dGEMDOS_PW_NOBLOCK		1
#define	dGEMDOS_PW_STOPPED		2

#define	dGEMDOS_S_IRUSR			0x100
#define	dGEMDOS_S_IWUSR			0x80
#define	dGEMDOS_S_IXUSR			0x40
#define	dGEMDOS_S_IRGRP			0x20
#define	dGEMDOS_S_IWGRP			0x10
#define	dGEMDOS_S_IXGRP			0x8
#define	dGEMDOS_S_IROTH			0x4
#define	dGEMDOS_S_IWOTH			0x2
#define	dGEMDOS_S_IXWOTH		0x1

#define	dGEMDOS_S_READ			0x0
#define	dGEMDOS_S_WRITE			0x1
#define	dGEMDOS_S_READWRITE		0x2
#define	dGEMDOS_S_DENYREADWRITE	0x10
#define	dGEMDOS_S_DENYWRITE		0x20
#define	dGEMDOS_S_DENYREAD		0x30
#define	dGEMDOS_S_DENYNONE		0x40

#define	dGEMDOS_SEEK_SET		0x0
#define	dGEMDOS_SEEK_CUR		0x1
#define	dGEMDOS_SEEK_END		0x2

#define	dGEMDOS_SEM_CREATE		0x0
#define	dGEMDOS_SEM_DESTROY		0x1
#define	dGEMDOS_SEM_LOCK		0x2
#define	dGEMDOS_SEM_UNLOCK		0x3

#define	dGEMDOS_SYS_MAXINQ		-1
#define	dGEMDOS_SYS_MAXREGIONS	0
#define	dGEMDOS_SYS_MAXCOMMAND	1
#define	dGEMDOS_SYS_MAXFILES	2
#define	dGEMDOS_SYS_MAXGROUPS	3
#define	dGEMDOS_SYS_MAXPROCS	4

#define	dGEMDOS_SUP_SET			0x0
#define	dGEMDOS_SUP_INQUIRE		0x1

#define	dGEMDOS_TERM_OK			0
#define	dGEMDOS_TERM_ERROR		1
#define	dGEMDOS_TERM_BADPARAMS	2
#define	dGEMDOS_TERM_CRASH		-1
#define	dGEMDOS_TERM_CTRLC		-32

#define	dGEMDOS_TTY_COOKED		0x1
#define	dGEMDOS_TTY_ECHO		0x2


/* ###################################################################################
#  TYPEDEF
################################################################################### */

typedef	void	(*fGemDosPsignal)( const U32 aVal );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sGemDosBasePage
{
	U32							mLowTPA;
	U32							mHiTPA;
	U8 *						mpText;
	U32							mTextSize;
	U8 *						mpData;
	U32							mDataSize;
	U8 *						mpBSS;
	U32							mBSSSize;
	struct sGemDosDTA *			mpDTA;
	struct sGemDosBasePage *	mpParent;
	U32							mReserved;
	char *						mpEnvString;
	U8							mUndef[ 80 ];
	char						mCmdLine[ 128 ];
} sGemDosBasePage;

typedef struct sGemDosDiskInfo
{
	U32		mFreeClusterCount;
	U32		mTotalClusterCount;
	U32		mSectorSize;
	U32		mSectorsPerCluster;
} sGemDosDiskInfo;


typedef	struct sGemDosDateTime
{
/*	S32	Hour : 5;
	S32	Minute : 6;
	S32	Second : 5;
	S32	Year : 7;
	S32	Month : 4;
	S32	Day : 4;
*/
	U32	mPackedDateTime;
} sGemDosDateTime;

typedef	struct sGemDosDTA
{
	U8		mReserved[ 21 ];
	U8		mAttrib;
	U16		mTime;
	U16		mDate;
	U32		mLength;
	char	mFileName[ 14 ];
} sGemDosDTA;

typedef	struct sGemDosXATTR
{
	U16	mMode;
	U32	mIndex;
	U16	mDev;
	U16	mReserved0;
	U16	mLinkCount;
	U16	mUID;
	U16	mGID;
	U32	mSize;
	U32	mBlockSize;
	U32	mBlockCount;
	U16	mModTime;
	U16	mModDate;
	U16	mAccessTime;
	U16	mAccessDate;
	U16	mCreationTime;
	U16	mCreationDate;
	U16	mAttribs;
	U16	mReserved1;
	U32	mReserved2;
	U32	mReserved3;
} sGemDosXATTR;

typedef struct sGemDosPMSG
{
	U32	mUser1;
	U32	mUser2;
	U16	mPID;
} sGemDosPMSG;

typedef struct sGemDosSigAction
{
	U32	mHandler;
	U16	mMask;
	U16	mFlags;
} sGemDosSigAction;

typedef	struct sGemDosFLOCK
{
	U16	mType;
	U16	mWhence;
	U32	mStart;
	U32	mLength;
	U16	mPID;
} sGemDosFLOCK;

typedef struct sGemDosPloadInfo
{
	U16		mFrameLength;
	char *	mpCmdLine;
	char *	mpFileName;
} sGemDosPloadInfo;

typedef	struct sGemDosSGTTYB
{
	char	mIspeed;
	char	mOspeed;
	char	mErase;
	char	mKill;
	U16		mFlags;
} sGemDosSGTTYB;

typedef	struct sGemDosTCHARS
{
	char	mIntr;
	char	mQuit;
	char	mStart;
	char	mStop;
	char	mEof;
	char	mBrk;
} sGemDosTCHARS;

typedef	struct sGemDosLTCHARS
{
	char	mSusp;
	char	mDsusp;
	char	mLineRedraw;
	char	mFlush;
	char	mWordErase;
	char	mLineNext;
} sGemDosLTCHARS;

typedef	struct sGemDosWinSize
{
	U16	mRowCount;
	U16	mColCount;
	U16	mWidth;
	U16	mHeight;
} sGemDosWinSize;

typedef struct sGemDosXKEY
{
	U16		mNum;
	char	mDef[ 8 ];
} sGemDosXKEY;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

S16				GemDos_Cauxin( void );
S16				GemDos_Cauxis( void );
S16				GemDos_Cauxos( void );
S16				GemDos_Cauxout( const U16 aChar );
S32				GemDos_Cconin( void );
S16				GemDos_Cconis( void );
S16				GemDos_Cconos( void );
S16				GemDos_Cconout( const U16 aChar );
void			GemDos_Cconrs( char * apString );
void			GemDos_Cconws( const char * apString );
S32				GemDos_Cnecin( void );
S16				GemDos_Cprnout( const U16 aChar );
S32				GemDos_Crawcin( void );
S32				GemDos_Crawio( const U16 aChar );

S32				GemDos_Dclosedir( const S32 aDirHandle );
S32				GemDos_Dcntl( const S16 aCmd, const char * apName, const S32 aArg );
S32				GemDos_Dcreate( const char * apPath );
S32				GemDos_Ddelete( const char * apPath );
S32				GemDos_Dfree( sGemDosDiskInfo * apInfo, const U16 aDrive );
U16				GemDos_Dgetdrv( void );
S32				GemDos_Dgetpath( char * apPath, const U16 aDrive );
S32				GemDos_Dlock( const U16 aMode, const U16 aDrive );
S32				GemDos_Dopendir( const char * apDir, const U16 aFlag );
S32				GemDos_Dpathconf( const char * apFileSys, const U16 aMode );
S32				GemDos_Dreaddir( const U16 aLen, const S32 aDirHandle, char * apBuffer );
S32				GemDos_Drewinddir( const S32 aHandle );
S32				GemDos_Dsetdrv( const U16 aDrive );
S32				GemDos_Dsetpath( const char * apPath );

S32				GemDos_Fattrib( const char * apFileName, const U16 aFlag, const U16 aAttr );
S32				GemDos_Fchmod( const char * apFileName, const U16 aMode );
S32				GemDos_Fchown( const char * apFileName, const U16 aUser, const U16 aGroup );
S32				GemDos_Fclose( const S16 aHandle );
S32				GemDos_Fcntl( const S16 aHandle, const S32 aArg, const U16 aCmd );
S32				GemDos_Fcreate( const char * apFileName, const S16 aHandle );
S32				GemDos_Fdatime( sGemDosDateTime * apDateTime, const S16 aHandle, const U16 aFlag );
S32				GemDos_Fdelete( const char * apFileName );
S32				GemDos_Fdup( const S16 aHandle );
S32				GemDos_Fforce( const S16 aOldHandle, const S16 aNewHandle );
S32				GemDos_Fgetchar( const S16 aHandle, const U16 aMode );
sGemDosDTA *	GemDos_Fgetdta( void );
S32				GemDos_Finstat( const S16 aHandle );
S32				GemDos_Flink( const char * apOldFileName, char * apNewFileName );
S32				GemDos_Flock( const S16 aHandle, const U16 aMode, const U32 aStart, const U32 aLength );
S32				GemDos_Fmidipipe( const U16 aPID, const S16 aInHandle, const S16 aOutHandle );
S32				GemDos_Fopen( const char * apFileName, const U16 aMode );
S32				GemDos_Foutstat( const S16 aHandle );
S32				GemDos_Fpipe( S16 * apHandles );
S32				GemDos_Fputchar( const S16 aHandle, const U32 aChar, const U16 aMode );
S32				GemDos_Fread( const S16 aHandle, const U32 aLength, void * apBuffer );
S32				GemDos_Freadlink( const U16 aBufferSize, char * apBuffer, const char * apName );
S32				GemDos_Frename( const U16 aNull, const char * apOldName, const char * apNewName );
S32				GemDos_Fseek( const U32 aOffset, const S16 aHandle, const U16 aMode );
S32				GemDos_Fselect( const U16 aTimeOut, const U32 * apRFDS, const U32 * apWFDS, const U32 aNull );
void			GemDos_Fsetdta( const sGemDosDTA * apDTA );
S16				GemDos_Fsfirst( const char * apFileSpec, const U16 aAttribs );
S16				GemDos_Fsnext( void );
S32				GemDos_Fsymlink( const char * apOldName, const char * apNewName );
S32				GemDos_Fwrite( const S16 aHandle, const U32 aCount, void * apBuffer );
S32				GemDos_Fxattr( const U16 aFlag, const char * apName, sGemDosXATTR * apXATTR );

S32				GemDos_Maddalt( void * apStar, const U32 aSize );
void *			GemDos_Malloc( const U32 aAmount );
S16				GemDos_Mfree( void * apAdr );
S16				GemDos_Mshrink( void * apStartAdr, const U32 aNewSize );
void *			GemDos_Mxalloc( const U32 aAmount, const U16 aMode );

void			GemDos_Pause( void );
S16				GemDos_Pdomain( const S16 aDomain );
S32				GemDos_Pexec( const U16 aMode, const char * apFileName, const char * apCmdLine, const char * apEnv );
S16				GemDos_Pfork( void );
S16				GemDos_Pgetegid( void );
S16				GemDos_Pgeteuid( void );
S16				GemDos_Pgetgid( void );
S16				GemDos_Pgetpgrp( void );
S16				GemDos_Pgetpid( void );
S16				GemDos_Pgetppid( void );
S16				GemDos_Pgetuid( void );
S16				GemDos_Pkill( const S16 aPID, const S16 aSignal );
S16				GemDos_Pmsg( const U16 aMode, const U32 aMboxid, sGemDosPMSG * apMsg );
S16				GemDos_Pnice( const S16 aDelta );
S32				GemDos_Prenice( const S16 aPID, const S16 aDelta );
void			GemDos_Prusage( U32 * apResInfo );
S32				GemDos_Psemaphoe( const U16 aMode, const U32 aID, const U32 aTimeOut );
S16				GemDos_Psetgid( const U16 aGID );
S32				GemDos_Psetlimit( const U16 aLimit, const U32 aValue );
S32				GemDos_Psetpgrp( const U16 aPID, const S16 aNewGrp );
S16				GemDos_Psetuid( const U16 aPID );
S32				GemDos_Psigaction( const U16 aSignal, const sGemDosSigAction * apAct, const sGemDosSigAction * apOact );
S32				GemDos_Psigblock( const U32 aMask );
S32				GemDos_Psignal( const S16 aSignal, fGemDosPsignal afPsignal );
S32				GemDos_Psigpause( const S32 aMask );
S32				GemDos_Psigpending( void );
S32				GemDos_Psigreturn( void );
S32				GemDos_Psigsetmask( const U32 aMask );
void			GemDos_Pterm( const S16 aRetCode );
void			GemDos_Pterm0( void );
void			GemDos_Ptermres( const U32 aSize, const S16 aRetCode );
S16				GemDos_Pumask( const S16 aMode );
S32				GemDos_Pusrval( const S32 aVal );
S16				GemDos_Pvfork( void );
S32				GemDos_Pwait( void );
S32				GemDos_Pwait3( const S16 aFlag, const U32 * apUsage );
S32				GemDos_Pwaitpid( const S16 aPid, const S16 aFlag, const U32 * apUsage );

void			GemDos_Salert( const char * apStr );
void *			GemDos_Super( void * apStack );
U16				GemDos_Sversion( void );
void			GemDos_Syield( void );
S32				GemDos_Sysconf( const S16 aInq );

S32				GemDos_Talarm( const S32 aTime );
U16				GemDos_Tgetdate( void );
U16				GemDos_Tgettime( void );
S16				GemDos_Tsetdate( const U16 aDate );
S16				GemDos_Tsettime( const U16 aDate );


/* ################################################################################ */

#endif	/*	INCLUDED_GEMDOS_H */
