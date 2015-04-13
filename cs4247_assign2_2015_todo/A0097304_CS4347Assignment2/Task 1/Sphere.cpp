//============================================================
// STUDENT NAME: Chin Ze-Ye Leon
// MATRIC NO.  : A0097304E
// NUS EMAIL   : leonchin@u.nus.edu
// COMMENTS TO GRADER:
// <comments to grader, if any>
//
// ============================================================
//
// FILE: Sphere.cpp



#include <cmath>
#include "Sphere.h"
#include <algorithm>

using namespace std;



bool Sphere::hit( const Ray &r, double tmin, double tmax, SurfaceHitRecord &rec ) const 
{
	//***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************

	// shifts the ray to the frame of the sphere as the sphere isn't centered at the origin
	Vector3d newRayOrigin = r.origin() - center;

	double a = 1;
	double b = 2 * dot(r.direction(), newRayOrigin);
	double c = dot(newRayOrigin, newRayOrigin) - radius * radius;
	
	double discriminant = b * b - 4 * a * c;

	// means there's no intersection at all
	if(discriminant < 0)
		return false;

	//calculates the t value and checks if it lies within the threshold
	else {
		double t0 = (-b + pow(discriminant, 0.5)) / (2 * a);
		double t1 = (-b - pow(discriminant, 0.5)) / (2 * a);
		double t;
		
		// checking for the threshold, and to obtain the closest positive value of t
		if(t0 < tmin && t1 < tmin) {
			return false;
		} else if (t0 >= tmin && t1 <= tmin) {
			t = t0;
		} else if (t0 <= tmin && t1 >= tmin) {
			t = t1;
		} else {
			t = min(t0, t1);
		}

		//outside of threshold
		if(t < tmin || t > tmax) {
			return false;
		}

		rec.t = t;
		rec.p = r.pointAtParam(t);
		Vector3d temp = rec.p - center;
		rec.normal = temp / temp.length();
		rec.mat_ptr = matp;
	
		return true;
	}

}




bool Sphere::shadowHit( const Ray &r, double tmin, double tmax ) const 
{
	//***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************
	Vector3d newRayOrigin = r.origin() - center;

	double a = 1;
	double b = 2 * dot(r.direction(), newRayOrigin);
	double c = dot(newRayOrigin, newRayOrigin) - radius * radius;

	double discriminant = b * b - 4 * a * c;

	if(discriminant < 0) {
		return false;
	} else {
		double t0 = (-b + pow(discriminant, 0.5)) / (2 * a);
		double t1 = (-b - pow(discriminant, 0.5)) / (2 * a);
		double t;
		if(t0 < tmin && t1 < tmin) {
			return false;
		} else if (t0 >= tmin && t1 <= tmin) {
			t = t0;
		} else if (t0 <= tmin && t1 >= tmin) {
			t = t1;
		} else {
			t = min(t0, t1);
		}

		return (t <= tmax);
	}
}