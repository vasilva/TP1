#include <GL/glut.h>
#include <cmath>
#include "ControlledBoid.h"
#include "vecFunctions.h"

ControlledBoid::ControlledBoid()
	: yaw(0.0), height(10.0), speed(0.0), maxSpeed(20.0), acceleration(2.0)
{
	setPosition(0.0, height, 0.0);
	setVelocity(0.0, 0.0, 0.0);
	setSize(0.5, 0.5, 0.5);
}

void ControlledBoid::rotateYaw(GLdouble angle)
{
	yaw += angle;
	if (yaw >= 360.0) yaw -= 360.0;
	if (yaw < -360.0) yaw += 360.0;
}

void ControlledBoid::moveForward(GLdouble amount)
{
	GLdouble newSpeed = speed + acceleration * amount;
	speed = std::min(newSpeed, maxSpeed);
}

void ControlledBoid::moveBackward(GLdouble amount)
{
	GLdouble newSpeed = speed - acceleration * amount;
	speed = std::max(newSpeed, -maxSpeed);
}

void ControlledBoid::stop()
{
	speed = 0.0;
	setVelocity(0.0, 0.0, 0.0);
}

void ControlledBoid::update(GLdouble deltaTime)
{
	// Update velocity based on facing direction and speed
	GLdouble yawRad = yaw * (PI / 180.0);
	Vec3 forwardDir = { std::sin(yawRad), 0.0, std::cos(yawRad) };
	Vec3 newVelocity = forwardDir * speed;
	setVelocity(newVelocity);

	// Update position using base class method
	Vec3 pos = getPosition();
	Vec3 newPos = pos + newVelocity * deltaTime;
	newPos.y = height; // Maintain fixed height
	setPosition(newPos);
}

void ControlledBoid::draw()
{
	// Draw the controlled boid as a distinct shape (e.g., a larger cone)
	Vec3 pos = getPosition();
	Vec3 rotation = getRotation();
	
	// Transform
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y + yaw, 0.0, 1.0, 0.0);
	glRotated(rotation.z + 45.0, 0.0, 0.0, 1.0);

	// Draw
	auto color = Color::Blue;
	glColor3d(color.x, color.y, color.z);
	glutSolidCone(0.3, 0.8, 4, 2);
	color = Color::Black;
	glColor3d(color.x, color.y, color.z);
	glutWireCone(0.3, 0.8, 4, 2);
	glPopMatrix();
}
