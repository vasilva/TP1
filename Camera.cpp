#include "Camera.h"

Camera::Camera()
	: yawAngle(0.0), pitchAngle(0.0),
	position(0.0, 0.0, 5.0),
	target(Zero),
	up(UnitY),
	minDistance(5.0), maxDistance(100.0)
{
	Vec3 dir = target - position;
	GLdouble dist = length(dir);
	if (dist > 0.0) distanceToTarget = dist;
	updateTargetFromAngles();
}

Camera::Camera(const Vec3 pos, const Vec3 target, const Vec3 up)
	: yawAngle(0.0), pitchAngle(0.0),
	position(pos), target(target), up(up),
	minDistance(5.0), maxDistance(100.0)
{
	Vec3 dir = target - position;
	GLdouble dist = length(dir);
	distanceToTarget = dist > 0.0 ? dist : 5.0;

	// Calculate initial yaw and pitch angles
	if (dist > 0.0)
	{
		normalize(dir);
		yawAngle = atan2(dir.x, dir.z) * 180.0 / PI;
		pitchAngle = asin(dir.y) * 180.0 / PI;
	}
	updateTargetFromAngles();
}

Camera::~Camera() {}

void Camera::applyView()
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

void Camera::moveForward(GLdouble distance)
{
	// Calculate the forward vector
	GLdouble yawRad = yawAngle * (PI / 180.0);
	GLdouble pitchRad = pitchAngle * (PI / 180.0);
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

void Camera::moveBackward(GLdouble distance)
{
	moveForward(-distance);
}

void Camera::strafeLeft(GLdouble distance)
{
	// Calculate the forward vector
	GLdouble yawRad = yawAngle * (PI / 180.0);
	GLdouble pitchRad = pitchAngle * (PI / 180.0);
	Vec3 forward = {
		std::cos(pitchRad) * std::sin(yawRad),
		std::sin(pitchRad),
		std::cos(pitchRad) * std::cos(yawRad)
	};
	normalize(forward);

	// Calculate the left vector
	Vec3 worldUp = UnitZ;
	Vec3 left = crossProduct(worldUp, forward); // left = up x forward
	normalize(left);

	// Move the camera position left
	position += left * distance;

	updateTargetFromAngles();
}

void Camera::strafeRight(GLdouble distance)
{
	strafeLeft(-distance);
}

void Camera::moveUp(GLdouble distance)
{
	position += up * distance;
	updateTargetFromAngles();
}

void Camera::moveDown(GLdouble distance)
{
	moveUp(-distance);
}

void Camera::rotate(GLdouble yawDeg, GLdouble pitchDeg)
{
	// Update yaw and pitch angles
	yawAngle += yawDeg;
	pitchAngle += pitchDeg;

	// Clamp pitch angle to avoid gimbal lock
	if (pitchAngle > 89.0)  pitchAngle = 89.0;
	if (pitchAngle < -89.0) pitchAngle = -89.0;
	updateTargetFromAngles();
}

void Camera::zoomIn(GLdouble amount)
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

void Camera::zoomOut(GLdouble amount)
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

void Camera::setPosition(GLdouble x, GLdouble y, GLdouble z)
{
	position = Vec3(x, y, z);
	updateTargetFromAngles();
}

void Camera::setPosition(Vec3 v)
{
	position = v;
	updateTargetFromAngles();
}

void Camera::setTarget(GLdouble x, GLdouble y, GLdouble z)
{
	target = Vec3(x, y, z);

	// Update yaw and pitch angles based on new target
	Vec3 dir = target - position;
	GLdouble d = length(dir);
	if (d > 0.0) {
		distanceToTarget = d;
		normalize(dir);
		pitchAngle = std::asin(dir.y) * (180.0 / PI);
		yawAngle = std::atan2(dir.x, dir.z) * (180.0 / PI);
	}
	updateTargetFromAngles();
}

void Camera::setTarget(Vec3 t)
{
	target = t;

	// Update yaw and pitch angles based on new target
	Vec3 dir = target - position;
	GLdouble d = length(dir);
	if (d > 0.0) {
		distanceToTarget = d;
		normalize(dir);
		pitchAngle = std::asin(dir.y) * (180.0 / PI);
		yawAngle = std::atan2(dir.x, dir.z) * (180.0 / PI);
	}
	updateTargetFromAngles();
}

void Camera::setUp(GLdouble x, GLdouble y, GLdouble z)
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
	GLdouble yawRad = yawAngle * PI / 180.0;
	GLdouble pitchRad = pitchAngle * PI / 180.0;

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
