/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CHECKSUM.H"


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dCHECKSUM_FLETCHER_LOOPBIG		360
#define	dCHECKSUM_FLETCHER_LOOPSMALL	21


/* ###################################################################################
#  DATA
################################################################################### */



/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : CheckSum_Fletcher_Init( sCheckSumFletcher * apFletcher )
* ACTION   : CheckSum_Fletcher_Init
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	CheckSum_Fletcher_Init( sCheckSumFletcher * apFletcher )
{
	if( apFletcher )
	{
		apFletcher->mLoopBig = dCHECKSUM_FLETCHER_LOOPBIG;
		apFletcher->mLoopSmall = dCHECKSUM_FLETCHER_LOOPSMALL;
		apFletcher->mSumSmall0 = 0xFF;
		apFletcher->mSumSmall1 = 0xFF;
		apFletcher->mSumBig0 = 0xFFFF;
		apFletcher->mSumBig1 = 0xFFFF;
		apFletcher->mCheckSum = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CheckSum_Fletcher_U8( sCheckSumFletcher * apFletcher, U8 aValue )
* ACTION   : CheckSum_Fletcher_U8
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	CheckSum_Fletcher_U8( sCheckSumFletcher * apFletcher, U8 aValue )
{
	if( apFletcher )
	{
		apFletcher->mSumSmall0 += aValue;
		apFletcher->mSumSmall1 += apFletcher->mSumSmall0;
		apFletcher->mLoopSmall--;
		if( apFletcher->mLoopSmall <= 0 )
		{
			apFletcher->mLoopSmall = dCHECKSUM_FLETCHER_LOOPSMALL;
			apFletcher->mSumSmall0 = (apFletcher->mSumSmall0 & 0xFF) + (apFletcher->mSumSmall0 >> 8);
			apFletcher->mSumSmall1 = (apFletcher->mSumSmall1 & 0xFF) + (apFletcher->mSumSmall1 >> 8);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CheckSum_Fletcher_U16( sCheckSumFletcher * apFletcher, U16 aValue )
* ACTION   : CheckSum_Fletcher_U16
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	CheckSum_Fletcher_U16( sCheckSumFletcher * apFletcher, U16 aValue )
{
	if( apFletcher )
	{
		apFletcher->mSumBig0 += aValue;
		apFletcher->mSumBig1 += apFletcher->mSumSmall0;
		apFletcher->mLoopBig--;
		if( apFletcher->mLoopBig <= 0 )
		{
			apFletcher->mLoopBig = dCHECKSUM_FLETCHER_LOOPSMALL;
			apFletcher->mSumBig0 = (apFletcher->mSumBig0 & 0xFFFFL) + (apFletcher->mSumBig0 >> 16L);
			apFletcher->mSumBig1 = (apFletcher->mSumBig1 & 0xFFFFL) + (apFletcher->mSumBig1 >> 16L);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CheckSum_Fletcher_U32( sCheckSumFletcher * apFletcher, U32 aValue )
* ACTION   : CheckSum_Fletcher_U32
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	CheckSum_Fletcher_U32( sCheckSumFletcher * apFletcher, U32 aValue )
{
	CheckSum_Fletcher_U16( apFletcher, (U16)(aValue & 0xFFFFL) );
	CheckSum_Fletcher_U16( apFletcher, (U16)((aValue>>16L) & 0xFFFFL) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CheckSum_Fletcher_Get( sCheckSumFletcher * apFletcher )
* ACTION   : CheckSum_Fletcher_Get
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U32	CheckSum_Fletcher_Get( sCheckSumFletcher * apFletcher )
{
	U32	lSum;
	U32	lSmall0;
	U32	lSmall1;
	U32	lBig0;
	U32	lBig1;

	lSmall0 = (apFletcher->mSumSmall0 & 0xFF)  + (apFletcher->mSumSmall0 >> 8);
	lSmall1 = (apFletcher->mSumSmall1 & 0xFF)  + (apFletcher->mSumSmall1 >> 8);
	lBig0   = (apFletcher->mSumBig0 & 0xFFFFL) + (apFletcher->mSumBig0 >> 16L);
	lBig1   = (apFletcher->mSumBig1 & 0xFFFFL) + (apFletcher->mSumBig1 >> 16L);

	lSum  = ((lBig1<<16)  | (lBig0));
	lSum += ((lSmall1<<8) | (lSmall0));

	return( lSum );
}


/* ################################################################################ */
