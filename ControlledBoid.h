#pragma once
#include "Boid.h"

class ControlledBoid : public Boid
{
public:
	ControlledBoid();

	// Control methods
	void rotateYaw(GLfloat angle);		// Rotate around Y-axis
	void moveForward(GLfloat amount);	// Move forward in facing direction
	void moveBackward(GLfloat amount); // Move backward in facing direction
	void stop();						// Stop movement

	// Height control
	void setHeight(GLfloat h) { height = h; }
	GLfloat getHeight() const { return height; }

	// Override update to include control
	void update(GLfloat deltaTime);

private:
	GLfloat speed;				 // Movement speed
	GLfloat acceleration;		 // Acceleration value
	GLfloat height;				// Current height	
	GLfloat targetHeight;		 // Target height
	GLfloat heightSmoothFactor; // Smoothing factor for height changes
};
