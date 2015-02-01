/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_GOD.H"

#include	"ACH_MAIN.H"

#include	<GODLIB\DRIVE\DRIVE.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\SYSTEM\SYSTEM.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_GodLib_Specs_Set( void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GodLib_AppInit( void )
* ACTION   : Achieve_GodLib_AppInit
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_GodLib_AppInit( void )
{
	Achieve_GodLib_Specs_Set();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_GodLib_AppDeInit( void )
* ACTION   : Achieve_GodLib_AppDeInit
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_GodLib_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Specs_Set( void )
* ACTION   : Achieve_Specs_Set
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_GodLib_Specs_Set( void )
{
	U32	lTemp;
	sAchieveSpecs	lSpecs;

	Memory_Clear( sizeof(sAchieveSpecs), &lSpecs );

	lTemp = Drive_GetDrive();
	if( lTemp < 2 )
	{
		lTemp = 1;
	}
	else
	{
		lTemp = 2;
	}

	lSpecs.mDrive   |= lTemp;
	lSpecs.mMachine |= (1 << System_GetMCH());
	lSpecs.mEmu     |= (1 << System_GetEMU());
	lSpecs.mCPU     |= (1 << System_GetCPU());
	lSpecs.mFPU     |= (1 << System_GetFPU());

	lTemp = System_GetTosVersion();
	lSpecs.mTosVersionLo |= (1 << (lTemp&0xF) );
	lTemp >>= 16L;
	lSpecs.mTosVersionHi |= (1 << (lTemp&0xF) );

	lSpecs.mRamST |= (U16)(System_GetRamST() >> 19L);
	lSpecs.mRamTT |= (U16)(System_GetRamTT() >> 19L);
}
