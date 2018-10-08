#ifndef INCLUDED_REFLECT_H
#define INCLUDED_REFLECT_H

#include	<GODLIB\BASE\BASE.H>

enum eReflectType
{
	eReflectType_Leaf   = ( 1 << 0 ),
	eReflectType_Signed = ( 1 << 1 ),
	eReflectType_String = ( 1 << 2 ),
};

typedef struct sReflectElement
{
	char *				mpTypeName;
	char *				mpElementName;
	U32					mOffset;
	struct sLexerType *	mpType;
} sReflectElement;

typedef struct sReflectType
{
	char *						mpTypeName;
	U32							mSizeBytes;
	U16							mTypeFlags;
	U16							mElementCount;
	struct sReflectElement *	mpElements;
	struct sReflectType *		mpTypeNext;
} sReflectType;

typedef struct sReflectDictionary
{
	U32						mTypeCount;
	struct sReflectType *	mpTypes;
}sReflectDictionary;

sReflectType *	Reflect_GetpType( const sReflectDictionary * apDictionary, const char * apType );
U8 *			Reflect_GetpData( const sReflectType * apType, U8 * apData,  const char * apElement );
U8				Reflect_SetData( const sReflectType * apType, const struct sString * apElementName, const struct sString * apValue, void * apStructBase );

#endif