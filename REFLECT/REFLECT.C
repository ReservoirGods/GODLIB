#include	"REFLECT.H"

#include	<GODLIB\STRING\STRING.H>

sReflectType	gReflectTypesFundamental[] =
{
	{ "U8",		sizeof( U8 ), 0, 0 },
	{ "U32",	sizeof( U32 ),	0, 0 },
	{ "string", sizeof( sString	), eReflectType_String, 0 },
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
			if( lpType )
			{
				if( lpType->mTypeFlags & eReflectType_String )
				{
					*(sString*)( &lpMem[ pE->mOffset ] ) = *apValue;
				}
			}
			break;
		}
	}
	return 0;
}

