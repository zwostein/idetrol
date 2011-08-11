#include "ata.h"
#include "atapi.h"
#include "atapiplayer.h"

#include "rs232/rs232.h"
#include "rs232/stdiowrap.h"
#include "irmp/irmp.h"
#include "irmp/irmpconfig.h"
#include "spi/spi.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
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
char irmp_protocol_string_jvc[] PROGMEM = "JVC";
char irmp_protocol_string_rc6a[] PROGMEM = "RC6A";
char irmp_protocol_string_nikon[] PROGMEM = "Nikon";
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
	irmp_protocol_string_rccar,
	irmp_protocol_string_jvc,
	irmp_protocol_string_rc6a,
	irmp_protocol_string_nikon
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

atapiplayer_t player;
IRMP_DATA irmp_data;

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

	printf_P( PSTR("\nCDTrol "__DATE__"\n") );
	if( !ata_init() )
	{
		printf_P( PSTR("ATA Initialization failed\n") );
		_delay_ms(1000);
		return 1;
	}
	if( !atapi_init() )
	{
		printf_P( PSTR("ATAPI Initialization failed\n") );
		_delay_ms(1000);
		return 1;
	}
	if( !atapiplayer_init( &player ) )
	{
		printf_P( PSTR("ATAPIPlayer Initialization failed\n") );
		_delay_ms(1000);
		return 1;
	}

	while( true )
	{
		if( updateFlag )
		{
			atapiplayer_update( &player );
			updateFlag = 0;
		}
		if( irmp_get_data( &irmp_data ) )
		{
			if( ! (irmp_data.flags & IRMP_FLAG_REPETITION) )
			{
				printf_P( PSTR("\nIRMP: protocol \"%S\", address %d, command %d\n"), (PGM_P)pgm_read_word(&(irmp_protocol_strings[irmp_data.protocol])), irmp_data.address, irmp_data.command );
				switch( irmp_data.command )
				{
					case 53:
						printf_P( PSTR("\nPlay\n") );
						atapiplayer_play( &player );
						break;
					case 48:
						printf_P( PSTR("\nPause\n") );
						atapiplayer_pause( &player );
						break;
					case 54:
						printf_P( PSTR("\nStop\n") );
						atapiplayer_stop( &player );
						break;
					case 36:
						printf_P( PSTR("\nPrevious\n") );
						atapiplayer_previous( &player );
						break;
					case 30:
						printf_P( PSTR("\nNext\n") );
						atapiplayer_next( &player );
						break;
					case 55:
						printf_P( PSTR("\nLoad/Eject\n") );
						atapiplayer_eject( &player );
						break;
				}
			}
			switch( irmp_data.command )
			{
				case 52:
					printf_P( PSTR("\nFastForward\n") );
					atapiplayer_forward( &player );
					break;
				case 50:
					printf_P( PSTR("\nFastRewind\n") );
					atapiplayer_rewind( &player );
					break;
				case 59:
					atapi_printError();
					break;
			}
		}
	}
	return 0;
}
