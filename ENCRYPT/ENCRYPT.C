/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ENCRYPT.H"


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dENCRYPT_DATA_LIMIT	64


/* ###################################################################################
#  DATA
################################################################################### */

U8	gEncryptData[ dENCRYPT_DATA_LIMIT ] =
{
	0x73,0x21,0xe9,0x52,0x8e,0x3c,0xf8,0xdf,0x40,0xca,0x64,0x06,0xad,0x9b,0xb7,0x15,
	0x21,0x9a,0x48,0xf3,0x67,0xab,0x1f,0x80,0xb9,0xdd,0x02,0x35,0xec,0xce,0x56,0x74,
	0x42,0x16,0x81,0xbe,0x9b,0x59,0x07,0x24,0xec,0x60,0xcd,0xaf,0xf8,0x7a,0xd5,0x33,
	0x74,0xa8,0x1d,0x01,0x25,0xba,0x40,0xcb,0x8f,0xe3,0x56,0xdc,0x9e,0xf7,0x39,0x62,
};


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Encrypt_Scramble( void * apData,const U32 aSize,const U32 aKey )
* ACTION   : Encrypt_Scramble
* CREATION : 20.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Encrypt_Scramble( void * apData,const U32 aSize,const U32 aKey )
{
	U32		i;
	U32		lIndex;
	U8 *	lpDst;
	
	lpDst = (U8*)apData;

	for( i=0; i<aSize; i++ )
	{
		lIndex = (i+aKey) & (dENCRYPT_DATA_LIMIT-1);
		lpDst[ i ] ^= (gEncryptData[ lIndex ] ^ aKey);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Encrypt_DeScramble( void * apData,const U32 aSize,const U32 aKey )
* ACTION   : Encrypt_DeScramble
* CREATION : 20.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Encrypt_DeScramble( void * apData,const U32 aSize,const U32 aKey )
{
	U32		i;
	U32		lIndex;
	U8 *	lpDst;
	
	lpDst = (U8*)apData;

	for( i=0; i<aSize; i++ )
	{
		lIndex = (i+aKey) & (dENCRYPT_DATA_LIMIT-1);
		lpDst[ i ] ^= (gEncryptData[ lIndex ] ^ aKey);
	}	
}


/* ################################################################################ */
