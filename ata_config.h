/*
 * Copyright (C) 2011 Tobias Himmer <provisorisch@online.de>
 *
 * This file is part of IDETrol.
 *
 * IDETrol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * IDETrol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IDETrol.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 * \defgroup ATA ATA-Interface
 * \brief Low-Level ATA-Interface GPIO configuration.
 *
 * Defines which GPIO is connected to which ATA Pin.
 *
 * @{
 **/


#ifndef __ATA_CONFIG_H__INCLUDED__
#define __ATA_CONFIG_H__INCLUDED__


////////////////////////////////////////////////////////////////
// pin configuration

// 2 complete ports are needed for the 16 bidirectional data pins - one for the high byte, and one for the low byte
#ifndef ATA_DATAL_DDR
#define ATA_DATAL_DDR	DDRA
#endif
#ifndef ATA_DATAL_PORT
#define ATA_DATAL_PORT	PORTA
#endif
#ifndef ATA_DATAL_PIN
#define ATA_DATAL_PIN	PINA
#endif
#ifndef ATA_DATAH_DDR
#define ATA_DATAH_DDR	DDRC
#endif
#ifndef ATA_DATAH_PORT
#define ATA_DATAH_PORT	PORTC
#endif
#ifndef ATA_DATAH_PIN
#define ATA_DATAH_PIN	PINC
#endif

// read strobe pin
#ifndef ATA_RD_DDR
#define ATA_RD_DDR	DDRB
#endif
#ifndef ATA_RD_PORT
#define ATA_RD_PORT	PORTB
#endif
#ifndef ATA_RD_PIN
#define ATA_RD_PIN	PINB
#endif
#ifndef ATA_RD
#define ATA_RD		PB0
#endif

// write strobe pin
#ifndef ATA_WR_DDR
#define ATA_WR_DDR	DDRB
#endif
#ifndef ATA_WR_PORT
#define ATA_WR_PORT	PORTB
#endif
#ifndef ATA_WR_PIN
#define ATA_WR_PIN	PINB
#endif
#ifndef ATA_WR
#define ATA_WR		PB1
#endif

// bus reset
#ifndef ATA_RESET_DDR
#define ATA_RESET_DDR	DDRB
#endif
#ifndef ATA_RESET_PORT
#define ATA_RESET_PORT	PORTB
#endif
#ifndef ATA_RESET_PIN
#define ATA_RESET_PIN	PINB
#endif
#ifndef ATA_RESET
#define ATA_RESET	PB3
#endif

// interrupt
#ifndef ATA_IRQ_DDR
#define ATA_IRQ_DDR	DDRD
#endif
#ifndef ATA_IRQ_PORT
#define ATA_IRQ_PORT	PORTD
#endif
#ifndef ATA_IRQ_PIN
#define ATA_IRQ_PIN	PIND
#endif
#ifndef ATA_IRQ
#define ATA_IRQ		PD2
#endif

// A0, A1, A2, CS0, CS1 must be on the same port
#ifndef ATA_ADDRESS_DDR
#define ATA_ADDRESS_DDR		DDRD
#endif
#ifndef ATA_ADDRESS_PORT
#define ATA_ADDRESS_PORT	PORTD
#endif
#ifndef ATA_ADDRESS_PIN
#define ATA_ADDRESS_PIN		PIND
#endif
#ifndef ATA_A0
#define ATA_A0			PD3
#endif
#ifndef ATA_A1
#define ATA_A1			PD4
#endif
#ifndef ATA_A2
#define ATA_A2			PD5
#endif
#ifndef ATA_CS0
#define ATA_CS0			PD6
#endif
#ifndef ATA_CS1
#define ATA_CS1			PD7
#endif


#endif


/** @} **/
