#pragma once

#include <vector>

#include "Obstacle.h"
#include "Floor.h"

class ObstacleManager
{
public:
	ObstacleManager() = default;
	~ObstacleManager() = default;

	void setFloor(const Floor& floor);

	// Add a single obstacle
	void addObstacle(); 

	// Remove the most recently added obstacle
	void removeObstacle();

	// Remove all obstacles and recreate them at random positions on the floor
	void reset();

	// Generate obstacles randomly placed on the floor
	void generateRandom(const Floor& floor, int count = 100, unsigned int seed = 0);

	// Access underlying vector for compatibility with existing code
	std::vector<Obstacle>& getObstacles() { return obstacles; }

	// Convenience
	size_t size() const { return obstacles.size(); }

private:
	std::vector<Obstacle> obstacles; // List of obstacles
	int minObstacleCount = 10;		 // Minimum number of obstacles
	int maxObstacleCount = 200;		 // Maximum number of obstacles

	Floor worldFloor;		// Reference floor for obstacle placement
	bool hasFloor = false;	// Flag indicating if floor is set
};
