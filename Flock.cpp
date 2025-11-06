#include <random>
#include "Flock.h"
#include "vecFunctions.h"


void Flock::init(int n, GLdouble spread)
{
	boids.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> dist(-spread, spread);

	for (int i = 0; i < n; i++)
	{
		Vec3 p(dist(gen), dist(gen) * 0.2, dist(gen));
		boids.emplace_back(p);

		// 
		std::uniform_real_distribution<GLdouble> vdist(-1.0, 1.0);
		boids.back().setVelocity(vdist(gen), vdist(gen), vdist(gen));
		boids.back().setSize(0.5, 0.5, 0.5);
	}
}

void Flock::update(GLdouble dt)
{
}

void Flock::draw(){}
