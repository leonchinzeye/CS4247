#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vector3d.h"
#include "Color.h"


//////////////////////////////////////////////////////////////////////////////
//
// The lighting model used here is a modified version of that on 
// Slide 8 of Lecture 4 (Ray Tracing).
// Here it is computed as
//
//     I_local = I_a * k_a  +  
//               SUM_OVER_ALL_LIGHTS ( I_source * [ k_d * (N.L) + k_r * (R.V)^n ] )
//
// and
//
//     I = I_local  +  k_rg * I_reflected
//
//
// Note that light sources only illuminate the scene, and they do not 
// appear in the rendered image.
//
//////////////////////////////////////////////////////////////////////////////


struct PointLightSource
{
	Vector3d position;
	Color I_source;
};



// There should just be one single AmbientLightSource object in each scene.

struct AmbientLightSource
{
	Color I_a;
};


#endif // _LIGHT_H_
