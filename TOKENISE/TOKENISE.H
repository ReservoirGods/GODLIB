#ifndef	INCLUDED_TOKENISE_H
#define	INCLUDED_TOKENISE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/HASHLIST/HASHLIST.H>
#include	<GODLIB/STRING/STRLIST.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

enum
{
	eTOKEN_ARG_LIMIT = 32,
	eTOKEN_STRINGSPACE_LIMIT = 256
};

enum
{
	eTOKENISER_TYPE_NONE,
	eTOKENISER_TYPE_U8,
	eTOKENISER_TYPE_S8,
	eTOKENISER_TYPE_U16,
	eTOKENISER_TYPE_S16,
	eTOKENISER_TYPE_U32,
	eTOKENISER_TYPE_S32,
	eTOKENISER_TYPE_FP32,

	eTOKENISER_TYPE_FIX8_8,
	eTOKENISER_TYPE_FIX16_16,

	eTOKENISER_TYPE_CHAR,
	eTOKENISER_TYPE_CHARMAP,
	eTOKENISER_TYPE_CONSTANT,
	eTOKENISER_TYPE_CHUNKHASH,
	eTOKENISER_TYPE_CHUNKNAME,
	eTOKENISER_TYPE_REFERENCE,
	eTOKENISER_TYPE_STRING,
	eTOKENISER_TYPE_TAGVALUE,

