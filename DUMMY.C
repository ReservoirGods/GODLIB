/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/AUDIO/AUDIO.H>
#include	<GODLIB/AUDIO/AMIXER.H>
#include	<GODLIB/BIOS/BIOS.H>
#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/MUSIC/PINKNOTE.H>
#include	<GODLIB/MFP/MFP.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VIDEO/VIDEO.H>
#include	<GODLIB/VIDEO/VID_D3D.H>
#include	<GODLIB/VIDEO/VID_SDL.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/XBIOS/XBIOS.H>

#include <string.h>

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	(*gfDummyVoid)();
void	(*gfDummyVoidV)( void );

/* ###################################################################################
#  DATA
################################################################################### */

U16	gDummyU16;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
	AMIXER_S.S
*-----------------------------------------------------------------------------------*/

U8 *			gpAudioMixerSilence;
U8 *			gpAudioMixerBuffer;
U8 *			gpAudioMixerMulTable;
U32				gAudioMixerBufferOffset;
U8				gAudioMixerLockFlag;
U8				gAudioMixerEnableFlag;
sAmixerSpl		gAudioMixerSamples[ 2 ];


/*-----------------------------------------------------------------------------------*
	AUDIO_S.S
*-----------------------------------------------------------------------------------*/

U8				gAudioDmaPlayingFlag;

void			Audio_ReplayEndInt( void ){}
void			Audio_SaveYM( void ){}
void			Audio_RestoreYM( void ){}
void			Audio_SaveDmaSte( void ){}
void			Audio_RestoreDmaSte( void ){}
void			Audio_SaveDmaFalcon( void ){}
void			Audio_RestoreDmaFalcon( void ){}
void			Audio_SoundChipOff( void ){}
void			Audio_MicroWireWrite( const U16 aVal ) { gDummyU16 = aVal; }

/*-----------------------------------------------------------------------------------*
	EXCEPT_S.S
*-----------------------------------------------------------------------------------*/

void			Except_Handler68000( void ){}
void			Except_Handler68030( void ){}
U32				gExcept_HandlersTable[ 64 ];
void			__text( void ){}

/*-----------------------------------------------------------------------------------*
	FADE_S.S
*-----------------------------------------------------------------------------------*/

void			Fade_Vbl( void ) {}
/*U8				gFadeVblActiveFlag;*/

/*-----------------------------------------------------------------------------------*
	GEMDOS_S.S
*-----------------------------------------------------------------------------------*/

