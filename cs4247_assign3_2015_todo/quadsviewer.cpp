#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>
#include <GL/glext.h>

#include "common.h"
#include "quadmodel.h"
#include "trackball.h"


/////////////////////////////////////////////////////////////////////////////
// CONSTANTS THAT YOU CHANGE FOR DIFFERENT INPUT FILE
/////////////////////////////////////////////////////////////////////////////

// Input model filename.
static const char inputModelFilename[] = "cornell_box.in";
static const float maxShooterQuadEdgeLength = 70.0f;	
static const float maxGathererQuadEdgeLength = 30.0f;	


/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////

// Light 0.
static const GLfloat light0Ambient[] = { 0.1, 0.1, 0.1, 1.0 };
static const GLfloat light0Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
static const GLfloat light0Specular[] = { 0.9, 0.9, 0.9, 1.0 };
static const GLfloat light0Position[] = { 1.0, 1.0, 1.0, 0.0 };

// Light 1.
static const GLfloat light1Ambient[] = { 0.1, 0.1, 0.1, 1.0 };
static const GLfloat light1Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
static const GLfloat light1Specular[] = { 0.9, 0.9, 0.9, 1.0 };
static const GLfloat light1Position[] = { -1.0, 0.0, -0.5, 0.0 };


/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

static QM_Model model;

// OpenGL display lists.
static GLuint origQuadsDList = 0;
static GLuint shooterQuadsDList = 0;
static GLuint gathererQuadsDList = 0;

// Window's size.
static int winWidth = 800;     // Window width in pixels.
static int winHeight = 600;    // Window height in pixels.

// Others.
static bool drawAxes = true;		// Draw world coordinate frame axes iff true.
static bool backfaceCulling = true; // Enable or disable back-face culling.

static int drawStyle = 2;			// Draw polygons in different drawing styles: 
									// 0: filled, 1: wireframe, 2: outlined fill.

static int drawWhichQuads = 0;		// Draw which types of quads: 
									// 0: original quads, 1: shooter quads, 2: gatherer quads.

// Trackball object.
TrackBall tb( GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON );



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

		// Set world positions of the two lights.
		glLightfv( GL_LIGHT0, GL_POSITION, light0Position );
		glLightfv( GL_LIGHT1, GL_POSITION, light1Position );

		glDepthRange( DEPTH_OFFSET, 1.0 );  // This is for outlined fill.

		// Draw axes.
		if ( drawAxes ) DrawAxes( 2.0 * model.radius );

		if ( drawStyle == 0 || drawStyle == 2 ) 
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Filled or outlined fill.
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	// Wireframe.

		if ( drawWhichQuads == 0 )
			glCallList( origQuadsDList );		// Draw original model.
		else if ( drawWhichQuads == 1 )
			glCallList( shooterQuadsDList );	// Draw shooter quads.
		else if ( drawWhichQuads == 2 )
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

			if ( drawWhichQuads == 0 )
				glCallList( origQuadsDList );		// Draw original model.
			else if ( drawWhichQuads == 1 )
				glCallList( shooterQuadsDList );	// Draw shooter quads.
			else if ( drawWhichQuads == 2 )
				glCallList( gathererQuadsDList );	// Draw gatherer quads.

			glPopAttrib();
		}

	glPopMatrix();

    glutSwapBuffers();

	stoptime = glutGet( GLUT_ELAPSED_TIME );
	double fps = 1.0 / ((stoptime - starttime) / 1000.0);

	static char s[256];
	sprintf( s, "Quads Viewer  (%.1f FPS)", fps );
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

		// Cycle thru different types of quads.
        case 'm':
        case 'M': 
            drawWhichQuads = ( drawWhichQuads + 1 ) % 3;
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
    glShadeModel( GL_SMOOTH );

	if ( backfaceCulling ) 
		glEnable( GL_CULL_FACE );
	else
		glDisable( GL_CULL_FACE );

    // Set Light 0.
    glLightfv( GL_LIGHT0, GL_AMBIENT, light0Ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light0Diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light0Specular );
    glEnable( GL_LIGHT0 );

    // Set Light 1.
    glLightfv( GL_LIGHT1, GL_AMBIENT, light1Ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, light1Diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, light1Specular );
    glEnable( GL_LIGHT1 );

    glEnable( GL_LIGHTING );

    // Set some global light properties.
    GLfloat globalAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

    //glEnable( GL_NORMALIZE );		// Let OpenGL automatically renomarlize all normal vectors.
	glEnable( GL_RESCALE_NORMAL );	// Use this only when scalings are all uniform.
}



