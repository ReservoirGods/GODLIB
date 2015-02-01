
/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"VID_SDL.H"

#ifdef	dGODLIB_SYSTEM_SDL

#include	"VIDEO.H"
#include	"VID_IMG.H"
#pragma pack(push,4)
#include	"SDL.h"
#pragma pack(pop)

#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/VBL/VBL.H>


#ifdef	dGODLIB_PLATFORM_WIN
/*#include	<d3dx8.h>
#include	<mmsystem.h>*/
#include	<windows.h>
#endif


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sVideoSdlClass
{
	SDL_Surface *	mpScreen;
	U16				mWidth;
	U16				mHeight;
	U16				mPalOriginalST[ 16 ];
	U32				mPalOriginalFalcon[ 256 ];
	U32				mPalConvST[ 16 ];
	U32				mPalConvFalcon[ 256 ];
} sVideoSdlClass;


/* ###################################################################################
#  DATA
################################################################################### */

sVideoSdlClass	gVideoSdlClass;
DWORD			gVideoSDLTicks = 0;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		Video_SDL_GetFormat( SDL_Surface * apScreen );
void	VideoD3D_16BPP_Display( const U16 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_Init( void )
* ACTION   : Video_SDL_Init
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SDL_Init( void )
{
	Memory_Clear( sizeof(sVideoSdlClass), &gVideoSdlClass );
	gVideoSdlClass.mpScreen = SDL_SetVideoMode( 320, 200, 0, SDL_SWSURFACE );
	gVideoSdlClass.mWidth = 320;
	gVideoSdlClass.mHeight = 200;
	Vbl_SetVideoFunc( Video_SDL_VblFunc );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_DeInit( void )
* ACTION   : Video_SDL_DeInit
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SDL_DeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_DisplayScreen( const U16 * apScreen )
* ACTION   : Video_SDL_DisplayScreen
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SDL_DisplayScreen( const U16 * apScreen )
{
	sVideoConfig	lConfig;
#ifdef	dGODLIB_PLATFORM_WIN
	MSG					msg;
	if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#endif

/*	Vbl_CallsProcess();*/

	if ( SDL_MUSTLOCK(gVideoSdlClass.mpScreen) )
	{
		if ( SDL_LockSurface(gVideoSdlClass.mpScreen) < 0 )
		{
			return;
		}
	}

	Video_GetConfig( &lConfig );

	switch( Video_GetMode() )
	{
	case	eVIDEO_MODE_4PLANE:
		switch (gVideoSdlClass.mpScreen->format->BytesPerPixel)
		{
		case 1:
			Video_Image_4BP_ToU8(
				(U8*)apScreen,
				(U8*)gVideoSdlClass.mpScreen->pixels,
				gVideoSdlClass.mWidth,
				gVideoSdlClass.mHeight,
				lConfig.mCanvasWidth >> 1,
				gVideoSdlClass.mpScreen->pitch,
				gVideoSdlClass.mPalConvST );
			break;

		case 2:
			Video_Image_4BP_ToU16(
				(U8*)apScreen,
				(U16*)gVideoSdlClass.mpScreen->pixels,
				gVideoSdlClass.mWidth,
				gVideoSdlClass.mHeight,
				lConfig.mCanvasWidth >> 1,
				gVideoSdlClass.mpScreen->pitch,
				gVideoSdlClass.mPalConvST );
			break;

		case 3:
			Video_Image_4BP_ToU24(
				(U8*)apScreen,
				(U8*)gVideoSdlClass.mpScreen->pixels,
				gVideoSdlClass.mWidth,
				gVideoSdlClass.mHeight,
				lConfig.mCanvasWidth >> 1,
				gVideoSdlClass.mpScreen->pitch,
				gVideoSdlClass.mPalConvST );
			break;

		case 4:
			Video_Image_4BP_ToU32(
				(U8*)apScreen,
				(U32*)gVideoSdlClass.mpScreen->pixels,
				gVideoSdlClass.mWidth,
				gVideoSdlClass.mHeight,
				lConfig.mCanvasWidth >> 1,
				gVideoSdlClass.mpScreen->pitch,
				gVideoSdlClass.mPalConvST );
			break;
		}
		break;
	case	eVIDEO_MODE_RGB565:
		VideoD3D_16BPP_Display(
			apScreen,
			(U8*)gVideoSdlClass.mpScreen->pixels,
			gVideoSdlClass.mWidth,
			gVideoSdlClass.mHeight,
			lConfig.mCanvasWidth << 1,
			gVideoSdlClass.mpScreen->pitch );
		break;
	}

	if ( SDL_MUSTLOCK(gVideoSdlClass.mpScreen) )
	{
		SDL_UnlockSurface(gVideoSdlClass.mpScreen);
	}

	SDL_UpdateRect( gVideoSdlClass.mpScreen, 0, 0, 0, 0);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_SetPalST( const U16 * apPal )
* ACTION   : Video_SDL_SetPalST
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPalST( const U16 * apPal )
{
	U16	i;
	U32	lCol;
	U32	lR,lG,lB;

	for( i=0; i<16; i++ )
	{
		lCol = apPal[ i ];
		gVideoSdlClass.mPalOriginalST[ i ] = (U16)(lCol);
		lR   = (lCol>>0) & 7;
		lG   = (lCol>>12) & 7;
		lB   = (lCol>>8) & 7;
		lR <<= 1;
		lG <<= 1;
		lB <<= 1;
		lR  |= (lCol>>3)&1;
		lG  |= (lCol>>15)&1;
		lB  |= (lCol>>11)&1;

		lR <<= 4;
		lG <<= 4;
		lB <<= 4;

		gVideoSdlClass.mPalConvST[ i ] = SDL_MapRGB( gVideoSdlClass.mpScreen->format, (U8)lR, (U8)lG, (U8)lB );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetPalST( U16 * apPal )
* ACTION   : Video_GetPalST
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_GetPalST( U16 * apPal )
{
	U16	i;

	if( apPal )
	{
		for( i=0; i<16; i++ )
		{
			apPal[ i ] = gVideoSdlClass.mPalOriginalST[ i ];
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_SetPalFalcon( const U16 * apPal )
* ACTION   : Video_SDL_SetPalFalcon
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SDL_SetPalFalcon( const U16 * apPal )
{
	(void)apPal;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_16BPP_Display( const U16 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch )
* ACTION   : VideoD3D_16BPP_Display
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_16BPP_Display( const U16 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch )
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U16 *	lpSrc0;
	const U16 *	lpSrc1;
	U32			lPixel;
	U16			lColour;
	U16			lH,lW;
	U16			lR,lG,lB;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{

			Endian_ReadBigU16( lpSrc1, lColour );
			lpSrc1++;

			if( lColour )
			{
				lR = 0;
			}

			lR = (U16)(lColour >> 11);
			lG = (U16)(lColour >> 5);
			lB = (U16)(lColour & 0x1F);
			lR &= 0x1F;
			lG &= 0x3F;


			lR <<= 3;
			lG <<= 2;
			lB <<= 3;
			lPixel = SDL_MapRGB( gVideoSdlClass.mpScreen->format, (U8)lR, (U8)lG, (U8)lB );

			switch (gVideoSdlClass.mpScreen->format->BytesPerPixel)
			{
			case 1:
				*lpDst1++ = (U8)lPixel;
				break;
			case 2:
				*(U16*)lpDst1 = (U16)lPixel;
				lpDst1       += 2;
				break;

			case 3:
				lpDst1[ 0 ] = (U8)((lPixel>>16) & 0xFF);
				lpDst1[ 1 ] = (U8)((lPixel>>8) & 0xFF);
				lpDst1[ 2 ] = (U8)((lPixel>>0) & 0xFF);
				lpDst1 += 3;
				break;
			case 4:
				*(U32*)lpDst1 = lPixel;
				lpDst1       += 4;
				break;
			}
		}
		lpSrc0 += aSrcPitch >> 1;
		lpDst0 += aDstPitch;
	}

}

U32	gVideoSDLVBLTicks = 20;

/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SDL_VblFunc( void )
* ACTION   : Video_SDL_VblFunc
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SDL_VblFunc( void )
{
	U32	lTicks;

	lTicks = (gVideoSDLTicks+gVideoSDLVBLTicks) - SDL_GetTicks();
	if( (lTicks > 0) && (lTicks <gVideoSDLVBLTicks) )
	{
		SDL_Delay( lTicks );
	}
	gVideoSDLTicks = SDL_GetTicks();

	Vbl_CallsProcess();
}


/* ################################################################################ */

#endif
