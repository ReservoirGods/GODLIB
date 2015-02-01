#ifndef	INCLUDED_GUIPARSE_H
#define	INCLUDED_GUIPARSE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GUI/GUI.H>
#include	<GODLIB/GUI/GUIDATA.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		GuiParse_Init( void );
void		GuiParse_DeInit( void );

sGuiData *	GuiParse_Text(const char * apText,const U32 aSize);
void		GuiParse_Delocate( sGuiData * apHeader );
void		GuiParse_Relocate( sGuiData * apHeader );
void		GuiParse_ShowInfo( sGuiData * apHeader );


/* ################################################################################ */

#endif	/* INCLUDED_GUIPARSE_H */
