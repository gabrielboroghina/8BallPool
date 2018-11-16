#pragma once

#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "UIConstants.h"
#include "MeshBuilder.h"
#include "Objects/Cue.h"

class BallPool : public SimpleScene
{
public:
	BallPool();
	~BallPool();

	void Init() override;

private:
	Mesh *floorMesh;
	Texture2D *floorTexture, *tex;

	// Objects
	Cue *cue;

	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
	                      const std::vector<Texture2D *> &textures) const;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
};
