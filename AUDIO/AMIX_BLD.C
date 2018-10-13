#include	"AMIXER.H"

#include	"MATH.H"

#include	<STDIO.H>

void	AudioMixer_StringWrite( FILE * apFile, const char * apString )
{
	U32 len = 0;
	for( len = 0; apString[ len ]; len++ );
	fwrite( apString, len, 1, apFile );
}

void	AudioMixer_ArrayWriteU8( FILE * apFile, const char * apArrayName, U8 * apArray, U16 aCount )
{
	char lLine[ 1024 ];
	U16	j;
	if( apFile )
	{
		sprintf( lLine, "\nU8 %s[%d]=\n{", apArrayName, aCount );
		AudioMixer_StringWrite( apFile, lLine );
		for( j = 0; j < aCount; j++ )
		{
			if( 0 == ( j & 31 ) )
				AudioMixer_StringWrite( apFile, "\n\t" );
			if( j+1 == aCount )
				sprintf( lLine, "0x%02X", apArray[ j ] );
			else
				sprintf( lLine, "0x%02X,", apArray[ j ] );
			AudioMixer_StringWrite( apFile, lLine );
		}
		AudioMixer_StringWrite( apFile, "\n};\n" );
	}
}

void	AudioMixer_SineTableBuild()
{
	U16	i;
	U8	lTable[256];

	for( i = 0; i<256; i++ )
	{
		FP32	f = (FP32)i;
		FP32	t;
		f /= 255;
		f *= ( 1.57079632679489661923132f );

		t = (FP32)cos( f );
		t *= 255.f;
		lTable[ i ] = (U8)t;
	}
	FILE * lpFile = fopen( "AM_COS.H", "w" );
	if( lpFile )
	{
		AudioMixer_ArrayWriteU8( lpFile, "gAudioMixerCosTable", lTable, 256 );
		fclose( lpFile );
	}
}

void	AudioMixer_PanLawTableBuild()
{
	U16	i;
	U8	lTable0[ 256 ];
	U8	lTable1[ 256 ];

	FP32	halfPI = 1.57079632679489661923132f;
	FP32	twoDivPI = 2.f / 3.14159265358979323846264f;

	for( i = 0; i<256; i++ )
	{
		FP32	t = (FP32)i;
		FP32	s, c;
		FP32	left, right;
		t /= 255;
		t *= ( 1.57079632679489661923132f );

		s = (FP32)sin( t );
		c = (FP32)cos( t );

		
		left = (FP32)sqrt( ( halfPI - t ) * twoDivPI * c );
		right = (FP32)sqrt( t * twoDivPI * s );

		left *= 255.f;
		right *= 255.f;

		lTable0[ i ] = (U8)left;
		lTable1[ i ] = (U8)right;
	}
	FILE * lpFile = fopen( "AMPANLAW.H", "w" );
	if( lpFile )
	{
		AudioMixer_ArrayWriteU8( lpFile, "gAudioMixerPanLawLeftTable", lTable0, 256 );
		AudioMixer_ArrayWriteU8( lpFile, "gAudioMixerPanLawRightTable", lTable1, 256 );
		fclose( lpFile );
	}
}



void	AudioMixer_TablesBuild()
{
	AudioMixer_SineTableBuild();
	AudioMixer_PanLawTableBuild();
}
