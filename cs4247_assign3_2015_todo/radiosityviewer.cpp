#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/glext.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include "common.h"
#include "trackball.h"


/////////////////////////////////////////////////////////////////////////////
// CONSTANTS THAT YOU CHANGE FOR DIFFERENT INPUT FILE
/////////////////////////////////////////////////////////////////////////////

// Input model filename.
static const char radiosityModelFilename[] = "myscene.out";


/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
/////////////////////////////////////////////////////////////////////////////

typedef struct RAD_Quad {
	float v[4][3];		// 3D coordinates of the 4 vertices of the quadrilateral.
	float rgb[4][3];	// The colors at the vertices.
}
RAD_Quad;

typedef struct RAD_Model {
	int numQuads;				// Number of quads.
	RAD_Quad *quads;			// Array of RAD_Quad.

	// Color stats. For tone mapping.
	float maxIntensity;
	float minIntensity;
	float max_rgb[3];

	// Axis-aligned bounding box (AABB).
	float min_xyz[3];		// Corner of bounding box with minimum x, y, z.
	float max_xyz[3];		// Corner of bounding box with maximum x, y, z.
	float dim_xyz[3];		// Dimensions of bounding box in x, y, z.
	float center[3];		// Center of bounding box.
	float radius;			// Radius of the bounding sphere enclosing the AABB.
}
RAD_Model;


/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

// The model with radiosity solution.
static RAD_Model model;

// OpenGL display lists.
static GLuint gathererQuadsDList = 0;
static GLuint gathererQuadsNoColorDList = 0;

// Window's size.
static int winWidth = 800;     // Window width in pixels.
static int winHeight = 600;    // Window height in pixels.

// Others.
static bool drawAxes = false;		// Draw world coordinate frame axes iff true.
static bool backfaceCulling = true; // Enable or disable back-face culling.

static int drawStyle = 0;			// Draw polygons in different drawing styles: 
									// 0: filled, 1: wireframe, 2: outlined fill.

// Trackball object.
TrackBall tb( GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON );



/////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS.
/////////////////////////////////////////////////////////////////////////////

static void ComputeBoundingBox( RAD_Model *m )
	// Compute an axis-aligned bounding box (AABB).
{
	if ( m == NULL || m->numQuads <= 0 ) return;

	m->min_xyz[0] = m->min_xyz[1] = m->min_xyz[2] = FLT_MAX;
	m->max_xyz[0] = m->max_xyz[1] = m->max_xyz[2] = -FLT_MAX;

	for ( int q = 0; q < m->numQuads; q++ )
	{
		RAD_Quad *quad = &(m->quads[q]);
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


static RAD_Model RAD_ReadFile( const char *filename )
	// Read radiosity solution model from input file.
	// The axis-aligned bounding box is computed.
{
	char badFile[] = "Invalid input model file";

	// Open input file
	FILE *fp = fopen( filename, "r" );
	if ( fp == NULL ) 
		ShowFatalError( __FILE__, __LINE__, "Cannot open input model file \"%s\"", filename );

	RAD_Model m;

	if ( fscanf( fp, "%d", &(m.numQuads) ) != 1 || m.numQuads < 0 )
		ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badFile, filename );

	m.quads = (RAD_Quad *) CheckedMalloc( sizeof(RAD_Quad) * m.numQuads );

	float minIntensity = FLT_MAX;
	float maxIntensity = 0.0f;
	float max_rgb[3] = { 0.0f, 0.0f, 0.0f };

	for ( int q = 0; q < m.numQuads; q++ )
	{
		for ( int i = 0; i < 4; i++ )
		{
			float vert[3], rgb[3];

			if ( fscanf( fp, "%f %f %f", &vert[0], &vert[1], &vert[2] ) != 3 )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badFile, filename );

			if ( fscanf( fp, "%f %f %f", &rgb[0], &rgb[1], &rgb[2] ) != 3 )
				ShowFatalError( __FILE__, __LINE__, "%s \"%s\"", badFile, filename );

			CopyArray3( m.quads[q].v[i], vert );
			CopyArray3( m.quads[q].rgb[i], rgb );

			float intensity = rgb[0] + rgb[1] + rgb[2];
			if ( intensity > maxIntensity ) maxIntensity = intensity;
			if ( intensity < minIntensity ) minIntensity = intensity;
			if ( rgb[0] > max_rgb[0] ) max_rgb[0] = rgb[0];
			if ( rgb[1] > max_rgb[1] ) max_rgb[1] = rgb[1];
			if ( rgb[2] > max_rgb[2] ) max_rgb[2] = rgb[2];
		}
	}

	fclose( fp );
	m.minIntensity = minIntensity;
	m.maxIntensity = maxIntensity;
	CopyArray3( m.max_rgb, max_rgb );
	ComputeBoundingBox( &m );
	return m;
}



