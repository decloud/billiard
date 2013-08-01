#ifndef VECTOR_H
#define VECTOR_H

class Vector
{
	public:
		Vector();
		Vector(const float x, const float y, const float z);
		Vector(const Vector &v);
		Vector &operator=(const Vector &rhs);
		Vector &operator+(const Vector &rhs);

		float length() const;

		void add(const Vector &v);
		void add(const float a, const float b, const float c);

		void set(const Vector &v);
		void set(const float a, const float b, const float c);

		void reset();
		void normalize();
		void print();

		static float dot(const Vector &lhs, const Vector &rhs);
		static Vector cross(const Vector &lhs, const Vector &rhs);

		float x, y, z;
		const float dEpsilon;
};

Vector operator*(const float f, const Vector &v);
Vector operator+(const Vector &lhs, const Vector &rhs);
Vector operator-(const Vector &lhs, const Vector &rhs);


#endif