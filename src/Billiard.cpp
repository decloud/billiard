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
#include <stdlib.h>
#include <math.h>
#include "Billiard.h"
#include "Vector.h"
#include <time.h>

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
#define FPS 25
#define PI 3.141592653589793238462
#define T_LENGTH 2.7
#define B_RADIUS 0.028575
#define NUM_OF_BALLS 16

const float T_WIDTH = T_LENGTH/2;
const float DEG2RAD = PI/180;
const float METER2COORD = TABLE_LENGTH/T_LENGTH;
const float MAX_FORCE = 100.0; //TODO: tweak
const float BALL_RADIUS = B_RADIUS * METER2COORD;
const float ROOT_THREE = sqrt(3);
const float FRAME_TIME = 1.0f / FPS;

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
	//NOTE: Place to draw balls
	for (int i = 0; i < 2; i++)
	{
		glPushMatrix();
		{
			glTranslatef(BORDER + balls[i]->position.x * METER2COORD,
						BORDER + balls[i]->position.y * METER2COORD, 0.0f);

			if (i == 0)
				glColor4fv(white);
			else
				glColor4fv(red);

			drawCircle(BALL_RADIUS);
		}
		glPopMatrix();
	}
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
	equalaterial triangle. The distance between each row is root_three * r.
	The distance between each column is 2*r.
	*/

	float x = T_LENGTH/4;
	float y = T_LENGTH/4;
	balls[0]->position.set(x, y, 0.0f); // cue ball

	x = x * 3;
	int counter = 0;
	for (int i = 1; i < 16; i++)
	{
		if (i == 2 || i == 4 || i == 7 || i == 11)
		{
			x = x + ROOT_THREE * B_RADIUS;
			y = y - B_RADIUS;
			counter = 0;
		}

		balls[i]->position.set(x, y + 2 * counter * B_RADIUS, 0.0f);
		counter++;
	}

	//DEBUG: create and place a custom ball for ball[1]
	balls[1]->position.set(T_LENGTH/4 + 4 * B_RADIUS,
						T_LENGTH/4 + B_RADIUS, 0.0f);

	//DEBUG: print out the position of all the balls
	for (int i = 0; i < 2; i++)
	{
		printf("Ball %d %f %f %f\n", i, balls[i]->position.x,
			balls[i]->position.y, balls[i]->position.z);
	}
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

float collisionPoint(Ball *ball1, Ball *ball2, float timePassed, float ballDistance, float collisionDistance)
{
	return timePassed;
}

void collide(Ball *ball1, Ball *ball2, float timePassed)
{
	Vector normalPlane = ball2->position - ball1->position;
	float ballDistance = normalPlane.length();

	float collisionDistance = ball1->radius + ball2->radius;

	if (ballDistance < collisionDistance)
	{
		float collisionTime = collisionPoint(ball1, ball2, timePassed,
			ballDistance, collisionDistance);

		normalPlane.normalize();

		Vector collisionPlane(-normalPlane.y, normalPlane.x, 0);

		float n_vel2 = Vector::dot(normalPlane, ball1->velocity);
		float c_vel1 = Vector::dot(collisionPlane, ball1->velocity);
		float n_vel1 = Vector::dot(normalPlane, ball2->velocity);
		float c_vel2 = Vector::dot(collisionPlane, ball2->velocity);

		Vector vel1 = (n_vel1 * normalPlane) + (c_vel1 * collisionPlane);
		Vector vel2 = (n_vel2 * normalPlane) + (c_vel2 * collisionPlane);

		ball1->position = ball1->position + (collisionTime * vel1);
		ball2->position = ball2->position + (collisionTime * vel2);

		ball1->velocity = vel1;
		ball2->velocity = vel2;
	}
}

void updatePhysics()
{
	printf("updatePhysics is called!\n");
	//TODO: update all the balls
	for (int i = 0; i < 2; i++)
	{
		// first, update the ball's position if it's moving
		if (balls[i]->velocity.length() >= 0.0f)
		{
			printf("Ball %d position before: ", i);
			balls[i]->position.print();

			balls[i]->position = balls[i]->position + (FRAME_TIME * balls[i]->velocity);

			printf("Ball %d position after: ", i);
			balls[i]->position.print();

		}

		// now check for collision with table

		// now check for collision with any other ball

		for (int j = i + 1; j < 2; j++)
		{
			collide(balls[i], balls[j], FRAME_TIME);
		}

		// now update velocity


		//printf("Ball %d ", i);
		//balls[i]->position.print();
		//balls[i]->velocity.print();
	}
}

clock_t startTime;
float accumulator = 0.0f;
//float alpha = 0.0f;

/*
* Update the parameters of the balls.
*/
void update()
{
	clock_t currentTime = clock();
	accumulator += 10 * (currentTime - startTime) / (double) CLOCKS_PER_SEC;
	startTime = currentTime;

	if (accumulator > 0.2f)
		accumulator = 0.2f;

	//printf("FRAME_TIME: %f\n", FRAME_TIME);
	//printf("accumulator: %f\n", accumulator);

	while (accumulator > FRAME_TIME)
	{
		updatePhysics();
		accumulator -= FRAME_TIME;
	}

	//alpha = accumulator / FRAME_TIME;

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
			// only works when the cue ball has stopped
			if (cueBallPower > 0.0) {
				/* Assume angle starts at the top of the y-axis
				and goes clockwise around the quadrant.The angles
				are decomposed into a pair of unit vectors that
				points to the direction and are assigned to the
				cue ball.*/

				float speed = cueBallPower * MAX_FORCE / METER2COORD;
				balls[0]->velocity.set(sin(cueBallAngle * DEG2RAD) * speed,
									cos(cueBallAngle * DEG2RAD) * speed,
									0.0f);

				cueBallPower = 0; // reset the power
				startTime = clock();

				//DEBUG: Printing out parameters of the cue ball
				printf("Cueball Velocity: x: %f y: %f z: %f\n",
										balls[0]->velocity.x,
										balls[0]->velocity.y,
										balls[0]->velocity.z);
				//printf("Start time: %ld\n", startTime);

				glutPostRedisplay();
			}
			break;
	}
	//DEBUG: Print out the key that is pressed
	//printf("Key pressed: %c\n", key);
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