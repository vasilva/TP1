#include "Floor.h"

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

	auto color = Color::Green;
	glColor3d(color.x, color.y, color.z); // Green

	// Draw solid cube
	glPushMatrix();
	glTranslated(pos.x, pos.x, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);
	glScaled(size.x, size.y, size.z);
	glutSolidCube(1.0);
	glPopMatrix();

	color = Color::Black;
	glColor3d(color.x, color.y, color.z); // Black
	
	// Draw wireframe
	glPushMatrix();
	glTranslated(pos.x, pos.x, pos.x);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);
	glScaled(size.x, size.y, size.z);
	glutWireCube(1.0);
	glPopMatrix();
}
