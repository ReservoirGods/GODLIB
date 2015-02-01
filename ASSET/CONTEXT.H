#ifndef	INCLUDED_CONTEXT_H
#define	INCLUDED_CONTEXT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/PACKAGE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	void	(*fContextPrint)( const char * apString );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sContext
{
	char				mName[ 16 ];
	U32					mID;
	U32					mSlowRamFlag;
	S32					mRefCount;
	struct sPackage *	mpPackages;
	struct sAsset *		mpAssets;
	struct sContext *	mpNext;
} sContext;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sContext *			Context_Create(  const char * apName );
void				Context_Destroy( sContext * apContext );
struct sAsset *		Context_AssetRegister( sContext * apContext, const char * apName );
void				Context_AssetUnRegister( sContext * apContext, struct sAsset * apAsset );

void				ContextManager_Init( void );
void				ContextManager_DeInit( void );
void				ContextManager_Update( void );
sContext *			ContextManager_ContextRegister( const char * apName );
void				ContextManager_ContextUnRegister( sContext * apContext );
void				ContextManager_ShowAll( fContextPrint aPrint );


/* ################################################################################ */

#endif	/* INCLUDED_CONTEXT_H */
