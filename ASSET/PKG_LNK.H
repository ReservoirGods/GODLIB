#ifndef	INCLUDED_PKG_LNK_H
#define	INCLUDED_PKG_LNK_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	"PACKAGE.H"


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32		PackageLnk_Load( sPackage * apPackage, const char * apDirName );
U32		PackageLnk_UnLoad( sPackage * apPackage );
void	PackageLnk_Destroy( sPackage * apPackage );


/* ################################################################################ */

#endif	/* INCLUDED_PKG_LNK_H */
