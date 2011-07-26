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


#include "atapiPlayer.h"


int8_t thisTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum )
{
	atapi_readSubChannel_currentPositionMSF_t current;
	if( ( !atapi_readSubChannel_currentPositionMSF( &current ) ) || (!numTracks) )
		return -1;
	if( !( current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PLAYING || current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PAUSED ) )
		return -1;
	return current.track - firstTrackNum;
}


int8_t nextAudioTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address )
{
	if( !numTracks )
		return -1;
	for( int8_t i=thisTrack(trackList, numTracks, firstTrackNum)+1; i<numTracks-1; i++ )
	{
		if( !(trackList[i].qBitADRControl & ATAPI_QFIELDCONTROL_DATATRACK) )
		{
			if( address )
				(*address) = trackList[i].address;
			return i;
		}
	}
	return -1;
}


int8_t previousAudioTrack( const atapi_trackMSF_t * trackList, int8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address )
{
	if( !numTracks )
		return -1;
	for( int8_t i=thisTrack(trackList, numTracks, firstTrackNum)-1; i>=0; i-- )
	{
		if( !(trackList[i].qBitADRControl & ATAPI_QFIELDCONTROL_DATATRACK) )
		{
			if( address )
				(*address) = trackList[i].address;
			return i;
		}
	}
	return -1;
}


int8_t lastNonAudioTrack( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, atapi_msf_t * address )
{
	if( !numTracks )
		return -1;
	uint8_t end = numTracks-1;
	(*address) = trackList[end].address;
	for( uint8_t i=end; i>0; i-- )
	{
		if( trackList[i].qBitADRControl & ATAPI_QFIELDCONTROL_DATATRACK )
		{
			if( address )
				(*address) = trackList[i].address;
			end = i;
		}
	}
	return end;
}


void play( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, atapi_msf_t * start )
{
	static const atapi_msf_t current = { 0xff, 0xff, 0xff };
	atapi_msf_t end;
	lastNonAudioTrack( trackList, numTracks, firstTrackNum, &end );
	if( !start )
		atapi_playAudioMSF( &current, &end );
	else
		atapi_playAudioMSF( start, &end );
}


void pause( void )
{
	atapi_startStopUnit( 0 );
}


void stop( void )
{
	atapi_lba_t beginning = 0;
	if( !atapi_seek( &beginning ) )
		return;
	if( !atapi_startStopUnit( 0 ) )
		return;
}


void forward( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, uint8_t seconds )
{
	atapi_readSubChannel_currentPositionMSF_t current;
	atapi_readSubChannel_currentPositionMSF( &current );
	if( current.absolute.seconds >= 60-seconds )
	{
		current.absolute.seconds -= 60-seconds;
		current.absolute.minutes++;
	}
	else
	{
		current.absolute.seconds += seconds;
	}
	play( trackList, numTracks, firstTrackNum, &current.absolute );
}


void rewind( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum, uint8_t seconds )
{
	atapi_readSubChannel_currentPositionMSF_t current;
	atapi_readSubChannel_currentPositionMSF( &current );
	if( current.absolute.seconds < seconds )
	{
		if( current.absolute.minutes > 0 )
		{
			current.absolute.seconds += 60-seconds;
			current.absolute.minutes--;
		}
		else
		{
			current.absolute.seconds = 0;
		}
	}
	else
	{
		current.absolute.seconds -= seconds;
	}
	play( trackList, numTracks, firstTrackNum, &current.absolute );
}


void next( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum )
{
	atapi_readSubChannel_currentPositionMSF_t current;
	atapi_readSubChannel_currentPositionMSF( &current );
	if( !(current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PLAYING || current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PAUSED) )
		return;
	atapi_msf_t start;
	if( nextAudioTrack( trackList, numTracks, firstTrackNum, &start )<0 )
		return;
	play( trackList, numTracks, firstTrackNum, &start );
}


void previous( const atapi_trackMSF_t * trackList, uint8_t numTracks, int8_t firstTrackNum )
{
	atapi_readSubChannel_currentPositionMSF_t current;
	atapi_readSubChannel_currentPositionMSF( &current );
	if( !(current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PLAYING || current.audioStatus == ATAPI_READSUBCHANNEL_AUDIOSTATUS_PAUSED) )
		return;
	atapi_msf_t start;
	if( previousAudioTrack( trackList, numTracks, firstTrackNum, &start )<0 )
		return;
	play( trackList, numTracks, firstTrackNum, &start );
}
