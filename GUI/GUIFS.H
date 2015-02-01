#ifndef	INCLUDED_GUIFS_H
#define	INCLUDED_GUIFS_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/HASHTREE/HASHTREE.H>
#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

enum
{
	eGUIFS_BUTTON_OK,
	eGUIFS_BUTTON_CANCEL,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

struct	sGuiFSInfo;

typedef	void	(*fGuiFSCB)( struct sGuiFSInfo * apInfo );

typedef	struct	sGuiFSInfo
{
	sString *	mpTitle;
	sString *	mpDrive;
	sString *	mpFilePath;
	sString *	mpFileName;
	sString *	mpFileMask;
	fGuiFSCB	mpCB;
	U16			mButton;
} sGuiFSInfo;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		GuiFS_Init( sHashTree * apTree );
void		GuiFS_DeInit( sHashTree * apTree );

void		GuiFSInfo_Init( sGuiFSInfo * apInfo, const char * apTitle, const char * apMask, const char * apFileName );
void		GuiFSInfo_DeInit( sGuiFSInfo * apInfo );

void		GuiFS_Open( sGuiFSInfo * apInfo );
sString	*	GuiFSInfo_FullNameBuild( sGuiFSInfo * apInfo );


/* ################################################################################ */

#endif	/* INCLUDED_GUIFS_H */
