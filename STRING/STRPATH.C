/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"STRPATH.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  ENUMS
################################################################################### */



/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_Init( sStringList * apList )
* ACTION   : StringList_Init
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const char *		StringPath_GetpExt( sStringPath * apPath )
{
	const char * lpExt = 0;
	U16 i;

	for( i=0; apPath->mChars[i]; i++ )
	{
		if( '.' == apPath->mChars[ i ] )
		{
			lpExt = &apPath->mChars[ i ];
		}
	}

	return( lpExt );
}

void		StringPath_SetExt( sStringPath * apPath, const char * apExt )
{
	char * lpExt = (char *)StringPath_GetpExt( apPath );
	if( lpExt )
	{
		U16 i = (U16)(lpExt - &apPath->mChars[0]);
		for( ; i<sizeof(sStringPath)-1 && *apExt; apPath->mChars[i++]=*apExt++);
		apPath->mChars[i]=0;
	}
}

void	StringPath_Append( sStringPath * apPath, const char * apFile )
{
	U16 i=0;
	for( i=0; apPath->mChars[i]; i++ );

	if( i && i<sizeof(sStringPath)-1 && '\\' !=apPath->mChars[i-1] && '/' != apPath->mChars[i-1])
	{
		apPath->mChars[i++] = '\\';
	}
	for( ; (i<sizeof(sStringPath)-1) && *apFile; apPath->mChars[ i++ ] = *apFile++);
	apPath->mChars[ i ] = 0;
}

void	StringPath_Set( sStringPath * apPath, const char * apFile )
{
	U16 i=0;
	for( ; (i<sizeof(sStringPath)-1) && *apFile; apPath->mChars[ i++ ] = *apFile++);
	apPath->mChars[ i ] = 0;
}


void	StringPath_Copy( sStringPath * apDst, const sStringPath * apSrc )
{
	U16 i;
	const char * lpSrc = apSrc->mChars;

	for( i=0; (i<sizeof(sStringPath)-1) && *lpSrc; apDst->mChars[i++] = *lpSrc++ );
	apDst->mChars[i] = 0;
}

void	StringPath_CopySS( sStringPath * apDst, const sString * apSrc )
{
	U32 len = String_GetLength( apSrc );
	U32 i;
	if( len > sizeof( sStringPath )-1 )
		len = sizeof( sStringPath )-1;

	for( i = 0; i < len; i++ )
		apDst->mChars[ i ] = apSrc->mpChars[ i ];
	apDst->mChars[ i ] = 0;
}



const char *		StringPath_GetpFileName( sStringPath * apPath )
{
	const char * lpFileName = apPath->mChars;
	U16 i;

	if( apPath->mChars[0])
	{
		for( i=1; apPath->mChars[i]; i++ )
		{
			if( ('\\' == apPath->mChars[i-1]) || ('/' == apPath->mChars[i-1]) )
			{
				lpFileName = &apPath->mChars[i];
			}
		}
	}
	return( lpFileName );
}


void				StringPath_SetFileName( sStringPath * apPath, const char * apFileName )
{
	U16 i;
	const char * lpFileName = StringPath_GetpFileName(apPath);
	
	for( i = (U16)(lpFileName - &apPath->mChars[0]); i<sizeof(sStringPath)-1 && *apFileName; apPath->mChars[i++]=*apFileName++);
}


void		StringPath_Combine( sStringPath * apPath, const char * apDir, const char * apFileName )
{
	U16 i;

	for( i=0; i<sizeof(sStringPath)-1 && *apDir; apPath->mChars[i++] = *apDir++ );
	if( i && i<sizeof(sStringPath)-1 && '\\' !=apPath->mChars[i-1] && '/' != apPath->mChars[i-1])
	{
		apPath->mChars[i++] = '\\';
	}
	for( ; i<sizeof(sStringPath)-1 && apFileName; apPath->mChars[i++] = *apFileName++ );
	apPath->mChars[i]=0;
}


void		StringPath_Combine2( sStringPath * apPath, const char * apDir0, const char * apDir1, const char * apFileName )
{
	U16 i;
	for( i=0; i<sizeof(sStringPath)-1 && apDir0[i]; apPath->mChars[i++] = *apDir0++ );
	if( i && i<sizeof(sStringPath)-1 && '\\' !=apPath->mChars[i-1] && '/' != apPath->mChars[i-1])
	{
		apPath->mChars[i++] = '\\';
	}
	for( ; i<sizeof(sStringPath)-1 && apDir1[i]; apPath->mChars[i++] = *apDir1++ );
	if( i && i<sizeof(sStringPath)-1 && '\\' !=apPath->mChars[i-1] && '/' != apPath->mChars[i-1])
	{
		apPath->mChars[i++] = '\\';
	}
	for( ; i<sizeof(sStringPath)-1 && apFileName[i]; apPath->mChars[i++] = *apFileName++ );
	apPath->mChars[ i ] =0;
}

char	StringPath_GetDrive( const sStringPath * apPath )
{
	char lRes = 0;
	if( ':' == apPath->mChars[ 1 ] )
		lRes = apPath->mChars[ 0 ];
	return lRes;
}

U16			StringPath_IsAbsolute( const sStringPath * apPath )
{
	return( 0 != StringPath_GetDrive( apPath ) );
}

void			StringPath_GetDirectory( sStringPath * apDst, const sStringPath * apSrc )
{
	U16 i;
	U16 d = 0;

	for( i = 0; i < sizeof(sStringPath) && apSrc->mChars[ i ]; i++ )
	{
		apDst->mChars[ i ] = apSrc->mChars[ i ];
		if( StringPath_IsSeperator( apSrc->mChars[ i ] ) )
			d = i;
	}
	apDst->mChars[ d ] = 0;

}
void			StringPath_GetFolder( sStringPath * apDst, const sStringPath * apSrc )
{
	U16 i = 0;
	U16 a = 0;
	U16 b = 0;
	for( i = 0; i < sizeof( sStringPath ) && apSrc->mChars[ i ]; i++ )
	{
		if( StringPath_IsSeperator( apSrc->mChars[i] ) )
		{
			a = b;
			b = i;
		}
	}
	if( StringPath_IsSeperator( apSrc->mChars[a] ) )
		a++;

	if( 2 == b && ':' == apSrc->mChars[ 1 ] )
	{
		apDst->mChars[ 0 ] = 0;
	}
	else
	{
		for( i = 0; a < b; )
			apDst->mChars[ i++ ] = apSrc->mChars[ a++ ];
		apDst->mChars[ i ] = 0;
	}
}

void			StringPath_Compact( sStringPath * apDst, const sStringPath * apSrc )
{
	U16	i,j=0;
	for( i = 0; i<sizeof(sStringPath) && apSrc->mChars[ i ];  )
	{
		apDst->mChars[ j++ ] = apSrc->mChars[ i++ ];
		if( (j > 2) && ('.' == apDst->mChars[ j - 1 ]) && ('.' == apDst->mChars[ j - 2 ]) )
		{
			for( j -= 3; j && !StringPath_IsSeperator( apDst->mChars[ j-1 ] ); j-- );
			if( j )
				j--;
		}
	}
	if( j && StringPath_IsSeperator( apDst->mChars[ j - 1 ] ) )
		j--;

	apDst->mChars[ j ] = 0;
}

/* ################################################################################ */
