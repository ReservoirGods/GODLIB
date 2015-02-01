/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"INPUT.H"

#include	<GODLIB/IKBD/IKBD.H>


/* ###################################################################################
#  DATA
################################################################################### */

sTagString	gInputTypeNames[ eINPUTTYPE_LIMIT ] =
{
	{	eINPUTTYPE_IKBD,		"KEYS"	},
	{	eINPUTTYPE_MOUSE,		"MOUSE"	},
	{	eINPUTTYPE_JOY0,		"JOY0"	},
	{	eINPUTTYPE_JOY1,		"JOY1"	},
	{	eINPUTTYPE_PADA,		"PADA"	},
	{	eINPUTTYPE_PADB,		"PADB"	},

	{	eINPUTTYPE_TAP0_PADA,	"TAP0A"	},
	{	eINPUTTYPE_TAP0_PADB,	"TAP0B"	},
	{	eINPUTTYPE_TAP0_PADC,	"TAP0C"	},
	{	eINPUTTYPE_TAP0_PADD,	"TAP0D"	},

	{	eINPUTTYPE_TAP1_PADA,	"TAP1A"	},
	{	eINPUTTYPE_TAP1_PADB,	"TAP1B"	},
	{	eINPUTTYPE_TAP1_PADC,	"TAP1C"	},
	{	eINPUTTYPE_TAP1_PADD,	"TAP1D"	},

	{	eINPUTTYPE_AI,			"AI"	},

	{	eINPUTTYPE_NONE,		"NONE"	},
};

