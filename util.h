#ifndef __UTIL_H__INCLUDED__
#define __UTIL_H__INCLUDED__

#define setOutput(DDRX,DDXn)	( (DDRX) |= _BV(DDXn) )
#define setInput(DDRX,DDXn)	( (DDRX) &= ~_BV(DDXn) )
#define setAllOutput(DDRX)	( (DDRX) = 0xFF )
#define setAllInput(DDRX)	( (DDRX) = 0x00 )
#define setLow(PORTX,PXn)	( (PORTX) &= ~_BV(PXn) )
#define setHigh(PORTX,PXn)	( (PORTX) |= _BV(PXn) )
#define getState(PINX,PXn) 	( (PINX) & _BV(PXn) )

#endif
