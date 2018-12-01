#ifndef	INCLUDED_PACKAGE_H
#define	INCLUDED_PACKAGE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/ASSET/CONTEXT.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	void	(*fPackagePrint)( const char * apString );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sPackageItem
{
	struct	sAsset *		mpAsset;
} sPackageItem;


typedef	struct sPackage
{
	char				mName[ 16 ];
	U32					mID;
	U32					mFileCount;
	U16					mStatus;
	U16					mLoaderType;
	struct	sContext *	mpContext;
	struct sLinkFile *	mpLinkFile;
	sPackageItem *		mpItems;
	struct sPackage *	mpNext;
	struct sPackage *	mpContextNext;
} sPackage;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		PackageManager_Init( void );
void		PackageManager_DeInit( void );
void		PackageManager_Update( void );

void		PackageManager_Load(   sPackage * apPackage );
void		PackageManager_UnLoad( sPackage * apPackage );
void		PackageManager_ReLoad( sPackage * apPackage );

void		PackageManager_SetLinkPath( const char * apPath );
void		PackageManager_SetFilePath( const char * apPath );
void		PackageManager_SetLinkEnableFlag( U16 aLinkEnableFlag );

U32			PackageManager_OpQueueIsEmpty( void );

void		Package_Init(  sPackage * apPackage, const char * apName, const char * apContext );
void		Package_DeInit(sPackage * apPackage );

void		PackageManager_ShowAll( fPackagePrint aPrint );
void		PackageManager_ShowStatus( fPackagePrint aPrint );
void		PackageManager_ShowUnused( fPackagePrint aPrint );

U8			PackageManager_AssetLoad( sPackage * apPackage, struct sAssetClient * apClient );


/* ################################################################################ */

#endif	/* INCLUDED_PACKAGE_H */
