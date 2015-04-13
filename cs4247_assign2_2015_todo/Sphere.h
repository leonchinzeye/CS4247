#ifndef _SPHERE_H_  
#define _SPHERE_H_

#include "Surface.h"


class Sphere : public Surface 
{
public:

    Vector3d center;
    double radius;


	Sphere( const Vector3d &theCenter, double theRadius, const Material *mat_ptr )
		{ center = theCenter;  radius = theRadius;  matp = mat_ptr; }


    virtual bool hit( 
					const Ray &r, // Ray being sent.
					double tmin,  // Minimum hit parameter to be searched for.
					double tmax,  // Maximum hit parameter to be searched for.
					SurfaceHitRecord &rec 
                    ) const;


    virtual bool shadowHit(        
					const Ray &r, // Ray being sent.
					double tmin,  // Minimum hit parameter to be searched for.
					double tmax   // Maximum hit parameter to be searched for.
					) const; 

};

#endif // _SPHERE_H_
