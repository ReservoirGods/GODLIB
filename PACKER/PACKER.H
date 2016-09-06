#ifndef	INCLUDED_PACKER_H
#define	INCLUDED_PACKER_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

typedef enum ePacker
{
	ePACKER_NONE,
	ePACKER_ATOMIC,
	ePACKER_AUTO5,
	ePACKER_ICE,
	ePACKER_SPEED3,
	ePACKER_GODPACK,

	ePACKER_LIMIT
} ePacker;


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sPackerHeader
{
	U32	m0;
	U32	m1;
	U32	m2;
	U32	m3;
} sPackerHeader;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U8		Packer_IsPacked( sPackerHeader * apHeader );
ePacker	Packer_GetType( sPackerHeader * apHeader );
U32		Packer_GetDepackSize( sPackerHeader * apHeader );
U32		Packer_GetHeaderSize( sPackerHeader * apHeader );
U32		Packer_GetLoadOffset( sPackerHeader * apHeader );
void	Packer_Depack( void * apSrc, void * apDst );

extern void	Packer_DepackAtomic( void * apData );
extern void	Packer_DepackAuto5( void * apData );
extern void	Packer_DepackIce( void * apData );
extern void	Packer_DepackSpeed3( void * apData );


/* ################################################################################ */

#endif	/* INCLUDED_PACKER_H */
