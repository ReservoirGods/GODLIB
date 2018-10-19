/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: FILE_PNT.C
::
:: File Pattern Expander
::
:: [c] 2018 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FILE_PTN.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DRIVE/DRIVE.H>
#include	<GODLIB/FILE/FILE.H>


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : FilePattern_Init( sFilePattern * apPattern )
* ACTION   : creates a file pattern
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U16		FilePattern_Init( sFilePattern * apPattern, const char * apString )
{
	StringPath_GetDirectory( &apPattern->mBaseDir, apString );

	apPattern->mpPattern = apString;
	apPattern->mStartedFlag = 0;

	/* deterimine if path is actually a directory */
	apPattern->mResult = (U16)(0 == File_ReadFirst( apPattern->mpPattern, dGEMDOS_FA_READONLY | dGEMDOS_FA_ARCHIVE | dGEMDOS_FA_DIR ));
	if( apPattern->mResult )
	{
		if( Drive_DirectoryExists( apPattern->mpPattern ) )
		{
			sStringPath lPath;
			StringPath_SetNT( &apPattern->mBaseDir, apString );
			StringPath_Combine( &lPath, apPattern->mpPattern, "*.*" );
			apPattern->mResult = (U16)( 0 == File_ReadFirst( lPath.mChars, dGEMDOS_FA_READONLY | dGEMDOS_FA_ARCHIVE | dGEMDOS_FA_DIR ) );
			return 1;
		}
	}

	/* determine if there are any wildcards in filepath */
	{
		const char * lpS = apString;
		for( ;*lpS; lpS++ )
			if( '*' == *lpS || '?' == *lpS )
				return 1;
	}

	return 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FilePattern_Init( sFilePattern * apPattern )
* ACTION   : builds next string from file pattern
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U16			FilePattern_Next( sFilePattern * apPattern )
{
	const char * lpFileName = File_GetDTA()->mFileName;

	if( apPattern->mStartedFlag )
		apPattern->mResult = ( 0 == File_ReadNext() );

	/* skip special directories */
	while( ( apPattern->mResult )
		&& ( lpFileName[ 0 ] == '.' )
		&& ( ( 0 == lpFileName[ 1 ] ) || ( ( '.' == lpFileName[ 1 ] ) && ( 0 == lpFileName[ 2 ] ) ) ) )
		apPattern->mResult = ( 0 == File_ReadNext() );

	apPattern->mStartedFlag = 1;

	StringPath_Combine( &apPattern->mPath, apPattern->mBaseDir.mChars, File_GetDTA()->mFileName );

	return( apPattern->mResult );
}


/* ################################################################################ */
