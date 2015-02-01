/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: SND.C
::
:: SND chip routines
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"SND.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/MFP/MFP.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

char *	Snd_GetpFromLong( sSndHeader * apHeader, char * apID );
char *	Snd_GetpFromWord( sSndHeader * apHeader, char * apID );
void	Snd_GetTimerInfo( sSndTimer * apTimer, char * apTxt );
void	Snd_CallFunction( void (*aFunc)() );
void	Snd_Player( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_Init( void )
* ACTION   : inits mfp
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_Init()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_DeInit( void )
* ACTION   : deinits mfp
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_DeInit()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_GetInfo( sSndHeader * apHeader, sSndInfo * apInfo )
* ACTION   : fills in info block structure
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_GetInfo( sSndHeader * apHeader, sSndInfo * apInfo )
{
	char *	lpTxtA;
	char *	lpTxtB;
	char *	lpTxtC;
	char *	lpTxtD;
	char *	lpTxtV;

	Assert( apHeader );

	apInfo->mStart = (fSnd)(U32)&apHeader->mStart;
	apInfo->mStop  = (fSnd)(U32)&apHeader->mStop;
	apInfo->mPlay  = (fSnd)(U32)&apHeader->mPlay;

	apInfo->mpComposerTxt   = Snd_GetpFromLong( apHeader, "COMM" );
	apInfo->mpConversionTxt = Snd_GetpFromLong( apHeader, "CONV" );
	apInfo->mpRipperTxt     = Snd_GetpFromLong( apHeader, "RIPP" );
	apInfo->mpTitleTxt      = Snd_GetpFromLong( apHeader, "TITL" );

	lpTxtA = Snd_GetpFromWord( apHeader, "TA" );
	lpTxtB = Snd_GetpFromWord( apHeader, "TB" );
	lpTxtC = Snd_GetpFromWord( apHeader, "TC" );
	lpTxtD = Snd_GetpFromWord( apHeader, "TD" );
	lpTxtV = Snd_GetpFromWord( apHeader, "V!" );

	if( lpTxtA )
	{
		apInfo->mTimer.mType = eSND_TIMER_A;
		Snd_GetTimerInfo( &apInfo->mTimer, lpTxtA );
	}
	else if( lpTxtB )
	{
		apInfo->mTimer.mType = eSND_TIMER_B;
		Snd_GetTimerInfo( &apInfo->mTimer, lpTxtB );		
	}
	else if( lpTxtC )
	{
		apInfo->mTimer.mType = eSND_TIMER_C;
		Snd_GetTimerInfo( &apInfo->mTimer, lpTxtC );		
	}
	else if( lpTxtD )
	{
		apInfo->mTimer.mType = eSND_TIMER_D;
		Snd_GetTimerInfo( &apInfo->mTimer, lpTxtD );		
	}
	else if( lpTxtV )
	{
		apInfo->mTimer.mType = eSND_TIMER_VBI;
		apInfo->mTimer.mFreq = 50;
	}
	else
	{
		apInfo->mTimer.mType = eSND_TIMER_C;
		apInfo->mTimer.mData = 246;
		apInfo->mTimer.mDiv  = 7;
		apInfo->mTimer.mFreq = 50;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_GetpFromLong( sSndHeader * apHeader, char * apID )
* ACTION   : returns pointer to ascii parameter for field apID
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

char *	Snd_GetpFromLong( sSndHeader * apHeader, char * apID )
{
	U16		i;

	for( i=0; i<apHeader->mStart.mOffset; i++ )
	{
		if( 
				( apHeader->mData[ i   ] == apID[ 0 ] )
			&&	( apHeader->mData[ i+1 ] == apID[ 1 ] )
			&&	( apHeader->mData[ i+2 ] == apID[ 2 ] )
			&&	( apHeader->mData[ i+3 ] == apID[ 3 ] )
			)
		{
			return( &apHeader->mData[ i+4 ] );
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_GetpFromWord( sSndHeader * apHeader, char * apID )
* ACTION   : returns pointer to ascii parameter for field apID
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

char *	Snd_GetpFromWord( sSndHeader * apHeader, char * apID )
{
	U16		i;

	for( i=0; i<apHeader->mStart.mOffset; i++ )
	{
		if( 
				( apHeader->mData[ i   ] == apID[ 0 ] )
			&&	( apHeader->mData[ i+1 ] == apID[ 1 ] )
			)
		{
			return( &apHeader->mData[ i+2 ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_AsciiToValue( char * apTxt )
* ACTION   : reads ascii decimal number and returns U32 containing value
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

U32		Snd_AsciiToValue( char * apTxt )
{
	U32	lValue;

	lValue = 0;

	while( *apTxt )
	{
		lValue *= 10;
		lValue += *apTxt++ - '0';
	}

	return( lValue );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_GetTimerInfo( sSndTimer * apTimer, char * apTxt )
* ACTION   : reads timer info from SND header tag
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_GetTimerInfo( sSndTimer * apTimer, char * apTxt )
{
	U32	lHz;
	U32	lData;

	lHz    = Snd_AsciiToValue( apTxt );
	lData  = 12288L;
	lData /= lHz;
	lData &= 0xFFFFL;

	apTimer->mData = (U8)lData;
	apTimer->mDiv  = 7;
	apTimer->mFreq = (U16)(lHz);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_TuneInit( sSndInfo * apInfo )
* ACTION   : starts playing tune
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_TuneInit( sSndInfo * apInfo )
{
	sMfpTimer	lTimer;

	if( apInfo )
	{
		lTimer.mData       = apInfo->mTimer.mData;
		lTimer.mFreq       = apInfo->mTimer.mFreq;
		lTimer.mMode       = apInfo->mTimer.mDiv;
		lTimer.mfTimerFunc = Snd_Player;

		Snd_SetpPlayFunc( apInfo->mPlay );

		Snd_CallFunction( apInfo->mStart );
		Mfp_HookIntoTimerC( &lTimer );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Snd_TuneDeInit( sSndInfo * apInfo )
* ACTION   : stops playing tune
* CREATION : 24.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	Snd_TuneDeInit( sSndInfo * apInfo )
{
	if( apInfo )
	{
		Mfp_HookDisableTimerC();	
		Snd_CallFunction( apInfo->mStop );
	}
}


/* ################################################################################ */
