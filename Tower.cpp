#include "Tower.h"

Tower::Tower()
	: fillColor(Color::Red), wireColor(Color::Black)
{
}

Tower::Tower(const Vec3 pos, const Vec3 size, const Vec3 color)
	: fillColor(color), wireColor(Color::Black)
{
	setPosition(pos);
	setSize(size);
}

// Draw the tower as a cone
void Tower::draw()
{
	auto pos = getPosition();
	auto size = getSize();
	auto rotation = getRotation();

	// Transformations
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);

	// Draw solid cone
	glColor3d(fillColor.x, fillColor.y, fillColor.z);
	glutSolidCone(size.x, size.y, 30, 1);

	// Draw wireframe
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(size.x, size.y, 30, 1);
	glPopMatrix();
}
