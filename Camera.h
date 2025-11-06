#pragma once

#include <GL/glut.h>
#include <cmath>
#include "vecFunctions.h"

class Camera
{
public:
	// Constructor and Destructor
	Camera();
	Camera(const Vec3 pos, const Vec3 target, const Vec3 up);
	~Camera();

	// Apply the camera transformation
	void applyView();

	// Movement methods
	void moveForward(GLdouble distance);
	void moveBackward(GLdouble distance);
	void strafeLeft(GLdouble distance);
	void strafeRight(GLdouble distance);
	void moveUp(GLdouble distance);
	void moveDown(GLdouble distance);

	// Rotation method for yaw and pitch 
	void rotate(GLdouble yawDeg, GLdouble pitchDeg);

	// Zoom methods
	void zoomIn(GLdouble amount);
	void zoomOut(GLdouble amount);

	// Setters
	void setPosition(GLdouble x, GLdouble y, GLdouble z);
	void setPosition(Vec3 v);
	
	void setTarget(GLdouble x, GLdouble y, GLdouble z);
	void setTarget(Vec3 t);

	void setUp(GLdouble x, GLdouble y, GLdouble z);
	void setUp(Vec3 u);

	// Getters
	const Vec3 getPosition() const { return position; }
	const Vec3 getTarget() const { return target; }
	const Vec3 getUp() const { return up; }

private:
	Vec3 position;
	Vec3 target;
	Vec3 up;
	GLdouble distanceToTarget;

	// Angular orientation
	GLdouble yawAngle;   // Rotation around Y-axis
	GLdouble pitchAngle; // Rotation around X-axis
	
	// Zoom level
	GLdouble minDistance;
	GLdouble maxDistance;

	// Helpers
	void updateTargetFromAngles();
};
