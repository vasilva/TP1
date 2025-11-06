#pragma once

#include "Object.h"

class Tower : public Object
{
public:
	Tower();
	Tower(const Vec3 pos, const Vec3 size);
	~Tower();
	void draw() override;
};
