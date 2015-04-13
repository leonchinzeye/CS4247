//============================================================
// STUDENT NAME: Chin Ze-Ye Leon
// MATRIC NO.  : A0097304E
// NUS EMAIL   : leonchin@u.nus.edu
// COMMENTS TO GRADER:
// <comments to grader, if any>
//
// ============================================================
//
// FILE: Main.cpp



#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "Util.h"
#include "Vector3d.h"
#include "Color.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Surface.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Scene.h"
#include "Raytrace.h"

using namespace std;


// Constants for Scene 1.
static const int imageWidth1 = 640;
static const int imageHeight1 = 480;
static const int reflectLevels1 = 2;  // 0 -- object does not reflect scene.
static const int hasShadow1 = true;

// Constants for Scene 2.
static const int imageWidth2 = 640;
static const int imageHeight2 = 480;
static const int reflectLevels2 = 2;  // 0 -- object does not reflect scene.
static const int hasShadow2 = true;




///////////////////////////////////////////////////////////////////////////
// Raytrace the whole image of the scene and write it to a file.
///////////////////////////////////////////////////////////////////////////

void RenderImage( const char *imageFilename, const Scene &scene, int reflectLevels, bool hasShadow )
{
	int imgWidth = scene.camera.getImageWidth();
	int imgHeight = scene.camera.getImageHeight();

	Image image( imgWidth, imgHeight );	// To store the result of ray tracing.

	double startTime = Util::GetCurrRealTime();
	double startCPUTime = Util::GetCurrCPUTime();

	// Generate image.
	for ( int y = 0; y < imgHeight; y++ )
	{
		double pixelPosY = y + 0.5;

		for ( int x = 0; x < imgWidth; x++ )
		{
			double pixelPosX = x + 0.5;
			Ray ray = scene.camera.getRay( pixelPosX, pixelPosY );
			Color pixelColor = Raytrace::TraceRay( ray, scene, reflectLevels, hasShadow );
			pixelColor.clamp();
			image.setPixel( x, y, pixelColor );
		}
		// printf( "%d ", y );
	}

	double stopCPUTime = Util::GetCurrCPUTime();
	double stopTime = Util::GetCurrRealTime();
	printf( "CPU time taken = %.1f sec\n", stopTime - startTime ); 
	printf( "Real time taken = %.1f sec\n", stopTime - startTime ); 

	// Write image to file.
	image.writeToFile( imageFilename );
}




// Forward declarations. These functions are defined later in the file.

void DefineScene1( Scene &scene, int imageWidth, int imageHeight );
void DefineScene2( Scene &scene, int imageWidth, int imageHeight );




void WaitForEnterKeyBeforeExit( void )
{
    fflush( stdin );
    getchar();
}




int main()
{
	atexit( WaitForEnterKeyBeforeExit );



// Define Scene 1.

	Scene scene1;
	DefineScene1( scene1, imageWidth1, imageHeight1 );

// Render Scene 1.

	printf( "Render Scene 1...\n" );
	RenderImage( "out1.png", scene1, reflectLevels1, hasShadow1 );
	printf( "Image completed.\n" );



// Define Scene 2.

	Scene scene2;
	DefineScene2( scene2, imageWidth2, imageHeight2 );

// Render Scene 2.

	printf( "Render Scene 2...\n" );
	RenderImage( "out2.png", scene2, reflectLevels2, hasShadow2 );
	printf( "Image completed.\n" );


	printf( "All done.\n" );
	return 0;
}





///////////////////////////////////////////////////////////////////////////
// Modeling of Scene 1.
///////////////////////////////////////////////////////////////////////////

