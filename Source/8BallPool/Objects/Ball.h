#pragma once

#include "Core/GPU/Mesh.h"
#include "8BallPool/ITargetObserver.h"

class Ball
{
public:
	Ball(const glm::vec3 &initialPos);
	~Ball();

	glm::vec2 Get2DPos() const;
	glm::mat4 GetModelMatrix() const;
	void Move(glm::vec3 delta);
	void AttachObserver(ITargetObserver *observer);

	glm::vec3 pos;
	Mesh *mesh;

private:
	float velocity, vx, vy;

	std::vector<ITargetObserver *> observers;// observers of the ball movement
};
