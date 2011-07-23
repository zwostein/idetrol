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


#include "atapi.h"
#include "util.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


bool atapi_isDataRequest( void )
{
	uint8_t status = ata_read8( ATA_ALTERNATESTATUS_REG );
	if( !( status & ATA_STATUS_BUSY ) )
		return status & ATAPI_STATUS_DATAREQUEST;
	return false;
}


bool atapi_waitDataRequestTimeout( uint8_t timeout )
{
	if( !atapi_isDataRequest() )
	{
//		printf_P( PSTR("Waiting for device to request data ... ") );
		for( uint8_t i=0; i<timeout; i++ )
		{
			if( atapi_isDataRequest() )
			{
//				printf_P( PSTR("proceeding\n") );
				return true;
			}
			_delay_ms( i );
		}
//		printf_P( PSTR("timeout\n") );
		printf_P( PSTR("atapi_waitDataRequestTimeout\n") );
		return false;
	}
	return true;
}

bool atapi_waitDataRequest( void )
{
	return atapi_waitDataRequestTimeout( ATAPI_WAITDATAREQUEST_DEFAULT_TIMEOUT );
}


bool atapi_waitNoDataRequest( void )
{
	uint8_t status;
	if( !ata_waitStatus( &status ) )
		return false;
	if( status & ATAPI_STATUS_DATAREQUEST )
	{
		printf_P( PSTR("Drive still expecting data\n") );
		return false;
	}
	return true;
}


bool atapi_writeCommandPacket( uint8_t command[12], uint16_t ioLength )
{
//	printf_P( PSTR("Writing command packet ... ") );
	ioLength += 12;
	if( !ata_waitNotBusy() )
		return false;
	ata_write8( ATA_FEATURES_REG, 0 );
	ata_write8( ATAPI_BYTECOUNTLOW_REG, ioLength );
	ata_write8( ATAPI_BYTECOUNTHIGH_REG, ioLength>>8 );
	ata_write8( ATA_COMMAND_REG, ATA_COMMAND_PACKET );
	for( uint8_t i = 0; i<6; i++ )
	{
		if( atapi_waitDataRequest() )
		{
			ata_write16( ATA_DATA_REG, ((uint16_t*)command)[i] );
		}
		else
		{
//			printf_P( PSTR("cancelled by device\n") );
			return false;
		}
	}
//	printf_P( PSTR("... done\n") );
	return true;
}


bool atapi_writePacket( uint8_t * source, uint16_t bytecount )
{
//	printf_P( PSTR("Writing packet ... ") );
	for( uint8_t i = 0; i<bytecount/2; i++ )
	{
		if( atapi_waitDataRequest() )
		{
			ata_write16( ATA_DATA_REG, ((uint16_t*)source)[i] );
		}
		else
		{
//			printf_P( PSTR("cancelled by device\n") );
			return false;
		}
	}
//	printf_P( PSTR("... done\n") );
	return true;
}


bool atapi_readPacket( uint8_t * destination, uint16_t bytecount )
{
//	printf_P( PSTR("Reading packet ... ") );
	for( uint16_t i = 0; i<bytecount/2; i++ )
	{
		if( atapi_waitDataRequest() )
		{
			((uint16_t*)destination)[i] = ata_read16( ATA_DATA_REG );
//			printf_P( PSTR("%X%X "), destination[i2], destination[i2+1] );
		}
		else
		{
//			printf_P( PSTR("cancelled by device\n") );
			return false;
		}
	}
//	printf_P( PSTR("... done\n") );
	return true;
}


bool atapi_readPacketSkip( uint16_t wordcount )
{
//	printf_P( PSTR("Skip packet ... ") );
	for( uint16_t i = 0; i<wordcount/2; i++ )
	{
		if( atapi_waitDataRequest() )
		{
			ata_read16( ATA_DATA_REG );
//			uint16_t data = ata_read16( ATA_DATA_REG );
//			printf_P( PSTR("%X%X "), (data&0xFF), (data>>8) );
		}
		else
		{
//			printf_P( PSTR("cancelled by device\n") );
			return false;
		}
	}
//	printf_P( PSTR("... done\n") );
	return true;
}


