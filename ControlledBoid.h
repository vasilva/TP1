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

	// Override update to include control
	void update(GLdouble deltaTime);

	// Draw method
	void draw() override;

	// Get facing direction
	GLdouble getYaw() const { return yaw; }

	// Set height
	void setHeight(GLdouble h) { height = h; }
	GLdouble getHeight() const { return height; }

private:
	GLdouble yaw;		// Facing direction in degrees, 0 = +Z direction
	GLdouble height;	// Fixed height above ground
	GLdouble speed;		// Movement speed
	GLdouble maxSpeed;	// Maximum speed
	GLdouble acceleration; // Acceleration value
};
