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

	// Initialize the flock with a number of boids around a leader
	void init(int n, ControlledBoid* leader, GLdouble spread);
	
	// Update and draw the flock
	void update(GLdouble dt);
	void draw();
	
	// Manage boids in the flock
	void addBoid();
	void removeBoid();

	const std::vector<Boid*> getBoids() const { return boids; }
	int getBoidCount() const { return static_cast<int>(boids.size()); }
	Vec3 getAvgPosition() const;

private:
	std::vector<Boid*> boids; // Collection of boid pointers
	ControlledBoid* leaderBoid = nullptr; // Pointer to the controlled boid leader
	int maxBoids = 200;    // Maximum number of boids in the flock
	int minBoids = 10;     // Minimum number of boids in the flock
};
