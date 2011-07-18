/**
 * \file
 * \brief RS232 interface using the hardware USART, interrupts and ringbuffers.
 */

#include "rs232.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


////////////////////////////////////////////////////////////////
// Buffers

volatile static char rs232RxBuffer[RS232_RX_BUFFERSIZE];
volatile static uint8_t rs232RxBufferBegin = 0;
volatile static uint8_t rs232RxBufferEnd = 0;
volatile static char rs232TxBuffer[RS232_TX_BUFFERSIZE];
volatile static uint8_t rs232TxBufferBegin = 0;
volatile static uint8_t rs232TxBufferEnd = 0;

inline static uint8_t rs232_getNextRxBufferPos( uint8_t current )
{
	current++;
	if( current >= RS232_RX_BUFFERSIZE) current = 0;
	return current;
}

inline static uint8_t rs232_getNextTxBufferPos( uint8_t current )
{
	current++;
	if( current >= RS232_TX_BUFFERSIZE) current = 0;
	return current;
}

////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Initializes AVR's USART

void rs232_init(uint32_t baud, uint8_t flags)
{
	uint16_t val_UBRR;

	if( flags & RS232_U2X )
	{
		UCSRA |= _BV(U2X);
		val_UBRR = (uint16_t) ( (uint16_t)( (double)F_CPU / (double)( (uint32_t)8 * baud ) ) - (uint16_t)1 );
	}
	else
	{
		UCSRA &= ~_BV(U2X);
		val_UBRR = (uint16_t) ( (uint16_t)( (double)F_CPU / (double)( (uint32_t)16 * baud ) ) - (uint16_t)1 );
	}

	UBRRL = (uint8_t) val_UBRR;
	UBRRH = ( (uint8_t) (val_UBRR>>8) ) & 0x0F;

	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
	UCSRB |= _BV(TXEN) | _BV(RXEN) | _BV(RXCIE);
}

////////////////////////////////////////////////////////////////



////////////////////////////////
// Receiver stuff
////////////////////////////////


////////////////////////////////////////////////////////////////
// Flushes the receiver
inline static void rs232_flushreceiver( void )
{
	volatile char dummy = UDR;	// FIXME: use inline asm instead, as this generates a warning!
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Receiver interrupt that gets triggered everytime
// a character is received.
// The new character will be saved in the buffer
ISR( USART_RXC_vect )
{
	if( UCSRA & ( _BV(FE) | _BV(DOR) | _BV(PE) ) ) { rs232_flushreceiver(); return; }

	uint8_t next = rs232_getNextRxBufferPos(rs232RxBufferEnd);
	if( next != rs232RxBufferBegin )	// place left in buffer?
	{
		rs232RxBuffer[rs232RxBufferEnd] = UDR;
		rs232RxBufferEnd = next;
	}
	else
	{
		rs232_flushreceiver();
	}
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Returns the next unread character in the buffer,
// -1 if there is no data left at the moment
int16_t rs232_get( void )
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if( rs232RxBufferBegin != rs232RxBufferEnd )	// if data in buffer
		{
			char tmp = rs232RxBuffer[rs232RxBufferBegin];
			rs232RxBufferBegin = rs232_getNextRxBufferPos(rs232RxBufferBegin);
			return tmp;
		}
	}
	return -1;
}
////////////////////////////////////////////////////////////////



////////////////////////////////
// Transmitter stuff
////////////////////////////////


////////////////////////////////////////////////////////////////
// Loads UDR register with the next char if UDR-Empty
// interrupt is triggered.
// If no next character is in the buffer, disable interrupt.
ISR( USART_UDRE_vect )
{
	if( rs232TxBufferBegin == rs232TxBufferEnd )	// if no data left in buffer
	{
		UCSRB &= ~_BV(UDRIE);	// disable this interrupt
	}
	else
	{
		UDR = rs232TxBuffer[rs232TxBufferBegin];
		rs232TxBufferBegin = rs232_getNextTxBufferPos(rs232TxBufferBegin);
	}
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Queues a character in the buffer.
// Returns 1 on success, 0 otherwise.
uint8_t rs232_transmit( char c )
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint8_t next = rs232_getNextTxBufferPos(rs232TxBufferEnd);
		if( next != rs232TxBufferBegin )	// place left in buffer?
		{
			rs232TxBuffer[rs232TxBufferEnd] = c;
			rs232TxBufferEnd = next;
			UCSRB |= _BV(UDRIE);
			return 1;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Putter functions

void rs232_put( char c )
{
	while( !rs232_transmit(c) );
}

void rs232_putString( const char * str )
{
	while( *str )
	{
		rs232_put(*str);
		str++;
	}
}

void rs232_putString_P( PGM_P str)
{
	char c;
	while( (c=pgm_read_byte(str)) )
	{
		rs232_put(c);
		str++;
	}
}

////////////////////////////////////////////////////////////////