char * 	gInputStatusChars = "LRDUABPOQ";


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Input_ProcessMouse( sInput * apInput );
void	Input_ProcessIKBD( sInput * apInput );
void	Input_ProcessKeyIKBD( sInput * apInput, const U16 aKeyIndex );
void	Input_ProcessJoystick( sInput * apInput, U8 aPacket );
void	Input_ProcessJagPad( sInput * apInput, U8 aDir, U16 aKey );
void	Input_UpdateStatus( sInput * apInput, const U16 aIndex, const U8 aState );
void	Input_BuildStatusString( const sInput * apInput, const U8 aFlag, char * apOutput );

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_Init( sInputLayout * apLayout )
* ACTION   : called at start of game
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_Init( sInput * apInput )
{
	U16	i;

	for( i=0; i<eINPUTKEY_LIMIT; i++ )
	{
		apInput->mKeyStatus[ i ] = eINPUTKEYSTATUS_NONE;
	}

	apInput->mInputType                     = eINPUTTYPE_IKBD;
	apInput->mScanCodes[ eINPUTKEY_LEFT   ] = eIKBDSCAN_LEFTARROW;
	apInput->mScanCodes[ eINPUTKEY_RIGHT  ] = eIKBDSCAN_RIGHTARROW;
	apInput->mScanCodes[ eINPUTKEY_DOWN   ] = eIKBDSCAN_DOWNARROW;
	apInput->mScanCodes[ eINPUTKEY_UP     ] = eIKBDSCAN_UPARROW;
	apInput->mScanCodes[ eINPUTKEY_FIREA  ] = eIKBDSCAN_SPACE;
	apInput->mScanCodes[ eINPUTKEY_PAUSE  ] = eIKBDSCAN_F1;
	apInput->mScanCodes[ eINPUTKEY_OPTION ] = eIKBDSCAN_F2;
	apInput->mScanCodes[ eINPUTKEY_QUIT   ] = eIKBDSCAN_F10;
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_UpdateStatus( sInput * apInput,U8 * apKey,U8 aState )
* ACTION   : Input_UpdateStatus
* CREATION : 09.04.2004 PNK
*-----------------------------------------------------------------------------------*/
#if 0
void	Input_UpdateStatus( sInput * apInput, const U16 aIndex, const U8 aState )
{
	U8 *	lpKey;

	lpKey = &apInput->mKeyStatus[ aIndex ];

	if( aState )
	{
		if( *lpKey ==eINPUTKEYSTATUS_NONE )  
		{
			*lpKey = eINPUTKEYSTATUS_HIT | eINPUTKEYSTATUS_HELD;
			apInput->mMovedFlag = 1;
		}
		else
		{
			*lpKey = eINPUTKEYSTATUS_HELD;
			apInput->mMovedFlag = 1;
		}
		/*		apInput->mRepeatCounts[ aIndex ]++;*/
	}
	else
	{
		if( *lpKey & eINPUTKEYSTATUS_HELD )
		{
			*lpKey = eINPUTKEYSTATUS_UNHIT;
			apInput->mMovedFlag = 1;
		}
		else
		{
			*lpKey = eINPUTKEYSTATUS_NONE;
		}
		/*		apInput->mRepeatCounts[ aIndex ] = 0;*/
	}
}

#else

#define	Input_UpdateStatus( apInput, aIndex, aState )					\
	{																	\
		U8 *	lpKey;													\
																		\
		lpKey = &apInput->mKeyStatus[ aIndex ];							\
																		\
		if( aState )													\
		{																\
			if( *lpKey ==eINPUTKEYSTATUS_NONE )							\
			{															\
			*lpKey = eINPUTKEYSTATUS_HIT | eINPUTKEYSTATUS_HELD;		\
																		\
			apInput->mMovedFlag = 1;									\
			}															\
			else														\
			{															\
				*lpKey = eINPUTKEYSTATUS_HELD;							\
				apInput->mMovedFlag = 1;								\
			}															\
		}																\
		else															\
		{																\
			if( *lpKey & eINPUTKEYSTATUS_HELD )							\
			{															\
				*lpKey = eINPUTKEYSTATUS_UNHIT;							\
				apInput->mMovedFlag = 1;								\
			}															\
			else														\
			{															\
				*lpKey = eINPUTKEYSTATUS_NONE;							\
			}															\
		}																\
	}

#endif

/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_Update( sInput * apInput )
* ACTION   : update input apInput
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_Update( sInput * apInput )
{
	apInput->mMovedFlag = 0;

	switch( apInput->mInputType )
	{
	case	eINPUTTYPE_NONE:
		Input_Init( apInput );
		break;

	case	eINPUTTYPE_MOUSE:
		Input_ProcessMouse( apInput );
		break;

	case	eINPUTTYPE_IKBD:
		Input_ProcessIKBD( apInput );
		break;

	case	eINPUTTYPE_JOY0:
		Input_ProcessKeyIKBD( apInput, eINPUTKEY_PAUSE );
		Input_ProcessKeyIKBD( apInput, eINPUTKEY_QUIT  );

		Input_ProcessJoystick( apInput, IKBD_GetJoy0Packet() );
		break;

	case	eINPUTTYPE_JOY1:
		Input_ProcessKeyIKBD( apInput, eINPUTKEY_PAUSE );
		Input_ProcessKeyIKBD( apInput, eINPUTKEY_QUIT  );

		Input_ProcessJoystick( apInput, IKBD_GetJoy1Packet() );
		break;

	case	eINPUTTYPE_PADA:
		IKBD_DisableTeamTap0();
		Input_ProcessJagPad( apInput, IKBD_GetPad0Dir(), IKBD_GetPad0Key() );
		break;

	case	eINPUTTYPE_PADB:
		IKBD_DisableTeamTap1();
		Input_ProcessJagPad( apInput, IKBD_GetPad1Dir(), IKBD_GetPad1Key() );
		break;

	case	eINPUTTYPE_TAP0_PADA:
		IKBD_EnableTeamTap0();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(0,0), IKBD_GetTeamTapPadKey(0,0) );
		break;
	case	eINPUTTYPE_TAP0_PADB:
		IKBD_EnableTeamTap0();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(0,1), IKBD_GetTeamTapPadKey(0,1) );
		break;
	case	eINPUTTYPE_TAP0_PADC:
		IKBD_EnableTeamTap0();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(0,2), IKBD_GetTeamTapPadKey(0,2) );
		break;
	case	eINPUTTYPE_TAP0_PADD:
		IKBD_EnableTeamTap0();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(0,3), IKBD_GetTeamTapPadKey(0,3) );
		break;

	case	eINPUTTYPE_TAP1_PADA:
		IKBD_EnableTeamTap1();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(1,0), IKBD_GetTeamTapPadKey(1,0) );
		break;
	case	eINPUTTYPE_TAP1_PADB:
		IKBD_EnableTeamTap1();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(1,1), IKBD_GetTeamTapPadKey(1,1) );
		break;
	case	eINPUTTYPE_TAP1_PADC:
		IKBD_EnableTeamTap1();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(1,2), IKBD_GetTeamTapPadKey(1,2) );
		break;
	case	eINPUTTYPE_TAP1_PADD:
		IKBD_EnableTeamTap1();
		Input_ProcessJagPad( apInput, IKBD_GetTeamTapPadDir(1,3), IKBD_GetTeamTapPadKey(1,3) );
		break;
	}
}




