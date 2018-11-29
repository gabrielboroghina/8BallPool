#pragma once

#include <include/glm.h>

namespace UIConstants
{
namespace Camera
{
    const float SENSITIVITY = 0.001f;
    const float MOVE_SPEED = 5;
    const float DIST_TO_TARGET = 2.5f;
    const glm::vec3 LOOKING_DIR_FIRST_PERSON = glm::vec3(0, 1, 0);
    const glm::vec3 LOOKING_DIR_THIRD_PERSON = glm::vec3(0, 0.5f, 0.45f);
}

namespace Light
{
    const glm::vec3 LIGHT_POS = glm::vec3(0, 2.5f, 0);
    const int MATERIAL_SHININESS = 30;
    const float MATERIAL_KD = 2;
    const float MATERIAL_KS = 1.4f;
}

namespace Cue
{
    const float LENGTH = 1.5f;
    const float SMALL_RADIUS = 0.006f;
    const float BIG_RADIUS = 0.025f;

    const glm::vec3 DIR_CAMERA_SPACE(-1, -0.4f, 0.0f);

    const int PULL_BACK_SPEED_FACTOR = 1;
    const int RETURN_SPEED_FACTOR = -10;
    const float MAX_PULLBACK_DIST = 1.5f;
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
    const float CORNER_RAD = 0.13f;
    const float MARGIN_W = 0.1f;
    const float MARGIN_H = 0.3f;

    const float POCKET_Z = LEN / 2 - 0.05f;
    const float POCKET_X = WIDTH / 2 - 0.05f;
    const float POCKET_X_CENTER = WIDTH / 2 - 0.02f;

    const glm::vec3 wallColor(0.06f, 0.02f, 0.0f);
}

namespace Ball
{
    const float RAD = 0.07f;
    const float YPOS = Table::HEIGHT + Ball::RAD;

    const float headZ = -0.31f * Table::LEN;
    const float distX = RAD + 0.002f;
    const float distZ = distX * (float)sqrt(3);

    const glm::vec3 initYellowBallPos[7] = {
        glm::vec3(-distX, YPOS, headZ - distZ),
        glm::vec3(2 * distX, YPOS, headZ - 2 * distZ),
        glm::vec3(3 * distX, YPOS, headZ - 3 * distZ),
        glm::vec3(-3 * distX, YPOS, headZ - 3 * distZ),
        glm::vec3(-4 * distX, YPOS, headZ - 4 * distZ),
        glm::vec3(2 * distX, YPOS, headZ - 4 * distZ),
        glm::vec3(0, YPOS, headZ - 4 * distZ),
    };

    const glm::vec3 initRedBallPos[7] = {
        glm::vec3(0, YPOS, headZ),
        glm::vec3(distX, YPOS, headZ - distZ),
        glm::vec3(-2 * distX, YPOS, headZ - 2 * distZ),
        glm::vec3(-distX, YPOS, headZ - 3 * distZ),
        glm::vec3(distX, YPOS, headZ - 3 * distZ),
        glm::vec3(4 * distX, YPOS, headZ - 4 * distZ),
        glm::vec3(-2 * distX, YPOS, headZ - 4 * distZ),
    };

    const glm::vec3 initBlackBallPos = glm::vec3(0, YPOS, headZ - 2 * distZ);
    const glm::vec3 initCueBallPos = glm::vec3(0, YPOS, -headZ);

    const float BALL_IN_HAND_SPEED = 1.5f;
    const float ACC = -1.6f; // friction acceleration
}
}
