#pragma once

#include "Core/GPU/Mesh.h"
#include "8BallPool/ITargetObserver.h"
#include "Object.h"

class Ball : public Object
{
    std::vector<ITargetObserver *> observers; // observers of the ball movement

public:
    glm::vec3 pos;
    Mesh *mesh;
    glm::vec2 velocity;

    Ball(const glm::vec3 &initialPos);
    ~Ball();

    glm::vec2 Get2DPos() const;
    glm::mat4 GetModelMatrix() const;
    void Move(glm::vec3 delta);
    void AttachObserver(ITargetObserver *observer);
    void ReceiveVelocity(glm::vec2 v);
    void Update(float deltaTime);
};
