#pragma once

#include "Core/GPU/Mesh.h"
#include "8BallPool/ITargetObserver.h"
#include "Object.h"

enum BallColor
{
    YELLOW,
    RED,
    BLACK,
    WHITE
};

class Ball : public Object
{
    std::vector<ITargetObserver *> observers; // observers of the ball movement
    float pocketingAnimTime;
    float scaleFactor;

public:
    glm::vec3 pos;
    Mesh *mesh;
    glm::vec2 velocity;
    bool isPocketed;
    BallColor color;

    Ball(const glm::vec3 &initialPos, BallColor color);
    ~Ball();

    /** Get ball's position on the table */
    glm::vec2 Get2DPos() const;
    glm::mat4 GetModelMatrix() const;
    void Move(glm::vec3 delta);
    void AttachObserver(ITargetObserver *observer);

    /** Give energy to the ball */
    void ReceiveVelocity(glm::vec2 v);
    void Update(float deltaTime);
    void AnimatePocketing();
    void Restore(glm::vec3 pos);
};
