/**
 * \file
 * \brief RS232 interface using the hardware USART, interrupts and ringbuffers.
 */

#ifndef __RS232_H__INCLUDED__
#define __RS232_H__INCLUDED__


#include <avr/pgmspace.h>
#include <stdint.h>


////////////////////////////////
// Configuration
#ifndef RS232_RX_BUFFERSIZE
#define RS232_RX_BUFFERSIZE	32
#endif
#ifndef RS232_TX_BUFFERSIZE
#define RS232_TX_BUFFERSIZE	16
#endif
////////////////////////////////

////////////////////////////////
// Init-Flags
#define RS232_U2X	1
////////////////////////////////


void rs232_init( uint32_t baud, uint8_t flags );

int16_t rs232_get( void );

void rs232_put( char c );
void rs232_putString( const char * str );
void rs232_putString_P( PGM_P str );


#endif
