#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
#define TABLE_LENGTH 960
#define TABLE_WIDTH 480
#define BORDER 10
#define FPS 60
#define PI 3.141592653589793238462
#define T_LENGTH 2.7
#define B_RADIUS 0.028575
#define NUM_OF_BALLS 16

const float T_WIDTH = T_LENGTH/2;
const float DEG2RAD = PI/180;
const float TIME_PASSED = 1.0/60;
const float METER2COORD = TABLE_LENGTH/T_LENGTH;
const float MAX_FORCE = 3.0; //TODO: tweak
const float BALL_RADIUS = B_RADIUS * METER2COORD;
const float ROOT_THREE = sqrt(3);

GLfloat white[] = {1, 1, 1, 1};
GLfloat green[] = {0, 1, 0, 1};
GLfloat red[] = {1, 0, 0, 1};
GLfloat blue[] = {0, 0, 1, 1};
GLfloat yellow[] = {1, 1, 0, 1};
GLfloat pink[] = {1, 0, 1, 1};
GLfloat lightBlue[] = {0, 1, 1, 1};

//GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLuint cueBallList, tableList;
GLfloat tableZ = 0.30;

float cueBallPower = 0.0f;
int cueBallAngle = 90;

Table* table;

// static array to hold all 15 balls
// balls[0] is cue ball
Ball* balls[NUM_OF_BALLS];

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
	//NOTE: draw only 4 balls right now
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		{
			glTranslatef(BORDER + balls[i]->x * METER2COORD,
						BORDER + balls[i]->y * METER2COORD, 0);
			glColor4fv(white);
			drawCircle(BALL_RADIUS);
		}
		glPopMatrix();
	}
}

/*
* Detects whether or not the ball is going to collide with the boundaries
* of the table. If it does, return the new directions.
*	returns "true" if the ball will collide and direction is changed
*	returns "false" otherwise
*/
bool collideWith(Ball *ball, Table *table)
{
	bool collided = false;

	if (ball->x - 0.028575 < 0 || ball->x + 0.028575 > 2.7) {
		ball->vx = -1 * ball->vx;
		return true;
	}

	if(ball->y - 0.028575 < 0 || ball->y + 0.028575 > 1.35) {
		ball->vy = -1 * ball->vy;
		return true;
	}
	return collided;
}

/*
* Detects whether or not ball1 is going to collide with ball2.
* If it does, calulate the new speed and directions of both balls.
*	returns "true" if the balls will collide
*	returns "false" otherwise
*/
bool collideWith(Ball *ball1, Ball *ball2)
{
	// special case where the ball is compared with itself
	if (ball1->id == ball2->id)
		return false;

	float distance = pow(ball1->x - ball2->x, 2) + pow(ball1->y - ball2->y, 2);

	if (distance > pow(2 * B_RADIUS, 2))
		return false;

	/* The physics of ball collision is based off the diagram on
	http://www.real-world-physics-problems.com/physics-of-billiards.html*/

	float V1A[2] = {ball1->vx, ball1->vy};

	// First, calculate the vector V2B and normalize it
	float V2B[2] = {V2B[0] = ball2->x - ball1->x, V2B[1] = ball2->y - ball1->y};
	float V2Blength = sqrt(V2B[0] * V2B[0] + V2B[1] * V2B[1]);
	V2B[0] = V2B[0] / V2Blength;
	V2B[1] = V2B[1] / V2Blength;

	// Then compute the vector V2A and normalize it
	// The x component is equal to the cos of V1A and V2B
	// The y component is equal to the sin of V1A and V2B
	float V2A[2] = {V1A[0] * V2B[0] - V1A[1] * V2B[1],
					V1A[0] * V2B[1] + V1A[1] * V2B[0]};

	// Now compute the magnitue of V2A and V2B
	// The speed can be computed using vector addition
	// The magnitude of V2A is V1A * sin
	// The magnitude of V2B is V1A * cos
	float V2Aspeed = ball1->speed * V2A[1];
	float V2Bspeed = ball1->speed * V2A[0];

	// Update both ball1 and ball2
	ball1->setVelocity(V2A[0], V2A[1], V2Aspeed);
	ball2->addVelocity(V2B[0], V2B[1], V2Bspeed);

	// NOTE: There might be an issue where I am adding the components to the second vector without calculating collision with the first. Will investagate later

	return true;
}



