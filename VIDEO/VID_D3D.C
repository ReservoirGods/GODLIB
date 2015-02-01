
/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"VID_D3D.H"
#include	"VIDEO.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/VBL/VBL.H>

#ifdef	dGODLIB_SYSTEM_D3D

/* ###################################################################################
#  DATA
################################################################################### */

LPDIRECT3D9             g_pD3D       = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
HWND					gVideoD3DHwnd;
U32						gVideoD3DPal[ 16 ];
U16						gVideoD3DColourMode = 0;
U16						gVideoD3DShadowPal[ 16 ];
DWORD					gVideoD3DTicks = 0;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void VideoD3D_4BP_ToU8(const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch);
void VideoD3D_4BP_ToU16(const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch);
void VideoD3D_4BP_ToU24(const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch);
void VideoD3D_4BP_ToU32(const U8 * apSrc,U32 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch);

void	VideoD3D_16BPP_Display( const U16 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch );
void	VideoD3D_BuildPalST( const U16 * apPal );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_Init( void )
* ACTION   : VideoD3D_Init
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

S32	VideoD3D_Init( void )
{
    D3DDISPLAYMODE			d3ddm;
    D3DPRESENT_PARAMETERS	d3dpp;

    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Get the current desktop display mode, so we can set up a back
    // buffer of the same format
    if( FAILED( IDirect3D9_GetAdapterDisplayMode( g_pD3D, D3DADAPTER_DEFAULT, &d3ddm ) ) )
        return E_FAIL;

    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed               = TRUE;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat       = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16_LOCKABLE;
	d3dpp.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	gVideoD3DColourMode = (U16)d3ddm.Format;

    if( FAILED( IDirect3D9_CreateDevice( g_pD3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, gVideoD3DHwnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    IDirect3DDevice9_SetRenderState( g_pd3dDevice, D3DRS_CULLMODE, D3DCULL_NONE );
    IDirect3DDevice9_SetRenderState( g_pd3dDevice, D3DRS_LIGHTING, FALSE );
    IDirect3DDevice9_SetRenderState( g_pd3dDevice, D3DRS_ZENABLE, FALSE );

	ShowWindow( gVideoD3DHwnd, SW_SHOWDEFAULT );
	UpdateWindow( gVideoD3DHwnd );

	gVideoD3DTicks = GetTickCount();
	Vbl_SetVideoFunc( VideoD3D_VblFunc );

    return S_OK;	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_DeInit( void )
* ACTION   : VideoD3D_DeInit
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_DeInit( void )
{
    if( g_pd3dDevice != NULL )
	{
		IDirect3DDevice9_Release( g_pd3dDevice );
		g_pd3dDevice = 0;
	}

    if( g_pD3D != NULL )
	{
		IDirect3D9_Release( g_pD3D );
		g_pD3D = 0;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_SetWindowHandle( HWND aHwnd )
* ACTION   : VideoD3D_SetWindowHandle
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_SetWindowHandle( HWND aHwnd )
{	
	gVideoD3DHwnd = aHwnd;	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_ResolutionUpdate( void )
* ACTION   : VideoD3D_ResolutionUpdate
* CREATION : 17.04.2005 PNK
*-----------------------------------------------------------------------------------*/

//#define	dVIDEOD3D_WINDOW_X	1280
#define	dVIDEOD3D_WINDOW_X	320	
#define	dVIDEOD3D_WINDOW_Y	128
#define	dVIDEOD3D_BORDER_WIDTH	10
#define	dVIDEOD3D_BORDER_HEIGHT	40

void	VideoD3D_ResolutionUpdate( void )
{
	sVideoConfig	lConfig;
	
	VideoD3D_DeInit();
	Video_GetConfig( &lConfig );

	MoveWindow( gVideoD3DHwnd, 
		dVIDEOD3D_WINDOW_X,
		dVIDEOD3D_WINDOW_Y,
		lConfig.mWidth + dVIDEOD3D_BORDER_WIDTH,
		lConfig.mHeight + dVIDEOD3D_BORDER_HEIGHT,
		1 );

	VideoD3D_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_DisplayScreen( const U16 * apScreen )
* ACTION   : VideoD3D_DisplayScreen
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_DisplayScreen( const U16 * apScreen )
{
#if	1
	U16					lW,lH;
	IDirect3DSurface9 *	lpSurface;
	D3DLOCKED_RECT		lRect;
	D3DSURFACE_DESC 	lDesc;
	MSG					msg;




	if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}


    // Clear the backbuffer and the zbuffer
    IDirect3DDevice9_Clear( g_pd3dDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	IDirect3DDevice9_GetBackBuffer( g_pd3dDevice, 0, 0, D3DBACKBUFFER_TYPE_MONO, &lpSurface );
	IDirect3DSurface9_GetDesc( lpSurface, &lDesc );
	IDirect3DSurface9_LockRect( lpSurface, &lRect, NULL, D3DLOCK_NOSYSLOCK );

	lW = Video_GetWidth();
	lH = Video_GetHeight();

	if( lDesc.Width < lW )
	{
		lW = (U16)(lDesc.Width);
	}
	if( lDesc.Height < lH )
	{
		lH = (U16)(lDesc.Height);
	}

	switch( Video_GetMode() )
	{
	case	eVIDEO_MODE_4PLANE:
		switch( gVideoD3DColourMode )
		{
		case	D3DFMT_R8G8B8:
				break;
		case	D3DFMT_A8R8G8B8:
		case	D3DFMT_X8R8G8B8:
		case	D3DFMT_A2B10G10R10:
		case	D3DFMT_G16R16:
				VideoD3D_4BP_ToU32( (const U8*)apScreen, (U32*)lRect.pBits, lW, lH, 160, lRect.Pitch );
				break;
		case	D3DFMT_R5G6B5:
		case	D3DFMT_X1R5G5B5:
		case	D3DFMT_A1R5G5B5:
		case	D3DFMT_A4R4G4B4:
		case	D3DFMT_X4R4G4B4:
				VideoD3D_4BP_ToU16( (const U8*)apScreen, (U16*)lRect.pBits, lW, lH, 160, lRect.Pitch );
				break;
		case	D3DFMT_R3G3B2:
		case	D3DFMT_A8R3G3B2:
				VideoD3D_4BP_ToU8( (const U8*)apScreen, (U8*)lRect.pBits, lW, lH, 160, lRect.Pitch );
				break;
		}
		break;
	case	eVIDEO_MODE_RGB565:
		VideoD3D_16BPP_Display( apScreen, (U8*)lRect.pBits, lW, lH, Video_GetWidth() << 1, lRect.Pitch );
		break;
	}


    // Present the backbuffer contents to the display
	IDirect3DSurface9_UnlockRect( lpSurface );
    IDirect3DDevice9_Present( g_pd3dDevice, NULL, NULL, NULL, NULL );

	if( lpSurface )
	{
		IDirect3D9_Release( lpSurface );
	}
	
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_BuildPalST( const U16 * apPal )
* ACTION   : VideoD3D_BuildPalST
* CREATION : 05.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_BuildPalST( const U16 * apPal )
{
	U16	i;
	U32	lCol;
	U32	lR,lG,lB;

	if( apPal )
	{
		for( i=0; i<16; i++ )
		{
			lCol = apPal[ i ];
			gVideoD3DShadowPal[ i ] = (U16)(lCol);
			lR   = (lCol>>0) & 7;
			lG   = (lCol>>12) & 7;
			lB   = (lCol>>8) & 7;
			lR <<= 1;
			lG <<= 1;
			lB <<= 1;
			lR  |= (lCol>>3)&1;
			lG  |= (lCol>>15)&1;
			lB  |= (lCol>>11)&1;
			switch( gVideoD3DColourMode )
			{
			case	D3DFMT_R8G8B8:
			case	D3DFMT_A8R8G8B8:
			case	D3DFMT_X8R8G8B8:
					gVideoD3DPal[ i ] = (lR<<20) | (lG<<12) | (lB<<4);
					break;
			case	D3DFMT_R5G6B5:
					gVideoD3DPal[ i ] = (lR<<12) | (lG<<7) | (lB<<1);
					break;
			case	D3DFMT_X1R5G5B5:
			case	D3DFMT_A1R5G5B5:
					gVideoD3DPal[ i ] = (lR<<12) | (lG<<6) | (lB<<1);
					break;
			case	D3DFMT_A4R4G4B4:
			case	D3DFMT_X4R4G4B4:
					gVideoD3DPal[ i ] = (lR<<8) | (lG<<4) | (lB);
					break;
			case	D3DFMT_R3G3B2:
			case	D3DFMT_A8R3G3B2:
					gVideoD3DPal[ i ] = ((lR<<1)<<5) | ((lG>>1)<<2) | (lB>>2);
					break;
			case	D3DFMT_A2B10G10R10:
					gVideoD3DPal[ i ] = (lR<<26) | (lG<<16) | (lB<<6);
			case	D3DFMT_G16R16:
					gVideoD3DPal[ i ] = (lR<<12) | (lG<<28);
					break;
			}
		}
	}
}

#if	0
/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetPal( U16 * apPal )
* ACTION   : Video_GetPal
* CREATION : 07.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Video_GetPalST( U16 * apPal )
{
	U16	i;
	
	if( apPal )
	{
		for( i=0; i<16; i++ )
		{
			apPal[ i ] = gVideoD3DShadowPal[ i ];
		}
	}
}
#endif

/*-----------------------------------------------------------------------------------*
* FUNCTION : void VideoD3D_4BP_ToU8(const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
* ACTION   : VideoD3D_4BP_ToU8
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void VideoD3D_4BP_ToU8(const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		VideoD3D_BuildPalST( Video_GetpPalSplitST( (aHeight-lH)-1 ) );
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U8)( gVideoD3DPal[ lColour ] & 0xFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += aDstPitch;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void VideoD3D_4BP_ToU16(const U8 * apSrc,U32 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
* ACTION   : VideoD3D_4BP_ToU16
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void VideoD3D_4BP_ToU16(const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
{
	U16 *		lpDst0;
	U16 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		VideoD3D_BuildPalST( Video_GetpPalSplitST( (aHeight-lH)-1 ) );
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U16)( gVideoD3DPal[ lColour ] & 0xFFFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += (aDstPitch>>1);
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void VideoD3D_4BP_ToU24(const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
* ACTION   : VideoD3D_4BP_ToU8
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void VideoD3D_4BP_ToU24(const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		VideoD3D_BuildPalST( Video_GetpPalSplitST( (aHeight-lH)-1 ) );
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U8)( (gVideoD3DPal[ lColour ]>>16) & 0xFF );
			*lpDst1++ = (U8)( (gVideoD3DPal[ lColour ]>>8) & 0xFF );
			*lpDst1++ = (U8)( (gVideoD3DPal[ lColour ]>>0) & 0xFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += aDstPitch;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void VideoD3D_4BP_ToU32(const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
* ACTION   : VideoD3D_4BP_ToU32
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void VideoD3D_4BP_ToU32(const U8 * apSrc,U32 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch)
{
	U32 *		lpDst0;
	U32 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		VideoD3D_BuildPalST( Video_GetpPalSplitST( (aHeight-lH)-1 ) );
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = gVideoD3DPal[ lColour ];
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += (aDstPitch>>2);
	}			
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_16BPP_ToU8( const U8 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch )
* ACTION   : VideoD3D_16BPP_ToU8
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	VideoD3D_16BPP_Display( const U16 * apSrc,U8 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch )
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U16 *	lpSrc0;
	const U16 *	lpSrc1;
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
			
			switch( gVideoD3DColourMode )
			{
			case	D3DFMT_R8G8B8:
					lpDst1[ 0 ] = (U8)(lR);
					lpDst1[ 1 ] = (U8)(lG);
					lpDst1[ 2 ] = (U8)(lB);
					lpDst1 += 3;
					break;
			case	D3DFMT_A8R8G8B8:
			case	D3DFMT_X8R8G8B8:
					lpDst1[ 0 ] = (U8)(lB<<3);
					lpDst1[ 1 ] = (U8)(lG<<2);
					lpDst1[ 2 ] = (U8)(lR<<3);
/*					gVideoD3DPal[ i ] = (lR<<20) | (lG<<12) | (lB<<4);*/
					lpDst1 += 4;
					break;
			case	D3DFMT_R5G6B5:
					*(U16*)lpDst1 = (U16)((lR<<11) | (lG<<5) | (lB));
					lpDst1 += 2;
					break;
			case	D3DFMT_X1R5G5B5:
			case	D3DFMT_A1R5G5B5:
					*(U16*)lpDst1 = (U16)((lR<<10) | ((lG>>1)<<5) | (lB));
					lpDst1 += 2;
					break;
			case	D3DFMT_A4R4G4B4:
			case	D3DFMT_X4R4G4B4:
					lpDst1[ 0 ] = (U8)(lR >> 1);
					lpDst1[ 1 ] = (U8)(((lG>>2)<<4) | (lB>>1));
					lpDst1 += 2;
					break;
			case	D3DFMT_R3G3B2:
					lpDst1[ 0 ] = (U8)((lR>>2) | (lG>>3) | (lB>>3));
					lpDst1++;
					break;
			case	D3DFMT_A8R3G3B2:
					lpDst1[ 0 ] = (U8)((lR>>2) | (lG>>3) | (lB>>3));
					lpDst1++;
					break;
			case	D3DFMT_A2B10G10R10:
					*(U32*)lpDst1 = (lB<<5) | (lG<<4) | (lR<<5);
					lpDst1 += 4;
					break;
			case	D3DFMT_G16R16:
					*(U32*)lpDst1 = (lG<<10) | (lR<<11);
					lpDst1 += 4;
					break;
			}

		}
		lpSrc0 += aSrcPitch >> 1;
		lpDst0 += aDstPitch;
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : VideoD3D_VblFunc( void )
* ACTION   : VideoD3D_VblFunc
* CREATION : 20.12.2008 PNK
*-----------------------------------------------------------------------------------*/

DWORD	gVideoD3DVBLTicks = 20;

void	VideoD3D_VblFunc( void )
{
	DWORD				lTicks;

	do
	{
		lTicks = (gVideoD3DTicks+gVideoD3DVBLTicks) - GetTickCount();
		if( (lTicks > 0) && (lTicks <gVideoD3DVBLTicks) )
		{
			Sleep( lTicks );
		}
		lTicks = (gVideoD3DTicks+gVideoD3DVBLTicks) - GetTickCount();
	} while( (lTicks > 0) && (lTicks <gVideoD3DVBLTicks) );

	gVideoD3DTicks = GetTickCount();
	Memory_Validate();
}	


/* ################################################################################ */

#endif

