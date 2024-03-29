#ifndef __SPI_IDETROL_PROTO_H__INCLUDED__
#define __SPI_IDETROL_PROTO_H__INCLUDED__


// responses from slave

#define SPI_IDETROL_OK				0
#define SPI_IDETROL_BUSY			1
#define SPI_IDETROL_UNKNOWNOPCODE		2
#define SPI_IDETROL_OUTOFRANGE			3
#define SPI_IDETROL_ADDITIONALBYTE		0xFF


// operations from master

#define SPI_IDETROL_NOP					0x00

#define SPI_IDETROL_GET_PLAYERSTATUS			0x01
#define SPI_IDETROL_GET_NUMTRACKS			0x02
#define SPI_IDETROL_GET_FIRSTTRACKNUMBER		0x03
#define SPI_IDETROL_GET_FIRSTAUDIOTRACK			0x04
#define SPI_IDETROL_GET_LASTAUIDIOTRACK			0x05
#define SPI_IDETROL_GET_CURRENT_AUDIOSTATUS		0x06
#define SPI_IDETROL_GET_CURRENT_QADRCONTROL		0x07
#define SPI_IDETROL_GET_CURRENT_TRACKNUMBER		0x08
#define SPI_IDETROL_GET_CURRENT_TRACKINDEX		0x09
#define SPI_IDETROL_GET_CURRENT_ABSOLUTE_MINUTES	0x0A
#define SPI_IDETROL_GET_CURRENT_ABSOLUTE_SECONDS	0x0B
#define SPI_IDETROL_GET_CURRENT_ABSOLUTE_FRAMES		0x0C
#define SPI_IDETROL_GET_CURRENT_RELATIVE_MINUTES	0x0D
#define SPI_IDETROL_GET_CURRENT_RELATIVE_SECONDS	0x0E
#define SPI_IDETROL_GET_CURRENT_RELATIVE_FRAMES		0x0F

#define SPI_IDETROL_TRACKPOINTER_RESET			0x10
#define SPI_IDETROL_TRACKPOINTER_INCREMENT		0x11
#define SPI_IDETROL_TRACKPOINTER_DECREMENT		0x12
#define SPI_IDETROL_TRACKPOINTER_GET_MINUTES		0x13
#define SPI_IDETROL_TRACKPOINTER_GET_SECONDS		0x14
#define SPI_IDETROL_TRACKPOINTER_GET_FRAMES		0x15
#define SPI_IDETROL_TRACKPOINTER_GET_QADRCONTROL	0x16

#define SPI_IDETROL_PLAY				0x80
#define SPI_IDETROL_PAUSE				0x81
#define SPI_IDETROL_STOP				0x82
#define SPI_IDETROL_REWIND				0x83
#define SPI_IDETROL_FORWARD				0x84
#define SPI_IDETROL_PREVIOUS				0x85
#define SPI_IDETROL_NEXT				0x86
#define SPI_IDETROL_LOADEJECT				0x87
#define SPI_IDETROL_PLAYTRACK				0x88
#define SPI_IDETROL_PLAYMSF				0x89


#endif
