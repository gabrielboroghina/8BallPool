#pragma once

class ITargetObserver
{
public:
    virtual ~ITargetObserver() = 0 {};

    virtual void SetTarget(glm::vec3 targetPos, glm::vec3 dir, float distToTarget) = 0;
    virtual void UpdatePos(glm::vec3 movement) = 0;
};
