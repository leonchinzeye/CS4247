#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "vector3.h"
#include "quadmodel.h"


#define EQUAL_VERTEX_THRESHOLD	(1e-6f)		// If the distance between two vertices are less than this threshold,
											// they are assumed to be the same vertex.


static const float ZERO_VEC_3F[3] = { 0.0f, 0.0f, 0.0f };



void QM_SurfaceInit( QM_Surface *s )
{
	if ( s == NULL ) return;
	CopyArray3( s->reflectivity, ZERO_VEC_3F );
	CopyArray3( s->emission, ZERO_VEC_3F );

	s->numOrigQuads = 0;
	s->origQuads = NULL;
	s->numShooterQuads = 0;
	s->shooters = NULL;
	s->numGathererQuads = 0;
	s->gatherers = NULL;
}


QM_Surface QM_SurfaceInit( void )
{
	QM_Surface s;
	QM_SurfaceInit( &s );
	return s;
}


void QM_SurfaceCleanUp( QM_Surface *s )
{
	if ( s == NULL ) return;
	free( s->origQuads );
	free( s->shooters );
	free( s->gatherers );
	QM_SurfaceInit( s );
}


void QM_ModelInit( QM_Model *m )
{
	if ( m == NULL ) return;
	m->numSurfaces = 0;
	m->surfaces = NULL;
	m->totalShooters = 0;
	m->shooters = NULL;
	m->totalGatherers = 0;
	m->gatherers = NULL;

	CopyArray3( m->min_xyz, ZERO_VEC_3F );
	CopyArray3( m->max_xyz, ZERO_VEC_3F );
	CopyArray3( m->dim_xyz, ZERO_VEC_3F );
	CopyArray3( m->center, ZERO_VEC_3F );
	m->radius = 0.0f;
}


QM_Model QM_ModelInit( void )
{
	QM_Model m;
	QM_ModelInit( &m );
	return m;
}


void QM_ModelCleanUp( QM_Model *m )
{
	if ( m == NULL ) return;
	free( m->surfaces );
	free( m->shooters );
	free( m->gatherers );
	QM_ModelInit( m );
}


static void ComputeBoundingBox( QM_Model *m )
	// Compute an axis-aligned bounding box (AABB).
{
	if ( m == NULL || m->numSurfaces <= 0 ) return;

	m->min_xyz[0] = m->min_xyz[1] = m->min_xyz[2] = FLT_MAX;
	m->max_xyz[0] = m->max_xyz[1] = m->max_xyz[2] = -FLT_MAX;

	for ( int s = 0; s < m->numSurfaces; s++ )
		for ( int q = 0; q < m->surfaces[s].numOrigQuads; q++ )
		{
			QM_OrigQuad *quad = &(m->surfaces[s].origQuads[q]);

			for ( int i = 0; i < 4; i++ )
			{
				if ( quad->v[i][0] < m->min_xyz[0] ) m->min_xyz[0] = quad->v[i][0];
				if ( quad->v[i][1] < m->min_xyz[1] ) m->min_xyz[1] = quad->v[i][1];
				if ( quad->v[i][2] < m->min_xyz[2] ) m->min_xyz[2] = quad->v[i][2];
				if ( quad->v[i][0] > m->max_xyz[0] ) m->max_xyz[0] = quad->v[i][0];
				if ( quad->v[i][1] > m->max_xyz[1] ) m->max_xyz[1] = quad->v[i][1];
				if ( quad->v[i][2] > m->max_xyz[2] ) m->max_xyz[2] = quad->v[i][2];

			}
		}

	m->dim_xyz[0] = m->max_xyz[0] - m->min_xyz[0];
	m->dim_xyz[1] = m->max_xyz[1] - m->min_xyz[1];
	m->dim_xyz[2] = m->max_xyz[2] - m->min_xyz[2];
	m->center[0] = 0.5f * (m->max_xyz[0] + m->min_xyz[0]);
	m->center[1] = 0.5f * (m->max_xyz[1] + m->min_xyz[1]);
	m->center[2] = 0.5f * (m->max_xyz[2] + m->min_xyz[2]);
	m->radius = (float) 0.5 * sqrt( Sqr( m->dim_xyz[0] ) + Sqr( m->dim_xyz[1] ) + Sqr( m->dim_xyz[2] ) );
}