/////////////////////////////////////////////////////////////////////////////
// Draw the x, y, z axes. Each is drawn with the input length.
// The x-axis is red, y-axis green, and z-axis blue.
/////////////////////////////////////////////////////////////////////////////

static void DrawAxes( double length )
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_LIGHTING );
    glLineWidth( 3.0 );
    glBegin( GL_LINES );
        // x-axis.
        glColor3f( 1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( length, 0.0, 0.0 );
        // y-axis.
        glColor3f( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( 0.0, length, 0.0 );
        // z-axis.
        glColor3f( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, length );
    glEnd();
    glPopAttrib();
}



#define DEPTH_OFFSET		(1.0/1024.0)

/////////////////////////////////////////////////////////////////////////////
// The display callback function.
/////////////////////////////////////////////////////////////////////////////

static void MyDisplay( void )
{
	static int starttime, stoptime;
	starttime = glutGet( GLUT_ELAPSED_TIME );

    glViewport( 0, 0, winWidth, winHeight );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, (double)winWidth/winHeight, model.radius, 10.0 * model.radius );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslated( 0.0, 0.0, -2.5 * model.radius );
	glPushMatrix();
		tb.applyTransform();
		glTranslatef( -model.center[0], -model.center[1], -model.center[2] );

		glDepthRange( DEPTH_OFFSET, 1.0 );  // This is for outlined fill.

		// Draw axes.
		if ( drawAxes ) DrawAxes( 2.0 * model.radius );

		if ( drawStyle == 0 || drawStyle == 2 ) 
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Filled or outlined fill.
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	// Wireframe.

		glCallList( gathererQuadsDList );	// Draw gatherer quads.

		if ( drawStyle == 2 )	// Draw the outlines of the outlined fill style.
		{
			glPushAttrib( GL_ALL_ATTRIB_BITS );
			glDisable( GL_LIGHTING );
			glDepthFunc( GL_LEQUAL );
			glDepthRange( 0.0, 1.0 - DEPTH_OFFSET );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	// Wireframe.
			glLineWidth( 1.0 );
			glColor3f( 0.0f, 0.0f, 0.0f );
			glCallList( gathererQuadsNoColorDList );	// Draw gatherer quads.
			glPopAttrib();
		}

	glPopMatrix();

    glutSwapBuffers();

	stoptime = glutGet( GLUT_ELAPSED_TIME );
	double fps = 1.0 / ((stoptime - starttime) / 1000.0);

	static char s[256];
	sprintf( s, "Radiosity Viewer  (%.1f FPS)", fps );
	glutSetWindowTitle( s );
}



/////////////////////////////////////////////////////////////////////////////
// The keyboard callback function.
/////////////////////////////////////////////////////////////////////////////

static void MyKeyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        // Quit program.
        case 'q':
        case 'Q': 
            exit(0);
            break;

        // Reset view.
        case 'r':
        case 'R': 
            tb.reset();
            glutPostRedisplay();
            break;

        // Toggle axes.
        case 'x':
        case 'X': 
            drawAxes = !drawAxes;
            glutPostRedisplay();
            break;

        // Toggle back-face culling.
        case 'c':
        case 'C': 
			backfaceCulling = !backfaceCulling;
			if ( backfaceCulling ) 
				glEnable( GL_CULL_FACE );
			else
				glDisable( GL_CULL_FACE );
			glutPostRedisplay();
            break;

		// Cycle thru different types of quads.
        case 's':
        case 'S': 
            drawStyle = ( drawStyle + 1 ) % 3;
            glutPostRedisplay();
            break;
    }
}



/////////////////////////////////////////////////////////////////////////////
// The reshape callback function.
/////////////////////////////////////////////////////////////////////////////

static void MyReshape( int w, int h )
{
	tb.reshape( w, h );
    winWidth = w;
    winHeight = h;
}


/////////////////////////////////////////////////////////////////////////////
// The mouse callback function.
/////////////////////////////////////////////////////////////////////////////

