/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"IKBD_SDL.H"

#ifdef	dGODLIB_SYSTEM_SDL

#include	"IKBD.H"
#include	<GODLIB/MEMORY/MEMORY.H>


#pragma pack(push,4)
#include	"SDL.h"
#pragma pack(pop)

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sIkbdSdlClass
{
	S16	mSdlToGod[ SDLK_LAST ];
	U8	mKeyStates[ SDLK_LAST ];
} sIkbdSdlClass;

typedef	struct	sIkbdSdlRemap
{
	U16	mGodKey;
	U16	mSdlKey;
} sIkbdSdlRemap;


/* ###################################################################################
#  DATA
################################################################################### */

extern	sIKBD	gIKBD;

U8 gIkbdSdlAscii[256] =
{
	/*   0 - 9   */		0,		0,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',
	/*  10 - 19  */		'9',	'0',	'-',	'=',	0,		0,		'q',	'w',	'e',	'r',
	/*  20 - 29  */		't',	'y',	'u',	'i',	'o',	'p',	'[',	']',	0,		0,
	/*  30 - 39  */ 	'a', 	's',	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
	/*  40 - 49  */		'\'',	'`',	0,		'#',	'z',	'x',	'c',	'v',	'b',	'n',
	/*  50 - 59  */		'm',	',',	'.',	'/',	'/',	'*',	0,		' ',	0,		0,
	/*  60 - 69  */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/*  70 - 79  */		0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
	/*  80 - 89  */		0,		0,		0,		0,		0,		0,		'\\',	0,		0,		0,
	/*  90 - 99  */		0,		0,		0,		0,		0,		0,		'/',	0,		0,		'(',
	/* 100 - 109 */		')',	'/',	'*',	0,		0,		0,		0,		0,		0,		0,
	/* 110 - 119 */		0,		0,		0,		'.',	0,		0,		0,		0,		0,		0,
	/* 120 - 129 */		0,		0,		0,		0,		0,		'`',	0,		0,		0,		0,
	/* 130 - 139 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 140 - 149 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 150 - 159 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 160 - 169 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 170 - 179 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 180 - 189 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 190 - 199 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 200 - 209 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 210 - 219 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 220 - 229 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 230 - 239 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 240 - 249 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 250 - 255 */		0,		0,		0,		0,		0,		0
};

U8 gIkbdSdlAsciiCaps[256] =
{
	/*   0 - 9   */		0,		0,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',
	/*  10 - 19  */		'9',	'0',	'-',	'=',	0,		0,		'Q',	'W',	'E',	'R',
	/*  20 - 29  */		'T',	'Y',	'U',	'I',	'O',	'P',	'[',	']',	0,		0,
	/*  30 - 39  */ 	'A', 	'S',	'D',	'F',	'G',	'H',	'J',	'K',	'L',	';',
	/*  40 - 49  */		'\'',	'`',	0,		'#',	'Z',	'X',	'C',	'V',	'B',	'N',
	/*  50 - 59  */		'M',	',',	'.',	'/',	'/',	'*',	0,		' ',	0,		0,
	/*  60 - 69  */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/*  70 - 79  */		0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
	/*  80 - 89  */		0,		0,		0,		0,		0,		0,		'\\',	0,		0,		0,
	/*  90 - 99  */		0,		0,		0,		0,		0,		0,		'/',	0,		0,		'(',
	/* 100 - 109 */		')',	'/',	'*',	0,		0,		0,		0,		0,		0,		0,
	/* 110 - 119 */		0,		0,		0,		'.',	0,		0,		0,		0,		0,		0,
	/* 120 - 129 */		0,		0,		0,		0,		0,		'`',	0,		0,		0,		0,
	/* 130 - 139 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 140 - 149 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 150 - 159 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 160 - 169 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 170 - 179 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 180 - 189 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 190 - 199 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 200 - 209 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 210 - 219 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 220 - 229 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 230 - 239 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 240 - 249 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 250 - 255 */		0,		0,		0,		0,		0,		0
};

