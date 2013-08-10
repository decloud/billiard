#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Billiard.h"
#include "Vector.h"
#include <time.h>

#define NUM_OF_BALLS 16
#define NUM_OF_POCKETS 6

const float converted_table_length = window_width - 2 * border;
const float converted_table_width = window_height - 2 * border;
const float meter_to_coord = converted_table_length/table_length;
const float converted_ball_radius = ball_radius * meter_to_coord;
const float converted_pocket_radius = pocket_radius * meter_to_coord;

const float MAX_FORCE = 200.0f; //TODO: tweak

const float frame_time = 1.0f / fps;

const float degree_to_radian = 3.14159265f/180.f;

GLfloat white[] = {1, 1, 1, 1};
GLfloat green[] = {0, 1, 0, 1};
GLfloat red[] = {1, 0, 0, 1};
GLfloat blue[] = {0, 0, 1, 1};
GLfloat yellow[] = {1, 1, 0, 1};
GLfloat pink[] = {1, 0, 1, 1};
GLfloat lightBlue[] = {0, 1, 1, 1};

float cueBallPower = 0.0f;
int cueBallAngle = 90;

Table *table;
Ball *balls[NUM_OF_BALLS];
Ball *pockets[NUM_OF_POCKETS];
bool ballVisible[NUM_OF_BALLS];

time_t startTime;
float accumulator = 0.0f;
//float alpha = 0.0f;

/*****************************************************************************
							Helper Functions
******************************************************************************/

/*
* Initialize the table object
*/
void setupTable(float length)
{
	table = new Table(length);
}

/*
* Initialize the balls and set their locations
*/
void setupBalls(float radius, int numOfBalls)
{
	/* The balls are set up as follows
						0





						1
					2		3
				4		5		6
			7		8		9		10
		11 		12 		13 		14 		15

	Note that the the radius of any three touching ball forms an
	equalaterial triangle. The distance between each row is root_three * r.
	The distance between each column is 2*r. A little extra room (0.0001) is
	added inbetween all of the balls. */

	for (int i = 0; i < numOfBalls; i++)
	{
		balls[i] = new Ball(radius, i);
		ballVisible[i] = true;
	}

	// add some extra room so the balls are not touching
	radius += 0.0005f;

	const float root_three = sqrt(3);

	float x = table->length/4;
	float y = table->length/4;
	balls[0]->position.set(x, y, 0.0f); // cue ball

	x = x * 3;
	int counter = 0;
	for (int i = 1; i < numOfBalls; i++)
	{
		if (i == 2 || i == 4 || i == 7 || i == 11)
		{
			x = x + root_three * radius;
			y = y - radius;
			counter = 0;
		}

		balls[i]->position.set(x, y + 2 * counter * radius, 0.0f);
		counter++;
	}
}

/*
* Initialize the pockets and set their locations
*/
void setupPockets(float radius, int numOfPockets)
{
	/* The pockets are set up as following
		P0------------------P1------------------P2
		|										|
		|										|
		|										|
		|										|
		|										|
		P3------------------P4------------------P5

		There are 4 corner pockets and 2 side pockets.
	*/

	for (int i = 0; i < numOfPockets; i++)
	{
		pockets[i] = new Ball(radius, i);
	}

	float x = 0.0f;
	float y = 0.0f;

	for (int i = 0; i < numOfPockets; i++)
	{
		if (i == 3)
		{
			x = 0.0f;
			y = table->width;
		}

		pockets[i]->position.set(x, y, 0.0f);
		x = x + table->width;
	}

	//DEBUG: printing out the locations of the pockets
	// for (int i = 0; i < numOfPockets; i++)
	// {
	// 	printf("Pocket %d ", i);
	// 	pockets[i]->position.print();
	// }
}

/*
* Helper function used to draw the balls in 2d
*/
void drawCircle(float radius)
{
   glBegin(GL_LINE_LOOP);

   for (int i=0; i<360; i++)
   {
      float degInRad = i*degree_to_radian;
      glVertex2f(cos(degInRad)*radius, sin(degInRad)*radius);
   }

   glEnd();
}

