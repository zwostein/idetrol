/**
 * \file
 * \brief SPI interface
 */

#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>


void spi_init( uint8_t flags )
{
	// set SerialClock, MasterOutput and SlaveSelect pins as output
	SPI_DATADIRREG |= _BV(SPI_DATADIR_SCK) | _BV(SPI_DATADIR_MOSI);
	// set MasterInput as input
	SPI_DATADIRREG &= ~( _BV(SPI_PORT_MISO) );

	// reset pins to initial configuration
	SPI_PORT &= ~( _BV(SPI_PORT_SCK) | _BV(SPI_PORT_MOSI) | _BV(SPI_PORT_MISO) );

	// enable SPI in MasterMode
	SPCR = _BV(SPE) | _BV(MSTR);

	// 'full' SPI speed is fCPU/2 using DoubleSpeed flag - 'half' SPI speed is fCPU/4 without DoubleSpeed flag
	if( !(flags&SPI_HALFSPEED) )
		SPSR |= _BV(SPI2X);
	// additional prescaler of 4
	if( (flags&SPI_QUARTERSPEED) )
		SPCR |= _BV(SPR0);

	// clear status flags and flush receiver buffer
	volatile uint8_t dummy;
	dummy = SPSR;
	dummy = SPDR;
}