U8 gIkbdSdlAsciiShift[256] =
{
	/*   0 - 9   */		0,		0,	'!',	'"',
		0, /* was '£',	*/
		'$',	'%',	'^',	'&',	'*',
	/*  10 - 19  */		'(',	')',	'_',	'+',	0,		0,		'Q',	'W',	'E',	'R',
	/*  20 - 29  */		'T',	'Y',	'U',	'I',	'O',	'P',	'{',	'}',	0,		0,
	/*  30 - 39  */ 	'A', 	'S',	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
	/*  40 - 49  */		'@',	'~',	0,		'~',	'Z',	'X',	'C',	'V',	'B',	'N',
	/*  50 - 59  */		'M',	'<',	'>',	'?',	0,		'*',	0,		' ',	0,		0,
	/*  60 - 69  */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/*  70 - 79  */		0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
	/*  80 - 89  */		0,		0,		0,		0,		0,		0,		'|',	0,		0,		0,
	/*  90 - 99  */		0,		0,		0,		0,		0,		0,		'/',	0,		0,		'(',
	/* 100 - 109 */		')',	'/',	'*',	0,		0,		0,		0,		0,		0,		0,
	/* 110 - 119 */		0,		0,		0,		'.',	0,		0,		0,		0,		0,		0,
	/* 120 - 129 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 130 - 139 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 140 - 149 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 150 - 159 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 160 - 169 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 170 - 179 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 180 - 189 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 190 - 199 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 200 - 209 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 210 - 219 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 220 - 229 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 230 - 239 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 240 - 249 */		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	/* 250 - 255 */		0,		0,		0,		0,		0,		0
};

