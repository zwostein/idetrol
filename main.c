#include "atapiPlayer.h"

#include "rs232/rs232.h"
#include "rs232/stdiowrap.h"
#include "irmp/irmp.h"
#include "irmp/irmpconfig.h"
#include "spi/spi.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <math.h>


char irmp_protocol_string_unknown[] PROGMEM = "unknown";
char irmp_protocol_string_sircs[] PROGMEM = "SIRCS";
char irmp_protocol_string_nec[] PROGMEM = "NEC";
char irmp_protocol_string_samsung[] PROGMEM = "Samsung";
char irmp_protocol_string_matsushita[] PROGMEM = "Matsushita";
char irmp_protocol_string_kaseikyo[] PROGMEM = "Kaseikyo";
char irmp_protocol_string_recs80[] PROGMEM = "RECS80";
char irmp_protocol_string_rc5[] PROGMEM = "RC5";
char irmp_protocol_string_denon[] PROGMEM = "Denon";
char irmp_protocol_string_rc6[] PROGMEM = "RC6";
char irmp_protocol_string_samsung32[] PROGMEM = "Samsung32";
char irmp_protocol_string_apple[] PROGMEM = "Apple";
char irmp_protocol_string_recs80ext[] PROGMEM = "RECS80EXT";
char irmp_protocol_string_nubert[] PROGMEM = "Nubert";
char irmp_protocol_string_bangolufsen[] PROGMEM = "Bang & Olufsen";
char irmp_protocol_string_grundig[] PROGMEM = "Grundig";
char irmp_protocol_string_nokia[] PROGMEM = "Nokia";
char irmp_protocol_string_siemens[] PROGMEM = "Siemens";
char irmp_protocol_string_fdc[] PROGMEM = "FDC Keyboard";
char irmp_protocol_string_rccar[] PROGMEM = "RC Car";
PGM_P irmp_protocol_strings[] PROGMEM =
{
	irmp_protocol_string_unknown,
	irmp_protocol_string_sircs,
	irmp_protocol_string_nec,
	irmp_protocol_string_samsung,
	irmp_protocol_string_matsushita,
	irmp_protocol_string_kaseikyo,
	irmp_protocol_string_recs80,
	irmp_protocol_string_rc5,
	irmp_protocol_string_denon,
	irmp_protocol_string_rc6,
	irmp_protocol_string_samsung32,
	irmp_protocol_string_apple,
	irmp_protocol_string_recs80ext,
	irmp_protocol_string_nubert,
	irmp_protocol_string_bangolufsen,
	irmp_protocol_string_grundig,
	irmp_protocol_string_nokia,
	irmp_protocol_string_siemens,
	irmp_protocol_string_fdc,
	irmp_protocol_string_rccar
};


////////////////////////////////////////////////////////////////
// Timer 2: polling interval for cd drive

volatile uint8_t updateFlag = 0;

static void timer2_init( void )
{
	// Normal Mode:		WGM21 = 0	WGM20 = 0
	// OC2 disconnected:	COM21 = 0	COM20 = 0
	// prescale 1024:	CS22 = 1	CS21 = 1	CS20 = 1
	// TCCR2:
	//	FOC2	WGM20	COM21	COM20	WGM21	CS22	CS21	CS20
	// 0b	X	0	0	0	1	1	1	1
	// 0x	|	0		|	|	F		|
	TCNT2 = 0;
	OCR2 = 0xff;
	TCCR2 = 0x0F;
	TIMSK |= _BV(OCIE2);	// enable timer2 output compare interrupt
}

ISR( TIMER2_COMP_vect )
{
	static uint8_t cnt = 0;
	cnt++;
	if( !cnt )
	{
		cnt = (int8_t)-4;
		updateFlag = 1;
	}
}


////////////////////////////////////////////////////////////////
// Timer 1: polling interval for remote control

static void timer1_init( void )
{
	OCR1A = (F_CPU / F_INTERRUPTS) - 1;		// compare value: 1/10000 of CPU frequency
	TCCR1B = _BV(WGM12) | _BV(CS10);		// switch CTC Mode on, set prescaler to 1
	TIMSK = _BV(OCIE1A);				// OCIE1A: Interrupt by timer compare
}

