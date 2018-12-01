#ifndef	INCLUDED_PKG_DIR_H
#define	INCLUDED_PKG_DIR_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	"PACKAGE.H"


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32		PackageDir_Load( sPackage * apPackage, const char * apDirName );
void	PackageDir_Destroy( sPackage * apPackage );

U8		PackageDir_AssetLoad( sPackage * apPackage, struct sAssetClient * apClient );

/* ################################################################################ */

#endif	/* INCLUDED_PKG_DIR_H */
