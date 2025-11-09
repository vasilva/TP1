#include <random>
#include "Flock.h"
#include "vecFunctions.h"

Flock::~Flock()
{
	for (auto b : boids)
		delete b;
	boids.clear();
}

void Flock::init(int n, ControlledBoid *leader, GLdouble spread)
{
	boids.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> dist(-spread, spread);

	Vec3 center = leader->getPosition();

	for (int i = 0; i < n; i++)
	{
		// Initial position around the leader
		Vec3 p = { center.x + dist(gen), center.y + dist(gen) * 0.01, center.z + dist(gen) };
		auto b = new Boid(p, leader);
		boids.push_back(b);

		// Initial velocity
		std::uniform_real_distribution<GLdouble> vdist(-1.0, 1.0);
		boids.back()->setVelocity(vdist(gen), 0.0, vdist(gen));
		boids.back()->setSize(0.5, 0.1, 0.5);
	}
}

void Flock::update(GLdouble dt)
{
	for (auto b : boids)
		b->update(boids, dt);
}

void Flock::draw()
{
	for (auto b : boids)
		b->draw();
}
