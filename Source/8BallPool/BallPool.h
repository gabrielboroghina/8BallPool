#pragma once

#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "Camera.h"
#include "Objects/Cue.h"
#include "Objects/PoolTable.h"
#include "Objects/Ball.h"
#include <unordered_set>
#include "Player.h"

enum GameState
{
    START,
    BREAK,
    BALL_IN_HAND,
    TURN,
    IN_MOVE,
    FOUL,
    ENDED
};

struct CollisionPair
{
    Ball *a;
    Object *b;
    float t;

    CollisionPair(Ball *a, Object *b, float t) : a(a), b(b), t(t) {}
};

class BallPool : public SimpleScene
{
public:
    BallPool();
    ~BallPool();

    void Init() override;

private:
    GameState gameState;
    Player player[3];
    int currentPlayer;
    bool scored;

    Camera *camera;
    Mesh *floorMesh;
    Texture2D *floorTexture;

    // Objects
    Cue *cue;
    PoolTable *poolTable;
    Ball *yellowBalls[7], *redBalls[7], *blackBall, *cueBall, *firstTouchedBall;
    std::unordered_set<Ball *> gameBalls, balls;

    int cueShotRunning; // tells if the cue is used in a shot right now
    float cueShotDist;
    float limX, limY; // limits of the game area

    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void SetShaderMVP(const Shader *shader, const glm::mat4 &modelMatrix) const;

    void RenderTexturedMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
                            const std::vector<Texture2D *> &textures) const;

    void RenderColoredMesh(const Mesh *mesh, const glm::mat4 &modelMatrix, const glm::vec3 &color) const;

    void RenderPullBackAnimatedMesh(const Mesh *mesh, const glm::mat4 &modelMatrix, const Texture2D *texture,
                                    const glm::vec3 &pullBackDir, float pullBackDist) const;

    void Foul();
    void InitBalls();
    void TryMoveCueBall(const glm::vec2 &move);
    void UpdateCue(float deltaTime);
    void UpdateBalls(float deltaTime);
    void ProcessMovements(float deltaTime);
    void DisplayStatistics();

    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
    void OnWindowResize(int width, int height) override;
};