#define MAX_LINE_LEN	1024	// Max number of characters, including newline char, per line in the input file.


static bool ReadDataLine( char *lineBuf, int *lineNum, const char *filename, FILE *fp )
{
	while ( !feof( fp ) ) {

		// Read next line from input file.
		(*lineNum)++;
		char *line = fgets( lineBuf, MAX_LINE_LEN + 1, fp );  
		if ( line == NULL )
			ShowFatalError( __FILE__, __LINE__, "Fail to read line %d of file \"%s\"", *lineNum, filename );

		// Check that the line is not too long.
		int lineLen = strlen( lineBuf );
		if ( lineLen == MAX_LINE_LEN && lineBuf[MAX_LINE_LEN-1] != '\n' )
			ShowFatalError( __FILE__, __LINE__, "Line %d of file \"%s\" is too long", *lineNum, filename );

		// Skip comment and empty line.
		if ( lineLen > 1 && lineBuf[0] == '#' ) continue;	// Skip comment line.
		if ( lineLen == 1 ) continue;  // Skip empty line.

		for ( int i = 0; i < lineLen; i++ )
			if ( !isspace( lineBuf[i] ) ) return true;   // Return the line if it is not all spaces.
	}

	return false;  // End of file.
}


QM_Model QM_ReadFile( const char *filename )
	// Read model from input file.
	// The output QM_Model has only QM_OrigQuad.
	// The axis-aligned bounding box is computed.
{
	char badFile[] = "Invalid input model file";
	char badEOF[] = "Unexpected end of file";
	char lineBuf[ MAX_LINE_LEN + 1 ];

	// Open input file
	FILE *fp = fopen( filename, "r" );
	if ( fp == NULL ) 
		ShowFatalError( __FILE__, __LINE__, "Cannot open input model file \"%s\"", filename );

	int lineNum = 0;

//=== VERTICES ===

	int numVertices = 0;
	float *vertexTable = NULL;  // An array of 3D vertices.

	// Read number of vertices.
	if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

	if ( sscanf( lineBuf, "%d", &numVertices ) != 1  ||  numVertices < 0 )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

	vertexTable = (float *) CheckedMalloc( sizeof(float) * 3 * numVertices );

	// Read the vertices.
	for ( int v = 0; v < numVertices; v++ )
	{
		float x, y, z;

		if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

		if ( sscanf( lineBuf, "%f %f %f", &x, &y, &z ) != 3 )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

		vertexTable[ 3 * v + 0 ] = x;
		vertexTable[ 3 * v + 1 ] = y;
		vertexTable[ 3 * v + 2 ] = z;
	}


//=== MATERIALS ===

	int numMaterials = 0;
	float *reflectivityTable = NULL;	// An array of RGB reflectivity values.
	float *emissionTable = NULL;		// An array of RGB emission values.

	// Read number of materials.
	if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

	if ( sscanf( lineBuf, "%d", &numMaterials ) != 1  ||  numMaterials < 0 )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

	reflectivityTable = (float *) CheckedMalloc( sizeof(float) * 3 * numMaterials );
	emissionTable = (float *) CheckedMalloc( sizeof(float) * 3 * numMaterials );

	// Read the materials.
	for ( int m = 0; m < numMaterials; m++ )
	{
		float r, g, b;

		if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

		if ( sscanf( lineBuf, "%f %f %f", &r, &g, &b ) != 3 )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

		reflectivityTable[ 3 * m + 0 ] = r;
		reflectivityTable[ 3 * m + 1 ] = g;
		reflectivityTable[ 3 * m + 2 ] = b;

		if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

		if ( sscanf( lineBuf, "%f %f %f", &r, &g, &b ) != 3 )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

		emissionTable[ 3 * m + 0 ] = r;
		emissionTable[ 3 * m + 1 ] = g;
		emissionTable[ 3 * m + 2 ] = b;
	}


//=== SURFACES ===

	int numSurfaces = 0;
	QM_Surface *surfaceTable = NULL;	// An array of surfaces.

	// Read number of surfaces.
	if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

	if ( sscanf( lineBuf, "%d", &numSurfaces ) != 1  ||  numSurfaces < 0 )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

	surfaceTable = (QM_Surface *) CheckedMalloc( sizeof(QM_Surface) * numSurfaces );

	// Read the surfaces.
	for ( int s = 0; s < numSurfaces; s++ )
	{
		QM_SurfaceInit( &surfaceTable[s] );

		int matID, numQuads;

		// Read material index.
		if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

		if ( sscanf( lineBuf, "%d", &matID ) != 1  ||  matID < 0 || matID >= numMaterials )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

		CopyArray3( surfaceTable[s].reflectivity, &reflectivityTable[3*matID] );
		CopyArray3( surfaceTable[s].emission, &emissionTable[3*matID] );

		// Read number of quadrilaterals in the surface.
		if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

		if ( sscanf( lineBuf, "%d", &numQuads ) != 1  ||  numQuads < 0 )
			ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

		surfaceTable[s].numOrigQuads = numQuads;
		surfaceTable[s].origQuads = (QM_OrigQuad *) CheckedMalloc( sizeof(QM_OrigQuad) * numQuads );

		// Read vertex indices for each quadrilateral.
		for ( int q = 0; q < numQuads; q++ )
		{
			int vertID[4];

			if ( !ReadDataLine( lineBuf, &lineNum, filename, fp ) )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badEOF, filename );

			if ( sscanf( lineBuf, "%d %d %d %d", &vertID[0], &vertID[1], &vertID[2], &vertID[3] ) != 4 || 
				 vertID[0] < 0 || vertID[0] >= numVertices || vertID[1] < 0 || vertID[1] >= numVertices ||
				 vertID[2] < 0 || vertID[2] >= numVertices || vertID[3] < 0 || vertID[3] >= numVertices )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\" at line %d", badFile, filename, lineNum );

			CopyArray3( surfaceTable[s].origQuads[q].v[0], &vertexTable[ 3*vertID[0] ] );
			CopyArray3( surfaceTable[s].origQuads[q].v[1], &vertexTable[ 3*vertID[1] ] );
			CopyArray3( surfaceTable[s].origQuads[q].v[2], &vertexTable[ 3*vertID[2] ] );
			CopyArray3( surfaceTable[s].origQuads[q].v[3], &vertexTable[ 3*vertID[3] ] );

			// Compute normal vector to the quadrilateral.
			VecTriNormal( surfaceTable[s].origQuads[q].normal, 
				          surfaceTable[s].origQuads[q].v[0], 
						  surfaceTable[s].origQuads[q].v[1], 
						  surfaceTable[s].origQuads[q].v[2] );
			VecNormalize( surfaceTable[s].origQuads[q].normal, surfaceTable[s].origQuads[q].normal );
		}
	}

	QM_Model model = QM_ModelInit();
	model.numSurfaces = numSurfaces;
	model.surfaces = surfaceTable;
	ComputeBoundingBox( &model );

	fclose( fp );
	free( vertexTable );
	free( reflectivityTable );
	free( emissionTable );
	return model;
}






