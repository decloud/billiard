/*
* An OpenGL implementation of billiards.
*
* Author: Qian Yu
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

#define WINDOW_WIDTH 980
#define WINDOW_HEIGHT 500
#define TABLE_WIDTH 960
#define TABLE_HEIGHT 480
#define BORDER 10
#define FPS 60
#define PI 3.141592653589793238462

GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLuint cueBallList, tableList;
GLfloat tableZ = 0.30;

const float DEG2RAD = 3.14159/180;
 
void drawCircle(float radius)
{
   glBegin(GL_LINE_LOOP);
 
   for (int i=0; i<360; i++)
   {
      float degInRad = i*DEG2RAD;
      glVertex2f(cos(degInRad)*radius, sin(degInRad)*radius);
   }
 
   glEnd();
}

/*
* Draw a green table and the pockets.
*/
void drawTable(void)
{
	glPushMatrix();
	{
		glTranslatef(BORDER, BORDER, 0);
		glCallList(tableList);
	}
	glPopMatrix();
}

/*
* Draw the balls.
*/
void drawBalls(void)
{
	glPushMatrix();
	{
		glTranslatef(BORDER + 100, BORDER + 100, 0);
		glColor3f(1,1,1);
		drawCircle(20);
	}
	glPopMatrix();
}

/*
* Set up the lights.
*/
void initLights(void)
{
	// Lights & Materials
	GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat position[] = {0.0, 0.0, 2.0, 1.0};
	GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {50.0};
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

/*
* Set up the rendering context.
*/
void setupRenderingContext(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// set up 2D projection
	glOrtho(0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	//initLights();

	// GLfloat white[] = {1, 1, 1, 1};
	// GLfloat green[] = {0, 1, 0, 1};
	// GLfloat red[] = {1, 0, 0, 1};
	// GLfloat blue[] = {0, 0, 1, 1};
	// GLfloat yellow[] = {1, 1, 0, 1};
	// GLfloat pink[] = {1, 0, 1, 1};
	// GLfloat lightBlue[] = {0, 1, 1, 1};

	tableList = glGenLists(1);
	glNewList(tableList, GL_COMPILE);
		glColor3f(0, 1, 1);
		glRectf(0, 0, TABLE_WIDTH, TABLE_HEIGHT);
	glEndList();

}

/*
* Draws the table and the balls
*/
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// reset modelview matrix, might not be necessary
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();
	{
		drawTable();
		drawBalls();
	}
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

/*
* Calculate and update the parameters of the balls.
*/
void update(void)
{
	//TODO: call glutPostRedisplay() if there is update
	//glutPostRedisplay();
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

void specialKeys(int key, int x, int y)
{

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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Billiard");
	setupRenderingContext();

	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//glutTimerFunc(1000/FPS, runMainLoop, 0);
	
	glutMainLoop();
}