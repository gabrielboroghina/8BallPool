#pragma once

#include "Core/GPU/Mesh.h"
#include "8BallPool/ITargetObserver.h"

class Cue : public ITargetObserver
{
    const glm::vec3 initCueDir = glm::vec3(0, -1, 0);

public:
    Mesh *mesh;
    Texture2D *texture;
    glm::mat4 translateMatrix, rotateMatrix;
    float pullBackDist;
    glm::vec3 cueDir;

    Cue();
    ~Cue();

    glm::mat4 GetModelMatrix() const;
    void PullBack(float dist);
    void UpdatePos(glm::vec3 movement) override;
    void SetTarget(glm::vec3 targetPos, glm::vec3 dir, float distToTarget) override;
};
