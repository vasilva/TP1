#include <random>
#include <GL/glut.h>

#include "ObstacleManager.h"
#include "vecFunctions.h"

// Add a single obstacle at a random position on the floor
void ObstacleManager::addObstacle()
{
	if (size() >= static_cast<size_t>(maxObstacleCount))
		return; // Max reached

	// RNG
	auto floorSize = hasFloor ? worldFloor->getSize() : Vec3{ 1000.0f, 0.0f, 1000.0f };
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLfloat> distPos(-floorSize.x * 0.5f, floorSize.x * 0.5f);
	std::uniform_real_distribution<GLfloat> distSize(5.0f, 30.0f);

	// Generate random size and position
	GLfloat s = distSize(gen);
	Vec3 size = { s, s * 2.0f, s };

	GLfloat px = distPos(gen);
	GLfloat pz = distPos(gen);

	// ensure not too close to center
	auto distCenter2 = px * px + pz * pz;
	if (distCenter2 < 400.0f) // min distance 20 units
	{
		px += (px < 0.0f) ? -20.0f : 20.0f;
		pz += (pz < 0.0f) ? -20.0f : 20.0f;
	}
	// Create obstacle
	Vec3 pos = { px, size.y * 0.5f, pz };
	obstacles.emplace_back(pos, size);
	obstacles.back().enableCollision();
}

// Remove the most recently added obstacle
void ObstacleManager::removeObstacle()
{
	if (size() <= static_cast<size_t>(minObstacleCount))
		return; // Min reached
	obstacles.pop_back();
}

// Remove all obstacles and recreate them at random positions on the floor
void ObstacleManager::reset()
{
	obstacles.clear();

	// Regenerate
	if (hasFloor)
		generateRandom(100);
}

// Generate obstacles randomly placed on the floor
void ObstacleManager::generateRandom(int count, unsigned int seed)
{
	obstacles.clear();
	
	// Clamp count
	if (count < minObstacleCount) count = minObstacleCount;
	else if (count > maxObstacleCount) count = maxObstacleCount;
	
	// RNG
	std::random_device rd;
	std::mt19937 gen(seed == 0 ? rd() : seed);
	auto floorSize = hasFloor ? worldFloor->getSize() : Vec3{ 1000.0f, 0.0f, 1000.0f };

	std::uniform_real_distribution<GLfloat> distPos(-floorSize.x * 0.5f, floorSize.x * 0.5f);
	std::uniform_real_distribution<GLfloat> distSize(5.0f, 30.0f);

	for (int i = 0; i < count; ++i)
	{
		GLfloat s = distSize(gen);
		Vec3 size = { s, s * 2.0f, distSize(gen) };

		GLfloat px = distPos(gen);
		GLfloat pz = distPos(gen);

		// ensure not too close to center
		auto distCenter2 = px * px + pz * pz;
		if (distCenter2 < 400.0f) // min distance 20 units
		{
			px += (px < 0.0f) ? -20.0f : 20.0f;
			pz += (pz < 0.0f) ? -20.0f : 20.0f;
		}

		Vec3 pos = { px, size.y * 0.5f, pz };
		obstacles.emplace_back(pos, size);
		obstacles.back().enableCollision();
	}
}
