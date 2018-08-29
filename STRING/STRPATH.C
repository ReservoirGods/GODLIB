/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"STRPATH.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>


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



/* ################################################################################ */