S32	GemDos_Call( const S16 aOp ){ (void)aOp; return(0); }
S32	GemDos_Call_L( const S16 aOp, const S32  aL0 ){ (void)aOp;(void)aL0;return(0); }
S32	GemDos_Call_P( const S16 aOp, const void *  ap0 ){ (void)aOp;(void)ap0;return(0); }
S32	GemDos_Call_W( const S16 aOp, const S16 aW0 ){ (void)aOp;(void)aW0;return(0); }
S32	GemDos_Call_LW( const S16 aOp, const S32 aL0, const S16 aW0 ){ (void)aOp;(void)aL0;(void)aW0;return(0); }
S32	GemDos_Call_PL( const S16 aOp, const void * ap0, const S32 aL0 ){ (void)aOp;(void)ap0;(void)aL0;return(0); }
S32	GemDos_Call_PP( const S16 aOp, const void * ap0, const void * ap1 ){ (void)aOp;(void)ap0;(void)ap1;return(0); }
S32	GemDos_Call_PW( const S16 aOp, const void * ap0, const S16 aW0 ){ (void)aOp;(void)ap0;(void)aW0;return(0); }
S32	GemDos_Call_WL( const S16 aOp, const S16 aW0, const S32 aL0 ){ (void)aOp;(void)aW0;(void)aL0;return(0); }
S32	GemDos_Call_WP( const S16 aOp, const S16 aW0, const void * ap0 ){ (void)aOp;(void)aW0;(void)ap0;return(0); }
S32	GemDos_Call_WW( const S16 aOp, const S16 aW0, const S16 aW1 ){ (void)aOp;(void)aW0;(void)aW1;return(0); }
S32	GemDos_Call_LWW( const S16 aOp, const S32 aL0, const S16 aW0, const S16 aW1 ){ (void)aOp;(void)aL0;(void)aW0;(void)aW1;return(0); }
S32	GemDos_Call_PWW( const S16 aOp, const void * ap0, const S16 aW0, const S16 aW1 ){ (void)aOp; (void)ap0; (void)aW0; (void)aW1; return(0); }
S32	GemDos_Call_WLP( const S16 aOp, const S16 aW0, const S32 aL0, const void * ap0 ){ (void)aOp; (void)aW0; (void) aL0; (void)ap0; return(0); }
S32	GemDos_Call_WLL( const S16 aOp, const S16 aW0, const S32 aL0, const S32 aL1 ){ (void)aOp; (void)aW0; (void) aL0; (void)aL1; return(0); }
S32	GemDos_Call_WLW( const S16 aOp, const S16 aW0, const S32 aL0, const S16 aW1 ){ (void)aOp; (void)aW0; (void)aL0; (void)aW1; return(0); }
S32	GemDos_Call_WPL( const S16 aOp, const S16 aW0, const void * ap0, const S32 aL0 ){ (void)aOp; (void)aW0; (void)ap0; (void)aL0; return(0); }
S32	GemDos_Call_WPP( const S16 aOp, const S16 aW0, const void * ap0, const void * ap1 ){ (void)aOp; (void)aW0; (void)ap0; (void)ap1;return(0); }
S32	GemDos_Call_WWP( const S16 aOp, const S16 aW0, const S16 aW1, const void * ap0 ){ (void)aOp; (void)aW0; (void)aW1; (void)ap0; return(0); }
S32	GemDos_Call_WWW( const S16 aOp, const S16 aW0, const S16 aW1, const S16 aW2 ){ (void)aOp; (void)aW0; (void)aW1; (void)aW2; return(0); }
S32	GemDos_Call_WPPL( const S16 aOp, const S16 aW0, const void * ap0, const void * ap1, const S32 aL0 ){ (void)aOp; (void)aW0; (void)ap0; (void)ap1; (void)aL0; return(0); }
S32	GemDos_Call_WPPP( const S16 aOp, const S16 aW0, const void * ap0, const void * ap1, const void * ap2 ){ (void)aOp; (void)aW0; (void)ap0; (void)ap1; (void)ap2; return(0); }
S32	GemDos_Call_WWLL( const S16 aOp, const S16 aW0, const S16 aW1, const S32 aL0, const S32 aL1 ){ (void)aOp; (void)aW0; (void)aW1; (void)aL0; (void)aL1; return(0); }

/*-----------------------------------------------------------------------------------*
	GRF_4_S.S
*-----------------------------------------------------------------------------------*/
/*
void Graphic_4BP_Blit(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc, void * apDst ) { (void)apCanvas; (void)apCoords; (void)apRect; (void)apSrc; (void)apDst;}
void Graphic_4BP_ClearScreen(  struct sGraphicCanvas * apCanvas, void * apDst ){ (void)apCanvas; (void)apDst; }
void Graphic_4BP_CopyScreen(   struct sGraphicCanvas * apCanvas, void * apSrc, void * apDst ){ (void)apCanvas; (void)apSrc; (void)apDst; }
void Graphic_4BP_DrawBox(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour ){ (void)apCanvas, (void)apCoords; (void)aColour; }
void Graphic_4BP_DrawLine(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){ (void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawPixel(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){ (void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawSprite(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite ){(void)apCanvas, (void)apCoords; (void)apSprite;}
void Graphic_4BP_DrawTri(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawQuad(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}

void Graphic_4BP_Blit_Clip(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc, void * apDst ){(void)apCanvas; (void)apCoords; (void)apRect; (void)apSrc; (void)apDst;}
void Graphic_4BP_DrawBox_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawLine_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawPixel_Clip(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawSprite_Clip(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite ){(void)apCanvas, (void)apCoords; (void)apSprite;}
void Graphic_4BP_DrawTri_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawQuad_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
*/
void Graphic_4BP_Blit_BLT(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc, void * apDst ){(void)apCanvas; (void)apCoords; (void)apRect; (void)apSrc; (void)apDst;}
void Graphic_4BP_ClearScreen_BLT(  struct sGraphicCanvas * apCanvas, void * apDst ){ (void)apCanvas, (void)apDst;}
void Graphic_4BP_CopyScreen_BLT(   struct sGraphicCanvas * apCanvas, void * apSrc, void * apDst ){ (void)apCanvas; (void)apSrc; (void)apDst;}
void Graphic_4BP_DrawBox_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawLine_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawPixel_BLT(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawSprite_BLT(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite ){(void)apCanvas, (void)apCoords; (void)apSprite;}
void Graphic_4BP_DrawTri_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawQuad_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}

