/*
* An OpenGL implementation of billiards.
*
* The following are the dimensions of the table and balls:
*	Table length: 2.7 meters (9 foot)
*	Table width: 1.35 meters
*	Ball diameter: 0.05715 meters (American-style)
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

const float DEG2RAD = 3.14159/180;
const float TIME_PASSED = 1.0/60;
const float METER2COORD = WINDOW_WIDTH/2.7; // double check this later
const float MAX_FORCE = 25; // maximum amount of force on the cue ball

GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLuint cueBallList, tableList;
GLfloat tableZ = 0.30;

float cueBallPower = 0.0f;
int cueBallAngle = 0;

// temporary variables for cue ball
float cbX = 100.0;
float cbY = 100.0;
float cbZ = 0.0f;

float cbDX = 0.0f;
float cbDY = 0.0f;
float cbDZ = 0.0f;

float cbSpeed = 0.0f;
// end of temporary variables

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
		glTranslatef(BORDER + cbX, BORDER + cbY, 0);
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
int i = 0;
void update(void)
{
	//DEBUG: Update is called
	printf("update is called! cbX: %f cbY: %f TIME_PASSED %f\n", cbX, cbY, TIME_PASSED);
	//NOTE: Might keep track of changes and only call glutPostRedisplay when necessary

	//Assume that the program calls update every 1/60 second

	cbX = cbX + cbDX * cbSpeed * TIME_PASSED;
	cbY = cbY + cbDY * cbSpeed * TIME_PASSED;

	glutPostRedisplay();
}

/*
* Adjust the drawing canvas when the window size changes
*/
void reshape(int width, int height)
{

}

/*
* Handles basic input from the Keyboard.
*	esc: quit the game
*	p: release the cue ball
*/
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
		case 27: // Escape key
			exit(0);
			break;
		case 112: // p key
			if (cueBallPower > 0.0) {

				/*
				* Assume angle starts at the top of the y-axis
				* and goes clockwise around the quadrant.
				* The angles are decomposed into a pair of unit
				* vectors that points to the direction.
				*/
				cbDX = sin(cueBallAngle * PI / 180);
				cbDY = cos(cueBallAngle * PI / 180);
				cbSpeed = cueBallPower * MAX_FORCE;

				// cueBall->dx = sin(cueBallAngle * PI / 180);
				// cueBall->dy = cos(cueBallAngle * PI / 180);
				// cueBall->speed = cueBallPower;

				// reset the power and angle
				cueBallPower = 0;
				cueBallAngle = 0;

				//DEBUG: Printing out parameters of the cue ball
				printf("cbDX: %f cbDY: %f cbSpeed: %f\n", cbDX, cbDY, cbSpeed);

				glutPostRedisplay();
			}
			break;
	}
	//DEBUG: Print out the key that is pressed
	printf("the key is: %c\n", key);
}

/*
* Handles special key inputs.
*	up arrow: increase cue ball power
*	down arrow: decrease cue ball power
*	left arrow: move angle to the left
*	right arrow: move angle to the right
*/
void specialKeys(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			cueBallPower += .1;
			if (cueBallPower > 1.0)
				cueBallPower = 1.f;
			break;
		case GLUT_KEY_DOWN:
			cueBallPower -= .1;
			if (cueBallPower < 0.0)
				cueBallPower = 0;
			break;
		case GLUT_KEY_LEFT:
			cueBallAngle -= 20;
			if (cueBallAngle < 0)
				cueBallAngle += 360;
			break;
		case GLUT_KEY_RIGHT:
			cueBallAngle += 20;
			if (cueBallAngle > 360)
				cueBallAngle -= 360;
			break;
	}

	//DEBUG: Print out cue ball power and angle
	printf("cueball power: %f\n", cueBallPower);
	printf("cueBall angle: %d\n", cueBallAngle);
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

	//glutTimerFunc(1000/FPS, update, 0);

	glutMainLoop();
}