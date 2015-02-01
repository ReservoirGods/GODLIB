#ifndef	INCLUDED_VID_IMG_H
#define	INCLUDED_VID_IMG_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eVIDEO_IMAGE_FMT_A1R5G5B5,
	eVIDEO_IMAGE_FMT_A2B10G10R10,
	eVIDEO_IMAGE_FMT_A4R4G4B4,
	eVIDEO_IMAGE_FMT_A8R8G8B8,
	eVIDEO_IMAGE_FMT_A8R3G3B2,

	eVIDEO_IMAGE_FMT_G16R16,

	eVIDEO_IMAGE_FMT_R3G3B2,
	eVIDEO_IMAGE_FMT_R5G6B5,
	eVIDEO_IMAGE_FMT_R8G8B8,

	eVIDEO_IMAGE_FMT_X1R5G5B5,
	eVIDEO_IMAGE_FMT_X4R4G4B4,
	eVIDEO_IMAGE_FMT_X8R8G8B8,
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Video_Image_Init( void );
void	Video_Image_DeInit( void );

void	Video_Image_ConvPalST( const U16 aFormat );
void	Video_Image_ConvPalFalcon( const U16 aFormat );

void Video_Image_4BP_ToU8( const U8 * apSrc, U8 *  apDst, const U16 aWidth, const U16 aHeight, const U32 aSrcPitch, const U32 aDstPitch, U32 * apPal );
void Video_Image_4BP_ToU16(const U8 * apSrc, U16 * apDst, const U16 aWidth, const U16 aHeight, const U32 aSrcPitch, const U32 aDstPitch, U32 * apPal );
void Video_Image_4BP_ToU24(const U8 * apSrc, U8 *  apDst, const U16 aWidth, const U16 aHeight, const U32 aSrcPitch, const U32 aDstPitch, U32 * apPal );
void Video_Image_4BP_ToU32(const U8 * apSrc, U32 * apDst, const U16 aWidth, const U16 aHeight, const U32 aSrcPitch, const U32 aDstPitch, U32 * apPal );


/* ################################################################################ */

#endif	/* INCLUDED_VID_IMG_H */
