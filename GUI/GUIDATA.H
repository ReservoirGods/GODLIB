#ifndef	INCLUDED_GUIDATA_H
#define	INCLUDED_GUIDATA_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GUI/GUI.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sGuiData
{
	U32				mID;
	U32				mVersion;

	U16				mActionCount;
	U16				mAssetCount;

	U16				mButtonCount;
	U16				mButtonStyleCount;

	U16				mColourCount;
	U16				mCursorCount;

	U16				mFillCount;
	U16				mFontGroupCount;

	U16				mKeyActionCount;
	U16				mListCount;

	U16				mLockCount;
	U16				mSliderCount;

	U16				mValueCount;
	U16				mVarCount;

	U16				mWindowCount;
	U16				mPad;

	sGuiAsset *			mpAssets;
	sGuiAction *		mpActions;
	sGuiButton *		mpButtons;
	sGuiButtonStyle *	mpButtonStyles;
	sGuiColour *		mpColours;
	sGuiCursor *		mpCursors;
	sGuiFill *			mpFills;
	sGuiFontGroup *		mpFontGroups;
	sGuiKeyAction *		mpKeyActions;
	sGuiList *			mpLists;
	sGuiLock *			mpLocks;
	sGuiSlider *		mpSliders;
	sGuiValue *			mpValues;
	sGuiVar *			mpVars;
	sGuiWindow *		mpWindows;
} sGuiData;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		GuiData_Init( sGuiData * apData, sHashTree * apTree );
void		GuiData_DeInit( sGuiData * apData, sHashTree * apTree  );
void		GuiData_Relocate( sGuiData * apData );
void		GuiData_Delocate( sGuiData * apData );
sGuiData *	GuiData_Serialise( sGuiData * apData );
U32			GuiData_GetSize( sGuiData * apData );
U32			GuiData_GetArraysSize( sGuiData * apData );
U32			GuiData_GetStringsSize( sGuiData * apData );

void		GuiData_EventsRegister(   sGuiData * apData, sHashTree * apTree );
void		GuiData_EventsUnRegister( sGuiData * apData, sHashTree * apTree );


/* ################################################################################ */

#endif	/* INCLUDED_GUIDATA_H */
