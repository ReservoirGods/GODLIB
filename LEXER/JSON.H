#ifndef	INCLUDED_JSON_H
#define	INCLUDED_JSON_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB\STRING\STRING.H>

/* ###################################################################################
#  ENUMS
################################################################################### */


enum eTypeJSON
{
	eTypeJSON_ArrayBegin,
	eTypeJSON_ArrayEnd,
	eTypeJSON_ObjectName,
	eTypeJSON_ObjectBegin,
	eTypeJSON_ObjectEnd,
	eTypeJSON_PropertyName,
	eTypeJSON_PropertyValue,
	eTypeJSON_Seperator,

	eTypeJSON_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sElementJSON
{
	sString	mToken;
	enum eTypeJSON		mTypeFlags;
	U16		mDepth;
} sElementJSON;

typedef struct sElementCollectionJSON
{
	U32				mCount;
	sElementJSON*	mpElements;
} sElementCollectionJSON;

typedef struct sElementCollectionWalkerJSON
{
	U32								mIndex;
	sString							mObjectName;
	const sElementCollectionJSON *	mpCollection;
}sElementCollectionWalkerJSON;

typedef struct sPropertyJSON
{
	sString					mPropertyName;
	U32						mValueCount;
	sString *				mpValues;
	struct sPropertyJSON *	mpSibling;
} sPropertyJSON;

typedef struct sObjectJSON
{
	sString					mObjectName;
	struct sObjectJSON *	mpChildren;
	struct sObjectJSON *	mpSibling;
	U32						mPropertyCount;
	sPropertyJSON *			mpProperties;
} sObjectJSON;

typedef struct sTreeCollectorJSON
{
	U32	mObjectCount;
	U32 mPropertyCount;
	U32 mValueCount;
	U32 mTextSize;
} sTreeCollectorJSON;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	JSON_Parse( sString * apSrc, sElementCollectionJSON  * apCollection );
void	JSON_Destroy( sElementCollectionJSON * apCollection );

void	JSON_ElementsTrim( const sElementCollectionJSON * apCollection );
void	JSON_ElementsToStruct( const sElementCollectionJSON * apCollection,  const struct sReflectType * apType, U32 * apIndex, void * apDest );

void	JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection );
void	JSON_ElementWalker_DeInit( sElementCollectionWalkerJSON * apWalker );
U8		JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker );

U32		JSON_GetObjectCount( const sElementCollectionJSON * apCollection, const char * apObjectName );
U32		JSON_GetValueCount( const sElementCollectionJSON * apCollection, const char * apObjectName, const char * apPropertyName );

sObjectJSON	*	JSON_TreeCreate( const sElementCollectionJSON * apCollection );
void		 	JSON_TreeDestroy( sObjectJSON * apJSON );

U32				JSON_Tree_GetObjectCount( const sObjectJSON * apTree, const char * apObjectName );
U32				JSON_Tree_GetPropertyCount( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName );
U32				JSON_Tree_GetPropertyValueCount( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName );
U32				JSON_Tree_GetObjectPropertyCount( const sObjectJSON * apTree, const char * apObjectName );
U32				JSON_Tree_GetPropertyTextSize( const sObjectJSON * apTree, const char * apPropertyName );
U32				JSON_Tree_GetPropertyValuesTextSize( const sObjectJSON * apTree, const char * apPropertyName );
U32				JSON_Tree_GetObjectPropertyTextSize( const sObjectJSON * apTree, const char * apObjectName );

sTreeCollectorJSON*	JSON_Tree_Collect( const sObjectJSON * apTree, const char * apObjectName, const char * apPropertyName, sTreeCollectorJSON * apCol );

#define	JSON_ElementIsObject( aElement )		(aElement=>mFlags&eJSON_Object)
#define	JSON_ElementIsProperty( aElement )		(aElement=>mFlags&eJSON_Property)
#define	JSON_ElementIsValue( aElement )			(aElement=>mFlags&eJSON_Value)
#define	JSON_ElementIsSeperator( aElement )		(aElement=>mFlags&eJSON_Seperator)


/* ################################################################################ */

#endif	/*	INCLUDED_JSON_H	*/
