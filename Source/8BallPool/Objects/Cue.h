#pragma once

#include "../UIConstants.h"
#include "Core/GPU/Mesh.h"
#include "8BallPool/ITargetObserver.h"

class Cue : public ITargetObserver
{
public:
	Mesh *mesh;
	std::vector<Texture2D *> textures;
	glm::mat4 translateMatrix, rotateMatrix;

public:
	Cue();
	~Cue();

	std::vector<Texture2D *> GetTextures() const;
	glm::mat4 GetModelMatrix() const;

	void UpdatePos(glm::vec3 movement) override;

	void SetTarget(glm::vec3 targetPos, glm::vec3 dir, float distToTarget) override;
};
