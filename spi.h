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


#ifndef __SPI_H__INCLUDED__
#define __SPI_H__INCLUDED__


#include "spi_config.h"

#include <avr/io.h>
#include <stdint.h>


#if !SPI_MASTER && !SPI_SLAVE
#error At least one of SPI_MASTER or SPI_SLAVE must be set!
#endif


#define SPI_HALFSPEED			_BV(0)
#define SPI_QUARTERSPEED		_BV(1)
#define SPI_EIGHTHSPEED			_BV(2)
#define SPI_LSBFIRST			_BV(3)
#define SPI_SCK_HIGHONIDLE		_BV(4)
#define SPI_SCK_SAMPLEFALLINGEDGE	_BV(5)


void spi_init( uint8_t flags );
void spi_setMode( uint8_t flags );
void spi_flush( void );

#if SPI_MASTER
void spi_setSpeed( uint8_t flags );
#endif


inline static void spi_byte_begin( uint8_t data )
{
	SPDR = data;
}


inline static uint8_t spi_byte_end( void )
{
	loop_until_bit_is_set( SPSR, SPIF );
	return SPDR;
}


inline static uint8_t spi_byte( uint8_t data )
{
	spi_byte_begin( data );
	return spi_byte_end();
}


#endif
