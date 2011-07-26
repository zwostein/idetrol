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
 * \brief Low-Level ATA-Interface.
 *
 * Implements a Low-Level ATA-Driver on GPIO-Pins.
 *
 * @{
 **/


#ifndef __ATA_H__INCLUDED__
#define __ATA_H__INCLUDED__


#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>


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


////////////////////////////////////////////////////////////////
// register address definitions based upon the ATA/ATAPI-4 Working Draft
// address pins are asserted or negated - CSx pins are inverted
// negated	|	asserted
// CSx=1	|	CSx=0
// Ax=0		|	Ax=1

//	A0	A1	A2	CS0	CS1	ACCESS
//	0	0	0	0	1	RW
#define ATA_DATA_REG		( _BV(ATA_CS1) )

//	0	1	1	1	0	R
#define ATA_ALTERNATESTATUS_REG	( _BV(ATA_A1) | _BV(ATA_A2) | _BV(ATA_CS0) )
//						W
#define ATA_DEVICECONTROL_REG	ATA_ALTERNATESTATUS_REG

//	1	1	1	0	1	W
#define ATA_COMMAND_REG		( _BV(ATA_A0) | _BV(ATA_A1) | _BV(ATA_A2) | _BV(ATA_CS1) )
//						R
#define ATA_STATUS_REG		ATA_COMMAND_REG

//	1	0	0	0	1	R
#define ATA_ERROR_REG		( _BV(ATA_A0) | _BV(ATA_CS1) )
//						W
#define ATA_FEATURES_REG	ATA_ERROR_REG

//	0	1	1	0	1	RW
#define ATA_DEVICEHEAD_REG	( _BV(ATA_A1) | _BV(ATA_A2) | _BV(ATA_CS1) )

//	1	0	1	0	1	RW
#define ATA_CYLINDERHIGH_REG	( _BV(ATA_A0) | _BV(ATA_A2) | _BV(ATA_CS1) )

//	0	0	1	0	1	RW
#define ATA_CYLINDERLOW_REG	( _BV(ATA_A2) | _BV(ATA_CS1) )

//	0	1	0	0	1	RW
#define ATA_SECTORCOUNT_REG	( _BV(ATA_A1) | _BV(ATA_CS1) )

//	1	1	0	0	1	RW
#define ATA_SECTORNUMBER_REG	( _BV(ATA_A0) | _BV(ATA_A1) | _BV(ATA_CS1) )


////////////////////////////////////////////////////////////////
// register bit definitions
#define ATA_STATUS_BUSY			_BV(7)
#define ATA_STATUS_DEVICEREADY		_BV(6)
#define ATA_STATUS_DATAREQUEST		_BV(3)
#define ATA_STATUS_ERROR		_BV(0)
#define ATA_ERROR_ABORT			_BV(2)
#define ATA_DEVICEHEAD_DEVICE		_BV(4)
#define ATA_DEVICECONTROL_IEN		_BV(1)
#define ATA_DEVICECONTROL_RESET		_BV(2)


////////////////////////////////////////////////////////////////
// ATA Task File Commands
#define ATA_COMMAND_NOP				0x00
#define ATA_COMMAND_DEVICERESET			0x08
#define ATA_COMMAND_DOORLOCK			0xDE
#define ATA_COMMAND_DOORUNLOCK			0xDF
#define ATA_COMMAND_STANDBYIMMEDIATE		0xE0
#define ATA_COMMAND_MEDIAEJECT			0xED
#define ATA_COMMAND_IDENTIFYDRIVE		0xEC
#define ATA_COMMAND_PACKET			0xA0
#define ATA_COMMAND_IDENTIFYPACKETDEVICE	0xA1
#define ATA_IDENTIFYPACKETDEVICE_SERIALNUMBER_LEN	20
#define ATA_IDENTIFYPACKETDEVICE_FIRMWAREREVISION_LEN	8
#define ATA_IDENTIFYPACKETDEVICE_MODELNUMBER_LEN	40


////////////////////////////////////////////////////////////////
// other stuff

#define ATA_WAITNOTBUSY_DEFAULT_TIMEOUT		160

////////////////////////////////////////////////////////////////
// data types

typedef uint32_t ata_lba_t;


////////////////////////////////////////////////////////////////
// functions

uint16_t ata_read16( uint8_t reg );
uint8_t ata_read8( uint8_t reg );
void ata_write16( uint8_t reg, uint16_t data );
void ata_write8( uint8_t reg, uint8_t data );
bool ata_isNotBusy( void );
bool ata_waitNotBusyTimeout( uint8_t timeout );
bool ata_waitNotBusy( void );
bool ata_waitStatusTimeout( uint8_t timeout, uint8_t * status );
bool ata_waitStatus( uint8_t * status );
void ata_selectDevice( uint8_t dev );
void ata_hardReset( void );
void ata_softReset( void );
bool ata_init( void );
void ata_printRegisters( void );


#endif


/** @} **/
