#ifndef	INCLUDED_FILE_PATTERN_H
#define	INCLUDED_FILE_PATTERN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/STRING/STRPATH.H>


/* ###################################################################################
#  DEFINES
################################################################################### */


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef struct sFilePattern
{
	const char*	mpPattern;
	sStringPath	mPath;
	sStringPath	mBaseDir;
	U16			mStartedFlag;
	U16			mResult;
} sFilePattern;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

/* returns 0 if non-expandable, 1 if expandable*/
U16		FilePattern_Init( sFilePattern * apPattern, const char * apString );
U16		FilePattern_Next( sFilePattern * apPattern );


/* ################################################################################ */

#endif	/*	INCLUDED_FILE_PATTERN_H */
