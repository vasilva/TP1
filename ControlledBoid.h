#pragma once
#include "Boid.h"

class ControlledBoid : public Boid
{
public:
	ControlledBoid();

	// Control methods
	void rotateYaw(GLdouble angle);		// Rotate around Y-axis
	void moveForward(GLdouble amount);	// Move forward in facing direction
	void moveBackward(GLdouble amount); // Move backward in facing direction
	void stop();						// Stop movement

	// Height control
	void setHeight(GLdouble h);
	GLdouble getHeight() const { return height; }

	// Override update to include control
	void update(GLdouble deltaTime);

	// Get facing direction
	GLdouble getYaw() const { return yaw; }

private:
	GLdouble yaw;				 // Facing direction in degrees, 0 = +Z direction
	GLdouble speed;				 // Movement speed
	GLdouble maxSpeed;			 // Maximum speed
	GLdouble acceleration;		 // Acceleration value
	GLdouble height;			 // Current height	
	GLdouble targetHeight;		 // Target height
	GLdouble heightSmoothFactor; // Smoothing factor for height changes
};
