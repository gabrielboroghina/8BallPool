#include "Ball.h"

#include "Core/Managers/ResourcePath.h"
#include "8BallPool/UIConstants.h"
#include <algorithm>

using namespace UIConstants::Ball;

Ball::Ball(const glm::vec3 &initialPos, BallColor color) :
    color(color), pos(initialPos), velocity(0), scaleFactor(2 * RAD), isPocketed(false), Object(BALL)
{
    mesh = new Mesh("ball");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
}

Ball::~Ball()
{
    delete mesh;
}

glm::vec2 Ball::Get2DPos() const
{
    return glm::vec2(pos.x, pos.z);
}

glm::mat4 Ball::GetModelMatrix() const
{
    return scale(translate(glm::mat4(1), pos), glm::vec3(scaleFactor));
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
    // update pocketing animation (if active)
    if (pocketingAnimTime > 0) {
        float delta = 3 * deltaTime;
        if (pocketingAnimTime < delta) {
            pocketingAnimTime = 0;
            isPocketed = true;
        }
        else pocketingAnimTime -= delta;

        pos.y -= delta;
    }
    else {
        // update the position considering the friction with the table
        float v_abs = glm::length(velocity);
        if (v_abs > 0) {
            float t = std::max(v_abs / ACC, deltaTime);
            glm::vec2 movement = glm::normalize(velocity) * (std::max(v_abs + ACC * t / 2, 0.0f) * t);
            pos.x += movement.x;
            pos.z += movement.y;
            velocity = glm::normalize(velocity) * std::max(v_abs + ACC * t, 0.0f);
        }
    }
}

void Ball::AnimatePocketing()
{
    pocketingAnimTime = 2 * RAD;
    velocity = glm::vec3(0);
}

void Ball::Restore(glm::vec3 pos)
{
    this->pos = pos;
    isPocketed = false;
}
