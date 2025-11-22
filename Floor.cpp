#include "Floor.h"
#include "vecFunctions.h"
#include <algorithm>

Floor::Floor() : fillColor(Color::DarkGreen), wireColor(Color::Black) {}

Floor::Floor(const Vec3 pos, const Vec3 size, const Vec3 color)
	: fillColor(color), wireColor(Color::Black)
{
	setPosition(pos);
	setSize(size);
}

// Draw the floor as a grid with filled quads
void Floor::draw()
{
	auto pos = getPosition();
	auto size = getSize();
	auto rotation = getRotation();

	// Grid parameters
	const GLfloat spacing = 10.0f;
	GLfloat width = std::max(0.0f, size.x);
	GLfloat halfW = width * 0.5f;
	GLfloat depth = std::max(0.0f, size.z);
	GLfloat halfD = depth * 0.5f;

	// Calculate number of rows and columns
	int cols = std::max(1, static_cast<int>(std::floor(width / spacing)));
	int rows = std::max(1, static_cast<int>(std::floor(depth / spacing)));

	// Small offsets to avoid z-fighting
	const GLfloat lineOffset = 0.001f;
	const GLfloat quadOffset = 0.0f;

	// Transformations
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

	GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
	if (!lightingEnabled) glEnable(GL_LIGHTING);

	// Use polygon offset to reduce z-fighting with grid lines
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	glColor3f(fillColor.x, fillColor.y, fillColor.z);
	glBegin(GL_QUADS);

	// normal pointing up (assumes floor horizontal after rotation)
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-halfW, quadOffset, -halfD);
	glVertex3f(-halfW, quadOffset, halfD);
	glVertex3f(halfW, quadOffset, halfD);
	glVertex3f(halfW, quadOffset, -halfD);
	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);

	// Draw grid lines (no lighting for crisp lines)
	if (lightingEnabled) glDisable(GL_LIGHTING);

	glLineWidth(1.0f);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);

	glBegin(GL_LINES);
	// Lines parallel to x
	for (int r = 0; r <= rows; ++r)
	{
		GLfloat z = -halfD + (static_cast<GLfloat>(r) / rows) * (2.0f * halfD);
		glVertex3f(-halfW, lineOffset, z);
		glVertex3f(halfW, lineOffset, z);
	}
	// Lines parallel to z
	for (int c = 0; c <= cols; ++c)
	{
		GLfloat x = -halfW + (static_cast<GLfloat>(c) / cols) * (2.0f * halfW);
		glVertex3f(x, lineOffset, -halfD);
		glVertex3f(x, lineOffset, halfD);
	}
	glEnd();

	// Highlight central axes
	glLineWidth(2.0f);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);

	glBegin(GL_LINES);
	// Z axis (X varies)
	glVertex3f(-halfW, lineOffset * 2.0f, 0.0f);
	glVertex3f(halfW, lineOffset * 2.0f, 0.0f);
	// X axis (Z varies)
	glVertex3f(0.0, lineOffset * 2.0f, -halfD);
	glVertex3f(0.0, lineOffset * 2.0f, halfD);
	glEnd();

	// Restore lighting state
	if (lightingEnabled) glEnable(GL_LIGHTING);

	glPopMatrix();
}
