#pragma once
#include "Object.h"

class Tower : public Object
{
public:
	Tower();
	Tower(const Vec3 pos, const Vec3 size, const Vec3 color = Color::Red);
	~Tower() = default;

	void draw() override;

private:
	Vec3 fillColor; // Color of the tower
	Vec3 wireColor; // Color of the wireframe
};
