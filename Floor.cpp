#include "Floor.h"
#include "vecFunctions.h"
#include <algorithm>

Floor::Floor() : Object() {}

Floor::Floor(const Vec3 pos, const Vec3 size)
	: Object()
{
	setPosition(pos);
	setSize(size);
}

Floor::~Floor() {}

void Floor::draw()
{
	// Draw a floor as a scaled cube
	auto pos = getPosition();
	auto size = getSize();
	auto rotation = getRotation();

	// Grid parameters
	const GLdouble spacing = 10.0;
	GLdouble width = std::max(0.0, size.x);
	GLdouble halfW = width * 0.5;
	GLdouble depth = std::max(0.0, size.z);
	GLdouble halfD = depth * 0.5;

	int cols = std::max(1, static_cast<int>(std::floor(width / spacing)));
	int rows = std::max(1, static_cast<int>(std::floor(depth / spacing)));

	// Small offsets to avoid z-fighting
	const GLdouble lineOffset = 0.001;
	const GLdouble quadOffset = 0.0;

	// Apply transforms
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);

	GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
	if (!lightingEnabled) glEnable(GL_LIGHTING);

	// Use polygon offset to reduce z-fighting with grid lines
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	auto color = Color::DarkGreen;
	glColor3d(color.x, color.y, color.z);
	glBegin(GL_QUADS);
	// normal pointing up (assumes floor horizontal after rotation)
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(-halfW, quadOffset, -halfD);
	glVertex3d(-halfW, quadOffset, halfD);
	glVertex3d(halfW, quadOffset, halfD);
	glVertex3d(halfW, quadOffset, -halfD);
	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);

	// Draw grid lines (no lighting for crisp lines)
	if (lightingEnabled) glDisable(GL_LIGHTING);

	glLineWidth(1.0f);
	color = Color::Black;
	glColor3d(color.x, color.y, color.z);

	glBegin(GL_LINES);
	// Lines parallel to x
	for (int r = 0; r <= rows; ++r)
	{
		GLdouble z = -halfD + (static_cast<GLdouble>(r) / rows) * (2.0 * halfD);
		glVertex3d(-halfW, lineOffset, z);
		glVertex3d(halfW, lineOffset, z);
	}
	// Lines parallel to z
	for (int c = 0; c <= cols; ++c)
	{
		GLdouble x = -halfW + (static_cast<GLdouble>(c) / cols) * (2.0 * halfW);
		glVertex3d(x, lineOffset, -halfD);
		glVertex3d(x, lineOffset, halfD);
	}
	glEnd();

	// Highlight central axes
	glLineWidth(2.0f);
	glColor3d(color.x, color.y, color.z);

	glBegin(GL_LINES);
	// Z axis (X varies)
	glVertex3d(-halfW, lineOffset * 2.0, 0.0);
	glVertex3d(halfW, lineOffset * 2.0, 0.0);
	// X axis (Z varies)
	glVertex3d(0.0, lineOffset * 2.0, -halfD);
	glVertex3d(0.0, lineOffset * 2.0, halfD);
	glEnd();

	// Restore lighting state
	if (lightingEnabled) glEnable(GL_LIGHTING);

	glPopMatrix();
}