void Graphic_4BP_Blit_Clip_BLT(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, void * apSrc, void * apDst ){(void)apCanvas; (void)apCoords; (void)apRect; (void)apSrc; (void)apDst;}
void Graphic_4BP_DrawBox_Clip_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawLine_Clip_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawPixel_Clip_BLT(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawSprite_Clip_BLT(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite ){(void)apCanvas, (void)apCoords; (void)apSprite;}
void Graphic_4BP_DrawTri_Clip_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}
void Graphic_4BP_DrawQuad_Clip_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour ){(void)apCanvas, (void)apCoords; (void)aColour;}

/*-----------------------------------------------------------------------------------*
	IKBD_S.S
*-----------------------------------------------------------------------------------*/

void			IKBD_Flush( void ){}
void			IKBD_FlushGemdos( void ){}
void		 	(*IKBD_GetHandler( void ))(void){return( gfDummyVoidV );}
void			IKBD_SetHandler( void (*apHandler)(void) ){ (void)apHandler;}
void			IKBD_SendCmd( U8 aCmd ){ (void)aCmd; }
void			IKBD_MainHandler( void ){}
void			IKBD_PowerpadHandler( void ){}
void			IKBD_InitTosLink( U32 apBuffer, U32 aFunction ){ (void)apBuffer;(void)aFunction; }
U16				IKBD_GetMidiBytesWaiting( void ){return(0);}
U8				IKBD_PopMidiByte( void ){return(0);}


/*-----------------------------------------------------------------------------------*
	MEMORY_S.S
*-----------------------------------------------------------------------------------*/

void	Memory_ClearSimple_1( U32 aSize, void * apMemory )	{ (void)aSize; (void)apMemory; memset(apMemory,0,aSize); }
void	Memory_ClearSimple_16( U32 aSize, void * apMemory )	{ (void)aSize; (void)apMemory;memset(apMemory,0,aSize); }
void	Memory_ClearMajor( U32 aSize, void * apMemory )	{ (void)aSize; (void)apMemory;memset(apMemory,0,aSize); }

/*-----------------------------------------------------------------------------------*
	MFP_S.S
*-----------------------------------------------------------------------------------*/

