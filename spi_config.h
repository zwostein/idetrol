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


#ifndef __SPI_CONFIG_H__INCLUDED__
#define __SPI_CONFIG_H__INCLUDED__


#include <avr/io.h>


#ifndef SPI_MASTER
#define SPI_MASTER	0
#endif


#ifndef SPI_SLAVE
#define SPI_SLAVE	1
#endif


#ifndef SPI_DDR
#define SPI_DDR		DDRB
#endif
#ifndef SPI_PORT
#define SPI_PORT	PORTB
#endif
#ifndef SPI_PIN
#define SPI_PIN		PINB
#endif


#ifndef SPI_SS
#define SPI_SS		PB4
#endif
#ifndef SPI_MOSI
#define SPI_MOSI	PB5
#endif
#ifndef SPI_MISO
#define SPI_MISO	PB6
#endif
#ifndef SPI_SCK
#define SPI_SCK		PB7
#endif


#endif
