#ifndef	INCLUDED_ACH_DISP_H
#define	INCLUDED_ACH_DISP_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Display_AppInit( void );
void	Achieve_Display_AppDeInit( void );

void	Achieve_Display_ScoreTable_Init( const U16 aTableIndex );
void	Achieve_Display_ScoreTable_DeInit( void );
U16		Achieve_Display_ScoreTable_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey );

void	Achieve_Display_Stats_Init( void );
void	Achieve_Display_Stats_DeInit( void );
U16		Achieve_Display_Stats_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey );

void	Achieve_Display_Tasks_Init( void );
void	Achieve_Display_Tasks_DeInit( void );
U16		Achieve_Display_Tasks_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey );

void	Achieve_Display_Unlocked_Init( const U16 aTaskIndex );
void	Achieve_Display_Unlocked_DeInit( void );
U16		Achieve_Display_Unlocked_Update( void * apPhysic, void * apLogic, const U8 aPacket, const U8 aKey );


/* ################################################################################ */

#endif	/*	INCLUDED_SIGN_H	*/
