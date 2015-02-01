#ifndef	INCLUDED_SCREEN_H
#define	INCLUDED_SCREEN_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/GRAPHIC/GRAPHIC.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eSCREEN_PHYSIC,
	eSCREEN_LOGIC,
	eSCREEN_BACK,
	eSCREEN_MISC,

	eSCREEN_LIMIT
};

enum
{
	eSCREEN_SCROLL_NONE = 0,
	eSCREEN_SCROLL_H    = (1<<0),
	eSCREEN_SCROLL_V    = (1<<1),
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Screen_Init( const U16 aWidth, const U16 aHeight, const U16 aBitDepth, const U16 aScrollFlags );
void	Screen_Update( void );
void	Screen_DeInit( void );
 
U16 *	Screen_GetpPhysic( void );
U16 *	Screen_GetpLogic( void );
U16 *	Screen_GetpBack( void );

U16		Screen_GetPhysicIndex( void );
U16		Screen_GetLogicIndex( void );

void	Screen_SetFrameRate( U32 aRate );
U16		Screen_GetScrollY( void );
void	Screen_SetScrollY( U16 aScrollY );

extern	sGraphicCanvas	gScreenBackGraphic;
extern	sGraphicCanvas	gScreenLogicGraphic;
extern	sGraphicCanvas	gScreenPhysicGraphic;
extern	sGraphicCanvas	gScreenMiscGraphic;

#define	Screen_GetpPhysicGraphic()	(&gScreenPhysicGraphic)
#define	Screen_GetpLogicGraphic()	(&gScreenLogicGraphic)
#define	Screen_GetpBackGraphic()	(&gScreenBackGraphic)
#define	Screen_GetpMiscGraphic()	(&gScreenMiscGraphic)


/* ################################################################################ */

#endif	/*	INCLUDED_SCREEN_H	*/
