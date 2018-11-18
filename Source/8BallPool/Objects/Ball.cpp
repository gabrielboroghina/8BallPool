#include "Ball.h"
#include "Core/Managers/ResourcePath.h"
#include "8BallPool/UIConstants.h"


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
	using namespace UIConstants::Ball;
	return scale(translate(glm::mat4(1), pos), glm::vec3(RAD, RAD, RAD));
}