bool atapi_requestSense( atapi_requestSense_t * sense )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_REQUESTSENSE,
		0,
		0,
		0,
		14,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	if( !atapi_writeCommandPacket( packet, 14 ) )
	{
		return false;
	}
	uint8_t data[14];
	for( uint8_t i=0; i<14; i++ )
	{
		data[i] = 0;
	}
	for( uint8_t i=0; i<7; i++ )
	{
		if( !atapi_waitDataRequest() )
		{
			break;
		}
		((uint16_t*)data)[i] = ata_read16( ATA_DATA_REG );
	}
	sense->errorCode = data[0] & 0x7F;
	sense->senseKey = data[2] & 0x0F;
	sense->additionalSenseCode = data[12];
	sense->additionalSenseCodeQualifier = data[13];
	return true;
}


bool atapi_readTOCMSF( atapi_trackMSF_t * tracks, int8_t * numTracks, int8_t * firstTrack )
{
	uint16_t ioLength = (*numTracks)*8+4;	// 4Bytes Header + TrackDescriptor each 8Bytes
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_READTOC,
		0x02,	// MSF
		0,
		0,
		0,
		0,
		0,	// starting track
		ioLength>>8,	// allocation length
		ioLength,	//
		0,
		0,
		0
	};
	if( !atapi_writeCommandPacket( packet, ioLength ) )
		return false;

	if( !atapi_waitDataRequest() )
		return false;
	ata_read16( ATA_DATA_REG );	// TOC data length

	if( !atapi_waitDataRequest() )
		return false;
	uint16_t data = ata_read16( ATA_DATA_REG );	// first and last track number
	(*firstTrack) = data;
	int8_t lastTrack = data>>8;
	if( lastTrack-(*firstTrack) >= (*numTracks) )
		lastTrack = (*numTracks)+(*firstTrack)-2;
	(*numTracks) = (lastTrack-(*firstTrack)) + 2;		// count from zero - include lead-out track

	for( uint8_t i=0; i<(*numTracks); i++ )
	{
		if( !atapi_waitDataRequest() )
			return false;
		tracks[i].qBitADRControl = ata_read16( ATA_DATA_REG ) >> 8;
		if( !atapi_waitDataRequest() )
			return false;
		ata_read16( ATA_DATA_REG );	// track number
		if( !atapi_waitDataRequest() )
			return false;
		tracks[i].address.minutes = ata_read16( ATA_DATA_REG ) >> 8;
		if( !atapi_waitDataRequest() )
			return false;
		data = ata_read16( ATA_DATA_REG );
		tracks[i].address.seconds = data;
		tracks[i].address.frames = data>>8;
	}
	return true;
}


bool atapi_startStopUnit( uint8_t loadEjectOperation )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_STARTSTOPUNIT,
		0x00,
		0,
		0,
		loadEjectOperation & 0x03,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	return atapi_writeCommandPacket( packet, 0 );
}


bool atapi_readSubChannel_currentPositionMSF( atapi_readSubChannel_currentPositionMSF_t * current )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_READSUBCHANNEL,
		0x02,	// MSF
		0x40,	// SubQ
		0x01,	// current position
		0,
		0,
		0,	// track number
		0,	// allocation length
		16,	//
		0,
		0,
		0
	};
	if( !atapi_writeCommandPacket( packet, 16 ) )
		return false;

	uint8_t data[16];
	for( uint8_t i=0; i<8; i++ )
	{
		if( !atapi_waitDataRequest() )
			return 0;
		((uint16_t*)data)[i] = ata_read16( ATA_DATA_REG );
	}
	current->audioStatus = data[1];
	current->subChannelDataLength = ((uint16_t*)data)[1];
	current->qBitADRControl = data[5];
	current->track = data[6];
	current->index = data[7];
	current->absolute.minutes = data[9];
	current->absolute.seconds = data[10];
	current->absolute.frames = data[11];
	current->relative.minutes = data[13];
	current->relative.seconds = data[14];
	current->relative.frames = data[15];
	return true;
}


