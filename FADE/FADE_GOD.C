/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: FADE.C
::
:: Fade routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FADE.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/VIDEO/VIDEO.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sFade
{
	U16		mGamma;
	U32		mVblAdd;
	U32		mVblScale;
	U16 *	mpVblTmpPal;
	U16 *	mpVblOldPal;
	U16 *	mpVblTargetPal;
	U16		mVblOldPal[ 16 ];
	U16		mVblTmpPal[ 16 ];
	U16		mCurrentBasePal[ 16 ];
} sFade;


/* ###################################################################################
#  DATA
################################################################################### */
sFade	gFade;
U8	gFadeVblLockFlag;
U8	gFadeVblActiveFlag;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Fade_Vbl( void );
void	Fade_Vbl_C(void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_Init( void )
* ACTION   : Fade_Init
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_Init( void )
{
	gFadeVblLockFlag   = 0;
	gFadeVblActiveFlag = 0;
#ifndef	dGODLIB_PLATFORM_ATARI
	Vbl_AddCall( Fade_Vbl_C );
#else
	Vbl_AddCall( Fade_Vbl );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_DeInit( void )
* ACTION   : Fade_DeInit
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_DeInit( void )
{
	Vbl_RemoveCall( Fade_Vbl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_UpdateCurrentPal( void )
* ACTION   : Fade_UpdateCurrentPal
* CREATION : 14.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_UpdateCurrentPal( void )
{
	Fade_SetPalDirect( gFade.mCurrentBasePal );		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_IsVblFadeFinished( void )
* ACTION   : Fade_IsVblFadeFinished
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U8	Fade_IsVblFadeFinished( void )
{
	return( (U8)(0 == gFadeVblActiveFlag) );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_Main( U16 * apPal,U16 aFrames )
* ACTION   : Fade_Main
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_Main( U16 * apPal,U16 aFrames )
{
	U32	lFade;
	U32	lFadeAdd;
	U32	lPos;
	U16	lNewPal[ 16 ];
	U16	lOldPal[ 16 ];
	U16	i;

	Fade_SetCurrentBasePal( apPal );

	Video_GetPalST( lOldPal );

	lFade     = 0;
	lFadeAdd  = 0x1000000L;
	lFadeAdd /= aFrames;

	for( i=0; i<=aFrames; i++ )
	{
		lPos = lFade>>16L;
		if( lPos > 0x100 )
		{
			lPos = 0x100;
		}

		Fade_PalSTE( &lNewPal[0], &lOldPal[0], apPal, 16, (U16)lPos);
		Video_SetNextPalST( lNewPal );
		Vbl_WaitVbl();
		lFade += lFadeAdd;
	}

	Video_SetNextPalST( apPal );
	Vbl_WaitVbl();	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_StartVblFade( U16 * apPal,const U16 aFrames )
* ACTION   : Fade_StartVblFade
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_StartVblFade( U16 * apPal,const U16 aFrames )
{
	Fade_SetCurrentBasePal( apPal );

	gFadeVblActiveFlag = 0;
	Video_GetPalST( &gFade.mVblOldPal[ 0 ] );
	gFade.mpVblOldPal    = &gFade.mVblOldPal[ 0 ];
	gFade.mpVblTargetPal = apPal;
	gFade.mpVblTmpPal    = &gFade.mVblTmpPal[ 0 ];
	gFade.mVblScale      = 0;
	gFade.mVblAdd        = 0x1000000L;
	gFade.mVblAdd       /= aFrames;
	gFadeVblActiveFlag = 1;	

#ifdef	dGODLIB_PLATFORM_WIN
	Video_SetNextPalST( apPal );
#endif
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_SetPalDirect( U16 * apPal )
* ACTION   : Fade_SetPalDirect
* CREATION : 14.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_SetPalDirect( U16 * apPal )
{
	while( !Fade_IsVblFadeFinished() )
	{
		Vbl_WaitVbl();
	}

	Fade_SetCurrentBasePal( apPal );
	Fade_BuildGammaCorrectedPal( apPal, &gFade.mVblTmpPal[ 0 ] );
	Video_SetNextPalST( &gFade.mVblTmpPal[ 0 ] );
	Vbl_WaitVbl();	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_SetCurrentBasePal( const U16 * apPal )
* ACTION   : Fade_SetCurrentBasePal
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_SetCurrentBasePal( const U16 * apPal )
{
	U16	i;

	for( i=0; i<16; i++ )
	{
		gFade.mCurrentBasePal[ i ] = apPal[ i ];
	}
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_Vbl_C( void )
* ACTION   : Fade_Vbl_C
* CREATION : 15.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_Vbl_C( void )
{
	if(	gFadeVblActiveFlag )
	{
		if( !gFadeVblLockFlag )
		{
			gFadeVblLockFlag = 1;

			gFade.mVblScale += gFade.mVblAdd;

			if( gFade.mVblScale >= 0x1000000L )
			{
				gFade.mVblScale    = 0x1000000L;
				gFadeVblActiveFlag = 0;
			}

			Fade_PalSTE( gFade.mVblTmpPal, gFade.mVblOldPal, gFade.mpVblTargetPal, 16, (U16)(gFade.mVblScale>>16L) );

			Video_SetNextPalST( gFade.mVblTmpPal );

			gFadeVblLockFlag = 0;
		}
	}
}


/* ################################################################################ */
