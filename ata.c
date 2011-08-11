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


#include "ata.h"
#include "util.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define ATA_ADDRESS_MASK ( _BV(ATA_A0) | _BV(ATA_A1) | _BV(ATA_A2) | _BV(ATA_CS0) | _BV(ATA_CS1) )


// performs a 16bit read on the given register address
uint16_t ata_read16( uint8_t reg )
{
	uint16_t data;
	// switch data pins to input mode
	setAllInput( ATA_DATAL_DDR );
	setAllInput( ATA_DATAH_DDR );
	// clear previous address and set new one
	ATA_ADDRESS_PORT = (ATA_ADDRESS_PORT & ~(ATA_ADDRESS_MASK)) | reg;
	// strobe and read in data from data ports
	setLow( ATA_RD_PORT, ATA_RD );
	asm volatile ("nop");
	asm volatile ("nop");
	data = ATA_DATAL_PIN | (ATA_DATAH_PIN<<8);
	setHigh( ATA_RD_PORT, ATA_RD );
	return data;
}


// performs a 8bit read on the given register address
uint8_t ata_read8( uint8_t reg )
{
	uint8_t data;
	// switch data pins to input mode
	setAllInput( ATA_DATAL_DDR );
	setAllInput( ATA_DATAH_DDR );
	// clear previous address and set new one
	ATA_ADDRESS_PORT = (ATA_ADDRESS_PORT & ~(ATA_ADDRESS_MASK)) | reg;
	// strobe and read in data from data ports
	setLow( ATA_RD_PORT, ATA_RD );
	asm volatile ("nop");
	asm volatile ("nop");
	data = ATA_DATAL_PIN;
	setHigh( ATA_RD_PORT, ATA_RD );
	return data;
}


// performs a 16bit write on the given register address
void ata_write16( uint8_t reg, uint16_t data )
{
	// clear previous address and set new one
	ATA_ADDRESS_PORT = (ATA_ADDRESS_PORT & ~(ATA_ADDRESS_MASK)) | reg;
	// write data to ports and strobe
	ATA_DATAL_PORT = data;
	ATA_DATAH_PORT = data>>8;
	// switch data pins to output mode
	setAllOutput( ATA_DATAL_DDR );
	setAllOutput( ATA_DATAH_DDR );
	setLow( ATA_RD_PORT, ATA_WR );
	asm volatile ("nop");
	asm volatile ("nop");
	setHigh( ATA_RD_PORT, ATA_WR );
}


// performs a 8bit write on the given register address
void ata_write8( uint8_t reg, uint8_t data )
{
	// clear previous address and set new one
	ATA_ADDRESS_PORT = (ATA_ADDRESS_PORT & ~(ATA_ADDRESS_MASK)) | reg;
	// write data to ports and strobe
	ATA_DATAL_PORT = data;
	// switch data pins to output mode
	setAllOutput( ATA_DATAL_DDR );
	setLow( ATA_RD_PORT, ATA_WR );
	asm volatile ("nop");
	asm volatile ("nop");
	setHigh( ATA_RD_PORT, ATA_WR );
}


// returns nonzero if selected device is ready to accept commands
bool ata_isNotBusy( void )
{
	return !( ata_read8( ATA_STATUS_REG ) & ATA_STATUS_BUSY );
}


// waits for the selected device beeing ready - aborts on timeout
bool ata_waitNotBusyTimeout( uint8_t timeout )
{
	if( !ata_isNotBusy() )
	{
//		printf_P( PSTR("Waiting for device ... ") );
		for( uint8_t i=0; i<timeout; i++ )
		{
			if( ata_isNotBusy() )
			{
//				printf_P( PSTR("proceeding\n") );
				return 1;
			}
			_delay_ms( i );
		}
//		printf_P( PSTR("timeout\n") );
		printf_P( PSTR("ata_waitNotBusyTimeout\n") );
		return false;
	}
	return true;
}


// ata_waitNotBusyTimeout with a default value
bool ata_waitNotBusy( void )
{
	return ata_waitNotBusyTimeout( ATA_WAITNOTBUSY_DEFAULT_TIMEOUT );
}


// waits for the selected device beeing ready and returns status - aborts on timeout
bool ata_waitStatusTimeout( uint8_t timeout, uint8_t * status )
{
	*status = ata_read8( ATA_STATUS_REG );
	if( !( *status & ATA_STATUS_BUSY ) )
	{
//		printf_P( PSTR("Waiting for device ... ") );
		for( uint8_t i=0; i<timeout; i++ )
		{
			*status = ata_read8( ATA_STATUS_REG );
			if( !( *status & ATA_STATUS_BUSY ) )
			{
//				printf_P( PSTR("proceeding\n") );
				return true;
			}
			_delay_ms( i );
		}
//		printf_P( PSTR("timeout\n") );
		printf_P( PSTR("ata_waitStatusTimeout\n") );
		return false;
	}
	return true;
}


