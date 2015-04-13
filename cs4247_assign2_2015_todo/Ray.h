#ifndef _RAY_H_
#define _RAY_H_

#include <iostream>
#include "Vector3d.h"

using namespace std;


class Ray  
{
public:
    
// Constructors

    Ray() {}

    Ray( const Vector3d &origin, const Vector3d &direction ) 
		{ data[0] = origin; data[1] = direction;  }


// Data setting and reading.

    Ray &setRay( const Vector3d &origin, const Vector3d &direction ) 
		{ data[0] = origin; data[1] = direction; return (*this); }

    Ray &setOrigin( const Vector3d &origin ) { data[0] = origin; return (*this); }
	Ray &setDirection( const Vector3d &direction ) { data[1] = direction; return (*this); }

    Vector3d origin() const { return data[0]; }
    Vector3d direction() const { return data[1]; }


// Other functions.

    Vector3d pointAtParam( double t ) const { return data[0] + t * data[1]; }

	Ray &makeUnitDirection()
	{
		data[1].makeUnitVector();
		return (*this);
	}

	Ray &moveOriginForward( double delta_t )
	{
		data[0] += delta_t * data[1];
		return (*this);
	}

private:

    Vector3d data[2];

}; // Ray



inline ostream &operator<< ( ostream &os, const Ray &r ) 
	{ return ( os << "(" << r.origin() << ") + t("  << r.direction() << ")" ); }


#endif // _RAY_H_
