#pragma once
#include <GL/glut.h>
#include <cmath>

static constexpr GLfloat PI = static_cast<GLfloat>(3.14159265358979323846);

// 3D Vector
struct Vec3
{
	// Components
	GLfloat x;
	GLfloat y;
	GLfloat z;

	// Constructors
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};

// Common vectors
const Vec3 Zero(0.0f, 0.0f, 0.0f);
const Vec3 One(1.0f, 1.0f, 1.0f);
const Vec3 UnitX(1.0f, 0.0f, 0.0f);
const Vec3 UnitY(0.0f, 1.0f, 0.0f);
const Vec3 UnitZ(0.0f, 0.0f, 1.0f);

/* Vector operations for 3D vectors */

// Length squared of vector
inline GLfloat length2(const Vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// Length of vector
inline GLfloat length(const Vec3& v)
{
	return std::sqrt(length2(v));
}

// Cross product of two vectors
inline Vec3 crossProduct(const Vec3& a, const Vec3& b)
{
	return Vec3{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

// Dot product of two vectors
inline GLfloat dotProduct(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* Operator overloads for Vec3 */

inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return Vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vec3 operator*(const Vec3& v, GLfloat scalar)
{
	return Vec3{ v.x * scalar, v.y * scalar, v.z * scalar };
}

inline Vec3 operator/(const Vec3& v, GLfloat scalar)
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

inline Vec3& operator*=(Vec3& v, GLfloat scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	return v;
}

inline Vec3& operator/=(Vec3& v, GLfloat scalar)
{
	v.x /= scalar;
	v.y /= scalar;
	v.z /= scalar;
	return v;
}

// Normalize vector to unit length
inline void normalize(Vec3& v)
{
	GLfloat len = length(v);
	if (len > 0.0f)
		v /= len;
}

// Limit vector length to max
inline void limit(Vec3& v, GLfloat max)
{
	GLfloat len2 = length2(v);
	if (len2 > max * max && len2 > 1e-9f)
	{
		GLfloat len = std::sqrt(len2);
		v *= (max / len);
	}
}

// Linear interpolation between two vectors
inline Vec3 lerp(const Vec3& a, const Vec3& b, GLfloat alpha)
{
	return a + (b - a) * alpha;
}

// Common colors
namespace Color
{
	// Grayscale
	static const Vec3 Black(0.0f, 0.0f, 0.0f);
	static const Vec3 White(1.0f, 1.0f, 1.0f);
	static const Vec3 Gray(0.5f, 0.5f, 0.5f);
	static const Vec3 DarkGray(0.25f, 0.25f, 0.25f);
	static const Vec3 LightGray(0.75f, 0.75f, 0.75f);
	// Reds
	static const Vec3 Red(1.0f, 0.0f, 0.0f);
	static const Vec3 DarkRed(0.5f, 0.0f, 0.0f);
	static const Vec3 LightRed(1.0f, 0.5f, 0.5f);
	// Greens
	static const Vec3 Green(0.0f, 1.0f, 0.0f);
	static const Vec3 DarkGreen(0.0f, 0.5f, 0.0f);
	static const Vec3 LightGreen(0.5f, 1.0f, 0.5f);
	// Blues
	static const Vec3 Blue(0.0f, 0.0f, 1.0f);
	static const Vec3 DarkBlue(0.0f, 0.0f, 0.5f);
	static const Vec3 LightBlue(0.5f, 0.5f, 1.0f);
	// Yellows
	static const Vec3 Yellow(1.0f, 1.0f, 0.0f);
	static const Vec3 DarkYellow(0.5f, 0.5f, 0.0f);
	static const Vec3 LightYellow(1.0f, 1.0f, 0.5f);
	// Cyans
	static const Vec3 Cyan(0.0f, 1.0f, 1.0f);
	static const Vec3 DarkCyan(0.0f, 0.5f, 0.5f);
	static const Vec3 LightCyan(0.5f, 1.0f, 1.0f);
	// Magentas
	static const Vec3 Magenta(1.0f, 0.0f, 1.0f);
	static const Vec3 DarkMagenta(0.5f, 0.0f, 0.5f);
	static const Vec3 LightMagenta(1.0f, 0.5f, 1.0f);
	// Oranges
	static const Vec3 Orange(1.0f, 0.5f, 0.0f);
	static const Vec3 DarkOrange(0.5f, 0.25f, 0.0f);
	static const Vec3 LightOrange(1.0f, 0.75f, 0.5f);
	// Purples
	static const Vec3 Purple(0.5f, 0.0f, 0.5f);
	static const Vec3 DarkPurple(0.25f, 0.0f, 0.25f);
	static const Vec3 LightPurple(0.75f, 0.5f, 0.75f);
	// Others
	static const Vec3 Brown(0.6f, 0.4f, 0.2f);
	static const Vec3 Pink(1.0f, 0.75f, 0.8f);
	static const Vec3 Lime(0.75f, 1.0f, 0.0f);
	static const Vec3 Olive(0.5f, 0.5f, 0.0f);
}