bool atapi_readSubChannel_currentPosition( atapi_readSubChannel_currentPosition_t * current )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_READSUBCHANNEL,
		0x00,	// MSF
		0x40,	// SubQ
		0x01,	// current position
		0,
		0,
		0,	// track number
		0,	// allocation length
		16,	//
		0,
		0,
		0
	};
	if( !atapi_writeCommandPacket( packet, 16 ) )
		return false;

	uint8_t data[16];
	for( uint8_t i=0; i<8; i++ )
	{
		if( !atapi_waitDataRequest() )
			return false;
		((uint16_t*)data)[i] = ata_read16( ATA_DATA_REG );
	}
	current->audioStatus = data[1];
	current->subChannelDataLength = ((uint16_t*)data)[1];
	current->track = data[6];
	current->index = data[7];
	current->absolute = ((uint32_t*)data)[2];
	current->relative = ((uint32_t*)data)[3];
	return true;
}


bool atapi_playAudioMSF( const atapi_msf_t * start, const atapi_msf_t * end )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_PLAYAUDIOMSF,
		0,
		0,
		start->minutes,
		start->seconds,
		start->frames,
		end->minutes,
		end->seconds,
		end->frames,
		0,
		0,
		0
	};
	return atapi_writeCommandPacket( packet, 0 );
}


bool atapi_playAudio( const atapi_lba_t * start, uint16_t length )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_PLAYAUDIO,
		0,
		(*start)>>24,
		(*start)>>16,
		(*start)>>8,
		(*start),
		0,
		length>>8,
		length,
		0,
		0,
		0
	};
	return atapi_writeCommandPacket( packet, 0 );
}


bool atapi_testUnitReady( void )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_TESTUNITREADY,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	if( !atapi_writeCommandPacket( packet, 0 ) )
	{
		return false;
	}
	ata_waitNotBusy();
	return !ata_read8( ATA_ERROR_REG );
}


bool atapi_scanMSF( atapi_msf_t * start, uint8_t reverse )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_SCAN,
		reverse?0x10:0x00,
		0,
		start->minutes,
		start->seconds,
		start->frames,
		0,
		0,
		0,
		0x40,	// type
		0,
		0
	};
	return atapi_writeCommandPacket( packet, 0 );
}


bool atapi_seek( atapi_lba_t * address )
{
	uint8_t packet[12] =
	{
		ATAPI_COMMAND_SEEK,
		0,
		(*address)>>24,
		(*address)>>16,
		(*address)>>8,
		(*address),
		0,
		0,
		0,
		0,
		0,
		0
	};
	return atapi_writeCommandPacket( packet, 0 );
}


bool atapi_waitReady( void )
{
	printf_P( PSTR("Waiting for device to be ready ") );
	while( !atapi_testUnitReady() )
	{
		_delay_ms(1000);
		if( (ata_read8( ATA_ERROR_REG ) & 0xF0) == 0x20 )
		{
			atapi_startStopUnit( ATAPI_STARTSTOPUNIT_START );
			printf_P( PSTR(".") );
		}
		else
		{
			printf_P( PSTR("!") );
		}
	}
	printf_P( PSTR(" proceeding\n") );
	return true;
}


bool atapi_printError( void )
{
	uint8_t status;
	ata_waitStatus( &status );
	if( status & ATA_STATUS_ERROR )
	{
		static uint8_t recursionStopper=0;
		if( recursionStopper==0 )
		{
			recursionStopper=1;
			atapi_requestSense_t sense;
			if( atapi_requestSense( &sense ) )
			{
				printf_P( PSTR("ErrorCode 0x%X   SenseKey 0x%X   ASC 0x%X   ASCQ 0x%X\n"), sense.errorCode, sense.senseKey, sense.additionalSenseCode, sense.additionalSenseCodeQualifier );
			}
			recursionStopper=0;
		}
		return false;
	}
	return true;
}


void stringRemoveSpacePadding( char * str, uint8_t size )
{
	str[size-1] = 0;
	for( uint8_t i=1; i<=size; i++ )
	{
		if( str[size-i] <= 0x20 || str[size-i] >= 0x7F )
			str[size-i] = 0;
		else
			break;
	}
}


