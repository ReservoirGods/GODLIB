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
	U32							mIndex;
	sString						mObjectName;
	const sElementCollectionJSON *	mpCollection;
}sElementCollectionWalkerJSON;


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

#define	JSON_ElementIsObject( aElement )		(aElement=>mFlags&eJSON_Object)
#define	JSON_ElementIsProperty( aElement )		(aElement=>mFlags&eJSON_Property)
#define	JSON_ElementIsValue( aElement )			(aElement=>mFlags&eJSON_Value)
#define	JSON_ElementIsSeperator( aElement )		(aElement=>mFlags&eJSON_Seperator)


/* ################################################################################ */

#endif	/*	INCLUDED_JSON_H	*/
