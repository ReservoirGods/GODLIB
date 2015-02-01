/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"IKBD_DI.H"
#include	"IKBD.H"

#ifdef	dGODLIB_SYSTEM_D3D

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<dinput.h>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dIKBDDI_KBDBUFFER_LIMIT		8
#define	dIKBDDI_MOUSEBUFFER_LIMIT	16


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sIkbdDiRemap
{
	U16	mGodKey;
	U16	mDiKey;
} sIkbdDiRemap;


/* ###################################################################################
#  DATA
################################################################################### */

extern	sIKBD	gIKBD;

LPDIRECTINPUT8       gpIkbdDI         = NULL;
LPDIRECTINPUTDEVICE8 gpIkbdDIKeyboard = NULL;
LPDIRECTINPUTDEVICE8 gpIkbdDIMouse    = NULL;
HWND				 gIkbdDIHwnd;

U8	gIkbdDiToGod[ 256 ];

U8 gIkbdDIAscii[256] =
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

U8 gIkbdDIAsciiCaps[256] =
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

U8 gIkbdDIAsciiShift[256] =
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

sIkbdDiRemap	gIkbdDiRemaps[] =
{
	{	eIKBDSCAN_A,	DIK_A	},
	{	eIKBDSCAN_B,	DIK_B	},
	{	eIKBDSCAN_C,	DIK_C	},
	{	eIKBDSCAN_D,	DIK_D	},
	{	eIKBDSCAN_E,	DIK_E	},
	{	eIKBDSCAN_F,	DIK_F	},
	{	eIKBDSCAN_G,	DIK_G	},
	{	eIKBDSCAN_H,	DIK_H	},
	{	eIKBDSCAN_I,	DIK_I	},
	{	eIKBDSCAN_J,	DIK_J	},
	{	eIKBDSCAN_K,	DIK_K	},
	{	eIKBDSCAN_L,	DIK_L	},
	{	eIKBDSCAN_M,	DIK_M	},
	{	eIKBDSCAN_N,	DIK_N	},
	{	eIKBDSCAN_O,	DIK_O	},
	{	eIKBDSCAN_P,	DIK_P	},
	{	eIKBDSCAN_Q,	DIK_Q	},
	{	eIKBDSCAN_R,	DIK_R	},
	{	eIKBDSCAN_S,	DIK_S	},
	{	eIKBDSCAN_T,	DIK_T	},
	{	eIKBDSCAN_U,	DIK_U	},
	{	eIKBDSCAN_V,	DIK_V	},
	{	eIKBDSCAN_W,	DIK_W	},
	{	eIKBDSCAN_X,	DIK_X	},
	{	eIKBDSCAN_Y,	DIK_Y	},
	{	eIKBDSCAN_Z,	DIK_Z	},

	{	eIKBDSCAN_0,	DIK_0	},
	{	eIKBDSCAN_1,	DIK_1	},
	{	eIKBDSCAN_2,	DIK_2	},
	{	eIKBDSCAN_3,	DIK_3	},
	{	eIKBDSCAN_4,	DIK_4	},
	{	eIKBDSCAN_5,	DIK_5	},
	{	eIKBDSCAN_6,	DIK_6	},
	{	eIKBDSCAN_7,	DIK_7	},
	{	eIKBDSCAN_8,	DIK_8	},
	{	eIKBDSCAN_9,	DIK_9	},

	{	eIKBDSCAN_F1,	DIK_F1	},
	{	eIKBDSCAN_F2,	DIK_F2	},
	{	eIKBDSCAN_F3,	DIK_F3	},
	{	eIKBDSCAN_F4,	DIK_F4	},
	{	eIKBDSCAN_F5,	DIK_F5	},
	{	eIKBDSCAN_F6,	DIK_F6	},
	{	eIKBDSCAN_F7,	DIK_F7	},
	{	eIKBDSCAN_F8,	DIK_F8	},
	{	eIKBDSCAN_F9,	DIK_F9	},
	{	eIKBDSCAN_F10,	DIK_F10	},

	{	eIKBDSCAN_AT				,	DIK_AT	},
//	{	eIKBDSCAN_AMPERSAND			,	DIK_8	},
	{	eIKBDSCAN_BAR				,	DIK_BACKSLASH	},
	{	eIKBDSCAN_BACKSLASH			,	DIK_BACKSLASH	},
	{	eIKBDSCAN_CLOSEAPOSTROPHE	,	DIK_APOSTROPHE	},
//	{	eIKBDSCAN_CLOSEBRACKET		,	DIK_0	},
	{	eIKBDSCAN_CLOSECURLYBRACKET	,	DIK_RBRACKET	},
	{	eIKBDSCAN_CLOSESQUAREBRACKET,	DIK_RBRACKET	},
	{	eIKBDSCAN_COMMA				,	DIK_COMMA	},
	{	eIKBDSCAN_COLON				,	DIK_COLON	},
//	{	eIKBDSCAN_DOLLARSIGN		,	DIK_4	},
	{	eIKBDSCAN_DOT				,	DIK_PERIOD	},
	{	eIKBDSCAN_EQUALS			,	DIK_EQUALS	},
	{	eIKBDSCAN_EXCLAMATIONMARK	,	DIK_1	},
	{	eIKBDSCAN_FORWARDSLASH		,	DIK_SLASH	},
	{	eIKBDSCAN_GREATERTHAN		,	DIK_PERIOD	},
//	{	eIKBDSCAN_HASH				,	0	},
	{	eIKBDSCAN_LESSTHAN			,	DIK_COMMA	},
	{	eIKBDSCAN_MINUS				,	DIK_UNDERLINE	},
	{	eIKBDSCAN_OPENAPOSTROPHE	,	DIK_APOSTROPHE	},
//	{	eIKBDSCAN_OPENBRACKET		,	DIK_F9	},
	{	eIKBDSCAN_OPENCURLYBRACKET	,	DIK_LBRACKET	},
	{	eIKBDSCAN_OPENSQUAREBRACKET	,	DIK_LBRACKET	},
	{	eIKBDSCAN_OVERSCORE			,	DIK_UNDERLINE	},
//	{	eIKBDSCAN_PERCENTAGE		,	DIK_5	},
	{	eIKBDSCAN_PLUS				,	DIK_EQUALS	},
//	{	eIKBDSCAN_POUNDSIGN			,	DIK_3	},
//	{	eIKBDSCAN_QUOTEMARK			,	DIK_2	},
	{	eIKBDSCAN_QUESTTIONMARK		,	DIK_SLASH	},
	{	eIKBDSCAN_SEMICOLON			,	DIK_SEMICOLON	},
	{	eIKBDSCAN_STAR				,	DIK_NUMPADSTAR	},
//	{	eIKBDSCAN_TILDE				,	DIK_0	},
	{	eIKBDSCAN_UNDERSCORE		,	DIK_UNDERLINE	},

	{	eIKBDSCAN_NUMPAD0			,	DIK_NUMPAD0 },
	{	eIKBDSCAN_NUMPAD1			,	DIK_NUMPAD1	},
	{	eIKBDSCAN_NUMPAD2			,	DIK_NUMPAD2	},
	{	eIKBDSCAN_NUMPAD3			,	DIK_NUMPAD3	},
	{	eIKBDSCAN_NUMPAD4			,	DIK_NUMPAD4	},
	{	eIKBDSCAN_NUMPAD5			,	DIK_NUMPAD5	},
	{	eIKBDSCAN_NUMPAD6			,	DIK_NUMPAD6	},
	{	eIKBDSCAN_NUMPAD7			,	DIK_NUMPAD7	},
	{	eIKBDSCAN_NUMPAD8			,	DIK_NUMPAD8	},
	{	eIKBDSCAN_NUMPAD9			,	DIK_NUMPAD9	},
//	{	eIKBDSCAN_NUMPADOPENBRACKET	,	0	},
//	{	eIKBDSCAN_NUMPADCLOSEBRACKET,	0	},
	{	eIKBDSCAN_NUMPADDOT			,	DIK_NUMPADPERIOD	},
	{	eIKBDSCAN_NUMPADENTER		,	DIK_NUMPADENTER	},
	{	eIKBDSCAN_NUMPADMINUS		,	DIK_NUMPADMINUS	},
	{	eIKBDSCAN_NUMPADPLUS		,	DIK_NUMPADPLUS	},
	{	eIKBDSCAN_NUMPADSLASH		,	DIK_NUMPADSLASH	},
	{	eIKBDSCAN_NUMPADSTAR		,	DIK_NUMPADSTAR	},

	{	eIKBDSCAN_DOWNARROW	,	DIK_DOWN	},
	{	eIKBDSCAN_LEFTARROW	,	DIK_LEFT	},
	{	eIKBDSCAN_RIGHTARROW,	DIK_RIGHT	},
	{	eIKBDSCAN_UPARROW	,	DIK_UP		},

	{	eIKBDSCAN_ALTERNATE	,	DIK_LALT		},
	{	eIKBDSCAN_BACKSPACE	,	DIK_BACKSPACE	},
	{	eIKBDSCAN_CAPSLOCK	,	DIK_CAPSLOCK	},
	{	eIKBDSCAN_CLRHOME	,	DIK_HOME		},
	{	eIKBDSCAN_CONTROL	,	DIK_LCONTROL	},
	{	eIKBDSCAN_DELETE	,	DIK_DELETE		},
	{	eIKBDSCAN_ESC		,	DIK_ESCAPE		},
	{	eIKBDSCAN_HELP		,	DIK_PGUP		},
	{	eIKBDSCAN_INSERT	,	DIK_INSERT		},
	{	eIKBDSCAN_LEFTSHIFT	,	DIK_LSHIFT		},
	{	eIKBDSCAN_RETURN	,	DIK_RETURN		},
	{	eIKBDSCAN_RIGHTSHIFT,	DIK_RSHIFT		},
	{	eIKBDSCAN_SPACE		,	DIK_SPACE		},
	{	eIKBDSCAN_TAB		,	DIK_TAB			},
	{	eIKBDSCAN_UNDO		,	DIK_PGDN		},
	{	0,	0	},
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	IKBD_DI_KbdUpdate( void );
void	IKBD_DI_MouseUpdate( void );
void	IKBD_DI_Flush( void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_Init( void )
* ACTION   : IKBD_DI_Init
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_Init( void )
{
	DIPROPDWORD		lDip;
	sIkbdDiRemap *	lpRemap;
	U16				i;

	lDip.diph.dwSize       = sizeof(DIPROPDWORD);
	lDip.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	lDip.diph.dwObj        = 0;
	lDip.diph.dwHow        = DIPH_DEVICE;

	DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		&IID_IDirectInput8,
		(void**)&gpIkbdDI,
		NULL );

	lDip.dwData            = dIKBDDI_KBDBUFFER_LIMIT;
	IDirectInput8_CreateDevice( gpIkbdDI, &GUID_SysKeyboard, &gpIkbdDIKeyboard, NULL );
	IDirectInputDevice8_SetDataFormat(  gpIkbdDIKeyboard, &c_dfDIKeyboard );
	IDirectInputDevice8_SetCooperativeLevel( gpIkbdDIKeyboard, gIkbdDIHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	IDirectInputDevice8_SetProperty( gpIkbdDIKeyboard, DIPROP_BUFFERSIZE, &lDip.diph );

	lDip.dwData            = dIKBDDI_MOUSEBUFFER_LIMIT;
	IDirectInput8_CreateDevice( gpIkbdDI, &GUID_SysMouse, &gpIkbdDIMouse, NULL );
	IDirectInputDevice8_SetDataFormat(  gpIkbdDIMouse, &c_dfDIMouse );
	IDirectInputDevice8_SetCooperativeLevel( gpIkbdDIMouse, gIkbdDIHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	IDirectInputDevice8_SetProperty( gpIkbdDIMouse, DIPROP_BUFFERSIZE, &lDip.diph );

	gIKBD.mpUnshiftTable = gIkbdDIAscii;
	gIKBD.mpShiftTable   = gIkbdDIAsciiShift;
	gIKBD.mpCapsTable    = gIkbdDIAsciiCaps;

	for( i=0; i<256; i++ )
	{
		gIkbdDiToGod[ i ] = (U8)(i & 0x7F);
	}
	lpRemap = gIkbdDiRemaps;
	while( lpRemap->mGodKey )
	{
		gIkbdDiToGod[ lpRemap->mDiKey & 0xFF ] = (U8)(lpRemap->mGodKey);
		lpRemap++;
	}

	IKBD_DI_Flush();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_DeInit( void )
* ACTION   : IKBD_DI_DeInit
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_DeInit( void )
{
	if( gpIkbdDIKeyboard )
	{
		IDirectInputDevice8_Unacquire( gpIkbdDIKeyboard );
		IDirectInputDevice8_Release(   gpIkbdDIKeyboard );
		gpIkbdDIKeyboard = 0;
	}
	if( gpIkbdDIMouse )
	{
		IDirectInputDevice8_Unacquire( gpIkbdDIMouse );
		IDirectInputDevice8_Release(   gpIkbdDIMouse );
		gpIkbdDIKeyboard = 0;
	}
	if( gpIkbdDI )
	{
		IDirectInput8_Release( gpIkbdDI );
		gpIkbdDI = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_SetWindowHandle( HWND aHwnd )
* ACTION   : IKBD_DI_SetWindowHandle
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_SetWindowHandle( HWND aHwnd )
{
	gIkbdDIHwnd	= aHwnd;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_Update( void )
* ACTION   : IKBD_DI_Update
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_Update( void )
{
	IKBD_DI_KbdUpdate();
	IKBD_DI_MouseUpdate();
}

void	IKBD_DI_Flush( void )
{
	DIDEVICEOBJECTDATA	lData[ dIKBDDI_KBDBUFFER_LIMIT ];
	DWORD				lElementCount;

	IDirectInputDevice8_GetDeviceData(
		gpIkbdDIKeyboard,
		sizeof(DIDEVICEOBJECTDATA),
		lData,
		&lElementCount,
		0 );

}

/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_KbdUpdate( void )
* ACTION   : IKBD_DI_KbdUpdate
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_KbdUpdate( void )
{
    DIDEVICEOBJECTDATA	lData[ dIKBDDI_KBDBUFFER_LIMIT ];
    DWORD				lElementCount;
    DWORD				i;
	U16					lIndex;
	U16					lPos;
    HRESULT				lRes;

	if( gpIkbdDIKeyboard )
	{
		lElementCount = dIKBDDI_KBDBUFFER_LIMIT;

		lRes = IDirectInputDevice8_GetDeviceData(
			gpIkbdDIKeyboard,
			sizeof(DIDEVICEOBJECTDATA),
			lData,
			&lElementCount,
			0 );

		if( DI_OK != lRes )
		{
			lRes = IDirectInputDevice8_Acquire( gpIkbdDIKeyboard );
			while( lRes == DIERR_INPUTLOST )
			{
				lRes = IDirectInputDevice8_Acquire( gpIkbdDIKeyboard );
			}
		}

		if( !FAILED( lRes ) )
		{
			for( i=0; i<lElementCount; i++ )
			{
				lIndex = (U16)(lData[ i ].dwOfs & 0xFF);
				lIndex = gIkbdDiToGod[ lIndex ];

				if( lData[ i ].dwData & 0x80 )
				{
					gIKBD.mKeyPressedFlag = 0xFF;
					gIKBD.mLastKeypress   = (U8)lIndex;

					gIKBD.mKbdPressTable[ lIndex ] = 0xFF;
					lIndex |= 0x80;
				}
				else
				{
					gIKBD.mKbdPressTable[ lIndex ] = 0x00;
				}

				gIKBD.mKbdBuffer[ gIKBD.mKbdTail ] = (U8)lIndex;

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
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DI_MouseUpdate( void )
* ACTION   : IKBD_DI_MouseUpdate
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DI_MouseUpdate( void )
{
	DWORD				lElementCount;
	DWORD				i;
	DIDEVICEOBJECTDATA	lData[ dIKBDDI_MOUSEBUFFER_LIMIT ];
	HRESULT				lRes;

	if( gpIkbdDIMouse )
	{
		lElementCount = dIKBDDI_MOUSEBUFFER_LIMIT;

		lRes = IDirectInputDevice8_GetDeviceData(
			gpIkbdDIMouse,
			sizeof(DIDEVICEOBJECTDATA),
			lData,
			&lElementCount,
			0 );

		if( DI_OK != lRes )
		{
			lRes = IDirectInputDevice8_Acquire( gpIkbdDIMouse );
			while( lRes == DIERR_INPUTLOST )
			{
				lRes = IDirectInputDevice8_Acquire( gpIkbdDIMouse );
			}
		}

		if( !FAILED( lRes ) )
		{
			for( i=0; i<lElementCount; i++ )
			{
				switch( lData[ i ].dwOfs )
				{
				case DIMOFS_BUTTON0:
					if( lData[ i ].dwData & 0x80 )
					{
						gIKBD.mMouseKeys |= eIKBD_MOUSEBUTTON_LEFT;
					}
					else
					{
						gIKBD.mMouseKeys &= ~eIKBD_MOUSEBUTTON_LEFT;
					}
					break;

				case DIMOFS_BUTTON1:
					if( lData[ i ].dwData & 0x80 )
					{
						gIKBD.mMouseKeys |= eIKBD_MOUSEBUTTON_RIGHT;
					}
					else
					{
						gIKBD.mMouseKeys &= ~eIKBD_MOUSEBUTTON_RIGHT;
					}
					break;

				case DIMOFS_X:
					gIKBD.mMouseX = (S16)(gIKBD.mMouseX + lData[ i ].dwData);
					break;

				case DIMOFS_Y:
					gIKBD.mMouseY =(S16)(gIKBD.mMouseY + lData[ i ].dwData);
					break;
				}
			}
		}
	}
}


/* ################################################################################ */

#endif
