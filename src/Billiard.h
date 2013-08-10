/*
* An OpenGL implementation of billiards.
*
* The following are the dimensions of the table and balls:
*	Table length: 2.7 meters (9 foot)
*	Table width: 1.35 meters
*	Ball diameter: 0.05715 meters (American-style)
*	Ball radius: 0.028575 meters
*	Pocket radius: 0.055 meters
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

#ifndef BILLIARD_H
#define BILLIARD_H

#include "Ball.h"
#include "Table.h"

#include <GL/glew.h>
#if defined(_WIN32)
#   include <GL/wglew.h>
#endif

#if defined(__APPLE__) || defined(MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

const int border = 40;
const int window_width = 980;
const int window_height = (window_width / 2) + border; // 500

const int fps = 25;

const float table_length = 2.7f;
const float ball_radius = 0.028575f;
const float pocket_radius = 0.055f;

void setupGame();
void initLights(void);
void setupRenderingContext(void);
void display(void);
void update(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

#endif