#pragma once
#include <GL/glut.h>
#include "vecFunctions.h"

// Base Object class representing a 3D object in the environment
class Object
{
public:
	// Constructor and Destructor
	Object();
	~Object() = default;
	
	// Pure virtual draw method to be implemented by derived classes
	virtual void draw() = 0;

	// Getters
	const Vec3 getPosition() const { return position; }
	const Vec3 getRotation() const { return rotation; }
	const Vec3 getVelocity() const { return velocity; }
	const Vec3 getSize() const { return size; }
	const bool canCollide() const { return hasCollider; }

	// Setters
	void setPosition(GLdouble x, GLdouble y, GLdouble z);
	void setPosition(Vec3 pos);

	void setRotation(GLdouble x, GLdouble y, GLdouble z);
	void setRotation(Vec3 rot);

	void setVelocity(GLdouble x, GLdouble y, GLdouble z);
	void setVelocity(Vec3 v);

	void setSize(GLdouble x, GLdouble y, GLdouble z);
	void setSize(Vec3 s);

	// Physics methods
	void applyForce(GLdouble fx, GLdouble fy, GLdouble fz);
	void applyForce(Vec3 force);
	void updatePosition(GLdouble deltaTime);
	
	// Collision methods
	void enableCollision() { hasCollider = true; }
	void disableCollision() { hasCollider = false; }

private:
	// Transformation attributes
	Vec3 position;	// Position vector (x, y, z)
	Vec3 rotation;	// Rotation angles (pitch, yaw, roll)
	Vec3 velocity;	// Velocity vector (vx, vy, vz)
	Vec3 size;		// Size dimensions (width, height, depth)

	bool hasCollider; // Collision flag
};