/*void collideWithBall(Ball &b1, Ball &b2)
{
	float V1A[2] = {b1.vx, b1.vy};

	// First, calculate the vector V2B and normalize it
	float V2B[2] = {V2B[0] = b2.x - b1.x, V2B[1] = b2.y - b1.y};
	float V2Blength = sqrt(V2B[0] * V2B[0] + V2B[1] * V2B[1]);
	V2B[0] = V2B[0] / V2Blength;
	V2B[1] = V2B[1] / V2Blength;

	// Then compute the vector V2A and normalize it
	// The x component is equal to the cos of V1A and V2B
	// The y component is equal to the sin of V1A and V2B
	float V2A[2] = {V1A[0] * V2B[0] - V1A[1] * V2B[1],
					V1A[0] * V2B[1] + V1A[1] * V2B[0]};

	// Now compute the magnitue of V2A and V2B
	// The speed can be computed using vector addition
	// The magnitude of V2A is V1A * sin
	// The magnitude of V2B is V1A * cos
	float V2Aspeed = b1.speed * V2A[1];
	float V2Bspeed = b1.speed * V2A[0];

	// Update all the values
	b1.vx = V2A[0];
	b1.vy = V2A[1];
	b1.speed = V2Aspeed;

	b2.vx = V2B[0];
	b2.vy = V2B[1];
	b2.speed = V2Bspeed;
	//NOTE: the second vector is currently not correctly updated
}*/

/*
* Perform collision detection by checking
*	collision with table
*	TODO: collision with pockets
*	collision with another ball
* in that order.
*/
bool collisionDetection(Ball *ball)
{
	if (collideWith(ball, table))
		return true;

	for (int i = 0; i < 16; i++)
	{
		if (collideWith(ball, balls[i]))
			return true;
	}
	return false;
}

/*
* Set up the game and initialize values:
*	coordinates of the balls
*	velocities of the balls
*	speed of the balls
*/
void setupGame()
{
	table = new Table(2.7);
	for (int i = 0; i < 16; i++)
	{
		balls[i] = new Ball(B_RADIUS, i);
	}

	/* The balls are set up as follows
						0





						1
					2		3
				4		5		6
			7		8		9		10
		11 		12 		13 		14 		15

	Note that the the radius of any three touching ball forms an
	equalaterial triangle.
	*/

	balls[0]->setLocation(T_LENGTH/4, T_LENGTH/4, 0.0f); // cue ball
	balls[1]->setLocation(T_LENGTH*3/4, T_LENGTH/4, 0.0f);
	balls[2]->setLocation(T_LENGTH*3/4 + ROOT_THREE * B_RADIUS,
							T_LENGTH/4, 0.0f);
	balls[3]->setLocation(T_LENGTH*3/4 + ROOT_THREE * B_RADIUS,
							T_LENGTH/4 - B_RADIUS, 0.0f);

	//TODO: add the rest of the coordinates
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

	tableList = glGenLists(1);
	glNewList(tableList, GL_COMPILE);
		glColor3f(0, 1, 1);
		glRectf(0, 0, TABLE_LENGTH, TABLE_WIDTH);
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
	//Assume that the program calls update every 1/60 second
	//NOTE: draw only 4 balls right now
	for (int i = 0; i < 4; i++)
	{
		// only check for balls that are moving
		if (balls[i]->speed <= 0.0f)
			continue;

		// update the current speed and location of the ball
		balls[i]->x = balls[i]->x +
						balls[i]->vx * balls[i]->speed * TIME_PASSED;

		balls[i]->y = balls[i]->y +
						balls[i]->vy * balls[i]->speed * TIME_PASSED;

		balls[i]->speed = balls[i]->speed - 1 * TIME_PASSED; //TODO: tweak

		// perform collision detection and if true, call glutpostredisplay
		collisionDetection(balls[i]);

		if (balls[i]->speed <= 0)
			balls[i]->resetVelocity();
		else
			glutPostRedisplay();
	}
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
			// only works when the cue ball has stopped
			if (cueBallPower > 0.0) {
				/* Assume angle starts at the top of the y-axis
				and goes clockwise around the quadrant.The angles
				are decomposed into a pair of unit vectors that
				points to the direction and are assigned to the
				cue ball.*/
				balls[0]->vx = sin(cueBallAngle * PI / 180);
				balls[0]->vy = cos(cueBallAngle * PI / 180);
				balls[0]->speed = cueBallPower * MAX_FORCE;

				// reset the power ONLY
				cueBallPower = 0;

				//DEBUG: Printing out parameters of the cue ball
				printf("vx: %f vy: %f speed: %f\n",
						balls[0]->vx, balls[0]->vy, balls[0]->vy);

				glutPostRedisplay();
			}
			break;
	}
	//DEBUG: Print out the key that is pressed
	//printf("the key is: %c\n", key);
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
	printf("power: %.1f angle: %d\n", cueBallPower, cueBallAngle);
}

/*
* Handles mouse inputs.
*/
void mouse(int button, int state, int x, int y)
{

}

/*
* Handles mouse movement while it is clicked.
*/
void motion(int x, int y)
{

}