#pragma once

#include <glm/detail/type_vec3.hpp>
#include <vector>

namespace UIConstants
{
namespace Camera
{
	const float SENSITIVITY = 0.001f;
	const float MOVE_SPEED = 5;
}

namespace Cue
{
	const float LENGTH = 1.5f;
	const float SMALL_RADIUS = 0.006f;
	const float BIG_RADIUS = 0.025f;
}

namespace Floor
{
	const float LEN = 10;
}

namespace Table
{
	const float LEN = 4.5f;
	const float WIDTH = 2.5f;
	const float HEIGHT = 1.0f;
	const float CORNER_RAD = 0.1f;
	const glm::vec3 wallColor(0.06f, 0.02f, 0.0f);
}

namespace Ball
{
	const float WEIGHT = 0.16f; // kg
	const float RAD = 0.1f;
	const float YPOS = Table::HEIGHT + Ball::RAD;

	const float headZ = -0.31f * Table::LEN;
	const float distZ = RAD * (float)sqrt(3) / 2, distX = RAD / 2;

	const glm::vec3 initYellowBallPos[7] = {
		glm::vec3(-distX, YPOS, headZ - distZ),
		glm::vec3(RAD, YPOS, headZ - 2 * distZ),
		glm::vec3(3 * distX, YPOS, headZ - 3 * distZ),
		glm::vec3(-3 * distX, YPOS, headZ - 3 * distZ),
		glm::vec3(-2 * RAD, YPOS, headZ - 4 * distZ),
		glm::vec3(1 * RAD, YPOS, headZ - 4 * distZ),
		glm::vec3(0, YPOS, headZ - 4 * distZ),
	};

	const glm::vec3 initRedBallPos[7] = {
		glm::vec3(0, YPOS, headZ),
		glm::vec3(distX, YPOS, headZ - distZ),
		glm::vec3(-RAD, YPOS, headZ - 2 * distZ),
		glm::vec3(-distX, YPOS, headZ - 3 * distZ),
		glm::vec3(distX, YPOS, headZ - 3 * distZ),
		glm::vec3(2 * RAD, YPOS, headZ - 4 * distZ),
		glm::vec3(-RAD, YPOS, headZ - 4 * distZ),
	};

	const glm::vec3 initBlackBallPos = glm::vec3(0, YPOS, headZ - 2 * distZ);
	const glm::vec3 initCueBallPos = glm::vec3(0, YPOS, -headZ);

	const float BALL_IN_HAND_SPEED = 1.5f;
}
}
