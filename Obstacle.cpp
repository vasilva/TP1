#include "Obstacle.h"

Obstacle::Obstacle()
	: fillColor(Color::DarkGray), wireColor(Color::Black)
{
}

Obstacle::Obstacle(const Vec3 pos, const Vec3 size, const Vec3 color)
	: fillColor(color), wireColor(Color::Black)
{
	setPosition(pos);
	setSize(size);
}

void Obstacle::draw()
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
	glScaled(size.x, size.y, size.z);

	// Draw solid cone
	glColor3d(fillColor.x, fillColor.y, fillColor.z);
	glutSolidCube(1.0);

	// Draw wireframe
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCube(1.0);
	glPopMatrix();
}