bool atapi_identifyPacketDevice( atapi_device_information_t * info )
{
	ata_write8( ATA_FEATURES_REG, 0 );
	ata_write8( ATAPI_BYTECOUNTLOW_REG, 0xff );
	ata_write8( ATAPI_BYTECOUNTHIGH_REG, 0x0f );
	ata_write8( ATA_COMMAND_REG, ATA_COMMAND_IDENTIFYPACKETDEVICE );

	atapi_waitDataRequest();
	info->generalConfig = ata_read16( ATA_DATA_REG );	// word 0

	for( uint8_t i=1; i<=9; i++ )	// words 1 to 9 reserved
	{
		atapi_waitDataRequest();
		ata_read16( ATA_DATA_REG );
	}

	for( uint8_t i=0; i<10; i++ )	// words 10 to 19 serial number
	{
		atapi_waitDataRequest();
		uint16_t data = ata_read16( ATA_DATA_REG );
		info->serialNumber[i*2] = data>>8;
		info->serialNumber[i*2+1] = data;
	}
	stringRemoveSpacePadding( info->serialNumber, 21 );

	for( uint8_t i=20; i<=22; i++ )	// words 20 to 22 reserved
	{
		atapi_waitDataRequest();
		ata_read16( ATA_DATA_REG );
	}

	for( uint8_t i=0; i<4; i++ )	// words 23 to 26 firmware revision
	{
		atapi_waitDataRequest();
		uint16_t data = ata_read16( ATA_DATA_REG );
		info->firmwareRev[i*2] = data>>8;
		info->firmwareRev[i*2+1] = data;
	}
	stringRemoveSpacePadding( info->firmwareRev, 9 );

	for( uint8_t i=0; i<20; i++ )	// words 27 to 48 model number
	{
		atapi_waitDataRequest();
		uint16_t data = ata_read16( ATA_DATA_REG );
		info->modelNr[i*2] = data>>8;
		info->modelNr[i*2+1] = data;
	}
	stringRemoveSpacePadding( info->modelNr, 41 );

	for( uint8_t i=47; i<=48; i++ )	// reserved
	{
		atapi_waitDataRequest();
		ata_read16( ATA_DATA_REG );
	}

	atapi_waitDataRequest();
	info->capabilities = ata_read16( ATA_DATA_REG ) >> 8;	// word 49

	atapi_waitDataRequest();
	ata_read16( ATA_DATA_REG );	// word 50

	atapi_waitDataRequest();
	info->pioModeNr = ata_read16( ATA_DATA_REG ) >> 8;	// word 51

	atapi_waitDataRequest();
	ata_read16( ATA_DATA_REG );	// word 52

	for( uint16_t i=53; i<=255; i++ )	// too lazy
	{
		atapi_waitDataRequest();
		ata_read16( ATA_DATA_REG );
	}

	if( !atapi_waitNoDataRequest() )
		return false;
	return true;
}


static bool atapi_isValidDevice( void )
{
	uint16_t cylinders = ata_read8( ATA_CYLINDERLOW_REG ) | ( ata_read8( ATA_CYLINDERHIGH_REG ) << 8 );
	return cylinders == ATAPI_MAGICNUMBER;
}


static bool atapi_initDevice( void )
{
	if( !atapi_isValidDevice() )
	{
		printf_P( PSTR("No device detected\n") );
		return false;
	}

	atapi_device_information_t info;
	if( !atapi_identifyPacketDevice( &info ) )
	{
		printf_P( PSTR("Could not identify device\n") );
		return false;
	}

	printf_P( PSTR("Device Information:\n") );
	printf_P( PSTR(" * Configuration  0x%04X\n"), info.generalConfig );
	printf_P( PSTR(" * Serial Nr.    \"%s\"\n"), info.serialNumber );
	printf_P( PSTR(" * Firmware Rev. \"%s\"\n"), info.firmwareRev );
	printf_P( PSTR(" * Model Nr.     \"%s\"\n"), info.modelNr );
	printf_P( PSTR(" * Capabilities   0x%02X\n"), info.capabilities );
	printf_P( PSTR(" * PIO Mode Nr.   0x%02X\n"), info.pioModeNr );
	return true;
}


bool atapi_init( void )
{
	bool foundMaster;
	ata_selectDevice( 0 );
	printf_P( PSTR("ATAPI Master: ") );
	foundMaster = atapi_initDevice();

	ata_selectDevice( 1 );
	printf_P( PSTR("ATAPI Slave: ") );
	if( atapi_initDevice() )
		return true;

	if( foundMaster )
		ata_selectDevice( 0 );

	return foundMaster;
}
