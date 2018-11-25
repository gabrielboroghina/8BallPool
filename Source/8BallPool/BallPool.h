#pragma once

#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "UIConstants.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "Objects/Cue.h"
#include "Objects/PoolTable.h"
#include "Objects/Ball.h"
#include <map>
#include <unordered_set>
#include "Objects/Cushion.h"
#include <set>

enum GameState
{
	START,
	BREAK,
	BALL_IN_HAND,
	TURN,
	IN_MOVE
};

struct CollisionPair
{
	Ball *a;
	Object *b;
	float t;

	CollisionPair(Ball *a, Object *b) : a(a), b(b), t(0) {}
};

class BallPool : public SimpleScene
{
public:
	BallPool();
	~BallPool();

	void Init() override;

private:
	Camera *camera;
	GameState gameState;
	Mesh *floorMesh;
	Texture2D *floorTexture;

	// Objects
	Cue *cue;
	PoolTable *poolTable;
	Cushion *cushion[4]; // 0 - left, 1 - top, 2 - right, 3 - bottom
	Ball *yellowBalls[7], *redBalls[7], *blackBall, *cueBall;
	std::unordered_set<Ball *> gameBalls, balls;
	std::map<std::pair<Object *, Object *>, CollisionPair *> pairs;

	int cueShotRunning;
	float cueShotDist;
	float limX, limY;

	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderTexturedMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
	                        const std::vector<Texture2D *> &textures) const;

	void RenderColoredMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color) const;

	void InitBalls();
	void TryMoveCueBall(const glm::vec2 &move);
	void UpdateCue(float deltaTime);
	void ProcessMovements(float deltaTime);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
};
