#include <GL/glut.h>
#include <cmath>

#include "ControlledBoid.h"
#include "vecFunctions.h"

ControlledBoid::ControlledBoid()
	: yaw(0.0), height(10.0), speed(0.0),
	maxSpeed(30.0), acceleration(4.0)
{
	setPosition(UnitY * height);
	setVelocity(Zero);
	setSize(1.0, 1.0, 1.0);
	setColors(Color::LightBlue, Color::LightGreen, Color::Cyan);
	disableCollision();
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

	// Calculate speed factor for wing animation
	GLdouble speedLength = length(newVelocity);
	GLdouble speedFactor = 0.0;
	if (maxSpeed > 1e-6) speedFactor = std::min(1.0, speedLength / maxSpeed);

	// Wing animation update
	GLdouble flapRate = getWingBaseRate() * (0.5 + 1.5 * speedFactor);
	setWingAngle(getWingAngle() + flapRate * deltaTime);

	// Update position based on velocity
	auto pos = getPosition();
	auto newPos = pos + newVelocity * deltaTime;
	newPos.y = height; // Maintain fixed height
	setPosition(newPos);
}
