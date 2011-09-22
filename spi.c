/*
 * Copyright (C) 2011 Tobias Himmer <provisorisch@online.de>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License.
 * If not, see <http://www.gnu.org/licenses/>.
 */


#include "spi.h"
#include "util.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#if SPI_MASTER
void spi_setSpeed( uint8_t flags )
{
	SPSR &= ~( _BV(SPI2X) | _BV(SPR0) | _BV(SPR1) );
	if( !(flags & SPI_HALFSPEED) )
		SPSR |= _BV(SPI2X);	// 'full' SPI speed is fCPU/2 using DoubleSpeed flag - 'half' SPI speed is fCPU/4 without DoubleSpeed flag
	if( flags & SPI_QUARTERSPEED )
		SPCR |= _BV(SPR0);	// additional prescaler of 4
	if( flags & SPI_EIGHTHSPEED )
		SPCR |= _BV(SPR1);	// additional prescaler of 8
}
#endif


void spi_setMode( uint8_t flags )
{
	SPSR &= ~( _BV(DORD) | _BV(CPOL) | _BV(CPHA) );
	if( flags & SPI_LSBFIRST )
		SPCR |= _BV(DORD);
	if( flags & SPI_SCK_HIGHONIDLE )
		SPCR |= _BV(CPOL);
	if( flags & SPI_SCK_SAMPLEFALLINGEDGE )
		SPCR |= _BV(CPHA);
}


void spi_flush( void )
{
	volatile uint8_t dummy;
	dummy = SPSR;	// clear status flags
	dummy = SPDR;	// flush receiver buffer
}


void spi_init( uint8_t flags )
{
#if SPI_MASTER
	setInput( SPI_DDR, SPI_MISO );
	setOutput( SPI_DDR, SPI_SCK );
	setOutput( SPI_DDR, SPI_MOSI );
	SPCR = _BV(MSTR);	// master mode
	spi_setSpeed( flags );
#else
	setInput( SPI_DDR, SPI_SCK );
	setInput( SPI_DDR, SPI_MOSI );
	setOutput( SPI_DDR, SPI_MISO );
	SPCR = _BV(SPIE);	// interrupt driven slave mode
#endif
#if SPI_SLAVE
	setInput( SPI_DDR, SPI_SS );
#else
	setOutput( SPI_DDR, SPI_SS );
#endif
	spi_setMode( flags );
	SPCR |= _BV(SPE);	// enable SPI
	spi_flush();
}
