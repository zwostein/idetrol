#ifndef __RS232_STDIOWRAPPER_H__INCLUDED__
#define __RS232_STDIOWRAPPER_H__INCLUDED__

#include "rs232.h"
#include <stdio.h>


extern FILE rs232inout;


int rs232_fdev_put( char c, FILE * stream );
int rs232_fdev_get( FILE * stream );


#endif