/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_ProcessIKBD( sInputLayout * apLayout )
* ACTION   : creates a packet from IKBD presses
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_ProcessIKBD( sInput * apInput )
{
	U16	i;

	for( i=0; i<eINPUTKEY_LIMIT; i++ )
	{
		Input_UpdateStatus( apInput, i, IKBD_GetKeyStatus( apInput->mScanCodes[ i ] ) );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_ProcessIKBD( sInputLayout * apLayout )
* ACTION   : creates a packet from IKBD presses
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_ProcessMouse( sInput * apInput )
{
	S16	lX,lY;
	S16	lDirX,lDirY;
	U8	lKey;


	if( !IKBD_IsMouseEnabled() )
	{
		lX   = apInput->mX;
		lY   = apInput->mY;
		lKey = 0;
	}
	else
	{
		lX   = IKBD_GetMouseX();
		lY   = IKBD_GetMouseY();
		lKey = IKBD_GetMouseKeys();
	}

	if( (apInput->mX != lX) || (apInput->mY != lY) )
	{
		apInput->mMovedFlag = 1;
	}

	lDirX = (S16)(lX - apInput->mX);
	lDirY = (S16)(lY - apInput->mY);

	Input_UpdateStatus( apInput, eINPUTKEY_LEFT , (U8)(lDirX <-4) );
	Input_UpdateStatus( apInput, eINPUTKEY_RIGHT, (U8)(lDirX > 4) );
	Input_UpdateStatus( apInput, eINPUTKEY_DOWN , (U8)(lDirY > 4) );
	Input_UpdateStatus( apInput, eINPUTKEY_UP   , (U8)(lDirY <-4) );

	apInput->mX = lX;
	apInput->mY = lY;
	Input_UpdateStatus( apInput, eINPUTKEY_FIREA , (U8)((lKey>>1) & 1 ) );
	Input_UpdateStatus( apInput, eINPUTKEY_FIREB , (U8)( lKey     & 1 ) );

	Input_UpdateStatus( apInput, eINPUTKEY_OPTION, IKBD_GetKeyStatus( apInput->mScanCodes[ eINPUTKEY_OPTION ] ) );
	Input_UpdateStatus( apInput, eINPUTKEY_PAUSE , IKBD_GetKeyStatus( apInput->mScanCodes[ eINPUTKEY_PAUSE  ] ) );
	Input_UpdateStatus( apInput, eINPUTKEY_QUIT  , IKBD_GetKeyStatus( apInput->mScanCodes[ eINPUTKEY_QUIT   ] ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_ProcessIKBD( sInput * apInput, const U16 aKeyIndex )
* ACTION   : process a single key
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_ProcessKeyIKBD( sInput * apInput, const U16 aKeyIndex )
{
	Input_UpdateStatus( apInput, aKeyIndex, IKBD_GetKeyStatus( apInput->mScanCodes[ aKeyIndex ] ) );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_ProcessJoystick( sInputLayout * apLayout, U8 aPacket )
* ACTION   : creates a packet from joystate state
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_ProcessJoystick( sInput * apInput, U8 aPacket )
{
	if( !IKBD_IsJoystickEnabled() )
	{
		aPacket = 0;
	}
	Input_UpdateStatus( apInput, eINPUTKEY_LEFT , (U8)((aPacket>>2)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_RIGHT, (U8)((aPacket>>3)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_DOWN , (U8)((aPacket>>1)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_UP   , (U8)((aPacket>>0)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_FIREA, (U8)((aPacket>>7)&1) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_ProcessJagPad( sInputInput * apInput, U8 aDir, U16 aKey )
* ACTION   : creates a packet from jagpad state
* CREATION : 20.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Input_ProcessJagPad( sInput * apInput, U8 aDir, U16 aKey )
{
	Input_UpdateStatus( apInput, eINPUTKEY_LEFT  , (U8)((aDir>>2)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_RIGHT , (U8)((aDir>>3)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_DOWN  , (U8)((aDir>>1)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_UP    , (U8)((aDir>>0)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_FIREA , (U8)((aDir>>7)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_FIREB , (U8)((aDir>>6)&1) );

	Input_UpdateStatus( apInput, eINPUTKEY_PAUSE , (U8)((aKey>>13)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_OPTION, (U8)((aKey>>12)&1) );
	Input_UpdateStatus( apInput, eINPUTKEY_QUIT  , (U8)((aKey>>8)&1) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_CombinedInit( sInput * apDst,sInput * apSrc,const U16 aCount )
* ACTION   : Input_CombinedInit
* CREATION : 02.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Input_CombinedInit( sInput * apDst,sInput * apSrc,const U16 aCount )
{
	U16	i;
	
	Input_Init( apDst );
	for( i=0; i<aCount; i++ )
	{
		Input_Init( &apSrc[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_CombinedUpdate( sInput * apDst,sInput * apSrc,const U16 aCount )
* ACTION   : Input_CombinedUpdate
* CREATION : 03.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Input_CombinedUpdate( sInput * apDst,sInput * apSrc,const U16 aCount )
{
	U16		i,j;
	U8 *	lpSrc;
	U8 *	lpDst;
	U8		lStat;
	sInput *	lpInputs;

	lpInputs = apSrc;
	i = aCount;
	while( i )
	{
		Input_Update( lpInputs );
		lpInputs++;
		i--;
	}

/*
	for( i=0; i<aCount; i++ )
	{
		Input_Update( &apSrc[ i ] );
	}
*/

	lpDst = &apDst->mKeyStatus[ 0 ];
	i = eINPUTKEY_LIMIT;
	while( i )
	{
		*lpDst++ = 0;
		i--;
	}
/*
	for( i=0; i<eINPUTKEY_LIMIT; i++ )
	{
		*lpDst++ = 0;
	}
*/

	apDst->mMovedFlag = 0;
	lpInputs = apSrc;
	i = aCount;
	while( i )
	{
		lpDst = &apDst->mKeyStatus[ 0 ];
		lpSrc = &lpInputs->mKeyStatus[ 0 ];
		j = eINPUTKEY_LIMIT;
		while( j )
		{
			lStat = *lpSrc++;
			if( lStat )
			{
				*lpDst = lStat;
			}
			lpDst++;
			j--;
		}
		apDst->mMovedFlag |= lpInputs->mMovedFlag;
		lpInputs++;
		i--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_GetTypeName( const sInput * apInput )
* ACTION   : Input_GetTypeName
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

char *	Input_GetTypeName( const sInput * apInput )
{
	return( sTagString_GetpString( apInput->mInputType,	gInputTypeNames, eINPUTTYPE_LIMIT ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_BuildHeldString( const sInput * apInput,char * apOutput )
* ACTION   : Input_BuildHeldString
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Input_BuildHeldString( const sInput * apInput,char * apOutput )
{
	Input_BuildStatusString( apInput, eINPUTKEYSTATUS_HELD, apOutput );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_BuildHitString( const sInput * apInput,char * apOutput )
* ACTION   : Input_BuildHitString
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Input_BuildHitString( const sInput * apInput,char * apOutput )
{
	Input_BuildStatusString( apInput, eINPUTKEYSTATUS_HIT, apOutput );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_BuildUnHitString( const sInput * apInput,char * apOutput )
* ACTION   : Input_BuildUnHitString
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Input_BuildUnHitString( const sInput * apInput,char * apOutput )
{
	Input_BuildStatusString( apInput, eINPUTKEYSTATUS_UNHIT, apOutput );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_BuildStatusString( const sInput * apInput,const U8 aFlag,char * apOutput )
* ACTION   : Input_BuildStatusString
* CREATION : 30.01.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Input_BuildStatusString( const sInput * apInput,const U8 aFlag,char * apOutput )
{
	U16	i;

	for( i=0; i<eINPUTKEY_LIMIT; i++ )
	{
		if( apInput->mKeyStatus[ i ] & aFlag )
		{
			apOutput[ i ] = gInputStatusChars[ i ];
		}
		else
		{
			apOutput[ i ] = '.';
		}
	}
	apOutput[ i ] = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Input_Combine(sInput * apDst,const sInput * apSrc0,const sInput * apSrc1)
* ACTION   : Input_Combine
* CREATION : 17.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void Input_Combine(sInput * apDst,const sInput * apSrc0,const sInput * apSrc1)
{
	U16			i;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U8 *		lpDst;

	lpDst  = &apDst->mKeyStatus[ 0 ];
	lpSrc0 = &apSrc0->mKeyStatus[ 0 ];
	lpSrc1 = &apSrc1->mKeyStatus[ 0 ];

	for( i=0; i<eINPUTKEY_LIMIT; i++ )
	{
		if( *lpSrc0 )
		{
			*lpDst = *lpSrc0;
		}
		if( *lpSrc1 )
		{
			*lpDst = *lpSrc1;
		}
		lpSrc0++;
		lpSrc1++;
		lpDst++;
	}
	apDst->mMovedFlag |= apSrc0->mMovedFlag;	
	apDst->mMovedFlag |= apSrc1->mMovedFlag;	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Input_CombineFire( sInput * apDst,const sInput * apSrc0,const sInput * apSrc1 )
* ACTION   : Input_CombineFire
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Input_CombineFire( sInput * apDst,const sInput * apSrc0,const sInput * apSrc1 )
{
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U8 *		lpDst;

	lpDst  = &apDst->mKeyStatus[ eINPUTKEY_FIREA ];
	lpSrc0 = &apSrc0->mKeyStatus[ eINPUTKEY_FIREA ];
	lpSrc1 = &apSrc1->mKeyStatus[ eINPUTKEY_FIREA ];

	if( *lpSrc0 )
	{
		*lpDst = *lpSrc0;
	}
	if( *lpSrc1 )
	{
		*lpDst = *lpSrc1;
	}
	apDst->mMovedFlag |= apSrc0->mMovedFlag;	
	apDst->mMovedFlag |= apSrc1->mMovedFlag;		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Input_DirClear(sInput * apInput)
* ACTION   : Input_DirClear
* CREATION : 17.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void Input_DirClear(sInput * apInput)
{
	apInput->mKeyStatus[ eINPUTKEY_LEFT  ] = eINPUTKEYSTATUS_NONE;	
	apInput->mKeyStatus[ eINPUTKEY_RIGHT ] = eINPUTKEYSTATUS_NONE;	
	apInput->mKeyStatus[ eINPUTKEY_DOWN  ] = eINPUTKEYSTATUS_NONE;	
	apInput->mKeyStatus[ eINPUTKEY_UP    ] = eINPUTKEYSTATUS_NONE;	
}


/* ################################################################################ */

