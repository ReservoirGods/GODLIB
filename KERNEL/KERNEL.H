#ifndef	INCLUDED_KERNEL_H
#define	INCLUDED_KERNEL_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/PACKAGE.H>
#include	<GODLIB/CLOCK/CLOCK.H>
#include	<GODLIB/INPUT/INPUT.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	void	(*fKernel)( void );
typedef	U16		(*fKernelUpdate)( sInput * apInput );


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eKERNEL_CLOCK_APP,
	eKERNEL_CLOCK_FE,
	eKERNEL_CLOCK_GAME,
	eKERNEL_CLOCK_LEVEL,

	eKERNEL_CLOCK_LIMIT,
};

typedef enum eKERNEL_INPUT
{
	eKERNEL_INPUT_NORMAL,
	eKERNEL_INPUT_SAVING,
	eKERNEL_INPUT_LOADING
} eKERNEL_INPUT;


/* ###################################################################################
#  STRUCTS
################################################################################### */

#define	dKERNEL_TASK_PROTOTYPES( _aGame, _aTask )	void _aGame##_KernelTask_##_aTask##_Init( void ); void _aGame##_KernelTask_##_aTask##_DeInit( void ); U16 _aGame##_KernelTask_##_aTask##_Update( sInput * apInput );

typedef struct sKernelTask
{
	U32				mIndex;
	U32				mPackageFlags;
	fKernel			mfInit;
	fKernel			mfDeInit;
	fKernelUpdate	mfUpdate;
} sKernelTask;

#define	dKERNEL_PACKAGEDEF_LINE( _aGame, _aTask )	{ e##_aGame##_KernelTask_##_aTask, e##_aGame##_Package_##_aTask, _aGame##_KernelTask_##_aTask##_Init, _aGame##_KernelTask_##_aTask##_DeInit, _aGame##_KernelTask_##_aTask##_Update }

typedef struct	sKernelPackageDef
{
	U32		mIndex;
	char *	mpPackageName;
	char *	mpContextName;
} sKernelPackageDef;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		Kernel_Init( sKernelTask * apTasks, const U16 aTaskCount, sPackage * apPackages, sKernelPackageDef * apDef, const U16 aPackageLimit );
void		Kernel_DeInit( void );
void		Kernel_Main( void );

void		Kernel_RequestShutdown( void );

sClock *	Kernel_GetpClock( const U16 aClockIndex );
void		Kernel_GlobalPackagesLoad( const U32 aFlags );
void		Kernel_GlobalPackagesUnLoad( const U32 aFlags );
void		Kernel_InputsEnumerate( void );

void		Kernel_ClockStartCountDown( const U16 aIndex, const U8 aMinutes, const U8 aSeconds );

void	Kernel_InputBufferLoad( const char * apFileName );
void	Kernel_InputBufferSave( void );

void	Kernel_RequestInputLoad( const char * apFileName );


/* ################################################################################ */

#endif	/*	INCLUDED_KERNEL_H	*/