sIkbdSdlRemap	gIkbdSdlRemaps[] =
{
	{	eIKBDSCAN_A,	SDLK_a	},
	{	eIKBDSCAN_B,	SDLK_b	},
	{	eIKBDSCAN_C,	SDLK_c	},
	{	eIKBDSCAN_D,	SDLK_d	},
	{	eIKBDSCAN_E,	SDLK_e	},
	{	eIKBDSCAN_F,	SDLK_f	},
	{	eIKBDSCAN_G,	SDLK_g	},
	{	eIKBDSCAN_H,	SDLK_h	},
	{	eIKBDSCAN_I,	SDLK_i	},
	{	eIKBDSCAN_J,	SDLK_j	},
	{	eIKBDSCAN_K,	SDLK_k	},
	{	eIKBDSCAN_L,	SDLK_l	},
	{	eIKBDSCAN_M,	SDLK_m	},
	{	eIKBDSCAN_N,	SDLK_n	},
	{	eIKBDSCAN_O,	SDLK_o	},
	{	eIKBDSCAN_P,	SDLK_p	},
	{	eIKBDSCAN_Q,	SDLK_q	},
	{	eIKBDSCAN_R,	SDLK_r	},
	{	eIKBDSCAN_S,	SDLK_s	},
	{	eIKBDSCAN_T,	SDLK_t	},
	{	eIKBDSCAN_U,	SDLK_u	},
	{	eIKBDSCAN_V,	SDLK_v	},
	{	eIKBDSCAN_W,	SDLK_w	},
	{	eIKBDSCAN_X,	SDLK_x	},
	{	eIKBDSCAN_Y,	SDLK_y	},
	{	eIKBDSCAN_Z,	SDLK_z	},

	{	eIKBDSCAN_0,	SDLK_0	},
	{	eIKBDSCAN_1,	SDLK_1	},
	{	eIKBDSCAN_2,	SDLK_2	},
	{	eIKBDSCAN_3,	SDLK_3	},
	{	eIKBDSCAN_4,	SDLK_4	},
	{	eIKBDSCAN_5,	SDLK_5	},
	{	eIKBDSCAN_6,	SDLK_6	},
	{	eIKBDSCAN_7,	SDLK_7	},
	{	eIKBDSCAN_8,	SDLK_8	},
	{	eIKBDSCAN_9,	SDLK_9	},

	{	eIKBDSCAN_F1,	SDLK_F1	},
	{	eIKBDSCAN_F2,	SDLK_F2	},
	{	eIKBDSCAN_F3,	SDLK_F3	},
	{	eIKBDSCAN_F4,	SDLK_F4	},
	{	eIKBDSCAN_F5,	SDLK_F5	},
	{	eIKBDSCAN_F6,	SDLK_F6	},
	{	eIKBDSCAN_F7,	SDLK_F7	},
	{	eIKBDSCAN_F8,	SDLK_F8	},
	{	eIKBDSCAN_F9,	SDLK_F9	},
	{	eIKBDSCAN_F10,	SDLK_F10	},

	{	eIKBDSCAN_AT				,	SDLK_AT	},
//	{	eIKBDSCAN_AMPERSAND			,	SDLK_8	},
	{	eIKBDSCAN_BAR				,	SDLK_BACKSLASH	},
	{	eIKBDSCAN_BACKSLASH			,	SDLK_BACKSLASH	},
//	{	eIKBDSCAN_CLOSEAPOSTROPHE	,	SDLK_APOSTROPHE	},
//	{	eIKBDSCAN_CLOSEBRACKET		,	SDLK_0	},
	{	eIKBDSCAN_CLOSECURLYBRACKET	,	SDLK_RIGHTBRACKET	},
	{	eIKBDSCAN_CLOSESQUAREBRACKET,	SDLK_RIGHTBRACKET	},
	{	eIKBDSCAN_COMMA				,	SDLK_COMMA	},
	{	eIKBDSCAN_COLON				,	SDLK_COLON	},
//	{	eIKBDSCAN_DOLLARSIGN		,	SDLK_4	},
	{	eIKBDSCAN_DOT				,	SDLK_PERIOD	},
	{	eIKBDSCAN_EQUALS			,	SDLK_EQUALS	},
	{	eIKBDSCAN_EXCLAMATIONMARK	,	SDLK_1	},
	{	eIKBDSCAN_FORWARDSLASH		,	SDLK_SLASH	},
	{	eIKBDSCAN_GREATERTHAN		,	SDLK_PERIOD	},
//	{	eIKBDSCAN_HASH				,	0	},
	{	eIKBDSCAN_LESSTHAN			,	SDLK_COMMA	},
//	{	eIKBDSCAN_MINUS				,	SDLK_UNDERLINE	},
//	{	eIKBDSCAN_OPENAPOSTROPHE	,	SDLK_APOSTROPHE	},
//	{	eIKBDSCAN_OPENBRACKET		,	SDLK_F9	},
	{	eIKBDSCAN_OPENCURLYBRACKET	,	SDLK_LEFTBRACKET	},
	{	eIKBDSCAN_OPENSQUAREBRACKET	,	SDLK_LEFTBRACKET	},
//	{	eIKBDSCAN_OVERSCORE			,	SDLK_UNDERLINE	},
//	{	eIKBDSCAN_PERCENTAGE		,	SDLK_5	},
	{	eIKBDSCAN_PLUS				,	SDLK_EQUALS	},
//	{	eIKBDSCAN_POUNDSIGN			,	SDLK_3	},
//	{	eIKBDSCAN_QUOTEMARK			,	SDLK_2	},
	{	eIKBDSCAN_QUESTTIONMARK		,	SDLK_SLASH	},
	{	eIKBDSCAN_SEMICOLON			,	SDLK_SEMICOLON	},
	{	eIKBDSCAN_STAR				,	SDLK_KP_MULTIPLY	},
//	{	eIKBDSCAN_TILDE				,	SDLK_0	},
//	{	eIKBDSCAN_UNDERSCORE		,	SDLK_UNDERLINE	},

	{	eIKBDSCAN_NUMPAD0			,	SDLK_KP0 },
	{	eIKBDSCAN_NUMPAD1			,	SDLK_KP1	},
	{	eIKBDSCAN_NUMPAD2			,	SDLK_KP2	},
	{	eIKBDSCAN_NUMPAD3			,	SDLK_KP3	},
	{	eIKBDSCAN_NUMPAD4			,	SDLK_KP4	},
	{	eIKBDSCAN_NUMPAD5			,	SDLK_KP5	},
	{	eIKBDSCAN_NUMPAD6			,	SDLK_KP6	},
	{	eIKBDSCAN_NUMPAD7			,	SDLK_KP7	},
	{	eIKBDSCAN_NUMPAD8			,	SDLK_KP8	},
	{	eIKBDSCAN_NUMPAD9			,	SDLK_KP9	},
//	{	eIKBDSCAN_NUMPADOPENBRACKET	,	0	},
//	{	eIKBDSCAN_NUMPADCLOSEBRACKET,	0	},
	{	eIKBDSCAN_NUMPADDOT			,	SDLK_KP_PERIOD		},
	{	eIKBDSCAN_NUMPADENTER		,	SDLK_KP_ENTER		},
	{	eIKBDSCAN_NUMPADMINUS		,	SDLK_KP_MINUS		},
	{	eIKBDSCAN_NUMPADPLUS		,	SDLK_KP_PLUS		},
	{	eIKBDSCAN_NUMPADSLASH		,	SDLK_KP_DIVIDE		},
	{	eIKBDSCAN_NUMPADSTAR		,	SDLK_KP_MULTIPLY	},

	{	eIKBDSCAN_DOWNARROW	,	SDLK_DOWN	},
	{	eIKBDSCAN_LEFTARROW	,	SDLK_LEFT	},
	{	eIKBDSCAN_RIGHTARROW,	SDLK_RIGHT	},
	{	eIKBDSCAN_UPARROW	,	SDLK_UP		},

	{	eIKBDSCAN_ALTERNATE	,	SDLK_LALT		},
	{	eIKBDSCAN_BACKSPACE	,	SDLK_BACKSPACE	},
	{	eIKBDSCAN_CAPSLOCK	,	SDLK_CAPSLOCK	},
	{	eIKBDSCAN_CLRHOME	,	SDLK_HOME		},
	{	eIKBDSCAN_CONTROL	,	SDLK_LCTRL		},
	{	eIKBDSCAN_DELETE	,	SDLK_DELETE		},
	{	eIKBDSCAN_ESC		,	SDLK_ESCAPE		},
	{	eIKBDSCAN_HELP		,	SDLK_PAGEUP		},
	{	eIKBDSCAN_INSERT	,	SDLK_INSERT		},
	{	eIKBDSCAN_LEFTSHIFT	,	SDLK_LSHIFT		},
	{	eIKBDSCAN_RETURN	,	SDLK_RETURN		},
	{	eIKBDSCAN_RIGHTSHIFT,	SDLK_RSHIFT		},
	{	eIKBDSCAN_SPACE		,	SDLK_SPACE		},
	{	eIKBDSCAN_TAB		,	SDLK_TAB			},
	{	eIKBDSCAN_UNDO		,	SDLK_PAGEDOWN	},
	{	0,	0	},
};

