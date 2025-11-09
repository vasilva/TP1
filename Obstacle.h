#pragma once
#include "Object.h"

// Obstacle class representing static obstacles in the environment
class Obstacle : public Object
{
public:
	Obstacle();
	Obstacle(const Vec3 pos, const Vec3 size, const Vec3 color = Color::DarkGray);
	~Obstacle() = default; 
	void draw() override;

private:
	Vec3 fillColor; // Color of the obstacle
	Vec3 wireColor; // Color of the wireframe
};
