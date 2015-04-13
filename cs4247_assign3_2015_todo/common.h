#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;
typedef unsigned short ushort;

#ifndef M_PI
#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923
#endif


extern void ShowFatalError( const char *srcfile, int lineNum, const char *format, ... );
	// Outputs an error message to the stderr and exits program.


extern void ShowWarning( const char *srcfile, int lineNum,  const char *format, ... );
	// Outputs a warning message to the stderr.


extern double GetCurrRealTime( void );
	// Returns time in seconds (plus fraction of a second) since midnight (00:00:00), 
	// January 1, 1970, coordinated universal time (UTC).
	// Up to millisecond precision.


#define CheckedMalloc(mem_size) _CheckedMalloc( (mem_size), __FILE__, __LINE__ )

inline void *_CheckedMalloc( size_t size, const char *srcfile, int lineNum )
	// same as malloc(), but checks for out-of-memory.
{
	void *p = malloc( size );
	if ( p == NULL ) ShowFatalError( srcfile, lineNum, "Cannot allocate memory." );
	return p;
}


template <typename Type>
inline Type Sqr( Type x )
	// returns the square of x.
{
	return ( x * x );
}


template <typename Type>
inline Type Cube( Type x )
	// returns the cube of x.
{
	return ( x * x * x );
}


template <typename Type>
inline Type Clamp( Type x, Type low, Type hi )
{
	if ( x < low ) return low;
	else if ( x > hi ) return hi;
	return x;
}


template <typename Type>
inline void CopyArrayN( Type dest[], const Type src[], size_t numElem )
{
	memcpy( dest, src, numElem * sizeof(Type) );
}


template <typename Type>
inline void CopyArray4( Type dest[4], const Type src[4] )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}


template <typename Type>
inline void CopyArray3( Type dest[3], const Type src[3] )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}


template <typename Type>
inline void CopyArray2( Type dest[2], const Type src[2] )
{
	dest[0] = src[0];
	dest[1] = src[1];
}


template <typename Type>
inline Type Min2( Type a, Type b )
{
	return ( ( a < b )? a : b );
}


template <typename Type>
inline Type Max2( Type a, Type b )
{
	return ( ( a > b )? a : b );
}


template <typename Type>
inline Type Min3( Type a, Type b, Type c )
{
    Type t = a;
    if ( b < t ) t = b;
	return ( ( c < t )? c : t );
}


template <typename Type>
inline Type Max3( Type a, Type b, Type c )
{
    Type t = a;
    if ( b > t ) t = b;
	return ( ( c > t )? c : t );
}

#endif
