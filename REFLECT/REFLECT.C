#include	"REFLECT.H"

#include	<GODLIB\STRING\STRING.H>
#include	<GODLIB\STRING\STRPATH.H>

sReflectType	gReflectTypesFundamental[] =
{
	{ "S8",		sizeof( S8 ),		eReflectType_Leaf | eReflectType_Signed,	0 },
	{ "S16",	sizeof( S16 ),		eReflectType_Leaf | eReflectType_Signed,	0 },
	{ "S32",	sizeof( S32 ),		eReflectType_Leaf | eReflectType_Signed,	0 },
	{ "U8",		sizeof( U8 ),		eReflectType_Leaf,							0 },
	{ "U16",	sizeof( U16 ),		eReflectType_Leaf,							0 },
	{ "U32",	sizeof( U32 ),		eReflectType_Leaf,							0 },
	{ "F32",	sizeof( F32 ),		eReflectType_Float,							0 },
	{ "string", sizeof( sString	),	eReflectType_Leaf | eReflectType_String,	0 },
};

sReflectDictionary gReflectFundamentalDictionary = { mARRAY_COUNT( gReflectTypesFundamental), gReflectTypesFundamental };

sReflectType *	Reflect_GetpType( const sReflectDictionary * apDictionary, const char * apType )
{
/*
	sReflectType * lpType = apDictionary->mpTypes;
	while( lpType && String_StrCmp( apType, lpType->mpTypeName ) )
		lpType = lpType->mpTypeNext;

	return( lpType );
*/
	U32 i;
	for( i = 0; i < apDictionary->mTypeCount; i++ )
	{
		if( !String_StrCmp( apDictionary->mpTypes[ i ].mpTypeName, apType ) )
			return( &apDictionary->mpTypes[ i ] );
	}
	return 0;
}

U8 *			Reflect_GetpData( const sReflectType * apType, U8 * apData, const char * apElement )
{
	U16 i;
	for( i = 0; i < apType->mElementCount; i++ )
	{
		if( !String_StrCmp( apElement, apType->mpElements[ i ].mpTypeName ) )
			return( apData + apType->mpElements[ i ].mOffset );
	}

	return( 0 );
}

U8			Reflect_SetData( const sReflectType * apType, const sString * apElementName, const sString * apValue, void * apStructBase )
{
	U16 i;
	U8 * lpMem = (U8*)apStructBase;
	for( i = 0; i < apType->mElementCount; i++ )
	{
		const sReflectElement * pE = &apType->mpElements[ i ];
		if( String_IsEqualNT( apElementName, pE->mpElementName ) )
		{
			const sReflectType * lpType = Reflect_GetpType( &gReflectFundamentalDictionary, pE->mpTypeName );
			if( lpType && lpType->mTypeFlags & eReflectType_Leaf )
			{
				if( lpType->mTypeFlags & eReflectType_String )
				{
					*(sString*)( &lpMem[ pE->mOffset ] ) = *apValue;
				}
				else if( lpType->mTypeFlags & eReflectType_Float )
				{

				}
				else if( lpType->mTypeFlags & eReflectType_Signed )
				{
					S32 lVal;
					sStringPath chars;
					StringPath_CopySS( &chars, apValue );
					if( !String_StrCmpi( "true", chars.mChars ) )
						lVal = 1;
					else if( !String_StrCmpi( "false", chars.mChars ) )
						lVal = 0;
					else
						lVal = atoi( chars.mChars );
					if( 1 == lpType->mSizeBytes )
						*(S8*)( &lpMem[ pE->mOffset ]) = (S8)lVal;
					else if( 2 == lpType->mSizeBytes )
						*(S16*)( &lpMem[ pE->mOffset ]) = (S16)lVal;
					else if( 2 == lpType->mSizeBytes )
						*(S32*)( &lpMem[ pE->mOffset ]) = lVal;

				}
				else
				{
					U32 lVal;
					sStringPath chars;
					StringPath_CopySS( &chars, apValue );
					if( !String_StrCmpi( "true", chars.mChars ) )
						lVal = 1;
					else if( !String_StrCmpi( "false", chars.mChars ) )
						lVal = 0;
					else
						lVal = atoi( chars.mChars );
					if( 1 == lpType->mSizeBytes )
						*(U8*)( &lpMem[ pE->mOffset ] ) = (U8)lVal;
					else if( 2 == lpType->mSizeBytes )
						*(U16*)( &lpMem[ pE->mOffset ] ) = (U16)lVal;
					else if( 2 == lpType->mSizeBytes )
						*(U32*)( &lpMem[ pE->mOffset ] ) = lVal;
				}
			}
			break;
		}
	}
	return 0;
}

