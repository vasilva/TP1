#pragma once
#include "Object.h"

class Floor : public Object
{
public:
	Floor();
	Floor(const Vec3 pos, const Vec3 size, const Vec3 color = Color::DarkGreen);
	~Floor() = default;
	void draw() override;

private:
	Vec3 fillColor; // Color of the floor
	Vec3 wireColor; // Color of the wireframe
};
