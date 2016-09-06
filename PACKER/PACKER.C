/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: PACKER.C
::
:: Depacking routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PACKER.H"
#include	"GODPACK.H"


/* ###################################################################################
#  FUNCTIONS
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Packer_IsPacked( sPackerHeader * apHeader )
* ACTION   : checks to see if data pointed to by apHeader is packed
*            0 if data is not packed
*            1 if data is packed
* CREATION : 09.01.99 PNK
*-----------------------------------------------------------------------------------*/

U8	Packer_IsPacked( sPackerHeader * apHeader )
{
	if( Packer_GetType(apHeader) == ePACKER_NONE )
	{
		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ePacker Packer_GetType( sPackerHeader * apHeader )
* ACTION   : returns type of packer used to pack data pointed to by apHeader
* CREATION : 09.01.99 PNK
*-----------------------------------------------------------------------------------*/

ePacker	Packer_GetType( sPackerHeader * apHeader )
{
	U32	lName;

	Endian_ReadBigU32_Unaligned( &apHeader->m0, lName );

	if( !apHeader )
	{
		return( ePACKER_NONE );
	}
	if( lName == mSTRING_TO_U32( 'I', 'C', 'E', '!' ) )
	{
		return( ePACKER_ICE );
	}
	if( lName == mSTRING_TO_U32( 'A', 'T', 'M', '5' ) )
	{
		return( ePACKER_ATOMIC );
	}
	if( lName == mSTRING_TO_U32( 'A', 'U', '5', '!' ) )
	{
		return( ePACKER_AUTO5 );
	}
	if( lName == mSTRING_TO_U32( 'S', 'P', 'v', '3' ) )
	{
		return( ePACKER_SPEED3 );
	}
	if( lName == mSTRING_TO_U32( 'G', 'D' , 'P', 'K' ) )
	{
		return( ePACKER_GODPACK );
	}
	return( ePACKER_NONE );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Packer_GetDepackSize( sPackerHeader * apHeader )
* ACTION   : returns unpacked size of data pointed to by apHeader
* CREATION : 09.01.99 PNK
*-----------------------------------------------------------------------------------*/

U32		Packer_GetDepackSize( sPackerHeader * apHeader )
{
	U32	lSize;

	lSize = 0;

	switch( Packer_GetType(apHeader) )
	{
	case ePACKER_ICE:
		Endian_ReadBigU32_Unaligned( &apHeader->m2, lSize );
		break;

	case ePACKER_ATOMIC:
		Endian_ReadBigU32_Unaligned( &apHeader->m1, lSize );
		break;

	case	ePACKER_AUTO5:
		Endian_ReadBigU32_Unaligned( &apHeader->m2, lSize );
		break;

	case	ePACKER_SPEED3:
		Endian_ReadBigU32_Unaligned( &apHeader->m3, lSize );
		break;

	case	ePACKER_GODPACK:
		Endian_ReadBigU32_Unaligned( &apHeader->m3, lSize );
		break;

	default:
		break;
	}
	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ePACKER Packer_Depack( void * apData )
* ACTION   : depacks data pointed to by apData
* CREATION : 09.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	Packer_Depack( void * apSrc, void * apDst )
{
	switch( Packer_GetType((sPackerHeader*)apSrc) )
	{
	case ePACKER_ICE:
		Packer_DepackIce( apSrc );
		break;

	case ePACKER_ATOMIC:
		Packer_DepackAtomic( apSrc );
		break;

	case ePACKER_AUTO5:
		Packer_DepackAuto5( apSrc );
		break;

	case ePACKER_SPEED3:
		Packer_DepackSpeed3( apSrc );
		break;

	case ePACKER_GODPACK:
		GodPack_DePack( apSrc, apDst );
		break;

	default:
		break;
	}
}

U32		Packer_GetHeaderSize( sPackerHeader * apHeader )
{
	U32 lSize = 0;

	switch( Packer_GetType(apHeader) )
	{
	case ePACKER_GODPACK:
		lSize = sizeof(sGodPackHeader);
		break;

	default:
		break;
	}

	return( lSize );
}


/*
	Let's  avoid memory copying by loading to end of memory buffer for packed data
*/

U32		Packer_GetLoadOffset( sPackerHeader * apHeader )
{
	U32 lOffset = 0;

	switch( Packer_GetType(apHeader) )
	{
	case ePACKER_GODPACK:
		lOffset = GodPack_GetLoadOffset( (sGodPackHeader*)apHeader );
		break;
	default:
		break;
	}
	return( lOffset );
}


/* ################################################################################ */
