#include <math.h>
#include "Ball.h"

Ball::Ball(float radius, int id)
	: radius(radius), x(0.0f), y(0.0f), z(0.0f), id(id),
	vx(0.0f), vy(0.0f), vz(0.0f), speed(0.0f)
{
}

void Ball::setLocation(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Ball::setVelocity(float vx, float vy, float speed)
{
	this->vx = vx;
	this->vy = vy;
	this->speed = speed;
}

void Ball::addVelocity(float xComp, float yComp, float speed)
{
	//since i am adding only components, i should be able to just add them normally
	float xNew = vx * this->speed + xComp * speed;
	float yNew = vy * this->speed + yComp * speed;
	float speedNew = sqrt(xNew * xNew + yNew * yNew);
	vx = xNew/speedNew;
	vy = yNew/speedNew;
	this->speed = speedNew;
}

void Ball::resetVelocity()
{
	this->vx = 0;
	this->vy = 0;
	this->vz = 0;
	this->speed = speed;
}