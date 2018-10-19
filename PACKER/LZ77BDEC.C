/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GODPACK.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GodPack_Lz77b_Decode( const void * apSrc,void * apDst,const U32 aSize )
* ACTION   : GodPack_Lz77b_Decode
* CREATION : 21.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	GodPack_Lz77b_Decode( const void * apSrc,void * apDst,const U32 aSize )
{
	U8 *	lpSrc;
	U8 *	lpDst;
	U8 *	lpString;
	U8		lByte;
	S32		lBackOff;
	U16		lCount;
	U32		i;

/*	DebugLog_Init( eDebugLog_File, "LZB_DEC.LOG" );*/

	lpSrc = (U8*)apSrc;
	lpDst = (U8*)apDst;

	i = 0;

	while( i < aSize )
	{
		lByte = (U8)((*lpSrc & dGODPACK_LZ77B_FLAG_MASK));

		if( lByte == dGODPACK_LZ77B_FLAG_LITERAL )
		{
			lCount = (U16)((*lpSrc++) & 31);
			lCount = (U16)(((lCount+1)<<2));
			if( (i+lCount) > aSize )
			{
				lCount = (U16)(aSize-i);
			}
			i += lCount;
			while( lCount )
			{
				*lpDst++ = *lpSrc++;
				lCount--;
			}
		}
		else
		{
			if( lByte == dGODPACK_LZ77B_FLAG_SYMBOL2 )
			{
				lBackOff   = (((U32)*lpSrc++)<< dGODPACK_LZ77B_OFF2_RSHIFT0);
				lBackOff  |= (((U32)*lpSrc++)<< dGODPACK_LZ77B_OFF2_RSHIFT1);
				lBackOff  |= (*lpSrc);
				lBackOff >>= dGODPACK_LZ77B_OFF2_LSHIFT;
				lBackOff  &= dGODPACK_LZ77B_OFF2_LIMIT;

				lCount   = (U16)(((U32)*lpSrc++) << dGODPACK_LZ77B_COUNT2_RSHIFT);
				lCount  |= *lpSrc;
				lCount >>= dGODPACK_LZ77B_LRUN_BITS;
				lCount &=((1<<dGODPACK_LZ77B_COUNT2_BITS)-1);
				lCount  += (U16)(dGODPACK_LZ77B_SYMBOL2_COST+1);
/*				DebugLog_Printf3( "%ld:\t%ld\t%ld\tS2\n", (U32)lpDst-(U32)apDst, lBackOff, lCount );*/
			}
			else if( !(lByte & 0x80) )
			{
				lBackOff   = (((U32)*lpSrc++) << dGODPACK_LZ77B_OFF0_RSHIFT);
				lBackOff  |= *lpSrc;
				lBackOff >>= dGODPACK_LZ77B_OFF0_LSHIFT;
				lBackOff  &= dGODPACK_LZ77B_OFF0_LIMIT;

				lCount  = (U16)((*lpSrc >> dGODPACK_LZ77B_LRUN_BITS));
				lCount &=((1<<dGODPACK_LZ77B_COUNT0_BITS)-1);
				lCount += (U16)(dGODPACK_LZ77B_SYMBOL0_COST+1);
/*				DebugLog_Printf3( "%ld:\t%ld\t%ld\tS0\n", (U32)lpDst-(U32)apDst, lBackOff, lCount );*/
			}
			else
			{
				lBackOff   = (((U32)*lpSrc++)<< dGODPACK_LZ77B_OFF1_RSHIFT);
				lBackOff  |= (*lpSrc);
				lBackOff >>= dGODPACK_LZ77B_OFF1_LSHIFT;
				lBackOff  &= dGODPACK_LZ77B_OFF1_LIMIT;

				lCount   = (U16)(((U32)*lpSrc++) << dGODPACK_LZ77B_COUNT1_RSHIFT);
				lCount  |= (*lpSrc);
				lCount >>= dGODPACK_LZ77B_LRUN_BITS;
				lCount  &=((1<<dGODPACK_LZ77B_COUNT1_BITS)-1);
				lCount  += (U16)(dGODPACK_LZ77B_SYMBOL1_COST+1);
/*				DebugLog_Printf3( "%ld:\t%ld\t%ld\tS1\n", (U32)lpDst-(U32)apDst, lBackOff, lCount );*/
			}
			lpString = lpDst - lBackOff;

			lBackOff = ((*lpSrc++)&3);
			i += lBackOff;
			lpString += lBackOff;
			while( lBackOff )
			{
				*lpDst++ = *lpSrc++;
				lBackOff--;
			}

			i += lCount;
			while( lCount )
			{
				*lpDst++ = *lpString++;
				lCount--;
			}

		}
	}
/*	DebugLog_DeInit();*/
	return( i );
}


/* ################################################################################ */
