#pragma once
#include <vector>
#include "Boid.h"

class Flock
{
public:
	Flock() {}
	~Flock();

	// Initialize n boids
	void init(int n, GLdouble spread = 50.0);

	// Update
	void update(GLdouble dt);

	// Draw
	void draw();

	const std::vector<Boid*> getBoids() const { return boids; }

private:
	std::vector<Boid*> boids;
};