void DefineScene1( Scene &scene, int imageWidth, int imageHeight )
{
	scene.backgroundColor = Color( 0.2f, 0.3f, 0.5f );

	scene.amLight.I_a = Color( 1.0f, 1.0f, 1.0f ) * 0.25f;

// Define materials.

	scene.numMaterials = 5;
	scene.material = new Material[ scene.numMaterials ];

	// Light red.
	scene.material[0].k_d = Color( 0.8f, 0.4f, 0.4f );
	scene.material[0].k_a = scene.material[0].k_d;
	scene.material[0].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[0].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[0].n = 64.0f;

	// Light green.
	scene.material[1].k_d = Color( 0.4f, 0.8f, 0.4f );
	scene.material[1].k_a = scene.material[0].k_d;
	scene.material[1].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[1].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[1].n = 64.0f;

	// Light blue.
	scene.material[2].k_d = Color( 0.4f, 0.4f, 0.8f ) * 0.9f;
	scene.material[2].k_a = scene.material[0].k_d;
	scene.material[2].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[2].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 2.5f;
	scene.material[2].n = 64.0f;

	// Yellow.
	scene.material[3].k_d = Color( 0.6f, 0.6f, 0.2f );
	scene.material[3].k_a = scene.material[0].k_d;
	scene.material[3].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[3].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[3].n = 64.0f;

	// Gray.
	scene.material[4].k_d = Color( 0.6f, 0.6f, 0.6f );
	scene.material[4].k_a = scene.material[0].k_d;
	scene.material[4].k_r = Color( 0.6f, 0.6f, 0.6f );
	scene.material[4].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[4].n = 128.0f;


// Define point light sources.

	scene.numPtLights = 2;
	scene.ptLight = new PointLightSource[ scene.numPtLights ];

	scene.ptLight[0].I_source = Color( 1.0f, 1.0f, 1.0f ) * 0.6f;
	scene.ptLight[0].position = Vector3d( 100.0, 120.0, 10.0 );

	scene.ptLight[1].I_source = Color( 1.0f, 1.0f, 1.0f ) * 0.6f;
	scene.ptLight[1].position = Vector3d( 5.0, 80.0, 60.0 );


// Define surface primitives.

	scene.numSurfaces = 15;
	scene.surfacep = new SurfacePtr[ scene.numSurfaces ];

	scene.surfacep[0] = new Plane( 0.0, 1.0, 0.0, 0.0, &(scene.material[2]) ); // Horizontal plane.
	scene.surfacep[1] = new Plane( 1.0, 0.0, 0.0, 0.0, &(scene.material[4]) ); // Left vertical plane.
	scene.surfacep[2] = new Plane( 0.0, 0.0, 1.0, 0.0, &(scene.material[4]) ); // Right vertical plane.
	scene.surfacep[3] = new Sphere( Vector3d( 40.0, 20.0, 42.0 ), 22.0, &(scene.material[0]) ); // Big sphere.
	scene.surfacep[4] = new Sphere( Vector3d( 75.0, 10.0, 40.0 ), 12.0, &(scene.material[1]) ); // Small sphere.

	// Cube +y face.
	scene.surfacep[5] = new Triangle( Vector3d( 50.0, 20.0, 90.0 ), Vector3d( 50.0, 20.0, 70.0 ),
		                              Vector3d( 30.0, 20.0, 70.0 ), &(scene.material[3]) );
	scene.surfacep[6] = new Triangle( Vector3d( 50.0, 20.0, 90.0 ), Vector3d( 30.0, 20.0, 70.0 ),
		                              Vector3d( 30.0, 20.0, 90.0 ), &(scene.material[3]) );

	// Cube +x face.
	scene.surfacep[7] = new Triangle( Vector3d( 50.0, 0.0, 70.0 ), Vector3d( 50.0, 20.0, 70.0 ),
		                              Vector3d( 50.0, 20.0, 90.0 ), &(scene.material[3]) );
	scene.surfacep[8] = new Triangle( Vector3d( 50.0, 0.0, 70.0 ), Vector3d( 50.0, 20.0, 90.0 ),
		                              Vector3d( 50.0, 0.0, 90.0 ), &(scene.material[3]) );

	// Cube -x face.
	scene.surfacep[9] = new Triangle( Vector3d( 30.0, 0.0, 90.0 ), Vector3d( 30.0, 20.0, 90.0 ),
		                              Vector3d( 30.0, 20.0, 70.0 ), &(scene.material[3]) );
	scene.surfacep[10] = new Triangle( Vector3d( 30.0, 0.0, 90.0 ), Vector3d( 30.0, 20.0, 70.0 ),
		                              Vector3d( 30.0, 0.0, 70.0 ), &(scene.material[3]) );

	// Cube +z face.
	scene.surfacep[11] = new Triangle( Vector3d( 50.0, 0.0, 90.0 ), Vector3d( 50.0, 20.0, 90.0 ),
		                              Vector3d( 30.0, 20.0, 90.0 ), &(scene.material[3]) );
	scene.surfacep[12] = new Triangle( Vector3d( 50.0, 0.0, 90.0 ), Vector3d( 30.0, 20.0, 90.0 ),
		                              Vector3d( 30.0, 0.0, 90.0 ), &(scene.material[3]) );

	// Cube -z face.
	scene.surfacep[13] = new Triangle( Vector3d( 30.0, 0.0, 70.0 ), Vector3d( 30.0, 20.0, 70.0 ),
		                              Vector3d( 50.0, 20.0, 70.0 ), &(scene.material[3]) );
	scene.surfacep[14] = new Triangle( Vector3d( 30.0, 0.0, 70.0 ), Vector3d( 50.0, 20.0, 70.0 ),
		                              Vector3d( 50.0, 0.0, 70.0 ), &(scene.material[3]) );


// Define camera.

	scene.camera = Camera( Vector3d( 150.0, 120.0, 150.0 ), Vector3d( 45.0, 22.0, 55.0 ), Vector3d( 0.0, 1.0, 0.0 ),
				   (-1.0 * imageWidth) / imageHeight, (1.0 * imageWidth) / imageHeight, -1.0, 1.0, 3.0, 
				   imageWidth, imageHeight );
}





