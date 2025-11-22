#include "Tower.h"

Tower::Tower() : fillColor(Color::Red), wireColor(Color::Black) {}

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
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

	// Draw solid cone
	glColor3f(fillColor.x, fillColor.y, fillColor.z);
	glutSolidCone(size.x, size.y, 30, 1);

	// Draw wireframe
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(size.x, size.y, 30, 1);
	glPopMatrix();
}
