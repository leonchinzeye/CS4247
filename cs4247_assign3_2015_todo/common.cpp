#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "common.h"

#define MSG_BUF_LEN		2048


void ShowFatalError( const char *srcfile, int lineNum, const char *format, ... )
	// Outputs an error message to the stderr and exits program.
{
	va_list args;
	char buffer[MSG_BUF_LEN];
	va_start( args, format );
	vsprintf( buffer, format, args );
	va_end( args );

    fprintf( stderr, "FATAL ERROR: %s (%s, line %d).\n", buffer, srcfile, lineNum );
    exit( 1 );	// terminate application.
}



void ShowWarning( const char *srcfile, int lineNum,  const char *format, ... )
	// Outputs a warning message to the stderr.
{
	va_list args;
	char buffer[MSG_BUF_LEN];
	va_start( args, format );
	vsprintf( buffer, format, args );
	va_end( args );

	fprintf( stderr, "WARNING: %s (%s, line %d).\n", buffer, srcfile, lineNum );
}



double GetCurrRealTime( void )
	// Returns time in seconds (plus fraction of a second) since midnight (00:00:00), 
	// January 1, 1970, coordinated universal time (UTC).
	// Up to millisecond precision.
{
#ifdef _WIN32
	struct _timeb timebuffer;
	_ftime( &timebuffer );
#else
	struct timeb timebuffer;
	ftime( &timebuffer );
#endif
	return ((double)timebuffer.time + ((double)timebuffer.millitm / 1000.0));
}
