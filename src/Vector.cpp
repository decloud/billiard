#include "Vector.h"
#include <math.h>
#include <iostream>

using namespace std;

Vector::Vector() : x(0.0f), y(0.0f), z(0.0f), dEpsilon(0.000001f)
{
}

Vector::Vector(const float x, const float y, const float z) : x(x), y(y), z(z), dEpsilon(0.000001f)
{
}

Vector::Vector(const Vector &v) : dEpsilon(0.000001f)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector &Vector::operator=(const Vector &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

Vector &Vector::operator+(const Vector &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

float Vector::length() const
{
	return (float) sqrt((x * x) + (y * y) + (z * z));
}

void Vector::add(const Vector &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vector::add(const float a, const float b, const float c)
{
	x += a;
	y += b;
	z += c;
}

void Vector::set(const Vector &v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void Vector::set(const float a, const float b, const float c)
{
	x = a;
	y = b;
	z = c;
}

void Vector::reset()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

void Vector::normalize()
{
	float l = length();

	// avoid divide by 0 error
	if (l > dEpsilon) {
		x = x / l;
		y = y / l;
		z = z / l;
	} else {
		reset();
	}
}

void Vector::print()
{
	cout << "x: " << x << " y: " << y << " z: " << z <<endl;
}

float Vector::dot(const Vector &lhs, const Vector &rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

Vector Vector::cross(const Vector &lhs, const Vector &rhs)
{
	float x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
	float y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
	float z = (lhs.x * rhs.y) - (lhs.y * rhs.x);

	return Vector(x,y,z);
}

Vector operator*(const float f, const Vector &v)
{
	return Vector(f * v.x, f * v.y, f * v.z);
}

Vector operator+(const Vector &lhs, const Vector &rhs)
{
	return Vector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector operator-(const Vector &lhs, const Vector &rhs)
{
	return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}