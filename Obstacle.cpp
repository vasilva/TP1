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
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
	glScalef(size.x, size.y, size.z);

	// Draw solid cone
	glColor3f(fillColor.x, fillColor.y, fillColor.z);
	glutSolidCube(1.0);

	// Draw wireframe
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glutWireCube(1.0);
	glPopMatrix();
}
