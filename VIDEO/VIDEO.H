#ifndef	INCLUDED_VIDEO_H
#define	INCLUDED_VIDEO_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eVIDEO_MODE_1PLANE,
	eVIDEO_MODE_2PLANE,
	eVIDEO_MODE_4PLANE,
	eVIDEO_MODE_8PLANE,
	eVIDOE_MODE_I8,
	eVIDEO_MODE_RGB565
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sVideoConfig
{
	U16	mWidth;
	U16	mHeight;
	U16	mMode;
	U16	mCanvasWidth;
	U8	mNTSCFlag;
	U8	mWideScreenFlag;
} sVideoConfig;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Video_Init( void );
void	Video_DeInit( void );

U8		Video_SetResolution( const U16 aWidth, const U16 aHeight, const U16 aMode, const U16 aCanvasWidth );
U8		Video_SetMode( const U16 aWidth, const U16 aHeight, const U16 aMode, const U16 aCanvasWidth, const U8 aNTSCflag, const U8 aWideScreenFlag );
U16		Video_GetMode( void );
U16		Video_GetWidth( void );
U16		Video_GetHeight( void );

void	Video_GetConfig( sVideoConfig * apConfig );
void	Video_SetConfig( sVideoConfig * apConfig );

void	Video_SetPhysic( const void * apPhysic );
void *	Video_GetpPhysic( void );

void	Video_SetPalST( const U16 * apPal );
void	Video_GetPalST( U16 * apPal );
void	Video_SetPalFalcon( const U32 * apPal );
void	Video_GetPalFalcon( U32 * apPal );

U32		Video_GetScreenSize( void );

void	Video_SetNextPalST( const U16 * apPal );
void	Video_SetNextPalTT( const U16 * apPal );
void	Video_SetNextPalFalcon( const U32 * apPal );

void	Video_SetHorizontalOffset( const S16 aOff );
S16		Video_GetHorizontalOffset( void );
void	Video_SetVerticalOffset( const S16 aOff );
S16		Video_GetVerticalOffset( void );

void	Video_PalSplitResetST( void );
void	Video_SetPalSplitColourST( const U16 aLineY, const U16 aIndex, const U16 aColour );
void	Video_SetPalSplitST( const U16 aLineY, const U16 * apPal );
U16 *	Video_GetpPalSplitST( const U16 aLineY );
void	Video_PalSplitAlloc( const U16 aLineY );


/* ################################################################################ */

#endif	/* INCLUDED_VIDEO_H */
