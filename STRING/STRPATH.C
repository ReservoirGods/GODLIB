/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"STRPATH.H"

#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_Set( sStringPath * apDst, const sStringPath * apSrc )
* ACTION   : sets a string path from another stringpath
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void	StringPath_Set( sStringPath * apDst, const sStringPath * apSrc )
{
	StringPath_SetNT( apDst, apSrc->mChars );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_Set( sStringPath * apPath, const char * apSrc )
* ACTION   : sets a stringpath from a null terminated string
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void	StringPath_SetNT( sStringPath * apPath, const char * apSrc )
{
	char * lpDst = StringPath_Begin( apPath );
	char * lpEnd = StringPath_End( apPath );

	for( ; ( lpDst < lpEnd ) && *apSrc; )
		*lpDst++ = *apSrc++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_SetSS( sStringPath * apDst, const sString * apSrc )
* ACTION   : sets a stringpath from a sString
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void	StringPath_SetSS( sStringPath * apDst, const sString * apSrc )
{
	U32 len = String_GetLength( apSrc );
	U32 i;
	if( len > sizeof( sStringPath ) - 1 )
		len = sizeof( sStringPath ) - 1;

	for( i = 0; i < len; i++ )
		apDst->mChars[ i ] = apSrc->mpChars[ i ];
	apDst->mChars[ i ] = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_GetpExt( const char * apPath )
* ACTION   : returns extension of null terminated string
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

const char *	StringPath_GetpExt( const char * apPath )
{
	const char * lpExt = 0;

	for( ;*apPath; apPath++ )
		if( '.' == *apPath )
			lpExt = apPath;

	return( lpExt );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_SetExt( sStringPath * apPath, const char * apExt )
* ACTION   : sets extension of string path
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void		StringPath_SetExt( sStringPath * apPath, const char * apExt )
{
	char * lpExt = (char *)StringPath_GetpExt( apPath->mChars );
	char * lpEnd = StringPath_End( apPath );
	if( !lpExt )
		for( lpExt=StringPath_Begin(apPath) ;*lpExt && ( lpExt < lpEnd ); lpExt++ );

	for( ;lpExt < lpEnd && *apExt; )
		*lpExt++ = *apExt++;

	*lpExt = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_Append( sStringPath * apPath, const char * apFile )
* ACTION   : appends to string path, and ensures seperators are inserted
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void	StringPath_Append( sStringPath * apPath, const char * apFile )
{
	char * lpStart = StringPath_Begin( apPath );
	char * lpEnd = StringPath_End( apPath );

	char * lpDst = lpStart;

	/* find end of string*/
	for( ;*lpDst && lpDst < lpEnd; )
		lpDst++;

	/* insert seperator if needed */
	if( lpDst > lpStart && lpDst < lpEnd )
		if( !StringPath_IsSeperator( lpDst[ -1 ] ) )
			*lpDst++ = '\\';

	for( ;lpDst < lpEnd && *apFile; )
		*lpDst++ = *apFile++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_GetpFileName( const sStringPath * apPath )
* ACTION   : gets filename from a path
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

const char *		StringPath_GetpFileName( const char * apPath )
{
	const char * lpFileName = apPath;

	if( apPath )
		for( apPath++; *apPath; apPath++ )
			if( StringPath_IsSeperator( apPath[ -1 ] ) )
				lpFileName = apPath;

	return( lpFileName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_SetFileName( sStringPath * apPath, const char * apFileName )
* ACTION   : sets filename in a stringpath
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void		StringPath_SetFileName( sStringPath * apPath, const char * apFileName )
{
	char * lpSrc = (char*)StringPath_GetpFileName( apFileName );
	char * lpDst = StringPath_Begin( apPath );
	char * lpEnd = StringPath_End( apPath );

	for( ;lpDst < lpEnd && *lpSrc; )
		*lpDst++ = *lpSrc++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_SetFileNameNoExt( sStringPath * apPath, const char * apFileName )
* ACTION   : gets a filename without an extension
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void	StringPath_SetFileNameNoExt( sStringPath * apPath, const char * apFileName )
{
	char * lpExt;

	StringPath_SetFileName( apPath, apFileName );
	lpExt = (char*)StringPath_GetpExt( apPath->mChars );
	if( lpExt )
		*lpExt = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_Combine( sStringPath * apPath, const char * apDir, const char * apFileName )
* ACTION   : combines a directory and filename into a path
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void		StringPath_Combine( sStringPath * apPath, const char * apDir, const char * apFileName )
{
	char * lpStart = StringPath_Begin( apPath );
	char * lpDst = lpStart;
	char * lpEnd = StringPath_End( apPath );

	for( ;lpDst < lpEnd && *apDir; )
		*lpDst++ = *apDir++;

	if( lpDst > lpStart && lpDst < lpEnd && !StringPath_IsSeperator( lpDst[ -1 ] ) )
		*lpDst++ = '\\';

	for( ;lpDst < lpEnd && *apFileName; )
		*lpDst++ = *apFileName++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_Combine2( sStringPath * apPath, const char * apDir0, const char * apDir1, const char * apFileName )
* ACTION   : combines two directories and filename into a path
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void		StringPath_Combine2( sStringPath * apPath, const char * apDir0, const char * apDir1, const char * apFileName )
{
	char * lpStart = StringPath_Begin( apPath );
	char * lpDst = lpStart;
	char * lpEnd = StringPath_End( apPath );

	for( ;lpDst < lpEnd && *apDir0; )
		*lpDst++ = *apDir0++;

	if( lpDst > lpStart && lpDst < lpEnd && !StringPath_IsSeperator( lpDst[ -1 ] ) )
		*lpDst++ = '\\';

	for( ;lpDst < lpEnd && *apDir1; )
		*lpDst++ = *apDir1++;

	if( lpDst > lpStart && lpDst < lpEnd && !StringPath_IsSeperator( lpDst[ -1 ] ) )
		*lpDst++ = '\\';

	for( ;lpDst < lpEnd && *apFileName; )
		*lpDst++ = *apFileName++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_GetDrive( const char * apPath );
* ACTION   : returns the drive if absolute path
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

char	StringPath_GetDrive( const char * apPath )
{
	char lRes = 0;
	if( apPath && ':' == apPath[1] )
		lRes = apPath[0];
	return lRes;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_IsAbsolute( const char * apPath )
* ACTION   : returns non zero if absolute
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

U16			StringPath_IsAbsolute( const char * apPath )
{
	return( 0 != StringPath_GetDrive( apPath ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_IsAbsolute( const char * apPath )
* ACTION   : gets directory from a path string
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void			StringPath_GetDirectory( sStringPath * apDst, const char * apSrc )
{
	char * lpDst = StringPath_Begin( apDst );
	char * lpEnd = StringPath_End( apDst );
	char * lpSep = 0;

	for( ;lpDst < lpEnd && *apSrc; )
	{
		if( StringPath_IsSeperator( *apSrc ) )
			lpSep = lpDst;
		*lpDst++ = *apSrc++;
	}

	if( lpDst < lpEnd )
		*lpDst = 0;

	if( lpSep )
		*lpSep = 0;
	else
		apDst->mChars[0] = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_GetFolder( sStringPath * apDst, const char * apSrc )
* ACTION   : gets folder from a path string
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void		StringPath_GetFolder( sStringPath * apDst, const char * apSrc )
{
	const char * lpSrc = apSrc;
	char * lpDst = StringPath_Begin( apDst );
	char * lpEnd = StringPath_End( apDst );

	const char * lpFolderStart = apSrc;
	const char * lpFolderEnd = apSrc;

	for( ;*lpSrc; lpSrc++ )
	{
		if( StringPath_IsSeperator( *lpSrc ) )
		{
			lpFolderStart = lpFolderEnd;
			lpFolderEnd = lpSrc;
		}
	}

	if( StringPath_IsSeperator( *lpFolderStart ) )
		lpFolderStart++;

	if( !( *lpFolderStart && ':' == lpFolderStart[ 1 ] ) )
		for( ;lpDst < lpEnd && lpFolderStart < lpFolderEnd; )
			*lpDst++ = *lpFolderStart++;

	*lpDst = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringPath_GetFolder( sStringPath * apDst, const char * apSrc )
* ACTION   : gets folder from a path string
* CREATION : 18.10.2018 PNK
*-----------------------------------------------------------------------------------*/

void			StringPath_Compact( sStringPath * apDst, const char * apSrc )
{
	char * lpStart = StringPath_Begin( apDst );
	char * lpDst = lpStart;
	char * lpEnd = StringPath_End( apDst );


	for( ;lpDst < lpEnd && *apSrc; )
	{
		*lpDst++ = *apSrc++;
		if( ( '.' == lpDst[ -1 ] ) && ( ( lpDst - lpStart ) > 2 ) && ( '.' == lpDst[ -2 ] ) )
		{
			for( lpDst -= 3; ( lpDst > lpStart ) && ( !StringPath_IsSeperator( lpDst[ -1 ] )); lpDst-- );
			if( lpDst > lpStart )
				lpDst--;

		}
	}
	if( lpDst > lpStart && StringPath_IsSeperator( lpDst[ -1 ] ) )
		lpDst--;

	*lpDst = 0;
}


char *	StringPath_GetFolder_Internal( sStringPath * apPath, char * apStart )
{
	char * begin;
	for( ;StringPath_IsSeperator(*apStart); apStart++ );
	begin = apStart;
	for( ;*apStart && !StringPath_IsSeperator(*apStart); apStart++ );
	if( !*apStart)
		return 0;
	apPath->mChars[255]=*apStart;
	*apStart=0;
	return begin;
}


char *	StringPath_GetFolderFirst( sStringPath * apPath )
{
	return StringPath_GetFolder_Internal( apPath, &apPath->mChars[0]);
}


char *	StringPath_GetFolderNext( sStringPath * apPath, char * apPrevous )
{
	for( ;*apPrevous; apPrevous++)	
		apPrevous[0]=apPath->mChars[255];
	return StringPath_GetFolder_Internal( apPath, (char*)apPrevous+1 );
}


/* ################################################################################ */
