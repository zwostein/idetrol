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


#include "atapiplayer.h"

#include "spi_idetrol_proto.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


#define SPI_IDETROL_BUFFERSIZE	4


static atapiplayer_t * _player;
static volatile bool _executeBuffer_executed = true;
static uint8_t _executeBuffer[SPI_IDETROL_BUFFERSIZE];


void spi_idetrol_slave_init( atapiplayer_t * player )
{
	_player = player;
}


void spi_idetrol_slave_update( void )
{
	if( _executeBuffer_executed )
		return;
	printf_P( PSTR("SPI: ") );
	switch( _executeBuffer[0] )
	{
		case SPI_IDETROL_PLAY:
			printf_P( PSTR("Play\n") );
			atapiplayer_play( _player );
			break;
		case SPI_IDETROL_PLAYTRACK:
			printf_P( PSTR("PlayTrack\n") );
			atapiplayer_playTrack( _player, _executeBuffer[1] );
			break;
		case SPI_IDETROL_PLAYMSF:
			printf_P( PSTR("PlayMSF\n") );
			atapiplayer_playMSF( _player, (atapi_msf_t*)&(_executeBuffer[1]) );
			break;
		case SPI_IDETROL_PAUSE:
			printf_P( PSTR("Pause\n") );
			atapiplayer_pause( _player );
			break;
		case SPI_IDETROL_STOP:
			printf_P( PSTR("Stop\n") );
			atapiplayer_stop( _player );
			break;
		case SPI_IDETROL_REWIND:
			printf_P( PSTR("Rewind\n") );
			atapiplayer_rewind( _player );
			break;
		case SPI_IDETROL_FORWARD:
			printf_P( PSTR("Forward\n") );
			atapiplayer_forward( _player );
			break;
		case SPI_IDETROL_PREVIOUS:
			printf_P( PSTR("Previous\n") );
			atapiplayer_previous( _player );
			break;
		case SPI_IDETROL_NEXT:
			printf_P( PSTR("Next\n") );
			atapiplayer_next( _player );
			break;
		case SPI_IDETROL_LOADEJECT:
			printf_P( PSTR("Load/Eject\n") );
			atapiplayer_loadEject( _player );
			break;
		default:
			printf_P( PSTR("???\n") );
			break;
	}
	_executeBuffer_executed = true;
}