// ata_waitStatusTimeout with a default value
bool ata_waitStatus( uint8_t * status )
{
	return ata_waitStatusTimeout( ATA_WAITNOTBUSY_DEFAULT_TIMEOUT, status );
}


// selects a specific device from the bus
void ata_selectDevice( uint8_t dev )
{
	ata_waitNotBusy();
	uint8_t dh = ata_read8( ATA_DEVICEHEAD_REG );
	if(dev)
		dh |= ATA_DEVICEHEAD_DEVICE;
	else
		dh &= ~(ATA_DEVICEHEAD_DEVICE);
	ata_write8( ATA_DEVICEHEAD_REG, dh );
	ata_waitNotBusy();
}


// performs a harware reset for all connected devices
void ata_hardReset( void )
{
	setLow( ATA_RESET_PORT, ATA_RESET );
	_delay_us( 25 );	// reset for at least 25us
	setHigh( ATA_RESET_PORT, ATA_RESET );
}


// soft reset selected device
void ata_softReset( void )
{
	ata_waitNotBusy();
	ata_write8( ATA_DEVICECONTROL_REG, ATA_DEVICECONTROL_IEN | ATA_DEVICECONTROL_RESET );
	_delay_ms(2);
	ata_write8( ATA_DEVICECONTROL_REG, ATA_DEVICECONTROL_IEN );
	_delay_ms(2);
	ata_waitNotBusy();
}


// initialize ATA
bool ata_init( void )
{
	// configure outputs
	setHigh( ATA_RESET_PORT, ATA_RESET );
	setHigh( ATA_WR_PORT, ATA_WR );
	setHigh( ATA_RD_PORT, ATA_RD );
	setLow( ATA_ADDRESS_PORT, ATA_A0 );
	setLow( ATA_ADDRESS_PORT, ATA_A1 );
	setLow( ATA_ADDRESS_PORT, ATA_A2 );
	setHigh( ATA_ADDRESS_PORT, ATA_CS0 );
	setHigh( ATA_ADDRESS_PORT, ATA_CS1 );
	setOutput( ATA_RESET_DDR, ATA_RESET );
	setOutput( ATA_WR_DDR, ATA_WR );
	setOutput( ATA_RD_DDR, ATA_RD );
	setOutput( ATA_ADDRESS_DDR, ATA_A0 );
	setOutput( ATA_ADDRESS_DDR, ATA_A1 );
	setOutput( ATA_ADDRESS_DDR, ATA_A2 );
	setOutput( ATA_ADDRESS_DDR, ATA_CS0 );
	setOutput( ATA_ADDRESS_DDR, ATA_CS1 );

	// configure inputs
	setInput( ATA_IRQ_DDR, ATA_IRQ );

	// configure bidirectional ports as input - for now
	setAllInput( ATA_DATAL_DDR );
	setAllInput( ATA_DATAH_DDR );

	ata_hardReset();
	_delay_ms(2000);
	ata_selectDevice(0);
	ata_write8( ATA_DEVICECONTROL_REG, ATA_DEVICECONTROL_IEN );
	ata_selectDevice(1);
	ata_write8( ATA_DEVICECONTROL_REG, ATA_DEVICECONTROL_IEN );
	ata_selectDevice(0);

	return true;
}


void ata_printRegisters( void )
{
	ata_waitNotBusy();
	printf_P( PSTR("\nCurrent Register:\n") );
	printf_P( PSTR(" * ATA_DEVICEHEAD_REG: 0x%02X\n"), ata_read8( ATA_DEVICEHEAD_REG ) );
	printf_P( PSTR(" * ATA_STATUS_REG: 0x%02X\n"), ata_read8( ATA_STATUS_REG ) );
	printf_P( PSTR(" * ATA_ERROR_REG: 0x%02X\n"), ata_read8( ATA_ERROR_REG ) );
	printf_P( PSTR(" * ATA_CYLINDERLOW_REG: 0x%02X\n"), ata_read8( ATA_CYLINDERLOW_REG ) );
	printf_P( PSTR(" * ATA_CYLINDERHIGH_REG: 0x%02X\n"), ata_read8( ATA_CYLINDERHIGH_REG ) );
	printf_P( PSTR(" * ATA_SECTORCOUNT_REG: 0x%02X\n"), ata_read8( ATA_SECTORCOUNT_REG ) );
	printf_P( PSTR(" * ATA_SECTORNUMBER_REG: 0x%02X\n"), ata_read8( ATA_SECTORNUMBER_REG ) );
}
