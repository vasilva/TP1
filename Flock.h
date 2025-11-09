#pragma once
#include <vector>
#include "Boid.h"
#include "ControlledBoid.h"

// Flock class managing a collection of boids
class Flock
{
public:
	Flock() = default;
	~Flock();

	// Initialize the flock with n boids around the leader within a spread radius
	void init(int n, ControlledBoid* leader, GLdouble spread);
	void update(GLdouble dt);
	void draw();
	const std::vector<Boid*> getBoids() const { return boids; }

private:
	std::vector<Boid*> boids; // Collection of boid pointers
};
