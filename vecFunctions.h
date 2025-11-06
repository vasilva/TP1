#pragma once

#include <GL/glut.h>
#include <cmath>

static const GLdouble PI = 3.14159265358979323846;

// 3D Vector
struct Vec3
{
	GLdouble x;
	GLdouble y;
	GLdouble z;

	Vec3() : x(0.0), y(0.0), z(0.0) {}
	Vec3(GLdouble x, GLdouble y, GLdouble z)
		: x(x), y(y), z(z) {
	}
};

// Consts
const Vec3 Zero(0.0, 0.0, 0.0);
const Vec3 UnitX(1.0, 0.0, 0.0);
const Vec3 UnitY(0.0, 1.0, 0.0);
const Vec3 UnitZ(0.0, 0.0, 1.0);

// Vector operations for 3D vectors
inline GLdouble length2(const Vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline GLdouble length(const Vec3& v)
{
	return std::sqrt(length2(v));
}

inline Vec3 crossProduct(const Vec3& a, const Vec3& b)
{
	return Vec3{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

inline GLdouble dotProduct(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vec3 operator*(const Vec3& v, GLdouble scalar)
{
	return Vec3{ v.x * scalar, v.y * scalar, v.z * scalar };
}

inline Vec3 operator/(const Vec3& v, GLdouble scalar)
{
	return Vec3{ v.x / scalar, v.y / scalar, v.z / scalar };
}

inline Vec3& operator+=(Vec3& a, const Vec3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline Vec3& operator-=(Vec3& a, const Vec3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline Vec3& operator*=(Vec3& v, GLdouble scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	return v;
}

inline Vec3& operator/=(Vec3& v, GLdouble scalar)
{
	v.x /= scalar;
	v.y /= scalar;
	v.z /= scalar;
	return v;
}

inline void normalize(Vec3& v)
{
	GLdouble len = length(v);
	if (len > 0.0)
		v /= len;
}

inline void limit(Vec3& v, GLdouble max)
{
	GLdouble len2 = length2(v);
	if (len2 > max * max && len2 > 1e-9)
	{
		GLdouble len = std::sqrt(len2);
		v *= (max / len);
	}
}

namespace Color
{
	static const Vec3 Black(0.0, 0.0, 0.0);
	static const Vec3 White(1.0, 1.0, 1.0);
	static const Vec3 Red(1.0, 0.0, 0.0);
	static const Vec3 Green(0.0, 1.0, 0.0);
	static const Vec3 Blue(0.0, 0.0, 1.0);
	static const Vec3 Yellow(1.0, 1.0, 0.0);
	static const Vec3 LightYellow(1.0, 1.0, 0.88);
	static const Vec3 LightBlue(0.68, 0.85, 0.9);
	static const Vec3 LightPink(1.0, 0.71, 0.76);
	static const Vec3 LightGreen(0.56, 0.93, 0.56);
}
