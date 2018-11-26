#include "Ball.h"

#include "Core/Managers/ResourcePath.h"
#include "8BallPool/UIConstants.h"
#include <algorithm>

using namespace UIConstants::Ball;

Ball::Ball(const glm::vec3 &initialPos) : pos(initialPos), velocity(0)
{
	mesh = new Mesh("ball");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
}


Ball::~Ball() {}

glm::vec2 Ball::Get2DPos() const
{
	return glm::vec2(pos.x, pos.z);
}

glm::mat4 Ball::GetModelMatrix() const
{
	float scaleFactor = 2 * RAD;
	return scale(translate(glm::mat4(1), pos), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
}

void Ball::Move(glm::vec3 delta)
{
	pos += delta;

	for (auto &observer : observers)
		observer->UpdatePos(delta);
}

void Ball::AttachObserver(ITargetObserver *observer)
{
	observers.push_back(observer);
}

void Ball::ReceiveVelocity(glm::vec2 v)
{
	velocity += v;
}

void Ball::Update(float deltaTime)
{
	// update the effect of friction
	float v_abs = glm::length(velocity);
	if (v_abs > 0) {
		float t = std::max(v_abs / ACC, deltaTime);
		glm::vec2 movement = glm::normalize(velocity) * (std::max(v_abs + ACC * t / 2, 0.0f) * t);
		pos.x += movement.x;
		pos.z += movement.y;
		velocity = glm::normalize(velocity) * std::max(v_abs + ACC * t, 0.0f);
	}
}
