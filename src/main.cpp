/* An OpenGL implementation of billiards.
*
* Author: Qian Yu
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#if defined(_WIN32)
#   include <GL/wglew.h>
#endif

#if defined(__APPLE__) || defined(MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define PI 3.141592653589793238462

/*
* Draws the table and the balls
*/
void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glPushMatrix();
	{
		//TODO: drawing the table and the ball goes here
	}
	glPopMatrix();
	
	glFlush();
	glutSwapBuffers();
}

/*
* Adjust the drawing canvas when the window size changes
*/
void reshape(int width, int height)
{

}

/*
* Handles basic input from the Keyboard. 
*/
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
		case 27: // Escape key
			exit(0);
			break;
	}
	//DEBUG:
	printf("the key is: %c\n", key);
}

/*
* Handles mouse inputs.
*/
void mouse( int button, int state, int x, int y)
{

}

/*
* Handles mouse movement while it is clicked.
*/
void motion(int x, int y)
{
	
}

int main( int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Billiard");
	//setupShaders();
	//setupRC();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(specialKeys)
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();
	//delete shaderProg;
}