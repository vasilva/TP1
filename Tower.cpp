#include "Tower.h"

Tower::Tower() : Object() {}

Tower::Tower(const Vec3 pos, const Vec3 size) 
	: Object()
{
	setPosition(pos);
	setSize(size);
}

Tower::~Tower() {}

void Tower::draw()
{
	// Draw a tower as a cone
	auto pos = getPosition();
	auto size = getSize();
	auto rotation = getRotation();
	
	auto color = Color::Red;
	glColor3d(color.x, color.y, color.z); // Red

	// Draw solid cone
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);
	glutSolidCone(size.x, size.y, 20, 5);

	// Draw wireframe
	color = Color::Black;
	glColor3d(color.x, color.y, color.z); // Black
	glutWireCone(size.x, size.y, 30, 1);
	glPopMatrix();
}