ISR( TIMER1_COMPA_vect )
{
	(void) irmp_ISR();
}


////////////////////////////////////////////////////////////////

static void jtag_disable( void )
{
	// JTD bit must be written twice in order to be changed ... for security reasons.
	MCUCSR |= _BV(JTD);
	MCUCSR |= _BV(JTD);
}

static atapi_trackMSF_t trackList[ ATAPIPLAYER_MAX_TRACKS ];
static int8_t numTracks = 0;
static int8_t firstTrackNum = 1;

int main( void )
{
	wdt_disable();
	jtag_disable();
	rs232_init( 9600, 0 );
	irmp_init();
	timer1_init();
	timer2_init();
	stdin = stdout = &rs232inout;
	sei();

	printf_P( PSTR("\nCDTrol\n") );
	if( !ata_init() )
	{
		printf_P( PSTR("ATA Initialization failed\n") );
		return 1;
	}
	if( !atapi_init() )
	{
		printf_P( PSTR("ATAPI Initialization failed\n") );
		return 1;
	}

	IRMP_DATA irmp_data;

	while( 1 )
	{
		if( updateFlag )
		{
			static uint8_t newCD = 1;
			if( atapi_testUnitReady() )
			{
				if( newCD )
				{
					newCD = 0;
					printf_P( PSTR("Reading TOC:\n") );
					numTracks = ATAPIPLAYER_MAX_TRACKS;
					atapi_readTOCMSF( trackList, &numTracks, &firstTrackNum );
					for( uint8_t i=0; i<numTracks; i++ )
					{
						printf_P( PSTR(" * Track %02d   QBitADRControl 0x%02X   Start %02d:%02d:%02d\n"), i, trackList[i].qBitADRControl, trackList[i].address.minutes, trackList[i].address.seconds, trackList[i].address.frames );
					}
					atapi_msf_t first;
					if( nextAudioTrack( trackList, numTracks, firstTrackNum, &first ) >= 0 )
						play( trackList, numTracks, firstTrackNum, &first);
					else
						printf_P( PSTR("No audio track found\n") );
				}
				static atapi_readSubChannel_currentPositionMSF_t last;
				atapi_readSubChannel_currentPositionMSF_t current;
				atapi_readSubChannel_currentPositionMSF( &current );
				if( last.absolute.seconds!=current.absolute.seconds || last.relative.seconds!=current.relative.seconds )
					printf_P( PSTR("Index %02d   Track %02d   Absolute %02d:%02d:%02d   Relative %02d:%02d:%02d   State 0x%02X   QBitADRControl: 0x%X\r"), current.index, current.track, current.absolute.minutes, current.absolute.seconds, current.absolute.frames, current.relative.minutes, current.relative.seconds, current.relative.frames, current.audioStatus, current.qBitADRControl );
				last = current;
			}
			else
			{
				newCD = 1;
			}
			updateFlag = 0;
		}
		if( irmp_get_data( &irmp_data ) )
		{
			if( ! (irmp_data.flags & IRMP_FLAG_REPETITION) )
				printf_P( PSTR("\nIRMP: protocol \"%S\", address %d, command %d\n"), (PGM_P)pgm_read_word(&(irmp_protocol_strings[irmp_data.protocol])), irmp_data.address, irmp_data.command );
			switch( irmp_data.command )
			{
				case 53:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					play( trackList, numTracks, firstTrackNum, NULL );
					break;
				case 48:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					pause();
					break;
				case 54:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					stop();
					break;
				case 36:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					previous( trackList, numTracks, firstTrackNum );
					break;
				case 30:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					next( trackList, numTracks, firstTrackNum );
					break;
				case 52:
					forward( trackList, numTracks, firstTrackNum, 5 );
					break;
				case 50:
					rewind( trackList, numTracks, firstTrackNum, 5 );
					break;
				case 55:
					if( irmp_data.flags & IRMP_FLAG_REPETITION )
						break;
					loadEject();
					break;
			}
		}
	}
	return 0;
}
