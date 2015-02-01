#ifndef	INCLUDED_GUIEDIT_H
#define	INCLUDED_GUIEDIT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GUI/GUI.H>
#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGUI_TEXTLINE_LIMIT		512

enum
{
	eGUI_TEXTLINE_NONE,
	eGUI_TEXTLINE_CANCEL,
	eGUI_TEXTLINE_UPDATE,
	eGUI_TEXTLINE_WRITEBACK,

	eGUI_TEXTLINE_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sGuiTextLine
{
	U16			mCursorX;
	U16			mEditType;
	U8			mShiftFlag;
	U8			mCapsFlag;
	U16			mEditFlag;
	sString *	mpString;
} sGuiTextLine;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U8				GuiTextLine_Init( sGuiTextLine * apLine, sHashTreeVar * apVar );
void			GuiTextLine_DeInit( sGuiTextLine * apLine );
U16				GuiTextLine_Update( sGuiTextLine * apLine, sGuiIKBD * apIKBD );
void			GuiTextLine_Insert( sGuiTextLine * apLine, const U8 aChar );
void			GuiTextLine_Delete( sGuiTextLine * apLine );
void			GuiTextLine_Backspace( sGuiTextLine * apLine );
U8				GuiTextLine_GetValidAscii( sGuiTextLine * apLine, const U8 aScan );


/* ################################################################################ */

#endif	/* INCLUDED_GUIEDIT_H */
