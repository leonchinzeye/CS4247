//============================================================
// STUDENT NAME: Chin Ze-Ye Leon
// MATRIC NO.  : A0097304E
// NUS EMAIL   : leonchin@u.nus.edu
// COMMENTS TO GRADER:
// <comments to grader, if any>
//
// ============================================================
//
// FILE: Raytrace.cpp


#include <cmath>
#include <cfloat>
#include "Vector3d.h"
#include "Color.h"
#include "Ray.h"
#include "Material.h"
#include "Surface.h"
#include "Plane.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Scene.h"
#include "Raytrace.h"

using namespace std;


// This is for avoiding the "epsilon problem" or the shadow acne problem.
#define DEFAULT_TMIN	10e-6		

// Use this for tmax for non-shadow ray intersection test.
#define DEFAULT_TMAX	DBL_MAX



//////////////////////////////////////////////////////////////////////////////
// Compute the outgoing mirror reflection vector.
// Input incoming vector L is pointing AWAY from surface point.
// Assume normal vector N is unit vector.
// The output reflection vector is pointing AWAY from surface point, and
// has same length as incoming vector L.
//////////////////////////////////////////////////////////////////////////////

static Vector3d mirrorReflect( const Vector3d &L, const Vector3d &N )
{
	return ( 2.0 * dot( N, L ) ) * N - L;
}



//////////////////////////////////////////////////////////////////////////////
// Compute I_source * [ k_d * (N.L) + k_r * (R.V)^n ].
// Input vectors L, N and V are pointing AWAY from surface point.
// Assume all vector L, N and V are unit vectors.
//////////////////////////////////////////////////////////////////////////////

static Color computePhongLighting( const Vector3d &L, const Vector3d &N, const Vector3d &V,
								   const Material &mat, const PointLightSource &ptLight )
{
	Vector3d NN = ( dot( L, N ) >= 0.0 )?  N : -N;

	Vector3d R = mirrorReflect( L, NN );
	float NL = (float) dot( NN, L );
	float RVn = pow( (float) dot( R, V ), (float) mat.n );

	return ptLight.I_source * ( mat.k_d * NL  +  mat.k_r * RVn );
}





//////////////////////////////////////////////////////////////////////////////
// Traces a ray into the scene.
// reflectLevels: specfies number of levels of reflections (0 for no reflection).
// hasShadow: specifies whether to generate shadows.
//////////////////////////////////////////////////////////////////////////////

Color Raytrace::TraceRay( const Ray &ray, const Scene &scene, 
					      int reflectLevels, bool hasShadow )
{
	Ray uRay( ray );
	uRay.makeUnitDirection();  // Normalize ray direction.


// Find whether and where the ray hits some surface. 
// Take the nearest hit point.

	bool hasHitSomething = false;
	double nearest_t = DEFAULT_TMAX;
	SurfaceHitRecord nearestHitRec;

	for ( int i = 0; i < scene.numSurfaces; i++ )
	{
		SurfaceHitRecord tempHitRec;
		bool hasHit = scene.surfacep[i]->hit( uRay, DEFAULT_TMIN, DEFAULT_TMAX, tempHitRec );

		if ( hasHit && tempHitRec.t < nearest_t )
		{
			hasHitSomething = true;
			nearest_t = tempHitRec.t;
			nearestHitRec = tempHitRec;
		}
	}

	if ( !hasHitSomething ) return scene.backgroundColor;

	nearestHitRec.normal.makeUnitVector();
	Vector3d N = nearestHitRec.normal;	// Unit vector.
	Vector3d V = -uRay.direction();		// Unit vector.

	Color result( 0.0f, 0.0f, 0.0f );	// The result will be accumulated here.


	////////////////////////////////////
	//result = nearestHitRec.mat_ptr->k_d; // REMOVE THIS LINE AFTER YOU HAVE FINISHED CODE BELOW.
	////////////////////////////////////


// Add to result the phong lighting contributed by each point light source.
// Compute shadow if hasShadow is true.

    //***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************
	
	for(int i = 0; i < scene.numPtLights; i++) {
		Vector3d pointLight = scene.ptLight[i].position;
		Vector3d L = pointLight - nearestHitRec.p;
		double newTmax = L.length();
		L.makeUnitVector();
		bool isShadowHit = false;

		if(hasShadow) {
			// this for loop checks if there is an occlusion of the surfaces,
			// if there is occlusion, it breaks out of the loop and moves on to the next
			// light source
			for(int j = 0; j < scene.numSurfaces; j++) { 
				Ray shadowRay(nearestHitRec.p, L);
				if(scene.surfacep[j]->shadowHit(shadowRay, DEFAULT_TMIN, newTmax)) {
					isShadowHit = true;
					break;
				}
			}
		}

		//add phong lighting
		if(!isShadowHit) {
			Color phongAns = computePhongLighting(L, N, V, *nearestHitRec.mat_ptr, scene.ptLight[i]);
			result += phongAns;
		}
	}




// Add to result the global ambient lighting.

	//***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************
	result += scene.amLight.I_a * nearestHitRec.mat_ptr->k_a;


// Add to result the reflection of the scene.

	//***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************

	// recursively adds the result of the reflected image.
	// Each time it recurses, the recursion level decreases
	// Base case is when the recursion level hits 0
	if(reflectLevels == 0)
		return result;
	
	Vector3d reflectRayDir = mirrorReflect(V, N);
	Ray newReflectRay(nearestHitRec.p, reflectRayDir);
	return  result + nearestHitRec.mat_ptr->k_rg * TraceRay(newReflectRay, scene, reflectLevels - 1, hasShadow);
}