static void QuadCentroid( float centroid[3], const float v[4][3] )
	// Compute the centroid of the input 4 vertices.
{
	for ( int i = 0; i < 3; i++ )
		centroid[i] = ( v[0][i] + v[1][i] + v[2][i] + v[3][i] ) / 4.0f;
}


static float QuadArea( const float v[4][3] )
{
	float normal1[3], normal2[3];
	VecTriNormal( normal1, v[0], v[1], v[2] );
	VecTriNormal( normal2, v[0], v[2], v[3] );
	return 0.5f * ( VecLen( normal1 ) + VecLen( normal2 ) );
}


static inline void LineInterpolate( float vo[3], float k, const float v1[3], const float v2[3] )
    // Computes the point vo = (1-k)*v1 + k*v2. 
{
	float m = 1.0f - k;
    vo[0] = m * v1[0] + k * v2[0];
    vo[1] = m * v1[1] + k * v2[1];
    vo[2] = m * v1[2] + k * v2[2];
}


static inline void QuadBilinearInterpolate( float vo[3], float x, float y, float v[4][3] )
	// Bilinearly interpolate the vertices of the input quad to get a point.
	// The edge v[0]v[1] is considered the x axis, and the edge v[0]v[3] is the y axis.
	// 0 <= x, y <= 1.
{
	float v01[3], v32[3];
	LineInterpolate( v01, x, v[0], v[1] );
	LineInterpolate( v32, x, v[3], v[2] );
	LineInterpolate( vo, y, v01, v32 );
}



