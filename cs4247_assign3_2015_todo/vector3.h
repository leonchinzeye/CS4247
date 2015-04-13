#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


inline double Vec_fsqr( double f )
    // returns the square of f.
{
    return ( f * f );
}

inline float Vec_fsqr( float f )
    // returns the square of f.
{
    return ( f * f );
}


inline void Vec3dToVec3f( float f[3], const double d[3] )
{
	f[0] = (float) d[0]; f[1] = (float) d[1]; f[2] = (float) d[2];
}

inline void Vec3fToVec3d( double d[3], const float f[3] )
{
	d[0] = f[0]; d[1] = f[1]; d[2] = f[2];
}



inline void VecPrint( const double v[3], FILE *fp=stdout )
{
	fprintf( fp, "[ %g, %g, %g ]", v[0], v[1], v[2] );
}

inline void VecPrint( const float v[3], FILE *fp=stdout )
{
	fprintf( fp, "[ %g, %g, %g ]", v[0], v[1], v[2] );
}



inline double *VecSum( double vo[3], const double v1[3], const double v2[3] )
    // computes the vector vo = v1 + v2. 
	// returns &vo[0]. 
{
    vo[0] = v1[0] + v2[0];
    vo[1] = v1[1] + v2[1];
    vo[2] = v1[2] + v2[2];
	return vo;
}

inline float *VecSum( float vo[3], const float v1[3], const float v2[3] )
    // computes the vector vo = v1 + v2. 
	// returns &vo[0]. 
{
    vo[0] = v1[0] + v2[0];
    vo[1] = v1[1] + v2[1];
    vo[2] = v1[2] + v2[2];
	return vo;
}



inline double *VecSum3( double vo[3], const double v1[3], const double v2[3], const double v3[3] )
    // computes the vector vo = v1 + v2 + v3. 
	// returns &vo[0]. 
{
    vo[0] = v1[0] + v2[0] + v3[0];
    vo[1] = v1[1] + v2[1] + v3[1];
    vo[2] = v1[2] + v2[2] + v3[2];
	return vo;
}

inline float *VecSum3( float vo[3], const float v1[3], const float v2[3], const float v3[3] )
    // computes the vector vo = v1 + v2 + v3. 
	// returns &vo[0]. 
{
    vo[0] = v1[0] + v2[0] + v3[0];
    vo[1] = v1[1] + v2[1] + v3[1];
    vo[2] = v1[2] + v2[2] + v3[2];
	return vo;
}



inline double *VecDiff( double vo[3], const double v1[3], const double v2[3] )
    // computes the vector vo = v1 - v2. 
	// returns &vo[0].
{
    vo[0] = v1[0] - v2[0];
    vo[1] = v1[1] - v2[1];
    vo[2] = v1[2] - v2[2];
	return vo;
}

inline float *VecDiff( float vo[3], const float v1[3], const float v2[3] )
    // computes the vector vo = v1 - v2. 
	// returns &vo[0].
{
    vo[0] = v1[0] - v2[0];
    vo[1] = v1[1] - v2[1];
    vo[2] = v1[2] - v2[2];
	return vo;
}



inline double *VecNeg( double vo[3], const double v[3] )
    // computes the vector vo = -v. 
	// returns &vo[0]. 
{
    vo[0] = -v[0];
    vo[1] = -v[1];
    vo[2] = -v[2];
	return vo;
}

inline float *VecNeg( float vo[3], const float v[3] )
    // computes the vector vo = -v. 
	// returns &vo[0]. 
{
    vo[0] = -v[0];
    vo[1] = -v[1];
    vo[2] = -v[2];
	return vo;
}



inline double *VecScale( double vo[3], double k, const double v[3] )
    // computes the vector vo = k*v. 
	// returns &vo[0]. 
{
    vo[0] = k * v[0];
    vo[1] = k * v[1];
    vo[2] = k * v[2];
	return vo;
}

inline float *VecScale( float vo[3], float k, const float v[3] )
    // computes the vector vo = k*v. 
	// returns &vo[0]. 
{
    vo[0] = k * v[0];
    vo[1] = k * v[1];
    vo[2] = k * v[2];
	return vo;
}



inline double VecDotProd( const double v1[3], const double v2[3] )
    // returns the dot product ( v1 . v2 ). 
{
    return ( v1[0] * v2[0]  +  v1[1] * v2[1]  +  v1[2] * v2[2] );
}

inline float VecDotProd( const float v1[3], const float v2[3] )
    // returns the dot product ( v1 . v2 ). 
{
    return ( v1[0] * v2[0]  +  v1[1] * v2[1]  +  v1[2] * v2[2] );
}



inline double *VecCrossProd( double vo[3], const double v1[3], const double v2[3] )
    // computes the vector vo = v1 x v2. 
	// returns &vo[0]. 
{
    double v3[3];
    v3[0] = v1[1] * v2[2]  -  v1[2] * v2[1];
    v3[1] = v1[2] * v2[0]  -  v1[0] * v2[2];
    v3[2] = v1[0] * v2[1]  -  v1[1] * v2[0];
    vo[0] = v3[0];
    vo[1] = v3[1];
    vo[2] = v3[2];
	return vo;
}

inline float *VecCrossProd( float vo[3], const float v1[3], const float v2[3] )
    // computes the vector vo = v1 x v2. 
	// returns &vo[0]. 
{
    float v3[3];
    v3[0] = v1[1] * v2[2]  -  v1[2] * v2[1];
    v3[1] = v1[2] * v2[0]  -  v1[0] * v2[2];
    v3[2] = v1[0] * v2[1]  -  v1[1] * v2[0];
    vo[0] = v3[0];
    vo[1] = v3[1];
    vo[2] = v3[2];
	return vo;
}



