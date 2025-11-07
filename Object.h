#pragma once
#include <GL/glut.h>
#include "vecFunctions.h"

// Static view attributes
static double globalAngle = 20.0;
static double globalZoom = -10.0;
static bool wireView = false;

class Object
{
public:
	// Constructor and Destructor
	Object();
	~Object();
	virtual void draw() = 0;

	// Getters
	const Vec3 getPosition() { return position; }
	const Vec3 getRotation() { return rotation; }
	const Vec3 getVelocity() { return velocity; }
	const Vec3 getSize()	 { return size; }

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

private:
	// Transformation attributes
	Vec3 position;
	Vec3 rotation;
	Vec3 velocity;
	Vec3 size;
};
