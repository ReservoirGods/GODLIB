**************************************************************************************
*	ASPRIT_S.S
*
*	assembly true colour sprite routs
*
*	[c] 2005 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	AsmSprite_Draw
	export	AsmSprite_DrawClip


**************************************************************************************
;	STRUCTS
**************************************************************************************

	OFFSET

sAsmSprite_mColours:	ds.l	13
sAsmSprite_mWidth:		ds.w	1
sAsmSprite_mHeight:		ds.w	1
sAsmSprite_mfLines:		ds.w	1


**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_Draw( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
* ACTION   : AsmSprite_Draw
* CREATION : 10.04.2005 PNK
*------------------------------------------------------------------------------------*

AsmSprite_Draw:
	movem.l	d0-d7/a0-a6,-(a7)
	bsr		AsmSprite_Show
	movem.l	(a7)+,d0-d7/a0-a6
	rts

AsmSprite_Show:
	ext.l	d0
	sub.w	sAsmSprite_mWidth(a0),d0
	add.l	d0,d0
	move.l	#sAsmSprite_mfLines,d1
	move.l	(a0,d1.l),-(a7)
	movem.l	(a0),d1-d7/a0/a2-a6
	rts
		


*------------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_DrawClip( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth,S16 aTopLine,S16 aBotLine )
* ACTION   : AsmSprite_DrawClip
* CREATION : 10.04.2005 PNK
*------------------------------------------------------------------------------------*

AsmSprite_DrawClip:
	
	rts


