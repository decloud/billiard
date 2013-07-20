/*
* An OpenGL implementation of billiards.
*
* The following are the dimensions of the table and balls:
*	Table length: 2.7 meters (9 foot)
*	Table width: 1.35 meters
*	Ball diameter: 0.05715 meters (American-style)
*	Ball radius: 0.028575 meters
*
* The following physics are used in the simulation:
*	displacement = initial_displacement + velocity * time
*	velocity = initial_velocity + acceleration * time
*
* The weight of the balls and the coefficient of friction are not taken into
* account. They might be incorporated in the future.
*
* Author: Qian Yu
*/

#include <stdio.h>
#include "Billiard.h"

#include <GL/glew.h>
#if defined(_WIN32)
#   include <GL/wglew.h>
#endif

#if defined(__APPLE__) || defined(MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

#define WINDOW_WIDTH 980
#define WINDOW_HEIGHT 500

int main( int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Billiard");
	setupRenderingContext();
	setupGame();

	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//glutTimerFunc(1000/FPS, update, 0); TODO: tweak

	glutMainLoop();
}