#include "stdiowrap.h"


FILE rs232inout = FDEV_SETUP_STREAM( rs232_fdev_put, rs232_fdev_get, _FDEV_SETUP_RW );


int rs232_fdev_put( char c, FILE * stream )
{
	if( c=='\n' )
		rs232_put( '\r' );
	rs232_put( c );
	return 0;
}


int rs232_fdev_get( FILE * stream )
{
	int c = rs232_get();
	if( c < 0 )
		return _FDEV_EOF;
	return c;
}
