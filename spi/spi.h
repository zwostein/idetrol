/**
 * \file
 * \brief SPI interface.
 */

#ifndef __SPI_H__INCLUDED__
#define __SPI_H__INCLUDED__

#include <avr/io.h>
#include <stdint.h>

////////////////////////////////
// Configuration
#ifndef SPI_DATADIRREG
#define SPI_DATADIRREG		DDRB
#endif
#ifndef SPI_DATADIR_MISO
#define SPI_DATADIR_MISO	DDB6
#endif
#ifndef SPI_DATADIR_MOSI
#define SPI_DATADIR_MOSI	DDB5
#endif
#ifndef SPI_DATADIR_SCK
#define SPI_DATADIR_SCK		DDB7
#endif
#ifndef SPI_DATADIR_SS
#define SPI_DATADIR_SS		DDB4
#endif
#ifndef SPI_PORT
#define SPI_PORT		PORTB
#endif
#ifndef SPI_PORT_MISO
#define SPI_PORT_MISO		PORTB6
#endif
#ifndef SPI_PORT_MOSI
#define SPI_PORT_MOSI		PORTB5
#endif
#ifndef SPI_PORT_SCK
#define SPI_PORT_SCK		PORTB7
#endif
////////////////////////////////

////////////////////////////////
// Init flags
#define SPI_HALFSPEED		1
#define SPI_QUARTERSPEED	2
////////////////////////////////


void spi_init( uint8_t flags );

inline static void spi_byte_begin( uint8_t data )
{
	SPDR = data;
}

inline static uint8_t spi_byte_end( void )
{
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

inline static uint8_t spi_byte( uint8_t data )
{
	spi_byte_begin(data);
	return spi_byte_end();
}


#endif
