#ifndef	INCLUDED_CHECKSUM_H
#define	INCLUDED_CHECKSUM_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sCheckSumFletcher
{
	S16	mLoopBig;
	S16	mLoopSmall;
	U16	mSumSmall0;
	U16	mSumSmall1;
	U32	mSumBig0;
	U32	mSumBig1;
	U32	mCheckSum;
} sCheckSumFletcher;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	CheckSum_Fletcher_Init( sCheckSumFletcher * apFletcher );
void	CheckSum_Fletcher_U8( sCheckSumFletcher * apFletcher, U8 aValue );
void	CheckSum_Fletcher_U16( sCheckSumFletcher * apFletcher, U16 aValue );
void	CheckSum_Fletcher_U32( sCheckSumFletcher * apFletcher, U32 aValue );
U32		CheckSum_Fletcher_Get( sCheckSumFletcher * apFletcher );


/* ################################################################################ */

#endif	/* INCLUDED_CHECKSUM_H */