inline double VecLen( const double v[3] )
    // returns |v|, the magnitude of v. 
{
    return sqrt( v[0] * v[0]  +  v[1] * v[1]  +  v[2] * v[2] );
}

inline float VecLen( const float v[3] )
    // returns |v|, the magnitude of v. 
{
    return (float)sqrt( v[0] * v[0]  +  v[1] * v[1]  +  v[2] * v[2] );
}



inline double VecSqrLen( const double v[3] )
    // returns |v|*|v|, the squared magnitude of v. 
{
    return ( v[0] * v[0]  +  v[1] * v[1]  +  v[2] * v[2] );
}

inline float VecSqrLen( const float v[3] )
    // returns |v|*|v|, the squared magnitude of v. 
{
    return ( v[0] * v[0]  +  v[1] * v[1]  +  v[2] * v[2] );
}



inline double VecDist( const double v1[3], const double v2[3] )
    // returns |v1-v2|, the distance between them. 
{
    return sqrt( Vec_fsqr(v2[0]-v1[0]) + Vec_fsqr(v2[1]-v1[1]) + Vec_fsqr(v2[2]-v1[2]) );
}

inline float VecDist( const float v1[3], const float v2[3] )
    // returns |v1-v2|, the distance between them. 
{
    return (float)sqrt( Vec_fsqr(v2[0]-v1[0]) + Vec_fsqr(v2[1]-v1[1]) + Vec_fsqr(v2[2]-v1[2]) );
}



inline double VecSqrDist( const double v1[3], const double v2[3] )
    // returns |v1-v2|^2, the distance between them. 
{
    return ( Vec_fsqr(v2[0]-v1[0]) + Vec_fsqr(v2[1]-v1[1]) + Vec_fsqr(v2[2]-v1[2]) );
}

inline float VecSqrDist( const float v1[3], const float v2[3] )
    // returns |v1-v2|^2, the distance between them. 
{
    return ( Vec_fsqr(v2[0]-v1[0]) + Vec_fsqr(v2[1]-v1[1]) + Vec_fsqr(v2[2]-v1[2]) );
}



inline bool VecEqual( const double v1[3], const double v2[3] )
    // returns TRUE iff v1 == v2 exactly.
{
    return ( ( v1[0] == v2[0] ) && ( v1[1] == v2[1] ) && ( v1[2] == v2[2] ) );
}

inline bool VecEqual( const float v1[3], const float v2[3] )
    // returns TRUE iff v1 == v2 exactly.
{
    return ( ( v1[0] == v2[0] ) && ( v1[1] == v2[1] ) && ( v1[2] == v2[2] ) );
}



inline bool VecEqual( const double v1[3], const double v2[3], double tolerance )
    // returns TRUE iff v1 == v2 approximately (within tolerance).
{
	double d[3];
	double sqrDist = VecSqrLen( VecDiff( d, v1, v2 ) );
	return sqrDist <= (tolerance * tolerance);
}

inline bool VecEqual( const float v1[3], const float v2[3], float tolerance )
    // returns TRUE iff v1 == v2 approximately (within tolerance).
{
	float d[3];
	float sqrDist = VecSqrLen( VecDiff( d, v1, v2 ) );
	return sqrDist <= (tolerance * tolerance);
}



inline double *VecNormalize( double vo[3], const double v[3] )
    // computes vo = v / |v|, i.e. the unit vector of v. 
    // NOTE: if v equals zero vector, vo will be zero vector. 
	// returns &vo[0]. 
{
    double v_len = VecLen( v );

    if ( v_len == 0.0 )
    {
		vo[0] = 0.0;
		vo[1] = 0.0;
		vo[2] = 0.0;
	}
	else
	{
		vo[0] = v[0] / v_len;
		vo[1] = v[1] / v_len;
		vo[2] = v[2] / v_len;
	}
	return vo;
}

inline float *VecNormalize( float vo[3], const float v[3] )
    // computes vo = v / |v|, i.e. the unit vector of v. 
    // NOTE: if v equals zero vector, vo will be zero vector. 
	// returns &vo[0]. 
{
    float v_len = VecLen( v );

    if ( v_len == 0.0f )
    {
		vo[0] = 0.0f;
		vo[1] = 0.0f;
		vo[2] = 0.0f;
	}
	else
	{
		vo[0] = v[0] / v_len;
		vo[1] = v[1] / v_len;
		vo[2] = v[2] / v_len;
	}
	return vo;
}



inline double *VecTriNormal( double vo[3], const double v1[3], const double v2[3], const double v3[3] )
    // computes the vector normal to the triangle's plane and
    // places the result into vo.
    // the normal is computed as ( v1v2 x v1v3 ).
    // REMARK: if v1, v2, v3 are collinear, the result is a zero vector.
 	// returns &vo[0].
{
    double v4[3], v5[3];
	VecDiff( v4, v2, v1 );
	VecDiff( v5, v3, v1 );
    VecCrossProd( vo, v4, v5 );
	return vo;
}

inline float *VecTriNormal( float vo[3], const float v1[3], const float v2[3], const float v3[3] )
    // computes the vector normal to the triangle's plane and
    // places the result into vo.
    // the normal is computed as ( v1v2 x v1v3 ).
    // REMARK: if v1, v2, v3 are collinear, the result is a zero vector.
 	// returns &vo[0].
{
    float v4[3], v5[3];
	VecDiff( v4, v2, v1 );
	VecDiff( v5, v3, v1 );
    VecCrossProd( vo, v4, v5 );
	return vo;
}



#endif
