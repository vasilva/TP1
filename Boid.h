#pragma once

#include <vector>
#include "Object.h"
#include "vecFunctions.h"

class Boid : public Object
{
public:
	// Constructors
	Boid();
	Boid(const Vec3 pos);
	
	// Boid behavior methods
	void update(const std::vector<Boid*>& boids, GLdouble deltaTime);
	void draw() override;

private:
	// Parameters
	GLdouble maxSpeed;
	GLdouble maxForce;
	GLdouble neighRadius;
	GLdouble separationRadius;

	// Weights for behaviors
	GLdouble weightCohesion;
	GLdouble weightSeparation;
	GLdouble weightAlignment;
	
	// Behavior methods
	void applyBehaviors(const std::vector<Boid*>& neighbors, GLdouble dt);
	void flock(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
	void separate(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
	void align(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
};