ISR( SPI_STC_vect )
{
	static uint8_t buffer[SPI_IDETROL_BUFFERSIZE];
	static uint8_t receiveLength = 0;
	static uint8_t currentPos = 0;
	static uint8_t trackPointer = 0;

	buffer[currentPos] = SPDR;

	if( currentPos == 0 )	// start of a new command
	{
		switch( buffer[0] )
		{
			default:
				SPDR = SPI_IDETROL_UNKNOWNOPCODE;
				return;

			case SPI_IDETROL_NOP:
				SPDR = SPI_IDETROL_OK;
				return;

			// single byte getters:
			case SPI_IDETROL_GET_PLAYERSTATUS:
				SPDR = _player->status;
				return;
			case SPI_IDETROL_GET_NUMTRACKS:
				SPDR = _player->numTracks;
				return;
			case SPI_IDETROL_GET_FIRSTTRACKNUMBER:
				SPDR = _player->firstTrackNr;
				return;
			case SPI_IDETROL_GET_FIRSTAUDIOTRACK:
				SPDR = _player->firstAudioTrack;
				return;
			case SPI_IDETROL_GET_LASTAUIDIOTRACK:
				SPDR = _player->lastAudioTrack;
				return;
			case SPI_IDETROL_GET_CURRENT_AUDIOSTATUS:
				SPDR = _player->currentSubChannel.audioStatus;
				return;
			case SPI_IDETROL_GET_CURRENT_QADRCONTROL:
				SPDR = _player->currentSubChannel.qADRControl;
				return;
			case SPI_IDETROL_GET_CURRENT_TRACKNUMBER:
				SPDR = _player->currentSubChannel.track;
				return;
			case SPI_IDETROL_GET_CURRENT_TRACKINDEX:
				SPDR = _player->currentSubChannel.index;
				return;
			case SPI_IDETROL_GET_CURRENT_ABSOLUTE_MINUTES:
				SPDR = _player->currentSubChannel.absolute.minutes;
				return;
			case SPI_IDETROL_GET_CURRENT_ABSOLUTE_SECONDS:
				SPDR = _player->currentSubChannel.absolute.seconds;
				return;
			case SPI_IDETROL_GET_CURRENT_ABSOLUTE_FRAMES:
				SPDR = _player->currentSubChannel.absolute.frames;
				return;
			case SPI_IDETROL_GET_CURRENT_RELATIVE_MINUTES:
				SPDR = _player->currentSubChannel.relative.minutes;
				return;
			case SPI_IDETROL_GET_CURRENT_RELATIVE_SECONDS:
				SPDR = _player->currentSubChannel.relative.seconds;
				return;
			case SPI_IDETROL_GET_CURRENT_RELATIVE_FRAMES:
				SPDR = _player->currentSubChannel.relative.frames;
				return;

			// single byte track pointer operations for TOC:
			case SPI_IDETROL_TRACKPOINTER_RESET:
				trackPointer = 0;
				SPDR = SPI_IDETROL_OK;
				return;
			case SPI_IDETROL_TRACKPOINTER_INCREMENT:
				if( trackPointer >= _player->numTracks-1 )
				{
					SPDR = SPI_IDETROL_OUTOFRANGE;
				}
				else
				{
					SPDR = SPI_IDETROL_OK;
					trackPointer++;
				}
				return;
			case SPI_IDETROL_TRACKPOINTER_DECREMENT:
				if( trackPointer == 0 )
				{
					SPDR = SPI_IDETROL_OUTOFRANGE;
				}
				else
				{
					SPDR = SPI_IDETROL_OK;
					trackPointer--;
				}
				return;
			case SPI_IDETROL_TRACKPOINTER_GET_MINUTES:
				SPDR = _player->tracks[trackPointer].address.minutes;
				return;
			case SPI_IDETROL_TRACKPOINTER_GET_SECONDS:
				SPDR = _player->tracks[trackPointer].address.seconds;
				return;
			case SPI_IDETROL_TRACKPOINTER_GET_FRAMES:
				SPDR = _player->tracks[trackPointer].address.frames;
				return;
			case SPI_IDETROL_TRACKPOINTER_GET_QADRCONTROL:
				SPDR = _player->tracks[trackPointer].qADRControl;
				return;

			// one byte commands:
			case SPI_IDETROL_PLAY:
			case SPI_IDETROL_PAUSE:
			case SPI_IDETROL_STOP:
			case SPI_IDETROL_REWIND:
			case SPI_IDETROL_FORWARD:
			case SPI_IDETROL_PREVIOUS:
			case SPI_IDETROL_NEXT:
			case SPI_IDETROL_LOADEJECT:
				receiveLength = 1;
				break;

			// two byte commands:
			case SPI_IDETROL_PLAYTRACK:
				receiveLength = 2;
				break;

			// four byte commands:
			case SPI_IDETROL_PLAYMSF:
				receiveLength = 4;
				break;
		}
	}

	currentPos++;	// advance to next position

	if( currentPos >= receiveLength )	// buffer filled with needed data for operation
	{
		if( _executeBuffer_executed )	// if not currently executing another command
		{
			SPDR = SPI_IDETROL_OK;	// send ok
			for( currentPos=0; currentPos<receiveLength; currentPos++ )	// copy received command into execute buffer
				_executeBuffer[currentPos] = buffer[currentPos];
			_executeBuffer_executed = false;
		}
		else	// if currently executing another command
		{
			SPDR = SPI_IDETROL_BUSY;	// send busy
		}
		receiveLength = 0;
		currentPos = 0;	// prepare for new command
	}
	else
	{
		SPDR = SPI_IDETROL_ADDITIONALBYTE;
	}
}
