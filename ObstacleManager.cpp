#include <random>
#include <GL/glut.h>

#include "ObstacleManager.h"
#include "vecFunctions.h"

// Set the floor for obstacle placement
void ObstacleManager::setFloor(const Floor& floor)
{
	worldFloor = floor;
	hasFloor = true;
}

// Add a single obstacle at a random position on the floor
void ObstacleManager::addObstacle()
{
	if (size() >= static_cast<size_t>(maxObstacleCount))
		return; // Max reached

	// RNG
	auto floorSize = hasFloor ? worldFloor.getSize() : Vec3{ 1000.0, 0.0, 1000.0 };
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> distPos(-floorSize.x * 0.5, floorSize.x * 0.5);
	std::uniform_real_distribution<GLdouble> distSize(5.0, 30.0);

	// Generate random size and position
	GLdouble s = distSize(gen);
	Vec3 size = { s, s * 2.0, s };

	GLdouble px = distPos(gen);
	GLdouble pz = distPos(gen);

	// ensure not too close to center
	auto distCenter2 = px * px + pz * pz;
	if (distCenter2 < 400.0) // min distance 20 units
	{
		px += (px < 0.0) ? -20.0 : 20.0;
		pz += (pz < 0.0) ? -20.0 : 20.0;
	}
	// Create obstacle
	Vec3 pos = { px, size.y * 0.5, pz };
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
		generateRandom(worldFloor);
	else
		generateRandom(Floor());
}

// Generate obstacles randomly placed on the floor
void ObstacleManager::generateRandom(const Floor& floor, int count, unsigned int seed)
{
	obstacles.clear();
	
	// Clamp count
	if (count < minObstacleCount) count = minObstacleCount;
	else if (count > maxObstacleCount) count = maxObstacleCount;
	
	// RNG
	std::random_device rd;
	std::mt19937 gen(seed == 0 ? rd() : seed);
	auto floorSize = hasFloor ? worldFloor.getSize() : Vec3{ 1000.0, 0.0, 1000.0 };

	std::uniform_real_distribution<GLdouble> distPos(-floorSize.x * 0.5, floorSize.x * 0.5);
	std::uniform_real_distribution<GLdouble> distSize(5.0, 30.0);

	for (int i = 0; i < count; ++i)
	{
		GLdouble s = distSize(gen);
		Vec3 size = { s, s * 2.0, distSize(gen) };

		GLdouble px = distPos(gen);
		GLdouble pz = distPos(gen);

		// ensure not too close to center
		auto distCenter2 = px * px + pz * pz;
		if (distCenter2 < 400.0) // min distance 20 units
		{
			px += (px < 0.0) ? -20.0 : 20.0;
			pz += (pz < 0.0) ? -20.0 : 20.0;
		}

		Vec3 pos = { px, size.y * 0.5, pz };
		obstacles.emplace_back(pos, size);
		obstacles.back().enableCollision();
	}
}
