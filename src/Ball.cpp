#include <math.h>
#include "Ball.h"
#include <stdio.h>

Ball::Ball() : radius(0.0f), id(-1), position(), velocity(), friction(1.0f), bounciness(1.0f)
{
}

Ball::Ball(float radius, int id)
	: radius(radius), id(id), position(), velocity(), friction(1.0f), bounciness(1.0f)
{
}

void Ball::setFriction(const float value)
{
	friction = value;
}

void Ball::setBounciness(const float value)
{
	bounciness = value;
}

