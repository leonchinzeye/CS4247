//  6/18/2000: Rotation animation disabled by Kok-Lim Low.
//  6/17/2000: Modified by Kok-Lim Low to include panning and zooming.

#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

/* 
 *  Simple trackball-like motion adapted (ripped off) from projtex.c
 *  (written by David Yu and David Blythe).  See the SIGGRAPH '96
 *  Advanced OpenGL course notes.
 *
 *
 *  Usage:
 *
 *  o  create a TrackBall object, say called tb
 *  o  call tb.reset() to reset to the initial transformation
 *  o  call tb.reshape() from the reshape callback
 *  o  call tb.applyTransform() to apply the trackball transformation
 *  o  call tb.motion() from the motion callback
 *  o  call tb.mouse() from the mouse callback
 *
 *  Typical setup:


	#include <GL/glext.h>
    #include "trackball.h"


	TrackBall tb( GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON );


	void display( void )
    {
		. . .
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -viewer_distance_from_object_center);
		glPushMatrix();
			tb.applyTransform();
			glTranslatef( -obj_center_x, -obj_center_y, -obj_center_z );

			. . . draw the scene . . .

		glPopMatrix();
		. ..
    } 

    void reshape( int width, int height )
    {
		tb.reshape( width, height );
		. . .
    }

    void mouse( int button, int state, int x, int y )
    {
		tb.mouse( button, state, x, y );
		. . .
		glutPostRedisplay();
    }


    void motion( int x, int y )
    {
		tb.motion(x, y);
        . . .
		glutPostRedisplay();
    } 


    int main( int argc, char** argv )
    {
		. . .
		glutDisplayFunc( display );
		glutReshapeFunc( reshape );
		glutMouseFunc( mouse );
		glutMotionFunc( motion );
		. . .
		glEnable( GL_RESCALE_NORMAL );  // NOTE: This is needed for zooming.
		                                // Need to include glext.h.
    }
 *
 * */


#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/glext.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif


class TrackBall 
{
public:
	TrackBall( void );
	TrackBall( int rot_button, int pan_button, int zoom_button );
	void reset( void );
	void applyTransform( void );
	void reshape( int width, int height );
	void mouse( int button, int state, int x, int y );
	void motion( int x, int y );
	void load( const char *file );
	void save( const char *file );

private:
	void captureTransform( void );

	GLdouble tb_angle;
	GLdouble tb_axis[3];
	GLdouble tb_transform[4*4];

	GLuint tb_width;
	GLuint tb_height;

	GLdouble tb_pan_x;
	GLdouble tb_pan_y;

	GLdouble tb_zoom;
	GLdouble tb_zoom_inc;

	GLint tb_rot_button;
	GLint tb_pan_button;
	GLint tb_zoom_button;

	GLint tb_mouse_button;
	GLint tb_mouse_x;
	GLint tb_mouse_y;

	GLdouble tb_model_mat[4*4];
	GLdouble tb_proj_mat[4*4];
	GLint tb_viewport[4];
};


#endif