sIkbdSdlClass	gIkbdSdlClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	IKBD_SDL_KbdUpdate( void );
void	IKBD_SDL_MouseUpdate( void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SDL_Init( void )
* ACTION   : IKBD_SDL_Init
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SDL_Init( void )
{
	U16				i;
	sIkbdSdlRemap *	lpRemap;

	Memory_Clear( sizeof(sIkbdSdlClass), &gIkbdSdlClass );

	gIKBD.mpUnshiftTable = gIkbdSdlAscii;
	gIKBD.mpShiftTable   = gIkbdSdlAsciiShift;
	gIKBD.mpCapsTable    = gIkbdSdlAsciiCaps;

	for( i=0; i<SDLK_LAST; i++ )
	{
		gIkbdSdlClass.mSdlToGod[ i ] = -1;
	}

	lpRemap = gIkbdSdlRemaps;
	while( lpRemap->mGodKey )
	{
		gIkbdSdlClass.mSdlToGod[ lpRemap->mSdlKey ] = (U8)(lpRemap->mGodKey);
		lpRemap++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SDL_DeInit( void )
* ACTION   : IKBD_SDL_DeInit
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SDL_DeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SDL_Update( void )
* ACTION   : IKBD_SDL_Update
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SDL_Update( void )
{
	SDL_PumpEvents();

	IKBD_SDL_KbdUpdate();
	IKBD_SDL_MouseUpdate();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SDL_KbdUpdate( void )
* ACTION   : IKBD_SDL_KbdUpdate
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SDL_KbdUpdate( void )
{
	U8 *	lpKeyStates;
	S16		lIndex;
	U16		i;
	U16		lPos;


	lpKeyStates = SDL_GetKeyState(0);
	if( lpKeyStates )
	{
		for( i=0; i<SDLK_LAST; i++ )
		{
			lIndex = gIkbdSdlClass.mSdlToGod[ i ];

			if( lIndex >= 0 )
			{
				if( lpKeyStates[ i ] )
				{
					gIKBD.mKbdPressTable[ lIndex ] = 0xFF;
					if( !gIkbdSdlClass.mKeyStates[ i ] )
					{
						gIKBD.mKeyPressedFlag = 0xFF;
						gIKBD.mLastKeypress   = (U8)lIndex;
						lIndex |= 0x80;
					}
				}
				else
				{
					gIKBD.mKbdPressTable[ lIndex ] = 0;
				}

				if( gIkbdSdlClass.mKeyStates[ i ] != lpKeyStates[ i ] )
				{
//					gIKBD.mKbdBuffer[ gIKBD.mKbdTail ] = (U8)lIndex;

					lPos  = gIKBD.mKbdTail += 1;
					lPos &= 2047;
					if( lPos == gIKBD.mKbdHead )
					{
						gIKBD.mKbdOverflowFlag = 1;
					}
					else
					{
						gIKBD.mKbdTail = lPos;
					}
					gIKBD.mKbdBuffer[ gIKBD.mKbdTail ] = (U8)lIndex;
				}

				gIkbdSdlClass.mKeyStates[ i ] = lpKeyStates[ i ];
			}
		}
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SDL_MouseUpdate( void )
* ACTION   : IKBD_SDL_MouseUpdate
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SDL_MouseUpdate( void )
{
	int	lX,lY;
	U8	lKey;

	lKey = SDL_GetRelativeMouseState( &lX, &lY );

	if( lKey & 1 )
	{
		gIKBD.mMouseKeys |= eIKBD_MOUSEBUTTON_LEFT;
	}
	else
	{
		gIKBD.mMouseKeys &= ~eIKBD_MOUSEBUTTON_LEFT;
	}

	if( lKey & 4 )
	{
		gIKBD.mMouseKeys |= eIKBD_MOUSEBUTTON_RIGHT;
	}
	else
	{
		gIKBD.mMouseKeys &= ~eIKBD_MOUSEBUTTON_RIGHT;
	}

	gIKBD.mMouseX = (S16)(gIKBD.mMouseX + lX);
	gIKBD.mMouseY = (S16)(gIKBD.mMouseY + lY);
}


/* ################################################################################ */

#endif