void QM_Subdivide( QM_Model *m, float maxShooterQuadEdgeLength, float maxGathererQuadEdgeLength )
	// Subdivide the original quads in the model to smaller
	// shooter quads and even-smaller gatherer quads.
	// Each shooter quad cannot have edge longer than maxShooterQuadEdgeLength, and
	// each gatherer quad cannot have edge longer than maxGathererQuadEdgeLength.
{
	if ( m == NULL || m->numSurfaces <= 0 ) return;

// Subdivide original quads to get shooter quads.

	int modelTotalShooters = 0;		// This will contain the total number of shooter quads in model.

	for ( int s = 0; s < m->numSurfaces; s++ )
	{
		QM_Surface *surface = &(m->surfaces[s]);

		// Find longest edge length of all original quads in the current surface.
		float maxEdgeLen = 0.0f;
		for ( int q = 0; q < surface->numOrigQuads; q++ )
		{
			QM_OrigQuad *origQuad = &(surface->origQuads[q]);
			float edgeLen;
			edgeLen = VecDist( origQuad->v[0], origQuad->v[1] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( origQuad->v[1], origQuad->v[2] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( origQuad->v[2], origQuad->v[3] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( origQuad->v[3], origQuad->v[0] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
		}

		// Compute how many regular segments to divide the longest edge into so that 
		// every resulting segment is not longer than maxShooterQuadEdgeLength.
		int numSegments = (int) ceil( maxEdgeLen / maxShooterQuadEdgeLength );

		// Each original quad on the current surface is going to be subdivided into
		// (numSegments*numSegments) shooter quads.
		// Therefore, the total number of shooter quads for this surface is...
		surface->numShooterQuads = surface->numOrigQuads * numSegments * numSegments;

		surface->shooters = (QM_ShooterQuad *) CheckedMalloc( sizeof(QM_ShooterQuad) * surface->numShooterQuads );
		int surfShootersCount = 0;  // This will contain the number of shooters in this surface.

		for ( int q = 0; q < surface->numOrigQuads; q++ )
		{
			QM_OrigQuad *origQuad = &(surface->origQuads[q]);

			for ( int y = 0; y < numSegments; y++ )
				for ( int x = 0; x < numSegments; x++ )
				{
					float newv[4][3];
					QuadBilinearInterpolate( newv[0], (float) x / numSegments, (float) y / numSegments, origQuad->v );
					QuadBilinearInterpolate( newv[1], (float) (x+1) / numSegments, (float) y / numSegments, origQuad->v );
					QuadBilinearInterpolate( newv[2], (float) (x+1) / numSegments, (float) (y+1) / numSegments, origQuad->v );
					QuadBilinearInterpolate( newv[3], (float) x / numSegments, (float) (y+1) / numSegments, origQuad->v );

					QM_ShooterQuad *shooterQuad = &(surface->shooters[ surfShootersCount ]);
					for ( int i = 0; i < 4; i++ ) CopyArray3( shooterQuad->v[i], newv[i] );
					QuadCentroid( shooterQuad->centroid, shooterQuad->v );
					CopyArray3( shooterQuad->normal, origQuad->normal );
					shooterQuad->area = QuadArea( shooterQuad->v );

					// Initialize the unshot power of the shooter quad.
					shooterQuad->unshotPower[0] = surface->emission[0] * shooterQuad->area;
					shooterQuad->unshotPower[1] = surface->emission[1] * shooterQuad->area;
					shooterQuad->unshotPower[2] = surface->emission[2] * shooterQuad->area;

					shooterQuad->surface = surface;
					surfShootersCount++;
					modelTotalShooters++;
				}
		}
	}


	// Build an array of pointers to all the shooters in the model.
	m->totalShooters = modelTotalShooters;
	m->shooters = (QM_ShooterQuad **) CheckedMalloc( sizeof(QM_ShooterQuad *) * modelTotalShooters );
	int modelTotalShootersCount = 0;

	for ( int s = 0; s < m->numSurfaces; s++ )
		for ( int q = 0; q < m->surfaces[s].numShooterQuads; q++ )
		{
			m->shooters[ modelTotalShootersCount ] = &(m->surfaces[s].shooters[q]);
			modelTotalShootersCount++;
		}



// Subdivide shooter quads to get gatherer quads.

	int modelTotalGatherers = 0;		// This will contain the total number of gatherers quads in model.

	for ( int s = 0; s < m->numSurfaces; s++ )
	{
		QM_Surface *surface = &(m->surfaces[s]);

		// Find longest edge length of all shooter quads in the current surface.
		float maxEdgeLen = 0.0f;
		for ( int q = 0; q < surface->numShooterQuads; q++ )
		{
			QM_ShooterQuad *shooterQuad = &(surface->shooters[q]);
			float edgeLen;
			edgeLen = VecDist( shooterQuad->v[0], shooterQuad->v[1] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( shooterQuad->v[1], shooterQuad->v[2] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( shooterQuad->v[2], shooterQuad->v[3] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
			edgeLen = VecDist( shooterQuad->v[3], shooterQuad->v[0] );
			if ( edgeLen > maxEdgeLen ) maxEdgeLen = edgeLen;
		}

		// Compute how many regular segments to divide the longest edge into so that 
		// every resulting segment is not longer than maxGathererQuadEdgeLength.
		int numSegments = (int) ceil( maxEdgeLen / maxGathererQuadEdgeLength );

		// Each shooter quad on the current surface is going to be subdivided into
		// (numSegments*numSegments) gatherer quads.
		// Therefore, the total number of gatherer quads for this surface is...
		surface->numGathererQuads = surface->numShooterQuads * numSegments * numSegments;

		surface->gatherers = (QM_GathererQuad *) CheckedMalloc( sizeof(QM_GathererQuad) * surface->numGathererQuads );
		int surfGatherersCount = 0;  // This will contain the number of gatherers in this surface.

		for ( int q = 0; q < surface->numShooterQuads; q++ )
		{
			QM_ShooterQuad *shooterQuad = &(surface->shooters[q]);

			for ( int y = 0; y < numSegments; y++ )
				for ( int x = 0; x < numSegments; x++ )
				{
					float newv[4][3];
					QuadBilinearInterpolate( newv[0], (float) x / numSegments, (float) y / numSegments, shooterQuad->v );
					QuadBilinearInterpolate( newv[1], (float) (x+1) / numSegments, (float) y / numSegments, shooterQuad->v );
					QuadBilinearInterpolate( newv[2], (float) (x+1) / numSegments, (float) (y+1) / numSegments, shooterQuad->v );
					QuadBilinearInterpolate( newv[3], (float) x / numSegments, (float) (y+1) / numSegments, shooterQuad->v );

					QM_GathererQuad *gathererQuad = &(surface->gatherers[ surfGatherersCount ]);
					for ( int i = 0; i < 4; i++ ) CopyArray3( gathererQuad->v[i], newv[i] );
					CopyArray3( gathererQuad->normal, shooterQuad->normal );
					gathererQuad->area = QuadArea( gathererQuad->v );

					// Initialize the radiosity of the gatherer quad.
					gathererQuad->radiosity[0] = surface->emission[0];
					gathererQuad->radiosity[1] = surface->emission[1];
					gathererQuad->radiosity[2] = surface->emission[2];

					CopyArray3( gathererQuad->vRadiosity[0], ZERO_VEC_3F );
					CopyArray3( gathererQuad->vRadiosity[1], ZERO_VEC_3F );
					CopyArray3( gathererQuad->vRadiosity[2], ZERO_VEC_3F );
					CopyArray3( gathererQuad->vRadiosity[3], ZERO_VEC_3F );

					gathererQuad->shooter = shooterQuad;
					gathererQuad->surface = surface;
					surfGatherersCount++;
					modelTotalGatherers++;
				}
		}
	}


	// Build an array of pointers to all the gatherers in the model.
	m->totalGatherers = modelTotalGatherers;
	m->gatherers = (QM_GathererQuad **) CheckedMalloc( sizeof(QM_GathererQuad *) * modelTotalGatherers );
	int modelTotalGatherersCount = 0;

	for ( int s = 0; s < m->numSurfaces; s++ )
		for ( int q = 0; q < m->surfaces[s].numGathererQuads; q++ )
		{
			m->gatherers[ modelTotalGatherersCount ] = &(m->surfaces[s].gatherers[q]);
			modelTotalGatherersCount++;
		}

	return;
}





void QM_ComputeVertexRadiosities( QM_Model *m )
	// Compute the radiosities at the vertices by averaging 
	// the radiosities of the quads that use the vertex.
{
	if ( m == NULL || m->numSurfaces <= 0 ) return;

	for ( int s = 0; s < m->numSurfaces; s++ )
	{
		QM_Surface *surface = &(m->surfaces[s]);

		for ( int g = 0; g < surface->numGathererQuads; g++ )
		{
			QM_GathererQuad *gatherer = &(surface->gatherers[g]);

			for ( int i = 0; i < 4; i++ )
			{
				int numQuadsUsingVertex = 0;
				CopyArray3( gatherer->vRadiosity[i], ZERO_VEC_3F );

				for ( int g2 = 0; g2 < surface->numGathererQuads; g2++ )
				{
					QM_GathererQuad *gatherer2 = &(surface->gatherers[g2]);

					for ( int i2 = 0; i2 < 4; i2++ )
					{
						float sqrDist = VecSqrDist( gatherer->v[i], gatherer2->v[i2] );
						if ( sqrDist <= EQUAL_VERTEX_THRESHOLD )
						{
							gatherer->vRadiosity[i][0] += gatherer2->radiosity[0];
							gatherer->vRadiosity[i][1] += gatherer2->radiosity[1];
							gatherer->vRadiosity[i][2] += gatherer2->radiosity[2];
							numQuadsUsingVertex++;
						}
					}
				}

				gatherer->vRadiosity[i][0] /= numQuadsUsingVertex;
				gatherer->vRadiosity[i][1] /= numQuadsUsingVertex;
				gatherer->vRadiosity[i][2] /= numQuadsUsingVertex;
			}
		}
	}
}



void QM_WriteGatherersToFile( const char *filename, const QM_Model *m )
	// Write the gatherer quads and their vertex radiosity values to a file.
{
	char badWrite[] = "Error writing to file";

	if ( m == NULL || m->totalGatherers <= 0 ) return;

	// Open output file.
	FILE *fp = fopen( filename, "w" );
	if ( fp == NULL ) 
		ShowFatalError( __FILE__, __LINE__, "Cannot open file \"%s\" for output", filename );

	// Write the number of gatherer quads.
	if ( fprintf( fp, "%d\n", m->totalGatherers ) < 0 )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badWrite, filename );

	for ( int q = 0; q < m->totalGatherers; q++ )
	{
		QM_GathererQuad *gatherer = m->gatherers[q];

		// For each quad, write each 3D vertex and its vertex radiosity.
		for ( int i = 0; i < 4; i++ )
		{
			if ( fprintf( fp, "%.6g %.6g %.6g\n", gatherer->v[i][0], gatherer->v[i][1], gatherer->v[i][2] ) < 0 )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badWrite, filename );

			// Write its RGB radiosity values.
			if ( fprintf( fp, "%.3f %.3f %.3f\n", gatherer->vRadiosity[i][0], gatherer->vRadiosity[i][1], gatherer->vRadiosity[i][2] ) < 0 )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badWrite, filename );
		}
	}

	fclose( fp );
}

