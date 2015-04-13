#ifndef _QUADMODEL_H_
#define _QUADMODEL_H_

// The followings define the three quadrilateral types. 
// A quadrilateral may be the original input quadrilateral,
// a subdivided shooter quadrilateral, or a subdivided gatherer quadrilateral.
// Gatherer quadrilaterals are always subdivision of shooter quadrilaterals.

struct QM_Surface;	// Forward declaration.


typedef struct QM_OrigQuad {
	float v[4][3];			// 3D coordinates of the 4 vertices of the quadrilateral.
	float normal[3];		// Unit normal vector.
}
QM_OrigQuad;


typedef struct QM_ShooterQuad {
	float v[4][3];			// 3D coordinates of the 4 vertices of the quadrilateral.
	float centroid[3];		// Centroid of the 4 vertices. A hemicube is placed at here.
	float normal[3];		// Unit normal vector.
	float area;				// Surface area of quadrilateral.
	float unshotPower[3];	// Unshot RGB light power = unshot radiosity * quad area.
	QM_Surface *surface;	// Pointer to the surface which the quadrilateral belongs to.
}
QM_ShooterQuad;


typedef struct QM_GathererQuad {
	float v[4][3];			// 3D coordinates of the 4 vertices of the quadrilateral.
	float normal[3];		// Unit normal vector.
	float area;				// Surface area of quadrilateral.
	float radiosity[3];		// The patch radiosity.
	float vRadiosity[4][3]; // The radiosities at the vertices.
	QM_ShooterQuad *shooter;	// Pointer to its parent shooter quadrilateral.
	QM_Surface *surface;		// Pointer to the surface which the quadrilateral belongs to.
}
QM_GathererQuad;



typedef struct QM_Surface {
	float reflectivity[3];		// R -- the RGB reflectivity.
	float emission[3];			// E -- the emitted RGB power per unit area.

	int numOrigQuads;			// Number of original quadrilaterals on the surface.
	QM_OrigQuad *origQuads;		// Array of QM_OrigQuad.

	int numShooterQuads;		// Number of shooter quadrilaterals on the surface.
	QM_ShooterQuad *shooters;	// Array of QM_ShooterQuad.

	int numGathererQuads;		// Number of shooter quadrilaterals on the surface.
	QM_GathererQuad *gatherers;	// Array of QM_GathererQuad.
}
QM_Surface;



typedef struct QM_Model {
	int numSurfaces;				// Number of surfaces.
	QM_Surface *surfaces;			// Array of QM_Surface.

	int totalShooters;				// Total number of shooter quadrilaterals on all surfaces.
	QM_ShooterQuad **shooters;		// Array of pointers to all QM_ShooterQuad.
									// NOTE: Use this array to search the shooters for the
									// one that has the highest unshot power.

	int totalGatherers;				// Total number of gatherer quadrilaterals on all surfaces.
	QM_GathererQuad **gatherers;	// Array of pointers to all QM_GathererQuad.
									// NOTE: Use the index of this array as a unique ID for each gatherer.
									// When using Hemicube, use this unique ID to render gatherer quad in 
									// a unique RGB color. Each pixel's RGB color is converted back to the
									// unique ID, and use it to index this array to access the 
									// corresponding gatherer quad.

	// Axis-aligned bounding box (AABB).
	float min_xyz[3];		// Corner of bounding box with minimum x, y, z.
	float max_xyz[3];		// Corner of bounding box with maximum x, y, z.
	float dim_xyz[3];		// Dimensions of bounding box in x, y, z.
	float center[3];		// Center of bounding box.
	float radius;			// Radius of the bounding sphere enclosing the AABB.
}
QM_Model;



extern void QM_SurfaceInit( QM_Surface *s );
extern QM_Surface QM_SurfaceInit( void );
extern void QM_SurfaceCleanUp( QM_Surface *s );

extern void QM_ModelInit( QM_Model *m );
extern QM_Model QM_ModelInit( void );
extern void QM_ModelCleanUp( QM_Model *m );

extern QM_Model QM_ReadFile( const char *filename );
	// Read model from input file.
	// The output QM_Model has only QM_OrigQuad.
	// The axis-aligned bounding box is computed.

extern void QM_Subdivide( QM_Model *m, float maxShooterQuadEdgeLength, float maxGathererQuadEdgeLength );
	// Subdivide the original quads in the model to smaller
	// shooter quads and even-smaller gatherer quads.
	// Each shooter quad cannot have edge longer than maxShooterQuadEdgeLength, and
	// each gatherer quad cannot have edge longer than maxGathererQuadEdgeLength.

extern void QM_ComputeVertexRadiosities( QM_Model *m );
	// Compute the radiosities at the vertices by averaging 
	// the radiosities of the quads that use the vertex.

extern void QM_WriteGatherersToFile( const char *filename, const QM_Model *m );
	// Write the gatherer quads and their vertex radiosity values to a file.

#endif