/*
* Draw a green table.
*/
void drawTable()
{
	glPushMatrix();
	{
		glTranslatef(border, border, 0);
		glColor3f(0, 1, 1);
		glRectf(0, 0, converted_table_length, converted_table_width);
	}
	glPopMatrix();
}

/*
* Draw the balls by looping through the global **balls.
*/
void drawBalls()
{
	for (int i = 0; i < NUM_OF_BALLS; i++)
	{
		if (!ballVisible[i])
		{
			continue;
		}

		//TODO: draw the balls with different colors
		glPushMatrix();
		{
			glTranslatef(border + balls[i]->position.x * meter_to_coord,
						border + balls[i]->position.y * meter_to_coord, 0.0f);

			if (i == 0)
				glColor4fv(white);
			else
				glColor4fv(red);

			drawCircle(converted_ball_radius);
		}
		glPopMatrix();
	}
}

/*
* Draw the pockets.
*/
void drawPockets()
{
	for (int i = 0; i < NUM_OF_POCKETS; i++)
	{
		glPushMatrix();
		{
			glTranslatef(border + pockets[i]->position.x * meter_to_coord,
						border + pockets[i]->position.y * meter_to_coord, 0.0f);

			glColor4fv(yellow);

			drawCircle(converted_pocket_radius);
		}
		glPopMatrix();
	}
}

/*
* Respond when the user presses the p key.
* Convert the angle and power into vectors and add to the cue ball.
*/
void powerKey()
{
	if (cueBallPower > 0.0)
	{
		/* Assume angle starts at the top of the y-axis
		and goes clockwise around the quadrant.The angles
		are decomposed into a pair of unit vectors that
		points to the direction and are assigned to the
		cue ball.*/

		float speed = cueBallPower * MAX_FORCE / meter_to_coord;
		balls[0]->velocity.set(sin(cueBallAngle * degree_to_radian) * speed,
							cos(cueBallAngle * degree_to_radian) * speed,
							0.0f);

		cueBallPower = 0; // reset the power
		startTime = time(NULL);

		//DEBUG: Printing out parameters of the cue ball
		printf("Cueball Velocity: x: %f y: %f z: %f\n",
								balls[0]->velocity.x,
								balls[0]->velocity.y,
								balls[0]->velocity.z);
		//printf("Start time: %ld\n", startTime);
		update();
		//glutPostRedisplay();
	}
}

/*
* Set up the rendering context.
*/
void setupRenderingContext()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set up 2D projection
	glOrtho(0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//initLights();
}

float collisionPoint(Ball *ball1, Ball *ball2, float frameTime,
							float distanceAtFrameEnd, float collisionDistance)
{
	Vector ball1FrameStartPosition = ball1->position - (frameTime * ball1->velocity);
	Vector ball2FrameStartPosition = ball2->position - (frameTime * ball2->velocity);

	float distanceAtFrameStart = (ball2FrameStartPosition  - ball1FrameStartPosition ).length();

	float collisionTime = frameTime * (distanceAtFrameStart - collisionDistance ) / (distanceAtFrameStart - distanceAtFrameEnd) ;

	ball1->position = ball1FrameStartPosition + (collisionTime * ball1->velocity);
	ball2->position = ball2FrameStartPosition + (collisionTime * ball2->velocity);

	return (frameTime - collisionTime);
}

