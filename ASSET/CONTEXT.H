#ifndef	INCLUDED_CONTEXT_H
#define	INCLUDED_CONTEXT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


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
	U16					mSlowRamFlag;
	U16					mAllocFlag;
	S32					mRefCount;
	struct sPackage *	mpPackages;
	struct sAsset *		mpAssets;
	struct sAssetClient *		mpAssetClients;
	struct sContext *	mpNext;
} sContext;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void				Context_Init( sContext * apContext, const char * apName );
void				Context_DeInit( sContext * apContext );

struct sAsset *		Context_AssetRegister( sContext * apContext, const char * apName );
void				Context_AssetUnRegister( sContext * apContext, struct sAsset * apAsset );

struct sAssetClient* Context_AssetClient_Find( const sContext * apContext, U32 aHashKey );

						/* returns parent ptr */
struct sAssetClient*	Context_AssetClient_Add( sContext * apContext, struct sAssetClient * apClient );
void					Context_AssetClient_Remove( struct sAssetClient * apClient );

void				ContextManager_Init( void );
void				ContextManager_DeInit( void );
void				ContextManager_Update( void );
sContext *			ContextManager_ContextRegister( const char * apName );
void				ContextManager_ContextUnRegister( sContext * apContext );
void				ContextManager_ShowAll( fContextPrint aPrint );


/* ################################################################################ */

#endif	/* INCLUDED_CONTEXT_H */