///////////////////////////////////////////////////////////////////////////
// Modeling of Scene 2.
///////////////////////////////////////////////////////////////////////////

void DefineScene2( Scene &scene, int imageWidth, int imageHeight )
{
    //***********************************************
    //*********** WRITE YOUR CODE HERE **************
    //***********************************************

	scene.backgroundColor = Color( 0.2f, 0.3f, 0.5f );

	scene.amLight.I_a = Color( 1.0f, 1.0f, 1.0f ) * 0.25f;

	// Define materials.

	scene.numMaterials = 9;
	scene.material = new Material[ scene.numMaterials ];

	// Light red.
	scene.material[0].k_d = Color( 0.8f, 0.4f, 0.4f );
	scene.material[0].k_a = scene.material[0].k_d;
	scene.material[0].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[0].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[0].n = 64.0f;

	// Light green.
	scene.material[1].k_d = Color( 0.4f, 0.8f, 0.4f );
	scene.material[1].k_a = scene.material[0].k_d;
	scene.material[1].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[1].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[1].n = 64.0f;

	// Light blue.
	scene.material[2].k_d = Color( 0.4f, 0.4f, 0.8f ) * 0.9f;
	scene.material[2].k_a = scene.material[0].k_d;
	scene.material[2].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[2].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 2.5f;
	scene.material[2].n = 64.0f;

	// Yellow.
	scene.material[3].k_d = Color( 0.6f, 0.6f, 0.2f );
	scene.material[3].k_a = scene.material[0].k_d;
	scene.material[3].k_r = Color( 0.8f, 0.8f, 0.8f ) / 1.5f;
	scene.material[3].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[3].n = 64.0f;

	// Gray.
	scene.material[4].k_d = Color( 0.6f, 0.6f, 0.6f );
	scene.material[4].k_a = scene.material[0].k_d;
	scene.material[4].k_r = Color( 0.6f, 0.6f, 0.6f );
	scene.material[4].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[4].n = 128.0f;

	// Dark red
	scene.material[5].k_d = Color( 1.0f, 0.0f, 0.0f );
	scene.material[5].k_a = scene.material[0].k_d;
	scene.material[5].k_r = Color( 0.6f, 0.6f, 0.6f );
	scene.material[5].k_rg = Color( 0.8f, 0.8f, 0.8f ) / 3.0f;
	scene.material[5].n = 90.0f;

	// Black 

	scene.material[6].k_d = Color(0.0f, 0.0f, 0.0f);
	scene.material[6].k_a = Color(0.0f, 0.0f, 0.0f);
	scene.material[6].k_r = Color(0.6f, 0.6f, 0.6f);
	scene.material[6].k_rg = Color(0.8f, 0.8f, 0.8f) / 3.0f;
	scene.material[6].n = 64.0f;

	// White

	scene.material[7].k_d = Color(1.0f, 1.0f, 1.0f);
	scene.material[7].k_a = Color(1.0f, 1.0f, 1.0f);
	scene.material[7].k_r = Color(0.6f, 0.6f, 0.6f);
	scene.material[7].k_rg = Color(0.8f, 0.8f, 0.8f) / 3.0f;
	scene.material[7].n = 64.0f;

	//silver-white for floor
	scene.material[8].k_d = Color(0.1f, 0.1f, 0.1f);
	scene.material[8].k_a = Color(1.0f, 1.0f, 1.0f);
	scene.material[8].k_r = Color(0.6f, 0.6f, 0.6f);
	scene.material[8].k_rg = Color(0.8f, 0.8f, 0.8f) / 3.0f;
	scene.material[8].n = 2.0f;


// Define point light sources.

	scene.numPtLights = 2;
	scene.ptLight = new PointLightSource[ scene.numPtLights ];

	scene.ptLight[0].I_source = Color( 1.0f, 1.0f, 1.0f ) * 0.6f;
	scene.ptLight[0].position = Vector3d( 200.0, 240.0, 20.0 );

	scene.ptLight[1].I_source = Color( 1.0f, 1.0f, 1.0f ) * 0.6f;
	scene.ptLight[1].position = Vector3d( 10.0, 160.0, 120.0 );
	

// Define surface primitives.

	scene.numSurfaces = 262;
	scene.surfacep = new SurfacePtr[ scene.numSurfaces ];

	int counter = 0;

	// 10 * 6 * 4 = 240 surfaces here
	// drawing the checkered floor
	for(int i = 0; i < 10; i++) {
		double xtranslation = 50 * i;

		int matTop, matBtm;

		if(i % 2 == 0) {
			matTop = 8;
			matBtm = 6;
		} else {
			matTop = 6;
			matBtm = 8;
		}

		for(int j = 0; j < 6; j++) {
			double ztranslation = 100 * j;

			scene.surfacep[counter++] = new Triangle( Vector3d( 0.0 + xtranslation, 0.0, 50.0 + ztranslation ), Vector3d( 0.0 + xtranslation, 0.0, 0.0 + ztranslation ),
										  Vector3d( 50.0 + xtranslation, 0.0, 0.0 + ztranslation ), &(scene.material[matTop]) );
			scene.surfacep[counter++] = new Triangle( Vector3d( 0.0 + xtranslation, 0.0, 50.0 + ztranslation ), Vector3d( 50.0 + xtranslation, 0.0, 50.0 + ztranslation),
										  Vector3d( 50.0 + xtranslation, 0.0, 0.0 + ztranslation ), &(scene.material[matTop]) );

			scene.surfacep[counter++] = new Triangle( Vector3d( 0.0 + xtranslation, 0.0, 100.0 + ztranslation ), Vector3d( 0.0 + xtranslation, 0.0, 50.0 + ztranslation ),
										  Vector3d( 50.0 + xtranslation, 0.0, 50.0 + ztranslation ), &(scene.material[matBtm]) );
			scene.surfacep[counter++] = new Triangle( Vector3d( 0.0 + xtranslation, 0.0, 100.0 + ztranslation ), Vector3d( 50.0 + xtranslation, 0.0, 100.0 + ztranslation),
										  Vector3d( 50.0 + xtranslation, 0.0, 50.0 + ztranslation ), &(scene.material[matBtm]) );
		}
	}
	
	// 2 surfaces 
	// for the vertical planes
	scene.surfacep[counter++] = new Plane( 1.0, 0.0, 0.0, 0.0, &(scene.material[7]) ); // Left vertical plane.
	scene.surfacep[counter++] = new Plane( 0.0, 0.0, 1.0, 0.0, &(scene.material[7]) ); // Right vertical plane.

	//sphere drawing
	double radius = 12.0;
	double sphereXtranslate = sqrt(pow(2 * radius, 2) - pow(radius, 2));

	// 10 surfaces
	// first layer
	for(int i = 0; i < 4; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(75.0, 10.0, 40.0 + sphereZtranslate), radius, &(scene.material[1]));
	}
	for(int i = 0; i < 3; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(75.0 + sphereXtranslate, 10.0, 52.0 + sphereZtranslate), radius, &(scene.material[1]));
	}
	for(int i = 0; i < 2; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(75.0 + 2 * sphereXtranslate, 10.0, 64.0 + sphereZtranslate), radius, &(scene.material[1]));
	}
	scene.surfacep[counter++] = new Sphere( Vector3d(75.0 + 3 * sphereXtranslate, 10.0, 76.0), radius, &(scene.material[1]));


	// 6 surfaces
	// 2nd layer
	double newCenterX = (75 * 3 + sphereXtranslate) / 3;
	double newCenterZ = (40.0 + 64.0 + 52.0) / 3;

	for(int i = 0; i < 3; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX, 28.0, newCenterZ + sphereZtranslate), radius, &(scene.material[2]));
	}
	newCenterZ = (52.0 + 64.0 + 76.0) / 3; 
	for(int i = 0; i < 2; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX + sphereXtranslate, 28.0, newCenterZ + sphereZtranslate), radius, &(scene.material[2]));
	}
	newCenterZ = 76.0;
	scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX + 2 * sphereXtranslate, 28.0, newCenterZ), radius, &(scene.material[2]));

	
	// 3 surfaces
	// 3rd layer
	double newCenterX2 = (newCenterX * 3 + sphereXtranslate) / 3;
	double newCenterZ2 = 64.0;

	for(int i = 0; i < 2; i++) {
		double sphereZtranslate = radius * i * 2;
		scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX2, 46.0, newCenterZ2 + sphereZtranslate), radius, &(scene.material[5]));
	}

	double newCenterX2_2 = newCenterX2 + sphereXtranslate;
	double newCenterZ2_2 = 76.0;
	scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX2_2, 46.0, newCenterZ2_2), radius, &(scene.material[5]));

	// 1 surface
	//4th layer
	double newCenterX3 = (newCenterX2 * 2 + newCenterX2_2) / 3;
	double newCenterZ3 = 76.0;
	scene.surfacep[counter++] = new Sphere( Vector3d(newCenterX3, 64.0, newCenterZ3), radius, &(scene.material[3]));

	//total surfaces = 240 + 2 + 10 + 6 + 3 + 1 = 262

// Define camera.

	scene.camera = Camera( Vector3d( 300.0, 240.0, 300.0 ), Vector3d( 45.0, 24.0, 55.0 ), Vector3d( 0.0, 1.0, 0.0 ),
				   (-1.0 * imageWidth) / imageHeight, (1.0 * imageWidth) / imageHeight, -1.0, 1.0, 3.0, 
				   imageWidth, imageHeight );
}
