#pragma once

#include <vector>

#include "Obstacle.h"
#include "Floor.h"

class ObstacleManager
{
public:
	ObstacleManager() = default;
	~ObstacleManager() = default;

	// Set the reference floor for obstacle placement
	void setFloor(Floor* floor) { worldFloor = floor, hasFloor = (floor != nullptr); }

	// Add a single obstacle
	void addObstacle();

	// Remove the most recently added obstacle
	void removeObstacle();

	// Remove all obstacles and recreate them at random positions on the floor
	void reset();

	// Generate obstacles randomly placed on the floor
	void generateRandom(int count, unsigned int seed = 0);

	std::vector<Obstacle>& getObstacles() { return obstacles; }
	size_t size() const { return obstacles.size(); }

private:
	std::vector<Obstacle> obstacles; // List of obstacles
	int minObstacleCount = 10;		 // Minimum number of obstacles
	int maxObstacleCount = 200;		 // Maximum number of obstacles

	Floor* worldFloor = nullptr;	 // Reference floor for obstacle placement
	bool hasFloor = false;			 // Flag indicating if floor is set
};
