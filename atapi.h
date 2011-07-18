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
 * \defgroup ATAPI ATAPI-Interface
 * \brief ATAPI-Interface based on the ATA-Interface.
 *
 * Implements an ATAPI-Driver.
 *
 * @{
 **/


#ifndef __ATAPI_H__INCLUDED__
#define __ATAPI_H__INCLUDED__


#include "ata.h"


////////////////////////////////////////////////////////////////
// register address definitions based upon the ATA/ATAPI-4 Working Draft

#define ATAPI_ALTERNATESTATUS_REG	ATA_ALTERNATESTATUS_REG
#define ATAPI_DEVICECONTROL_REG		ATA_DEVICECONTROL_REG
#define ATAPI_STATUS_REG		ATA_STATUS_REG
#define ATAPI_ERROR_REG			ATA_ERROR_REG
#define ATAPI_FEATURES_REG		ATA_FEATURES_REG
#define ATAPI_DRIVESELECT		ATA_DEVICEHEAD_REG
#define ATAPI_BYTECOUNTHIGH_REG		ATA_CYLINDERHIGH_REG
#define ATAPI_BYTECOUNTLOW_REG		ATA_CYLINDERLOW_REG
#define ATAPI_INTERRUPTREASON_REG	ATA_SECTORCOUNT_REG


////////////////////////////////////////////////////////////////
// register bit definitions

// initial cylindercount value on atapi devices
#define ATAPI_MAGICNUMBER		0xEB14

#define ATAPI_STATUS_BUSY		ATA_STATUS_BUSY
#define ATAPI_STATUS_DEVICEREADY	ATA_STATUS_DEVICEREADY
#define ATAPI_STATUS_DATAREQUEST	ATA_STATUS_DATAREQUEST
#define ATAPI_STATUS_CORRECTABLE	_BV(2)
#define ATAPI_STATUS_ERROR		ATA_STATUS_ERROR

#define ATAPI_ERROR_LENGTH		_BV(0)
#define ATAPI_ERROR_EOM			_BV(1)
#define ATAPI_ERROR_ABORT		ATA_ERROR_ABORT
#define ATAPI_ERROR_MEDIACHANGEREQ	_BV(3)

#define ATAPI_DEVICEHEAD_DRIVE		ATA_DEVICEHEAD_DEVICE


////////////////////////////////////////////////////////////////
// ATAPI Commands

#define ATAPI_COMMAND_STARTSTOPUNIT	0x1B
#define ATAPI_STARTSTOPUNIT_START	_BV(0)
#define ATAPI_STARTSTOPUNIT_LOEJ	_BV(1)

#define ATAPI_COMMAND_REQUESTSENSE	0x03
#define ATAPI_SENSE_NONE		0x00
#define ATAPI_SENSE_RECOVEREDERROR	0x01
#define ATAPI_SENSE_NOTREADY		0x02
#define ATAPI_SENSE_MEDIUMERROR		0x03
#define ATAPI_SENSE_HARDWAREERROR	0x04
#define ATAPI_SENSE_ILLEGALREQUEST	0x05
#define ATAPI_SENSE_UNITATTENTION	0x06
#define ATAPI_SENSE_DATAPROTECT		0x07
#define ATAPI_SENSE_ABORTETCOMMAND	0x0B
#define ATAPI_SENSE_MISCOMPARE		0x0E

#define ATAPI_COMMAND_READSUBCHANNEL	0x42
#define ATAPI_READSUBCHANNEL_DATAFORMAT_CURRENTPOSITION	0x01
#define ATAPI_READSUBCHANNEL_DATAFORMAT_UPC		0x02
#define ATAPI_READSUBCHANNEL_DATAFORMAT_ISRC		0x03
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_NOTSUPPORTED	0x00
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_PLAYING	0x11
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_PAUSED		0x12
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_COMPLETED	0x13
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_ERROR		0x14
#define ATAPI_READSUBCHANNEL_AUDIOSTATUS_NOTPLAYING	0x15

#define ATAPI_QFIELDADR_NOTSUPPLIED	0x00
#define ATAPI_QFIELDADR_CURRENTPOSITION	0x10
#define ATAPI_QFIELDADR_UPC		0x20
#define ATAPI_QFIELDADR_ISRC		0x30
#define ATAPI_QFIELDCONTROL_PREEMPHASIS		_BV(0)
#define ATAPI_QFIELDCONTROL_COPYPERMITTED	_BV(1)
#define ATAPI_QFIELDCONTROL_DATATRACK		_BV(2)
#define ATAPI_QFIELDCONTROL_FOURCHANNELAUDIO	_BV(3)

#define ATAPI_COMMAND_READTOC			0x43
#define ATAPI_COMMAND_PLAYAUDIO			0x45
#define ATAPI_COMMAND_PLAYAUDIOMSF		0x47
#define ATAPI_COMMAND_SCAN			0xBA
#define ATAPI_COMMAND_SEEK			0x2B
#define ATAPI_COMMAND_TESTUNITREADY		0x00


////////////////////////////////////////////////////////////////
// other stuff

#define ATAPI_WAITDATAREQUEST_DEFAULT_TIMEOUT	128


////////////////////////////////////////////////////////////////
// data types

typedef uint32_t atapi_lba_t;

typedef struct
{
	uint8_t minutes;
	uint8_t seconds;
	uint8_t frames;
} atapi_msf_t;

typedef struct
{
	uint8_t errorCode;
	uint8_t senseKey;
	uint8_t additionalSenseCode;
	uint8_t additionalSenseCodeQualifier;
} atapi_requestSense_t;

typedef struct
{
	uint8_t		audioStatus;
	uint16_t	subChannelDataLength;
	uint8_t		qBitADRControl;
	uint8_t		track;
	uint8_t		index;
	atapi_msf_t	absolute;
	atapi_msf_t	relative;
} atapi_readSubChannel_currentPositionMSF_t;

typedef struct
{
	uint8_t		audioStatus;
	uint16_t	subChannelDataLength;
	uint8_t		qBitADRControl;
	uint8_t		track;
	uint8_t		index;
	atapi_lba_t	absolute;
	atapi_lba_t	relative;
} atapi_readSubChannel_currentPosition_t;

typedef struct
{
	uint16_t generalConfig;
	char serialNumber[21];
	char firmwareRev[9];
	char modelNr[41];
	uint8_t capabilities;
	uint8_t pioModeNr;
} atapi_device_information_t;

typedef struct
{
	uint8_t qBitADRControl;
	atapi_msf_t address;
} atapi_trackMSF_t;

typedef struct
{
	uint8_t qBitADRControl;
	atapi_lba_t address;
} atapi_track_t;


////////////////////////////////////////////////////////////////
// functions

bool atapi_isDataRequest( void );
bool atapi_waitDataRequestTimeout( uint8_t timeout );
bool atapi_waitDataRequest( void );
bool atapi_waitNoDataRequest( void );
bool atapi_writeCommandPacket( uint8_t command[12], uint16_t ioLength );
bool atapi_writePacket( uint8_t * source, uint16_t bytecount );
bool atapi_readPacket( uint8_t * destination, uint16_t bytecount );
bool atapi_readPacketSkip( uint16_t wordcount );
bool atapi_requestSense( atapi_requestSense_t * sense );
bool atapi_readTOCMSF( atapi_trackMSF_t * tracks, int8_t * numTracks, int8_t * firstTrack );
bool atapi_startStopUnit( uint8_t loadEjectOperation );
bool atapi_readSubChannel_currentPositionMSF( atapi_readSubChannel_currentPositionMSF_t * current );
bool atapi_readSubChannel_currentPosition( atapi_readSubChannel_currentPosition_t * current );
bool atapi_playAudioMSF( const atapi_msf_t * start, const atapi_msf_t * end );
bool atapi_playAudio( const atapi_lba_t * start, uint16_t length );
bool atapi_testUnitReady( void );
bool atapi_scanMSF( atapi_msf_t * start, uint8_t reverse );
bool atapi_seek( atapi_lba_t * address );
bool atapi_waitReady( void );
bool atapi_printError( void );
bool atapi_identifyPacketDevice( atapi_device_information_t * info );
bool atapi_init( void );

#endif


/** @} **/
