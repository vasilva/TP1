#include <GL/glut.h>
#include <cmath>

#include "ControlledBoid.h"
#include "vecFunctions.h"

ControlledBoid::ControlledBoid()
	: speed(0.0f), acceleration(4.0f),
	height(10.0f), targetHeight(10.0f),
	heightSmoothFactor(10.0f)
{
	setVelocity(Zero);
	setMaxSpeed(30.0f);
	setColors(Color::LightBlue, Color::LightGreen, Color::Cyan);
	disableCollision();
}

void ControlledBoid::rotateYaw(GLfloat angle)
{
	auto yaw = getYaw();
	yaw += angle;
	if (yaw >= 360.0f) yaw -= 360.0f;
	if (yaw < -360.0f) yaw += 360.0f;
	setYaw(yaw);
}

void ControlledBoid::moveForward(GLfloat amount)
{
	GLfloat newSpeed = speed + acceleration * amount;
	speed = std::min(newSpeed, getMaxSpeed());
}

void ControlledBoid::moveBackward(GLfloat amount)
{
	GLfloat newSpeed = speed - acceleration * amount;
	speed = std::max(newSpeed, -getMaxSpeed());
}

void ControlledBoid::stop()
{
	speed = 0.0f;
	setVelocity(Zero);
}

void ControlledBoid::update(GLfloat deltaTime)
{
	// Update height towards target height
	if (deltaTime > 0.0f)
	{
		// Smooth height adjustment
		GLfloat alpha = 1.0f - std::exp(-heightSmoothFactor * deltaTime);
		height += (targetHeight - height) * alpha;
	}

	// Update velocity based on facing direction and speed
	GLfloat yawRad = getYaw() * (PI / 180.0f);
	Vec3 forwardDir = { std::sin(yawRad), 0.0f, std::cos(yawRad) };
	Vec3 newVelocity = forwardDir * speed;
	setVelocity(newVelocity);

	// Calculate speed factor for wing animation
	GLfloat speedLength = length(newVelocity);
	GLfloat speedFactor = 0.0f;
	if (getMaxSpeed() > 1e-6f) speedFactor = std::min(1.0f, speedLength / getMaxSpeed());

	// Wing animation update
	GLfloat flapRate = getWingBaseRate() * (0.5f + 1.5f * speedFactor);
	setWingAngle(getWingAngle() + flapRate * deltaTime);

	// Update position based on velocity
	auto pos = getPosition();
	auto newPos = pos + newVelocity * deltaTime;
	newPos.y = height;
	setPosition(newPos);
}
