#pragma once

#include "Core/GPU/Mesh.h"

class Ball
{
public:
	Ball(const glm::vec3 &initialPos);
	~Ball();

	glm::vec2 Get2DPos() const;
	glm::mat4 GetModelMatrix() const;

	glm::vec3 pos;
	Mesh *mesh;

private:
	float velocity, vx, vy;
};
