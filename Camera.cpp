#include <cmath>

#include "Camera.h"

Camera::Camera()
	: yawAngle(0.0f), pitchAngle(0.0f),
	position(Zero), target(Zero), up(UnitY),
	minDistance(5.0f), maxDistance(100.0f)
{
	distanceToTarget = minDistance * 2.0f;
	updateTargetFromAngles();
}

void Camera::applyView() const
{
	// Set the view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply the lookAt transformation
	gluLookAt(
		position.x, position.y, position.z,
		target.x, target.y, target.z,
		up.x, up.y, up.z
	);
}

void Camera::moveForward(GLfloat distance)
{
	// Calculate the forward vector
	GLfloat yawRad = yawAngle * (PI / 180.0f);
	GLfloat pitchRad = pitchAngle * (PI / 180.0f);
	Vec3 forward = {
		std::cos(pitchRad) * std::sin(yawRad),
		std::sin(pitchRad),
		std::cos(pitchRad) * std::cos(yawRad)
	};
	normalize(forward);

	// Move the camera position forward
	position += forward * distance;

	updateTargetFromAngles();
}

void Camera::moveBackward(GLfloat distance)
{
	moveForward(-distance);
}

void Camera::moveUp(GLfloat distance)
{
	position += up * distance;
	updateTargetFromAngles();
}

void Camera::moveDown(GLfloat distance)
{
	moveUp(-distance);
}

void Camera::rotate(GLfloat yawDeg, GLfloat pitchDeg)
{
	// Update yaw and pitch angles
	yawAngle += yawDeg;
	pitchAngle += pitchDeg;

	// Clamp pitch angle to avoid gimbal lock
	if (pitchAngle > 89.0f)  pitchAngle = 89.0f;
	if (pitchAngle < -89.0f) pitchAngle = -89.0f;
	updateTargetFromAngles();
}

void Camera::zoomIn(GLfloat amount)
{
	// Decrease distance to target
	distanceToTarget -= amount;
	if (distanceToTarget < minDistance)
	{
		distanceToTarget = minDistance;
		updateTargetFromAngles();
	}
	else
		moveForward(amount);
}

void Camera::zoomOut(GLfloat amount)
{
	// Increase distance to target
	distanceToTarget += amount;
	if (distanceToTarget > maxDistance)
	{
		distanceToTarget = maxDistance;
		updateTargetFromAngles();
	}
	else
		moveBackward(amount);
}

void Camera::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position = Vec3(x, y, z);
	updateTargetFromAngles();
}

void Camera::setPosition(Vec3 v)
{
	position = v;
	updateTargetFromAngles();
}

void Camera::setTarget(GLfloat x, GLfloat y, GLfloat z)
{
	target = Vec3(x, y, z);

	// Update yaw and pitch angles based on new target
	Vec3 dir = target - position;
	GLfloat d = length(dir);
	if (d > 0.0f) {
		distanceToTarget = d;
		normalize(dir);
		pitchAngle = std::asin(dir.y) * (180.0f / PI);
		yawAngle = std::atan2(dir.x, dir.z) * (180.0f / PI);
	}
	updateTargetFromAngles();
}

void Camera::setTarget(Vec3 t)
{
	target = t;

	// Update yaw and pitch angles based on new target
	Vec3 dir = target - position;
	GLfloat d = length(dir);
	if (d > 0.0f) {
		distanceToTarget = d;
		normalize(dir);
		pitchAngle = std::asin(dir.y) * (180.0f / PI);
		yawAngle = std::atan2(dir.x, dir.z) * (180.0f / PI);
	}
	updateTargetFromAngles();
}

void Camera::setUp(GLfloat x, GLfloat y, GLfloat z)
{
	up = Vec3(x, y, z);
	updateTargetFromAngles();
}

void Camera::setUp(Vec3 u)
{
	up = u;
	updateTargetFromAngles();
}

void Camera::updateTargetFromAngles()
{
	// Convert angles from degrees to radians
	GLfloat yawRad = yawAngle * PI / 180.0f;
	GLfloat pitchRad = pitchAngle * PI / 180.0f;

	// Calculate the new target position
	Vec3 forward = {
		cos(pitchRad) * sin(yawRad),
		sin(pitchRad),
		cos(pitchRad) * cos(yawRad)
	};
	normalize(forward);

	// Preserve distanceToTarget
	target = position + forward * distanceToTarget;
}
