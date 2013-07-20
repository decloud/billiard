#ifndef BALL_H
#define BALL_H

//NOTE: might add mass later
class Ball
{
	public:
		Ball(float radius, int id = -1);
		void setLocation(float x, float y, float z);
		void setVelocity(float vx, float vy, float speed);
		void addVelocity(float xComponent, float yComponent, float speed);
		void resetVelocity();
		float radius;
		float x, y, z;
		float vx, vy, vz;
		float speed;
		int id;
};

#endif