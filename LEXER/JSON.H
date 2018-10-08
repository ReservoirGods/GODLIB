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




/* ###################################################################################
#  STRUCTS
################################################################################### */

/*
[array of]
object
 [arrayof] objects
 [arrayof] properties
 [ [ [ a,b,c], [ d,e,f], [8,9,10] ] ]
 a->next = b
 a_

 [ -> create arraynode
 { -> create object node
 : "
*/

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

typedef struct sBaseJSON
{
	sString		mName;
	U32			mArrayDimensions;
}sBaseJSON;

typedef struct sValueJSON
{
	sString	mValue;
	struct sValueJSON * mpValueNext;
	struct sValueJSON *	mpChildren;
} sValueJSON;

typedef struct sPropertyJSON
{
	sString					mName;
	U32						mArrayDimensions;
	sValueJSON *			mpValues;
	struct sPropertyJSON *	mpSibling;
} sPropertyJSON;

typedef struct sObjectJSON
{
	sString					mName;
	U32						mArrayDimensions;
	struct sPropertyJSON *	mpProperties;
	struct sObjectJSON *	mpChildObjects;
	struct sObjectJSON *	mpSiblingObject;
	struct sObjectJSON *	mpParent;
} sObjectJSON;

typedef struct sNodeJSON
{
	sString				mName;
	struct sNodeJSON * mpChildren;
	struct sNodeJSON * mpSibling;
	struct sNodeJSON * mpParent;
} sNodeJSON;

enum eJSON
{
	eJSON_Object    = (1<<0),	// 1
	eJSON_Property  = (1<<1),	// 2
	eJSON_Seperator = (1<<2 ),	// 4
	eJSON_Value     = (1<<3),	// 8
};

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
};

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
	U32							mIndex;
	sString						mObjectName;
	const sElementCollectionJSON *	mpCollection;
}sElementCollectionWalkerJSON;

/*
sObjectJSON *	JSON_Parse(sString * apSrc);
void			JSON_Destroy(sObjectJSON * apJSON);
*/


sNodeJSON *	JSON_Parse(sString * apSrc);
void		JSON_Destroy(sNodeJSON * apJSON);

void				JSON_DebugShow( const sNodeJSON * apJSON );
U32					JSON_GetNodeCount( const sNodeJSON * apJSON );
const sNodeJSON *	JSON_GetNode( const sNodeJSON * apJSON, const char * apPath );
void				JSON_NodesToString( const sNodeJSON * apJSON, sString * apSrc );


void	JSON_Parse2( sString * apSrc, sElementCollectionJSON  * apCollection );
void	JSON_Destroy2( sElementCollectionJSON * apCollection );

void	JSON_ElementsTrim( const sElementCollectionJSON * apCollection );
void	JSON_ElementsToStruct( const sElementCollectionJSON * apCollection,  const struct sReflectType * apType, U32 * apIndex, void * apDest );

void	JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection );
void	JSON_ElementWalker_DeInit( sElementCollectionWalkerJSON * apWalker );
U8		JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker );

#define	JSON_ElementIsObject( aElement )		(aElement=>mFlags&eJSON_Object)
#define	JSON_ElementIsProperty( aElement )		(aElement=>mFlags&eJSON_Property)
#define	JSON_ElementIsValue( aElement )			(aElement=>mFlags&eJSON_Value)
#define	JSON_ElementIsSeperator( aElement )		(aElement=>mFlags&eJSON_Seperator)

/* ################################################################################ */

#endif	/*	INCLUDED_JSON_H	*/
