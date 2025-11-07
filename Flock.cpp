#include <random>
#include "Flock.h"
#include "vecFunctions.h"

Flock::~Flock()
{
	for (auto b : boids)
		delete b;
	boids.clear();
}

void Flock::init(int n, GLdouble spread)
{
	boids.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> dist(-spread, spread);

	for (int i = 0; i < n; i++)
	{
		Vec3 p = { dist(gen), dist(gen) * 0.2, dist(gen) };
		Boid* b = new Boid(p);
		boids.push_back(b);

		std::uniform_real_distribution<GLdouble> vdist(-1.0, 1.0);
		boids.back()->setVelocity(vdist(gen), 0.0, vdist(gen));
		boids.back()->setSize(0.5, 0.5, 0.5);
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