	eTOKENISER_TYPE_LIMIT,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sTokeniserArgs
{
	U16		mArgCount;
	U16		mLineLen;
	char *	mpStrings[ eTOKEN_ARG_LIMIT ];
	char	mStringSpace[ eTOKEN_STRINGSPACE_LIMIT ];
	char	mLine[ eTOKEN_STRINGSPACE_LIMIT ];
} sTokeniserArgs;

typedef	void	(*fTokeniseDeInit)( void );
typedef	void	(*fTokeniseInit)( const char * apTitle, const U16 aIndex, const char * apName );
typedef	void	(*fTokeniseVar)( sTokeniserArgs * apArgs );

typedef	struct	sTokeniserMember
{
	U16		mType;
	U32		mValue;
	char *	mpString;
	U32		mOffset;
} sTokeniserMember;

typedef	struct	sTokeniserStructMember
{
	U16		mType;
	U16		mSubType;
	U32		mCountOffset;
	U16		mArrayFlag;
	char *	mpString;
	U32		mOffset;
} sTokeniserStructMember;

typedef	struct	sTokeniserChunkDef
{
	U16							mIndex;
	char *						mpChunkName;
	sTokeniserStructMember *	mpStructDef;
	U32							mStructSize;
	U32							mCountOffset;
	U32							mPtrOffset;
} sTokeniserChunkDef;

typedef	struct	sTokeniserTagString
{
	U32		mID;
	char *	mpString;
} sTokeniserTagString;

typedef	struct	sTokeniserTagStringList
{
	U32						mID;
	U32						mCount;
	sTokeniserTagString *	mpTags;
} sTokeniserTagStringList;

typedef	struct	sTokeniserHandler
{
	fTokeniseInit	mpOnChunkInit;
	fTokeniseDeInit	mpOnChunkDeInit;
	fTokeniseVar	mpOnVar;
	fTokeniseVar	mpOnLine;
	char *			mpChunkName;
} sTokeniserHandler;


typedef	void	(*fTokeniseVarTypeParse)( U8 * mpStructBase, const sTokeniserStructMember * apMember, const sTokeniserArgs * apArgs );
typedef	void	(*fTokeniseVarTypeRelocate)( U8 * mpStructBase, const sTokeniserStructMember * apMember );
typedef	void	(*fTokeniseWarning)( const char * apString );

typedef	struct	sTokeniserInfo
{
	U16							mChunkCount;
	U32							mContainerSize;
	sTokeniserChunkDef *		mpChunkDefs;
	sTokeniserTagStringList *	mpTagStringList;
	fTokeniseWarning			mOnWarning;
} sTokeniserInfo;

typedef	struct	sTokeniserChunkContainer
{
	U32		mCount;
	U32		mIndex;
	U32		mStructSize;
	U32 *	mpHashes;
	U8 *	mpData;
	U8 *	mpDataMax;
	S32		mRelocOffset;
} sTokeniserChunkContainer;

typedef	struct	sTokeniserArray
{
	U16							mCount;
	U16							mItemSize;
	U16							mItemIndex;
	U8 *						mpItems;
	struct sTokeniserArray *	mpNext;
} sTokeniserArray;

typedef	struct	sTokeniserArrayList
{
	U16					mCount;
	sTokeniserArray *	mpArrays;
} sTokeniserArrayList;

typedef	struct	sTokeniserBuilder
{
	const char *				mpText;
	U32							mSize;
	U32							mChunkHashCurrent;
	const sTokeniserChunkDef *	mpChunkDefCurrent;
	sTokeniserStructMember *	mpStructMemberCurrent;
	sString						mChunkTitle;
	sString						mChunkName;
	U8 *						mpContainer;
	U8 *						mpDataItem;
	sTokeniserInfo *			mpInfo;
	sStringList					mStringList;
	sTokeniserArrayList			mArrayList;
	sTokeniserChunkContainer *	mpChunkContainers;
} sTokeniserBuilder;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Tokeniser_Parse( const char * apText, const U32 aSize, const sTokeniserHandler * apHandlers, const U16 aHandlerCount );

sTokeniserBuilder *	Tokeniser_Init( const char * apText, const U32 aSize, sTokeniserInfo * apInfo );
void				Tokeniser_DeInit( sTokeniserBuilder * apBuilder );
void *				Tokeniser_Serialise( sTokeniserBuilder * apBuilder );
void 				Tokeniser_Delocate( U8 * apData, sTokeniserInfo * apInfo );
void 				Tokeniser_Relocate( U8 * apData, sTokeniserInfo * apInfo );
void				Tokeniser_ShowInfo( U8 * apData, sTokeniserInfo * apInfo );

void				Tokeniser_MemberBuild( const sTokeniserStructMember * apMembers, const sTokeniserArgs * apArgs, void * apStruct );

U32					TokeniserArray_GetSize( sTokeniserArray * apArray );
void				TokeniserArrayList_Init( sTokeniserArrayList * apList );
void				TokeniserArrayList_DeInit( sTokeniserArrayList * apList );
sTokeniserArray *	TokeniserArrayList_ArrayCreate( sTokeniserArrayList * apList );
void				TokeniserArrayList_ArrayDestroy( sTokeniserArrayList * apList, sTokeniserArray * apArray );
U32					TokeniserArrayList_GetSize( sTokeniserArrayList * apList );
void 				TokeniserArrayList_Serialise( sTokeniserArrayList * apList, void * apDst );

U32					TokeniserArgs_Build( sTokeniserArgs * apArgs, const char * apText, U32 aOffset, const U32 aTextSize );
S16					TokeniserArgs_GetS16( const sTokeniserArgs * apArgs );
U16					TokeniserArgs_GetU16( const sTokeniserArgs * apArgs );
S32					TokeniserArgs_GetS32( const sTokeniserArgs * apArgs );
U32					TokeniserArgs_GetU32( const sTokeniserArgs * apArgs );
FP32				TokeniserArgs_GetFP32( const sTokeniserArgs * apArgs );
U32					TokeniserArgs_GetTagID( const sTokeniserArgs * apArgs, const sTagString * apTags, const U32 aTagLimit );
U32					TokeniserArgs_GetTagValue( const sTokeniserArgs * apArgs, const sTagValue * apTags, const U32 aTagLimit );
U8					TokeniserArgs_IsChunkHeader( const sTokeniserArgs * apArgs );

void				TokeniserArgs_MemberBuild( const sTokeniserArgs * apArgs, sTokeniserMember * apMembers, void * apStruct );

const sTokeniserStructMember *	Tokeniser_GetStructMember( const sTokeniserStructMember * apList, const char * apString );
const sTokeniserChunkDef *		Tokeniser_GetChunkDef( const sTokeniserChunkDef * apList, const char * apString );
const sTokeniserChunkDef *		Tokeniser_GetChunkDefFromIndex( const sTokeniserChunkDef * apList, const U16 aIndex );
const sTokeniserTagString *		Tokeniser_GetTagString( const sTokeniserTagString * apList, const char * apString );
const sTokeniserTagString *		Tokeniser_GetTagStringFromID( const sTokeniserTagString * apList, const U32 aID );
const sTokeniserTagStringList *	Tokeniser_GetTagStringList( const sTokeniserTagStringList * apList, const U32 aID );


/* ################################################################################ */

#endif	/* INCLUDED_TOKENISE_H */
