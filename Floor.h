#pragma once

#include "Object.h"

class Floor : public Object
{
public:
	Floor();
	Floor(const Vec3 pos, const Vec3 size);
	~Floor();
	void draw() override;
};