void collide(Ball *ball1, Ball *ball2, float frameTime)
{
	Vector normalPlane = ball2->position - ball1->position;
	float distanceAtFrameEnd = normalPlane.length();

	float collisionDistance = ball1->radius + ball2->radius;

	if (distanceAtFrameEnd <= collisionDistance)
	{
		float collisionTime = collisionPoint(ball1, ball2, frameTime,
			distanceAtFrameEnd, collisionDistance);

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

bool collideWithPockets(Ball *ball)
{
	float x = ball->position.x;
	float y = ball->position.y;
	float radius = ball->radius;
	int id = ball->id;

	// P0------------------P1------------------P2
	// |										|
	// |										|
	// |										|
	// |										|
	// |										|
	// P3------------------P4------------------P5

	// check for collision with left side of table
	if (x - radius < 0)
	{
		ball->velocity.x = -1 * ball->velocity.x;

		if (id != 0)
		{
			// check for collision with the pockets on the left (0 and 3)
			if (y < (pockets[0]->position.y + pockets[0]->radius) ||
				y > pockets[3]->position.y - pockets[3]->radius)
			{
				ballVisible[id] = false;
				return true;
			}
		}
	}

	// check for collision with rightside of table
	if (x + radius > table->length)
	{
		ball->velocity.x = -1 * ball->velocity.x;

		if (id != 0)
		{
			// check for collision with the pockets on the left (2 and 5)
			if (y < (pockets[2]->position.y + pockets[2]->radius) ||
				y > pockets[5]->position.y - pockets[5]->radius)
			{
				ballVisible[id] = false;
				return true;
			}
		}
	}

	// check for collision with top of table
	if (y - radius < 0)
	{
		ball->velocity.y = -1 * ball->velocity.y;

		if (id != 0)
		{
			// check for collision with the pockets on the top (0, 1, and 2)
			if (x < (pockets[0]->position.x + pockets[0]->radius) ||
				x > pockets[2]->position.x - pockets[2]->radius  ||
				(x > pockets[1]->position.x - pockets[1]->radius &&
				x < pockets[1]->position.x + pockets[1]->radius )
				)
			{
				ballVisible[id] = false;
				return true;
			}
		}
	}

	// check for collision with bottom of table
	if (y + radius > table->width)
	{
		ball->velocity.y = -1 * ball->velocity.y;

		if (id != 0)
		{
			// check for collision with the pockets on the top (3, 4, and 5)
			if (x < (pockets[3]->position.x + pockets[3]->radius) ||
				x > pockets[5]->position.x - pockets[5]->radius  ||
				(x > pockets[4]->position.x - pockets[4]->radius &&
				x < pockets[4]->position.x + pockets[4]->radius )
				)
			{
				ballVisible[id] = false;
				return true;
			}
		}
	}

	return false;
}

/*
* Perform collision detecton and collision resolution for all the balls
* and also update their speed
*/
void updatePhysics(float timePassed)
{
	//printf("updatePhysics is called!\n");
	for (int i = 0; i < NUM_OF_BALLS; i++)
	{
		// only update the physics for balls that are visible
		if (!ballVisible[i])
		{
			continue;
		}

		// first, update the ball's position if it's moving
		if (balls[i]->velocity.length() > 0.0f)
		{
			balls[i]->position = balls[i]->position +
											(timePassed * balls[i]->velocity);
		}

		if (collideWithPockets(balls[i]))
		{
			printf("collided with pocket!\n");
			continue;
		}

		// now check for collision with any other ball
		for (int j = i + 1; j < NUM_OF_BALLS; j++)
		{
			collide(balls[i], balls[j], timePassed);
		}

		// now update velocity
		if (balls[i]->velocity.length() > 0.0f)
		{
			balls[i]->velocity = 0.99 * balls[i]->velocity;
			if (balls[i]->velocity.length() < 0.00001)
			{
				balls[i]->velocity.reset();
			}
		}
	}
}


/*****************************************************************************
							Public Functions
******************************************************************************/

/*
* Set up the game components.
*/
void setupGame()
{
	setupTable(table_length);
	setupBalls(ball_radius, NUM_OF_BALLS);
	setupPockets(pocket_radius, NUM_OF_POCKETS);
}

/*
* Set up the lights.
*/
void initLights()
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
* Draws the table and the balls
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// reset modelview matrix, might not be necessary
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		drawTable();
		drawPockets();
		drawBalls();
	}
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

/*
* Update the parameters of the balls.
*/
void update()
{
	// time_t currentTime = time(NULL);
	// accumulator += difftime(currentTime, startTime);
	// startTime = currentTime;

	// if (accumulator > 1.f)
	// 	accumulator = 1.f;

	// // printf("accumulator: %f\n", accumulator);

	// while (accumulator >= frame_time)
	// {
	// 	updatePhysics(frame_time);
	// 	accumulator -= frame_time;
	// }

	//alpha = accumulator / frame_time;

	updatePhysics(frame_time);
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
	switch(key)
	{
		case 27: // Escape key
			exit(0);
			break;
		case 112: // p key
			powerKey();
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