static GLuint MakeOrigQuadsDisplayList( const QM_Model *m )
{
	GLuint dlist = glGenLists( 1 );
	if ( dlist == 0 ) ShowFatalError( __FILE__, __LINE__, "Cannot create display list" );
	glNewList( dlist, GL_COMPILE );

		for ( int s = 0; s < m->numSurfaces; s++ )
		{
			float am[4], di[4], sp[4], em[4], shininess = 32.0;
			CopyArray3( am, m->surfaces[s].reflectivity ); am[3] = 1.0f;
			CopyArray3( di, m->surfaces[s].reflectivity ); di[3] = 1.0f;
			CopyArray3( em, m->surfaces[s].emission ); em[3] = 1.0f;
			sp[0] = sp[1] = sp[2] = sp[3] = 0.5f;

			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, am );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, di );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, sp );
			glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, em );
			glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

			glBegin( GL_QUADS );
				for ( int q = 0; q < m->surfaces[s].numOrigQuads; q++ )
				{
					QM_OrigQuad *quad = &(m->surfaces[s].origQuads[q]);

					glNormal3fv( quad->normal );
					glVertex3fv( quad->v[0] );
					glVertex3fv( quad->v[1] );
					glVertex3fv( quad->v[2] );
					glVertex3fv( quad->v[3] );
				}
			glEnd();
		}

	glEndList();
	return dlist;
}


static GLuint MakeShooterQuadsDisplayList( const QM_Model *m )
{
	GLuint dlist = glGenLists( 1 );
	if ( dlist == 0 ) ShowFatalError( __FILE__, __LINE__, "Cannot create display list" );
	glNewList( dlist, GL_COMPILE );

		for ( int s = 0; s < m->numSurfaces; s++ )
		{
			float am[4], di[4], sp[4], em[4], shininess = 32.0;
			CopyArray3( am, m->surfaces[s].reflectivity ); am[3] = 1.0f;
			CopyArray3( di, m->surfaces[s].reflectivity ); di[3] = 1.0f;
			CopyArray3( em, m->surfaces[s].emission ); em[3] = 1.0f;
			sp[0] = sp[1] = sp[2] = sp[3] = 0.5f;

			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, am );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, di );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, sp );
			glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, em );
			glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

			glBegin( GL_QUADS );
				for ( int q = 0; q < m->surfaces[s].numShooterQuads; q++ )
				{
					QM_ShooterQuad *quad = &(m->surfaces[s].shooters[q]);

					glNormal3fv( quad->normal );
					glVertex3fv( quad->v[0] );
					glVertex3fv( quad->v[1] );
					glVertex3fv( quad->v[2] );
					glVertex3fv( quad->v[3] );
				}
			glEnd();
		}

	glEndList();
	return dlist;
}


static GLuint MakeGathererQuadsDisplayList( const QM_Model *m )
{
	GLuint dlist = glGenLists( 1 );
	if ( dlist == 0 ) ShowFatalError( __FILE__, __LINE__, "Cannot create display list" );
	glNewList( dlist, GL_COMPILE );

		for ( int s = 0; s < m->numSurfaces; s++ )
		{
			float am[4], di[4], sp[4], em[4], shininess = 32.0;
			CopyArray3( am, m->surfaces[s].reflectivity ); am[3] = 1.0f;
			CopyArray3( di, m->surfaces[s].reflectivity ); di[3] = 1.0f;
			CopyArray3( em, m->surfaces[s].emission ); em[3] = 1.0f;
			sp[0] = sp[1] = sp[2] = sp[3] = 0.5f;

			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, am );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, di );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, sp );
			glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, em );
			glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

			glBegin( GL_QUADS );
				for ( int q = 0; q < m->surfaces[s].numGathererQuads; q++ )
				{
					QM_GathererQuad *quad = &(m->surfaces[s].gatherers[q]);

					glNormal3fv( quad->normal );
					glVertex3fv( quad->v[0] );
					glVertex3fv( quad->v[1] );
					glVertex3fv( quad->v[2] );
					glVertex3fv( quad->v[3] );
				}
			glEnd();
		}

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
    glutCreateWindow( "Quads Viewer" );

    MyInit();

	// Read model file and subdivide the quads.
	model = QM_ReadFile( inputModelFilename );
	QM_Subdivide( &model, maxShooterQuadEdgeLength, maxGathererQuadEdgeLength );

	// Make OpenGL display lists.
	origQuadsDList = MakeOrigQuadsDisplayList( &model );
	shooterQuadsDList = MakeShooterQuadsDisplayList( &model );
	gathererQuadsDList = MakeGathererQuadsDisplayList( &model );

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
	printf( "Press 'M' to cycle thru different types of quads.\n" );
    printf( "Press 'Q' to quit.\n\n" );

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}
