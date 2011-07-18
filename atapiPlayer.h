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


#ifndef __ATAPIPLAYER_H__INCLUDED__
#define __ATAPIPLAYER_H__INCLUDED__


#include "atapi.h"

#include <stdint.h>
#include <stdbool.h>


#define ATAPIPLAYER_MAX_TRACKS	100	// atapi spec defines a maximum of 100 tracks


int8_t thisTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum );
int8_t nextAudioTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address );
int8_t previousAudioTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address );
int8_t lastNonAudioTrack( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address );
void play( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, atapi_msf_t * start );
void pause( void );
void stop( void );
void forward( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, uint8_t seconds );
void rewind( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, uint8_t seconds );
void next( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum );
void previous( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum );
void loadEject( void );


#endif