static void MyMouse( int button, int state, int x, int y )
{
	tb.mouse( button, state, x, y );
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////
// The mouse motion callback function.
/////////////////////////////////////////////////////////////////////////////

static void MyMotion( int x, int y )
{
	tb.motion(x, y);
	glutPostRedisplay();
} 


/////////////////////////////////////////////////////////////////////////////
// The init function. It initializes some OpenGL states.
/////////////////////////////////////////////////////////////////////////////

static void MyInit( void )
{
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // Set black background color.
    glEnable( GL_DEPTH_TEST ); // Use depth-buffer for hidden surface removal.
	glDisable( GL_DITHER );
	glDisable( GL_BLEND );
    glDisable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );

	if ( backfaceCulling ) 
		glEnable( GL_CULL_FACE );
	else
		glDisable( GL_CULL_FACE );

    //glEnable( GL_NORMALIZE );		// Let OpenGL automatically renomarlize all normal vectors.
	glEnable( GL_RESCALE_NORMAL );	// Use this only when scalings are all uniform.
}


/////////////////////////////////////////////////////////////////////////////
// Make a display list of the quads with vertex colors.
/////////////////////////////////////////////////////////////////////////////

static GLuint MakeGathererQuadsDisplayList( const RAD_Model *m )
{
	const float log2 = log( 2.0f );

	float maxColor = Max3( m->max_rgb[0], m->max_rgb[1], m->max_rgb[2] );
	float logMaxColor = log( maxColor + 1.0f );
	printf( "maxColor = %f\n", maxColor );

	GLuint dlist = glGenLists( 1 );
	if ( dlist == 0 ) ShowFatalError( __FILE__, __LINE__, "Cannot create display list" );
	glNewList( dlist, GL_COMPILE );
	glBegin( GL_QUADS );
		for ( int q = 0; q < m->numQuads; q++ )
		{
			RAD_Quad *quad = &(m->quads[q]);
			for ( int i = 0; i < 4; i++ )
			{
				float rgb[3];

				// Tone mapping.
				rgb[0] = pow( log( quad->rgb[i][0] + 1.0f ) / logMaxColor, 0.4f );
				rgb[1] = pow( log( quad->rgb[i][1] + 1.0f ) / logMaxColor, 0.4f );
				rgb[2] = pow( log( quad->rgb[i][2] + 1.0f ) / logMaxColor, 0.4f );

				glColor3fv( rgb );
				glVertex3fv( quad->v[i] );
			}
		}
	glEnd();
	glEndList();
	return dlist;
}


/////////////////////////////////////////////////////////////////////////////
// Make a display list of the quads without vertex colors.
/////////////////////////////////////////////////////////////////////////////

static GLuint MakeGathererQuadsNoColorDisplayList( const RAD_Model *m )
{
	GLuint dlist = glGenLists( 1 );
	if ( dlist == 0 ) ShowFatalError( __FILE__, __LINE__, "Cannot create display list" );
	glNewList( dlist, GL_COMPILE );
	glBegin( GL_QUADS );
		for ( int q = 0; q < m->numQuads; q++ )
		{
			RAD_Quad *quad = &(m->quads[q]);
			for ( int i = 0; i < 4; i++ )
			{
				glVertex3fv( quad->v[i] );
			}
		}
	glEnd();
	glEndList();
	return dlist;
}




/////////////////////////////////////////////////////////////////////////////
// The main function.
/////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{    
    glutInit( &argc, argv );
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( winWidth, winHeight );
    glutCreateWindow( "Radiosity Viewer" );

    MyInit();

	// Read model file.
	model = RAD_ReadFile( radiosityModelFilename );

	// Make OpenGL display lists.
	gathererQuadsDList = MakeGathererQuadsDisplayList( &model );
	gathererQuadsNoColorDList = MakeGathererQuadsNoColorDisplayList( &model );

    // Register the callback functions.
    glutDisplayFunc( MyDisplay ); 
    glutReshapeFunc( MyReshape );
    glutKeyboardFunc( MyKeyboard );
	glutMouseFunc( MyMouse );
	glutMotionFunc( MyMotion );

    // Display user instructions in console window.
    printf( "Press 'R' to reset view.\n" );
    printf( "Press 'X' to toggle axes.\n" );
	printf( "Press 'C' to toggle back-face culling.\n" );
    printf( "Press 'S' to cycle thru different drawing styles.\n" );
    printf( "Press 'Q' to quit.\n\n" );

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}
