#pragma once

#include <GL/glut.h>

void computeShadowMatrix(GLfloat matrix[16], const GLfloat plane[4], const GLfloat light[4])
{
	// Plane = Ax + By + Cz + D = 0
	GLfloat A = plane[0], B = plane[1], C = plane[2], D = plane[3];

	// Light = {Lx, Ly, Lz, Lw}
	GLfloat Lx = light[0], Ly = light[1], Lz = light[2], Lw = light[3];

	GLfloat dot = A * Lx + B * Ly + C * Lz + D * Lw;

	// Compute shadow matrix elements
	GLfloat m[4][4];
	m[0][0] = dot - Lx * A;	m[0][1] = -Lx * B;		m[0][2] = -Lx * C;		m[0][3] = -Lx * D;
	m[1][0] = -Ly * A;		m[1][1] = dot - Ly * B;	m[1][2] = -Ly * C;		m[1][3] = -Ly * D;
	m[2][0] = -Lz * A;		m[2][1] = -Lz * B;		m[2][2] = dot - Lz * C;	m[2][3] = -Lz * D;
	m[3][0] = -Lw * A;		m[3][1] = -Lw * B;		m[3][2] = -Lw * C;		m[3][3] = dot - Lw * D;

	// transpose to column-major order
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			matrix[j * 4 + i] = m[i][j];
}
