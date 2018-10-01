/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GUIEDIT.H"

#include	<GODLIB/GUI/GUIDATA.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/IKBD/IKBD.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiTextLine_Init( sGuiTextLine * apLine,sHashTreeVar * apVar )
* ACTION   : GuiTextLine_Init
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	GuiTextLine_Init( sGuiTextLine * apLine,sHashTreeVar * apVar )
{
	U8			lRes;
	sString *	lpString;

	DebugLog_Printf2( "GuiTextLineInit() apLine:%p apVar:%p", apLine, apVar );

	lRes = 0;
	if( apLine && apVar )
	{
		HashTree_VarRead( apVar, &lpString, sizeof(lpString) );
		if( lpString )
		{
			apLine->mEditType = eGUI_EDITTYPE_STRING;
			apLine->mpString  = lpString;
			apLine->mCursorX  = (U16)String_GetLength( lpString );
			apLine->mEditFlag = 1;
			lRes              = 1;
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiTextLine_DeInit( sGuiTextLine * apLine )
* ACTION   : GuiTextLine_DeInit
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiTextLine_DeInit( sGuiTextLine * apLine )
{
	apLine->mEditFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiTextLine_Update( sGuiTextLine * apLine,sGuiIKBD * apIKBD )
* ACTION   : GuiTextLine_Update
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	GuiTextLine_Update( sGuiTextLine * apLine,sGuiIKBD * apIKBD )
{
	U16	lRes;
	U16	lKey;
	U16	i;
	U8	lAsc;

	if( apLine->mpString )
	{

		lRes = eGUI_TEXTLINE_NONE;

		i = 0;
		while( (i<apIKBD->mCount) && (lRes == eGUI_TEXTLINE_NONE) )
		{
			lKey = (U16)apIKBD->mData[ i ];
			if( lKey > 0x80 )
			{
				lKey -= 0x80;
				switch( lKey )
				{
				case	eIKBDSCAN_LEFTSHIFT:
				case	eIKBDSCAN_RIGHTSHIFT:
					apLine->mShiftFlag = 0;
					break;
				}
			}

			else switch( lKey )
			{
			case	eIKBDSCAN_BACKSPACE:
				if( apLine->mCursorX )
				{
					apLine->mCursorX--;
					String_CharRemove( apLine->mpString, apLine->mCursorX );
				}
				break;

			case	eIKBDSCAN_DELETE:
				String_CharRemove( apLine->mpString, apLine->mCursorX );
				break;

			case	eIKBDSCAN_LEFTSHIFT:
			case	eIKBDSCAN_RIGHTSHIFT:
				apLine->mShiftFlag = 1;
				break;

			case	eIKBDSCAN_LEFTARROW:
				if( apLine->mCursorX )
				{
					if( apLine->mShiftFlag )
					{
						apLine->mCursorX = 0;
					}
					else
					{
						apLine->mCursorX--;
					}
					lRes = eGUI_TEXTLINE_UPDATE;
				}
				break;

			case	eIKBDSCAN_RIGHTARROW:
				if( apLine->mCursorX < String_GetLength( apLine->mpString ) )
				{
					if( apLine->mShiftFlag )
					{
						apLine->mCursorX = (U16)String_GetLength( apLine->mpString );
					}
					else
					{
						apLine->mCursorX++;
					}
					lRes = eGUI_TEXTLINE_UPDATE;
				}
				break;

			case	eIKBDSCAN_CLRHOME:
				apLine->mCursorX = 0;
				lRes = eGUI_TEXTLINE_UPDATE;
				break;

			case	eIKBDSCAN_ESC:
				lRes = eGUI_TEXTLINE_CANCEL;
				break;

			case	eIKBDSCAN_NUMPADENTER:
			case	eIKBDSCAN_RETURN:
			case	eIKBDSCAN_TAB:
				lRes = eGUI_TEXTLINE_WRITEBACK;
				break;

			default:
				lAsc = GuiTextLine_GetValidAscii( apLine, apIKBD->mData[ i ] );
				if( lAsc )
				{
					String_CharInsert( apLine->mpString, apLine->mCursorX, lAsc );
					apLine->mCursorX++;
				}
				break;

			}
			i++;
		}
		apIKBD->mCount = 0;
	}
	else
	{
		lRes = eGUI_TEXTLINE_CANCEL;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiTextLine_GetValidAscii( sGuiTextLine * apLine,const U8 aScan )
* ACTION   : GuiTextLine_GetValidAscii
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	GuiTextLine_GetValidAscii( sGuiTextLine * apLine,const U8 aScan )
{
	U8	lAscii;

	if( apLine->mShiftFlag )
	{
		lAscii = IKBD_GetAsciiShift( aScan );
	}
	else if( apLine->mCapsFlag )
	{
		lAscii = IKBD_GetAsciiCaps( aScan );
	}
	else
	{
		lAscii = IKBD_GetAsciiNormal( aScan );
	}

	if( lAscii )
	{
		switch( apLine->mEditType )
		{
		case	eGUI_EDITTYPE_DECIMAL:
			if( (lAscii < '0') || (lAscii > '9') )
			{
				lAscii = 0;
			}
			break;
		case	eGUI_EDITTYPE_HEX:
			if( (lAscii < '0') ||
				( (lAscii > '9') && (lAscii <'A') ) ||
				( (lAscii > 'F') && (lAscii <'a') ) ||
				(lAscii > 'f') )
			{
				lAscii = 0;
			}
			break;
		case	eGUI_EDITTYPE_FLOAT:
			if( (lAscii < '0') || (lAscii > '9') || (lAscii!='.') )
			{
				lAscii = 0;
			}
			break;
		case	eGUI_EDITTYPE_FILENAME:
		case	eGUI_EDITTYPE_PAGE:
		case	eGUI_EDITTYPE_STRING:
			break;
		}
	}

	return( lAscii );
}


/* ################################################################################ */