typedef	struct
{
	U16		Instruction;
	void	(*Func)(void);
} sMfpJumpLong;
sMfpJumpLong	gMfpStcOldJump;
sMfpJumpLong	gMfpStcNewJump;
U8				gMfpNewEnableFlag;
U32				gMfpStcCounter;
sTime			gMfpStcTime;
void			Mfp_SaveRegisters( sMfpRegDump * apDump ){ (void)apDump; }
void			Mfp_RestoreRegisters( sMfpRegDump * apDump ){ (void)apDump; }
void			Mfp_InstallStandardTimerC( void ){}
void			Mfp_GetTimerA( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_GetTimerB( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_GetTimerC( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_GetTimerD( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_InstallTimerA( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_InstallTimerB( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_InstallTimerC( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_InstallTimerD( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_HookIntoTimerC( sMfpTimer * apTimer ){ (void)apTimer; }
void			Mfp_HookDisableTimerC( void ){}
void			Mfp_HookEnableTimerC( void ){}
void			Mfp_DisableSystemTimerC( void ){}

/*-----------------------------------------------------------------------------------*
	PACKER_S.S
*-----------------------------------------------------------------------------------*/

/*
ICE DEPACKER FROM NO CREW
*/

#define ICE_MAGIC (('I' << 24) | ('C' << 16) | ('E' << 8) | '!')
#define ICE_HEADER_SIZE 12
#define ICE_DECRUNCH2_EXTRA_SIZE 120

 int	is_ice_data( char *data );
 size_t	ice_crunched_length( char *data );
 size_t	ice_decrunched_length( char *data );

 char *	ice_crunch( char *data, size_t length, int level );
 size_t	ice_decrunch( char *data, char *destination );
 size_t	ice_decrunch_2( char *data );

 static size_t
	 getinfo( char *data )
 {
	 size_t info = 0;
	 int i;

	 for( i = 0; i < 4; i++ )
		 info = ( info << 8 ) | (unsigned char)*data++;

	 return info;
 }

 int
	 is_ice_data( char *data )
 {
	 return getinfo( data ) == ICE_MAGIC;
 }

 size_t
	 ice_crunched_length( char *data )
 {
	 if( !is_ice_data( data ) )
		 return 0;

	 return getinfo( data + 4 );
 }

 size_t
	 ice_decrunched_length( char *data )
 {
	 if( !is_ice_data( data ) )
		 return 0;

	 return getinfo( data + 8 );
 }

typedef struct ice_decrunch_state
{
	char *unpacked_stop;
	char *unpacked;
	char *packed;
	int bits;
} state_t;

static char *packed_start;

static size_t
init_state( state_t *state, char *data, char *destination )
{
	size_t unpacked_length;


	state->unpacked_stop = destination;
	state->packed = data + ice_crunched_length( data );
	packed_start = state->packed;
	state->bits = ( unsigned char )*--state->packed;
	unpacked_length = ice_decrunched_length( data );
	state->unpacked = destination + unpacked_length;

	return unpacked_length;
}

static void
memcpybwd( char *to, char *from, size_t n )
{
	to += n;
	from += n;
	while( n-- > 0 )
		*--to = *--from;
}

static int
get_bit( state_t *state )
{
	int bit;

	bit = ( state->bits & 0x80 ) != 0;
	state->bits = ( state->bits << 1 ) & 0xff;
	if( state->bits == 0 )
	{
		state->bits = *--state->packed;
		bit = ( state->bits & 0x80 ) != 0;
		state->bits = ( ( state->bits << 1 ) & 0xff ) + 1;
	}

	return bit;
}

static int
get_bits( state_t *state, int n )
{
	int bits = 0;

	while( n-- )
		bits = bits << 1 | get_bit( state );

	return bits;
}

static int
get_depack_length( state_t *state )
{
	static int bits_to_get[] = { 0, 0, 1, 2, 10 };
	static int number_to_add[] = { 2, 3, 4, 6, 10 };
	int i, length;
	int bits;

	for( i = 0; i < 4; i++ )
	{
		if( get_bit( state ) == 0 )
			break;
	}

	bits = bits_to_get[ i ];
	if( bits > 0 )
		length = get_bits( state, bits );
	else
		length = 0;
	length += number_to_add[ i ];

	return length;
}

static int
get_depack_offset( state_t *state, int length )
{
	int i, offset;
	int bits, add;

	if( length == 2 )
	{
		if( get_bit( state ) )
		{
			bits = 9;
			add = 0x3f;
		}
		else
		{
			bits = 6;
			add = -1;
		}

		offset = get_bits( state, bits ) + add;
	}
	else
	{
		static int bits_to_get[] = { 8,  5,  12 };
		static int number_to_add[] = { 31, -1, 287 };

		for( i = 0; i < 2; i++ )
		{
			if( get_bit( state ) == 0 )
				break;
		}

		bits = bits_to_get[ i ];
		add = number_to_add[ i ];
		offset = get_bits( state, bits ) + add;
		if( offset < 0 )
			offset -= length - 2;
	}

	return offset;
}

static int
get_direct_length( state_t *state )
{
	int bits_to_get[] = { 1, 2, 2, 3,    8,     15 };
	int all_ones[] = { 1, 3, 3, 7, 0xff, 0x7fff };
	int number_to_add[] = { 1, 2, 5, 8,   15,    270, 270 };
	int i, n;

	for( i = 0; i < 6; i++ )
	{
		n = get_bits( state, bits_to_get[ i ] );
		if( n != all_ones[ i ] )
			break;
	}
	n += number_to_add[ i ];

	return n;
}

static void
normal_bytes( state_t *state )
{
	int length, offset;

	for( ;;)
	{
		if( get_bit( state ) )
		{
			length = get_direct_length( state );
#ifdef ICE_DEBUG
			fprintf( stderr, "copy: length = %d\n", length );
#endif
			state->packed -= length;
			state->unpacked -= length;

			if( state->unpacked < state->unpacked_stop )
				exit( 1 );
			memcpy( state->unpacked, state->packed, length );
		}

		if( state->unpacked > state->unpacked_stop )
		{
			length = get_depack_length( state );
			offset = get_depack_offset( state, length );

#ifdef ICE_DEBUG
			fprintf( stderr,
				"pack: length = %d, offset = %d\n",
				length,
				offset >= 0 ? offset + length : offset + length - 2 );
#endif

			state->unpacked -= length;

			if( state->unpacked < state->unpacked_stop )
				exit( 1 );
			memcpybwd( state->unpacked,
				state->unpacked + length + offset,
				length );
		}
		else
			return;
	}
}

size_t
ice_decrunch( char *data, char *destination )
{
	size_t unpacked_length;
	state_t state;

	if( !is_ice_data( data ) )
		return 0;

	unpacked_length = init_state( &state, data, destination );
	normal_bytes( &state );

#if 0
	if( is_picture_data )
		something();
#endif

	return unpacked_length;
}

size_t
ice_decrunch_2( char *data )
{
	char save[ ICE_DECRUNCH2_EXTRA_SIZE ];
	size_t unpacked_length;
	state_t state;

	if( !is_ice_data( data ) )
		return 0;

	unpacked_length = init_state( &state, data, data + ICE_DECRUNCH2_EXTRA_SIZE );
	memcpy( save, data + unpacked_length, ICE_DECRUNCH2_EXTRA_SIZE );
	normal_bytes( &state );

#if 0
	if( is_picture_data )
		something();
#endif

	memcpy( data + unpacked_length, save, ICE_DECRUNCH2_EXTRA_SIZE );
	return unpacked_length;
}

void			Packer_DepackIce( void * apData )
{
	ice_decrunch_2( apData );
}

void			Packer_DepackAtomic( void * apData ){ (void)apData; }
void			Packer_DepackAuto5( void * apData ){ (void)apData; }
void			Packer_DepackSpeed3( void * apData ){ (void)apData; }

/*-----------------------------------------------------------------------------------*
	PNKNOT_S.S
*-----------------------------------------------------------------------------------*/

sPinkNoteChannel	gPinkNoteChannel[ 3 ];
void			PinkNote_Player( void ){}
void			PinkNote_NotePlayChannelA( sPinkNoteChannel * apChannel ){ (void)apChannel; }
void			PinkNote_NotePlayChannelB( sPinkNoteChannel * apChannel ){ (void)apChannel; }
void			PinkNote_NotePlayChannelC( sPinkNoteChannel * apChannel ){ (void)apChannel; }

/*-----------------------------------------------------------------------------------*
	PROFILES.S
*-----------------------------------------------------------------------------------*/

void			Profiler_HBL( void ){}
void			Profiler_VBL( void ){}

/*-----------------------------------------------------------------------------------*
	SCRGRABS.S
*-----------------------------------------------------------------------------------*/

void			ScreenGrab_Vbl( void ){}

/*-----------------------------------------------------------------------------------*
	SND_S.S
*-----------------------------------------------------------------------------------*/

void			Snd_SetpPlayFunc( void (*aPlayFunc)(void) ){ (void)aPlayFunc; }
void			Snd_SetpChaserFunc( void (*aChaserFunc)(void) ){ (void)aChaserFunc; }
void			Snd_CallFunction( void (*aFunc)() ){ (void)aFunc; }
void			Snd_Player( void ){}

/*-----------------------------------------------------------------------------------*
	SSD_S.S
*-----------------------------------------------------------------------------------*/

void	Ssd_RepStart( void * apVoiceSet, void * apSong )	{	(void)apVoiceSet; (void)apSong;	}
void	Ssd_RepPlay( void )	{}
void	Ssd_RepStop( void )	{}

/*-----------------------------------------------------------------------------------*
	SYSTEM_S.S
*-----------------------------------------------------------------------------------*/

typedef	struct
{
	U32	Vectors[ 62 ];
} sSystemVectors;
U32				gSystemHblTempCounter;
U32				gSystem200hzTempCounter;
void			System_SaveVectors( sSystemVectors * apVectors ){ (void)apVectors; }
void			System_RestoreVectors( sSystemVectors * apVectors ){ (void)apVectors; }
void			System_SetDataCache030( U32 aFlag ){ (void)aFlag; }
void			System_SetDataCache060( U32 aFlag ){ (void)aFlag; }
void			System_SetInstructionCache030( U32 aFlag ){ (void)aFlag; }
void			System_SetInstructionCache060( U32 aFlag ){ (void)aFlag; }
void			System_HblTemp( void ){}
void			System_200hzTemp( void ){}
void			System_SetIML( U16 aIML ){ (void)aIML; }
U16				System_GetIML( void ){return(0);}
U32				System_GetEmuName0( void ){return(0);}
U32				System_GetEmuName1( void ){return(0);}
U32				System_GetEmuVers( void ){return(0);}
sSystemEmuDesc *	System_GetpEmuDescLL( void )	{ return(0); }

/*-----------------------------------------------------------------------------------*
	VBL_S.S
*-----------------------------------------------------------------------------------*/


void			Vbl_SetHandler( void (*apHandler)() ){ (void)apHandler; }
void			(*Vbl_GetHandler( void ) ) (){return( gfDummyVoid );}
void	Vbl_DummyFunc( void )	{}

/*-----------------------------------------------------------------------------------*
	VIDEO_S.S
*-----------------------------------------------------------------------------------*/

void			Video_SaveRegsST(     void * apBuffer ){ (void)apBuffer; }
void			Video_RestoreRegsST(  void * apBuffer ){ (void)apBuffer; }
void			Video_SaveRegsSTE(    void * apBuffer ){ (void)apBuffer; }
void			Video_RestoreRegsSTE( void * apBuffer ){ (void)apBuffer; }
void			Video_SaveRegsTT(     void * apBuffer ){ (void)apBuffer; }
void			Video_RestoreRegsTT(  void * apBuffer ){ (void)apBuffer; }
void			Video_SaveRegsFalcon(     void * apBuffer ){ (void)apBuffer; }
void			Video_RestoreRegsFalcon(  void * apBuffer ){ (void)apBuffer; }
void			Video_XbiosSetScreen( U16 aMode ){ (void)aMode; }
void			Video_XbiosFalconSetLow( void ){}
void			Video_SetFalconSTLowVGA( void ){}
void			Video_SetFalconSTLowRGBPAL( void ){}
void			Video_SetFalconSTLowRGBNTSC( void ){}
void			Video_SetFalconSTLowRGBPAL_W( void ){}
void			Video_SetFalconSTLowRGBNTSC_W( void ){}

void	Video_UpdateRegsST( void ){}
void	Video_UpdateRegsSTE( void ){}
void	Video_UpdateRegsTT( void ){}
void	Video_UpdateRegsFalcon( void ){}

/*-----------------------------------------------------------------------------------*
	WIPE_S.S
*-----------------------------------------------------------------------------------*/

void	Wipe_In_Init( U16 aIndex, U16 * apGfx )	{ (void)aIndex; (void)apGfx; }
void	Wipe_Out_Init( U16 aIndex )				{ (void)aIndex; }
U16		Wipe_Update( U16 * apScreen )			{ (void)apScreen; return(0);	}

U16		Wipe_IsFinished( void )					{	return(1);	}

/*-----------------------------------------------------------------------------------*
	XBIOS_S.S
*-----------------------------------------------------------------------------------*/

S32	Xbios_Call( const S16 aOp ){ (void)aOp; return(0); }
S32	Xbios_Call_L( const S16 aOp, const S32 aL0 ){ (void)aOp; (void)aL0; return(0); }
S32	Xbios_Call_P( const S16 aOp, const void * ap0 ){ (void)aOp; (void)ap0; return(0); }
S32	Xbios_Call_W( const S16 aOp, const S16 aW0 ){ (void)aOp; (void)aW0; return(0); }
S32	Xbios_Call_LL( const S16 aOp, const S32 aL0, const S32 aL1 ){ (void)aOp; (void)aL0; (void)aL1; return(0); }
S32	Xbios_Call_PP( const S16 aOp, const void * ap0, const void * ap1 ){ (void)aOp; (void)ap0; (void)ap1; return(0); }
S32	Xbios_Call_WP( const S16 aOp, const S16 aW0, const void * ap0 ){ (void)aOp; (void)aW0; (void)ap0; return(0); }
S32	Xbios_Call_WW( const S16 aOp, const S16 aW0, const S16 aW1 ){ (void)aOp; (void)aW0; (void)aW1; return(0); }
S32	Xbios_Call_LLW( const S16 aOp, const S32 aL0, const S32 aL1, const S16 aW0 ){ (void)aOp; (void)aL0; (void)aL1; (void)aW0; return(0); }
S32	Xbios_Call_WWL( const S16 aOp, const S16 aW0, const S16 aW1, const S32 aL0 ){ (void)aOp; (void)aW0; (void)aW1; (void)aL0; return(0); }
S32	Xbios_Call_WWP( const S16 aOp, const S16 aW0, const S16 aW1, const void * ap0 ){ (void)aOp; (void)aW0; (void)aW1; (void)ap0; return(0); }
S32	Xbios_Call_PLW( const S16 aOp, const void * ap0, const S32 aL0, const S16 aW0 ){ (void)aOp; (void)ap0; (void)aL0; (void)aW0; return(0); }
S32	Xbios_Call_PPP( const S16 aOp, const void * ap0, const void * ap1, const void * ap2 ){ (void)aOp; (void)ap0; (void)ap1; (void)ap2; return(0); }
S32	Xbios_Call_PPW( const S16 aOp, const void * ap0, const void * ap1, const S16 aW0 ){ (void)aOp; (void)ap0; (void)ap1; (void)aW0; return(0); }
S32	Xbios_Call_PWP( const S16 aOp, const void * ap0, const S16 aW0, const void * ap1 ){ (void)aOp; (void)ap0; (void)aW0; (void)ap1; return(0); }
S32	Xbios_Call_WPP( const S16 aOp, const S16 aW0, const void * ap0, const void * ap1 ){ (void)aOp; (void)aW0; (void)ap0; (void)ap1; return(0); }
S32	Xbios_Call_WWWP( const S16 aOp, const S16 aW0, const S16 aW1, const S16 aW2, const void * ap0 ){ (void)aOp; (void)aW0; (void)aW1; (void)aW2; (void)ap0; return(0); }
S32	Xbios_Call_WLPL( const S16 aOp, const S16 aW0, const S32 aL0, const void * ap0, const S32 aL1 ){ (void)aOp; (void)aW0; (void)aL0; (void)ap0; (void)aL1; return(0); }
S32	Xbios_Call_LLPP( const S16 aOp, const S32 aL0, const S32 aL1, const void * ap0, const void * ap1 ){ (void)aOp; (void)aL0; (void)aL1; (void)ap0; (void)ap1;return(0); }
S32	Xbios_Call_LWPW( const S16 aOp, const S32 aL0, const S16 aW0, const void * ap0, const S16 aW1 ){ (void)aOp; (void)aL0; (void)aW0; (void)ap0; (void)aW1; return(0); }
S32	Xbios_Call_PLLP( const S16 aOp, const void * ap0, const S32 aL0, const S32 aL1, const void * ap1 ){ (void)aOp; (void)ap0; (void)aL0; (void)aL1; (void)ap1; return(0); }
S32	Xbios_Call_PLPL( const S16 aOp, const void * ap0, const S32 aL0, const void * ap1, const S32 aL1 ){ (void)aOp; (void)ap0; (void)aL0; (void)ap1; (void)aL1; return(0); }
S32	Xbios_Call_PLWW( const S16 aOp, const void * ap0, const S32 aL0, const S16 aW0, const S16 aW1 ){ (void)aOp; (void)ap0; (void)aL0; (void)aW0; (void)aW1; return(0); }
S32	Xbios_Call_PPWW( const S16 aOp, const void * ap0, const void * ap1, const S16 aW0, const S16 aW1 ){ (void)aOp; (void)ap0; (void)ap1; (void)aW0; (void)aW1; return(0); }
S32	Xbios_Call_WWWWW( const S16 aOp, const S16 aW0, const S16 aW1, const S16 aW2, const S16 aW3, const S16 aW4 ){ (void) aOp; (void)aW0; (void)aW1; (void)aW2; (void)aW3; (void)aW4; return(0); }
S32	Xbios_Call_WWWWWW( const S16 aOp, const S16 aW0, const S16 aW1, const S16 aW2, const S16 aW3, const S16 aW4, const S16 aW5 ){ (void) aOp; (void)aW0; (void)aW1; (void)aW2; (void)aW3; (void)aW4; (void)aW5; return(0); }
S32	Xbios_Call_PPLLLP( const S16 aOp, const void * ap0,  const void * ap1, const S32 aL0, const S32 aL1, const S32 aL2, const void * ap2 ){ (void)aOp; (void)ap0; (void)ap1; (void)aL0; (void)aL1; (void)aL2; (void)ap2; return(0); }
S32	Xbios_Call_PLWWWWW( const S16 aOp, const void * ap0, const S32 aL0, const S16 aW0, const S16 aW1, const S16 aW2, const S16 aW3, const S16 aW4 ){ (void)aOp; (void)ap0; (void)aL0; (void)aW0; (void)aW1; (void)aW2; (void)aW3;(void)aW4; return(0); }
S32	Xbios_Call_PLWWWWWW( const S16 aOp, const void * ap0, const S32 aL0, const S16 aW0, const S16 aW1, const S16 aW2, const S16 aW3, const S16 aW4, const S16 aW5 ){ (void)aOp; (void)ap0; (void)aL0; (void)aW0; (void)aW1; (void)aW2; (void)aW3;(void)aW4; (void)aW5; return(0); }
S32	Xbios_Call_PPWWWWWLW( const S16 aOp, const void * ap0,  const void * ap1, const S16 aW0,  const S16 aW1,  const S16 aW2,  const S16 aW3,  const S16 aW4, const S32 aL0, const S16 aW5 ){ (void)aOp; (void)ap0; (void)ap1; (void)aW0; (void)aW1; (void)aW2; (void)aW3;(void)aW4; (void)aL0; (void)aW5; return(0); }


/* ################################################################################ */
