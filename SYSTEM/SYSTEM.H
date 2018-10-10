#ifndef	INCLUDED_SYSTEM_H
#define	INCLUDED_SYSTEM_H


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/COOKIE/COOKIE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

typedef enum eSYSTEM_MCH
{
	MCH_UNKNOWN,
	MCH_ST,
	MCH_STE,
	MCH_STBOOK,
	MCH_MEGASTE,
	MCH_TT,
	MCH_FALCON,
	MCH_MILAN,
	MCH_HADES,
	MCH_PHENIX,

	MCH_LIMIT
} eSYSTEM_MCH;


typedef enum eSYSTEM_CPU
{
	CPU_UNKNOWN,
	CPU_68000,
	CPU_68010,
	CPU_68020,
	CPU_68030,
	CPU_68040,
	CPU_68060,
	CPU_G4,

	CPU_LIMIT
} eSYSTEM_CPU;


typedef enum eSYSTEM_FPU
{
	FPU_NONE,
	FPU_UNKNOWN,
	FPU_SFP004,
	FPU_68881,
	FPU_68882,
	FPU_68040,

	FPU_LIMIT
} eSYSTEM_FPU;


typedef enum eSYSTEM_DSP
{
	DSP_NONE,
	DSP_UNKNOWN,
	DSP_56000,
	DSP_56001,

	DSP_LIMIT
} eSYSTEM_DSP;


typedef	enum eSYSTEM_VDO
{
	VDO_UNKNOWN,
	VDO_ST,
	VDO_STE,
	VDO_TT,
	VDO_FALCON,

	VDO_LIMIT
} eSYSTEM_VDO;


typedef	enum eSYSTEM_MON
{
	MON_TV,
	MON_RGB,
	MON_VGA,
	MON_MONO,
	MON_MULTISYNC,
	MON_HDTV,

	MON_LIMIT
} eSYSTEM_MON;


typedef	enum eSYSTEM_BLT
{
	BLT_NONE,
	BLT_BLITTER,

	BLT_LIMIT,
} eSYSTEM_BLT;

typedef	enum	eSYSTEM_EMU
{
	EMU_NONE,
	EMU_UNKNOWN,
	EMU_PACIFIST,
	EMU_STEEM,
	EMU_TOSBOX,

	EMU_LIMIT
} eSYSTEM_EMU;


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8	mVersionMajor;
	U8	mVersionMinor;
	U8	mSlowMotionFlag;
	U8	mSlowMotionSpeed;

	U8	mFastForwardFlag;
	U8	mMhz;
	U8	mDebugBuildFlag;
	U8	mSnapShotFlag;

	U16	mRunSpeed;			/*  8 */
	U16	mCurrentSpeed;		/* 10 */
	U32	mCycleCounter;		/* 12 */
	U32	mCycleCountAtVBL;	/* 16 */
	U32	mCycleCountAtHBL;	/* 20 */
	U16	mPad;				/* 24 */
	U8	mPad2;				/* 26 */
	U8	mColourDepth;		/* 27 */
	U8	mResolution;
} sSystemEmuDesc;


typedef struct
{
	uU32				mRefreshRate;
	uU32				mHblRate;
	U32					mMemory;
	U32					mRamST;
	U32					mRamTT;
	eSYSTEM_MCH			mMCH;
	eSYSTEM_CPU			mCPU;
	eSYSTEM_FPU			mFPU;
	eSYSTEM_DSP			mDSP;
	eSYSTEM_VDO			mVDO;
	eSYSTEM_MON			mMON;
	eSYSTEM_BLT			mBLT;
	eSYSTEM_EMU			mEMU;
	U16					mTosVersion;
	U16					mEmuVersion;
	sSystemEmuDesc *	mpEmuDesc;
} sSYSTEM;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		System_Init( void );
void		System_DeInit( void );

eSYSTEM_MCH	System_GetMCH( void );
eSYSTEM_CPU	System_GetCPU( void );
eSYSTEM_FPU	System_GetFPU( void );
eSYSTEM_DSP	System_GetDSP( void );
eSYSTEM_VDO	System_GetVDO( void );
eSYSTEM_MON	System_GetMON( void );
eSYSTEM_BLT	System_GetBLT( void );
eSYSTEM_EMU	System_GetEMU( void );
char *		System_GetNameCPU( void );
char *		System_GetNameDSP( void );
char *		System_GetNameFPU( void );
char *		System_GetNameMCH( void );
char *		System_GetNameMON( void );
char *		System_GetNameVDO( void );
char *		System_GetNameBLT( void );
char *		System_GetNameEMU( void );
uU32 *		System_GetRefreshRate( void );
uU32 *		System_GetHblRate( void );
U16			System_GetTosVersion( void );
U16			System_GetEmuVersion( void );
U32			System_GetMemory( void );
U32			System_GetRamST( void );
U32			System_GetRamTT( void );

U8			System_GetDebugBuildFlag( void );
U8			System_GetFastForwardFlag( void );
void		System_SetFastForwardFlag( U8 aFlag );
U8			System_GetMhz( void );
U8			System_GetSlowMotionFlag( void );
U8			System_GetSlowMotionSpeed( void );
U16			System_GetCurrentSpeed( void );
U16			System_GetRunSpeed( void );
U8			System_GetSnapShotFlag( void );
void		System_SetSnapShotFlag( const U8 aFlag );
U32			System_GetCycleCounter( void );

void		System_SetIML( U16 aIML );
U16			System_GetIML( void );

void		System_CalcInfo( void );
void		System_Calibrate( void );
void		System_CalibrateVbl( void );

void		System_DataCacheDisable( void );
void		System_DataCacheEnable( void );

void		System_InstructionCacheDisable( void );
void		System_InstructionCacheEnable( void );

void		System_SetCPUSpeed( const U16 aMhz );
sSystemEmuDesc *	System_GetpEmuDesc( void );


/* ################################################################################ */

#endif	/* INCLUDED_SYSTEM_H */
