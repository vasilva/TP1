#pragma once
#include <GL/glut.h>

#include "vecFunctions.h"

// Camera class for managing 3D camera transformations
class Camera
{
public:
	// Constructor and Destructor
	Camera();
	~Camera() = default;

	// Apply the camera transformation
	void applyView() const;

	// Movement methods
	void moveForward(GLfloat distance);
	void moveBackward(GLfloat distance);
	void moveUp(GLfloat distance);
	void moveDown(GLfloat distance);

	// Rotation method for yaw and pitch
	void rotate(GLfloat yawDeg, GLfloat pitchDeg);

	// Zoom methods
	void zoomIn(GLfloat amount);
	void zoomOut(GLfloat amount);

	// Setters
	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	void setPosition(Vec3 v);

	void setTarget(GLfloat x, GLfloat y, GLfloat z);
	void setTarget(Vec3 t);

	void setUp(GLfloat x, GLfloat y, GLfloat z);
	void setUp(Vec3 u);

	// Getters
	const Vec3 getPosition() const { return position; }
	const Vec3 getTarget() const { return target; }
	const Vec3 getUp() const { return up; }

private:
	Vec3 position;				// Camera position
	Vec3 target;				// Camera target point
	Vec3 up;					// Up vector
	GLfloat distanceToTarget;	// Distance from position to target

	// Angular orientation
	GLfloat yawAngle;	 // Rotation around Y-axis
	GLfloat pitchAngle; // Rotation around X-axis

	// Zoom level
	GLfloat minDistance;
	GLfloat maxDistance;

	// Helper method to update target based on current angles and distance
	void updateTargetFromAngles();
};
