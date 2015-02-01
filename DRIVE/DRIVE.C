/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: DRIVE.C
::
:: Low level drive access routines
::
:: This file contains all the platform specific drive manipulation routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"DRIVE.H"

#include	<GODLIB/GEMDOS/GEMDOS.H>

#ifdef	dGODLIB_PLATFORM_WIN
#include	"DIRECT.H"
#endif

/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_CreateDirectory( char * apDirName )
* ACTION   : creates a directory with the specified name
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		Drive_CreateDirectory( char * apDirName )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Dcreate( apDirName ) );
#else
	return( GemDos_Dcreate( apDirName ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_DeleteDirectory( char * apDirName )
* ACTION   : deletes a directory with the specified name
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		Drive_DeleteDirectory( char * apDirName )
{
	return( GemDos_Ddelete( apDirName ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_GetFree( U16 aDrive )
* ACTION   : returns the number of free bytes left on drive aDrive
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32		Drive_GetFree( U16 aDrive )
{
	sGemDosDiskInfo	lDiskInfo;
	S32				lFree;

	lFree = GemDos_Dfree( &lDiskInfo, aDrive );

	if( lFree >= 0 )
	{
		lFree = lDiskInfo.mFreeClusterCount * lDiskInfo.mSectorsPerCluster * lDiskInfo.mSectorSize;
	}

	return( lFree );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_GetDrive()
* ACTION   : gets the number of the currently active drive
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

U16	Drive_GetDrive()
{
#ifdef	dGODLIB_COMPILER_GCC
	return( 2 );
#elif defined(dGODLIB_PLATFORM_WIN)
	return( (U16)(_getdrive() - 1) );
#else
	return( GemDos_Dgetdrv() );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_SetDrive( U16 aDrive )
* ACTION   : sets the number of the currently active drive
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32	Drive_SetDrive( U16 aDrive )
{
#ifdef	dGODLIB_COMPILER_GCC
	return( 0 );
#elif defined(dGODLIB_PLATFORM_WIN)
	return( _chdrive( aDrive ) );
#else
	return( GemDos_Dsetdrv( aDrive ) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_GetPath( U16 aDrive, char * apPath )
* ACTION   : reads current path of aDrive into character buffr apPath
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32	Drive_GetPath( U16 aDrive, char * apPath )
{
#ifdef	dGODLIB_COMPILER_GCC
	S32	lRes;
	U16	i;
	lRes = (S32)getcwd( apPath, 256 );
	if( apPath[ 1 ] == ':' )
	{
		i = 2;
		while( (i<256) && (apPath[i]) )
		{
			apPath[ i-2 ] = apPath[ i ];
			i++;
		}
		apPath[ i-2] =0;
	}
	return( lRes );

#elif defined(dGODLIB_PLATFORM_WIN)
	S32	lRes;
	U16	i;
	int	lDrive;

	if( aDrive )
	{
		lDrive = aDrive + 1;
	}
	else
	{
		lDrive = _getdrive();
	}

	lRes = (S32)_getdcwd( lDrive, apPath, 256 );
	if( apPath[ 1 ] == ':' )
	{
		i = 2;
		while( (i<256) && (apPath[i]) )
		{
			apPath[ i-2 ] = apPath[ i ];
			i++;
		}
		apPath[ i-2] =0;
	}

	return( lRes );
#else
	return( GemDos_Dgetpath( apPath, aDrive) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Drive_SetPath( char * apPath )
* ACTION   : sets current path
* CREATION : 11.09.99 PNK
*-----------------------------------------------------------------------------------*/

S32	Drive_SetPath( char * apPath )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	return( GemDos_Dsetpath( apPath ) );
#elif defined(dGODLIB_COMPILER_GCC)
	S32		lRes;

	lRes = chdir( apPath );

	return( lRes );

#elif defined(dGODLIB_PLATFORM_WIN)
	S32		lRes;

	lRes = _chdir( apPath );

	return( lRes );
#endif
}


/* ################################################################################ */
