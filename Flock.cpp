#include <random>
#include "Flock.h"
#include "vecFunctions.h"

Flock::~Flock()
{
	for (auto b : boids)
		delete b;
	boids.clear();
}

// Initialize the flock with a number of boids around a leader
void Flock::init(int n, ControlledBoid* leader, GLfloat spread)
{
	// Clear existing boids
	for (auto b : boids) delete b;
	boids.clear();

	// Validate leader and number of boids
	if (!leader) return;
	if (n < minBoids) n = minBoids;
	if (n > maxBoids) n = maxBoids;

	leaderBoid = leader;

	// Random number generation for initial positions
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLfloat> dist(-spread, spread);
	std::uniform_real_distribution<GLfloat> vdist(-1.0f, 1.0f);
	Vec3 center = leader->getPosition();

	for (int i = 0; i < n; i++)
	{
		// Initial position around the leader
		Vec3 pos = center + Vec3(dist(gen), dist(gen) * 0.1f, dist(gen));
		auto b = new Boid(pos, leader);
		boids.push_back(b);

		// Initial velocity
		boids.back()->setVelocity(vdist(gen), 0.0f, vdist(gen));
		boids.back()->setSize(0.5f, 0.1f, 0.5f);
	}
}

// Add a new boid to the flock near the leader
void Flock::addBoid()
{
	// Validate leader and flock size
	if (!leaderBoid) return;
	if (boids.size() >= static_cast<size_t>(maxBoids)) return;
	
	// Add a new boid near the leader
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLfloat> dist(-10.0f, 10.0f);
	std::uniform_real_distribution<GLfloat> vdist(-1.0f, 1.0f);

	Vec3 center = leaderBoid->getPosition();
	Vec3 pos = center + Vec3(dist(gen), dist(gen) * 0.1f, dist(gen));
	auto b = new Boid(pos, leaderBoid);
	b->setVelocity(vdist(gen), 0.0f, vdist(gen));
	b->setSize(0.5f, 0.1f, 0.5f);
	boids.push_back(b);
}

// Remove a boid from the flock
void Flock::removeBoid()
{
	// Validate flock size
	if (boids.size() <= static_cast<size_t>(minBoids)) return;
	
	// Remove the last boid
	auto b = boids.back();
	boids.pop_back();
	delete b;
}

Vec3 Flock::getAvgPosition() const
{
	auto pos = Zero;
	if (boids.empty()) return pos;
	auto count = static_cast<GLfloat>(boids.size());
	for (auto b : boids)
	{
		if (!b) continue;
		pos += b->getPosition();
	}
	if (count > 0.0f) pos /= count;
	return pos;
}

// Update all boids in the flock
void Flock::update(GLfloat dt)
{
	for (auto b : boids)
		b->update(boids, dt);
}

// Draw all boids in the flock
void Flock::draw()
{
	for (auto b : boids)
		b->draw();
}
