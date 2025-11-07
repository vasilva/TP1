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

// Common colors
namespace Color
{
	// Grayscale
	static const Vec3 Black(0.0, 0.0, 0.0);
	static const Vec3 White(1.0, 1.0, 1.0);
	static const Vec3 Gray(0.5, 0.5, 0.5);
	static const Vec3 DarkGray(0.25, 0.25, 0.25);
	static const Vec3 LightGray(0.75, 0.75, 0.75);
	// Reds
	static const Vec3 Red(1.0, 0.0, 0.0);
	static const Vec3 DarkRed(0.5, 0.0, 0.0);
	static const Vec3 LightRed(1.0, 0.5, 0.5);
	// Greens
	static const Vec3 Green(0.0, 1.0, 0.0);
	static const Vec3 DarkGreen(0.0, 0.5, 0.0);
	static const Vec3 LightGreen(0.5, 1.0, 0.5);
	// Blues
	static const Vec3 Blue(0.0, 0.0, 1.0);
	static const Vec3 DarkBlue(0.0, 0.0, 0.5);
	static const Vec3 LightBlue(0.5, 0.5, 1.0);
	// Yellows
	static const Vec3 Yellow(1.0, 1.0, 0.0);
	static const Vec3 DarkYellow(0.5, 0.5, 0.0);
	static const Vec3 LightYellow(1.0, 1.0, 0.5);
	// Cyans
	static const Vec3 Cyan(0.0, 1.0, 1.0);
	static const Vec3 DarkCyan(0.0, 0.5, 0.5);
	static const Vec3 LightCyan(0.5, 1.0, 1.0);
	// Magentas
	static const Vec3 Magenta(1.0, 0.0, 1.0);
	static const Vec3 DarkMagenta(0.5, 0.0, 0.5);
	static const Vec3 LightMagenta(1.0, 0.5, 1.0);
	// Oranges
	static const Vec3 Orange(1.0, 0.5, 0.0);
	static const Vec3 DarkOrange(0.5, 0.25, 0.0);
	static const Vec3 LightOrange(1.0, 0.75, 0.5);
	// Purples
	static const Vec3 Purple(0.5, 0.0, 0.5);
	static const Vec3 DarkPurple(0.25, 0.0, 0.25);
	static const Vec3 LightPurple(0.75, 0.5, 0.75);
	// Others
	static const Vec3 Brown(0.6, 0.4, 0.2);
	static const Vec3 Pink(1.0, 0.75, 0.8);
	static const Vec3 Lime(0.75, 1.0, 0.0);
	static const Vec3 Olive(0.5, 0.5, 0.0);
}
