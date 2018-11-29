#include "BallPool.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>
#include "MeshBuilder.h"

using namespace std;

BallPool::BallPool()
    : cueShotRunning(false),
      currentPlayer(1),
      gameState(START) {}

BallPool::~BallPool()
{
    delete floorTexture;
    delete floorMesh;
    delete cue;
    delete poolTable;
}

void BallPool::Init()
{
    {
        // build floor
        using namespace UIConstants;
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-Floor::LEN, 0, -Floor::LEN), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(-Floor::LEN, 0, Floor::LEN), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(Floor::LEN, 0, Floor::LEN), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(Floor::LEN, 0, -Floor::LEN), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
        };

        vector<unsigned short> indices = {
            0, 1, 2,
            0, 2, 3
        };

        floorMesh = MeshBuilder::CreateMesh("floor", vertices, indices);
        floorTexture = new Texture2D();
        floorTexture->Load2D((RESOURCE_PATH::TEXTURES + "parquet.jpg").c_str(), GL_REPEAT);
    }

    LoadShader("TextureByPos", "TextureByPos", "TextureByPos");
    LoadShader("Texture", "Texture", "Texture");
    LoadShader("PullBackAnim", "PullBackAnim", "Texture");
    LoadShader("LightColor", "Texture", "Color");

    // initialize objects
    InitBalls();
    cue = new Cue();
    poolTable = new PoolTable();

    // initialize camera
    camera = new Camera(60, window->props.aspectRatio);
    camera->SwitchMode(FirstPerson, CameraLayout::TopDown);

    cueBall->AttachObserver(camera);
    cueBall->AttachObserver(cue);

    // compute the limits of the game area
    limX = UIConstants::Table::WIDTH / 2 - UIConstants::Ball::RAD;
    limY = UIConstants::Table::LEN / 2 - UIConstants::Ball::RAD;
}

void BallPool::InitBalls()
{
    for (int i = 0; i < 7; i++) {
        yellowBalls[i] = new Ball(UIConstants::Ball::initYellowBallPos[i], YELLOW);
        redBalls[i] = new Ball(UIConstants::Ball::initRedBallPos[i], RED);

        gameBalls.insert(yellowBalls[i]);
        gameBalls.insert(redBalls[i]);
    }

    blackBall = new Ball(UIConstants::Ball::initBlackBallPos, BLACK);
    gameBalls.insert(blackBall);
    cueBall = new Ball(UIConstants::Ball::initCueBallPos, WHITE);

    balls = gameBalls;
    balls.insert(cueBall);
}

void BallPool::TryMoveCueBall(const glm::vec2 &move)
{
    using namespace UIConstants::Ball;
    float distX = move.x * BALL_IN_HAND_SPEED;
    float distY = move.y * BALL_IN_HAND_SPEED;

    // check cushions touches during the move
    if (cueBall->pos.x + distX < -limX)
        distX = -limX - cueBall->pos.x, distY = distX * move.y / move.x;
    if (cueBall->pos.x + distX > limX)
        distX = limX - cueBall->pos.x, distY = distX * move.y / move.x;
    if (cueBall->pos.z + distY < -limY)
        distY = -limY - cueBall->pos.z, distX = distY * move.x / move.y;
    if (cueBall->pos.z + distY > limY)
        distY = limY - cueBall->pos.z, distX = distY * move.x / move.y;

    // check balls touches during move
    float fmax = 1;
    for (auto &ball : gameBalls) {
        float rx = ball->pos.x - cueBall->pos.x, ry = ball->pos.z - cueBall->pos.z;

        float a = distX * distX + distY * distY;
        float b = -2 * (rx * distX + ry * distY);
        float c = pow(rx, 2) + pow(ry, 2) - 4 * RAD * RAD;
        float d = sqrt(b * b - 4 * a * c);
        float r1 = (-b - d) / (2 * a);

        if (0 < r1 && r1 < 1)
            fmax = min(fmax, r1);
        else if (r1 <= 0 && 0 <= -b / (2 * a))
            fmax = 0;
    }

    if (gameState == BREAK)
        // move the ball only in the first third part of the table
        cueBall->Move(glm::vec3(fmax * distX, 0, std::max(fmax * distY, UIConstants::Table::LEN / 6 + RAD - cueBall->pos.z)));
    else
        cueBall->Move(glm::vec3(fmax * distX, 0, fmax * distY));
}

void BallPool::FrameStart()
{
    // clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void BallPool::UpdateCue(float deltaTime)
{
    // run cue pull-back
    if (cueShotRunning != 0) {
        // animate cue movement
        cue->PullBack(deltaTime * cueShotRunning);

        if (cueShotRunning == UIConstants::Cue::RETURN_SPEED_FACTOR && cue->pullBackDist < 0.1f) {
            // launch cue ball
            cue->pullBackDist = 0;
            cueShotRunning = 0;
            cueBall->ReceiveVelocity(-glm::vec2(cue->cueDir.x, cue->cueDir.z) * (cueShotDist * 6));
        }
    }
}

void BallPool::DisplayStatistics()
{
    cout << "\n Score (Fouls): " << player[1].score << " (" << player[1].fouls << ")  -  "
        << player[2].score << " (" << player[2].fouls << ")\n";
}

void BallPool::UpdateBalls(float deltaTime)
{
    for (auto it = balls.begin(); it != balls.end();) {
        (*it)->Update(deltaTime); // bring the balls to present (move without collisions)

        // check if the ball was pocketed
        if ((*it)->isPocketed && cueBall != *it) {
            if ((*it)->color == BLACK) {
                cout << "---------------\nGAME ENDED!\n---------------\n";
                cout << "Player " << ((gameBalls.size() == 1) ? currentPlayer : 3 - currentPlayer) << " won! :)\n";

                gameState = ENDED;
            }

            if (player[1].color == BLACK) {
                // this is the first pocketed ball
                player[currentPlayer].color = (*it)->color;
                player[3 - currentPlayer].color = ((*it)->color == YELLOW) ? RED : YELLOW;

                for (int i = 1; i <= 2; i++)
                    cout << "\n<<< Player " << i << " color:  " << (player[i].color == YELLOW ? "Yellow" : "Red") << " >>>\n";
            }

            // update score
            for (int i = 1; i <= 2; i++)
                if ((*it)->color == player[i].color) {
                    player[i].score++;

                    if (i == currentPlayer)
                        scored = true;
                }

            DisplayStatistics();

            gameBalls.erase(*it);
            it = balls.erase(it);
        }
        else ++it;
    }
}

void BallPool::ProcessMovements(float deltaTime)
{
    using namespace UIConstants::Ball;
    CollisionPair firstCollision(nullptr, nullptr, deltaTime + 1);

    const auto CheckBallCushionCollision = [&](Ball *ball, int cushionIndex, float timeToCollision) {
        if (timeToCollision <= deltaTime && timeToCollision < firstCollision.t)
            firstCollision = CollisionPair(ball, poolTable->cushions[cushionIndex], timeToCollision);
    };

    const auto DetectBallChusionCollision = [&](Ball *ball) {
        float vx = ball->velocity.x, vy = ball->velocity.y;
        if (vx < 0) // left cushion
            CheckBallCushionCollision(ball, 0, (-vx - sqrt(pow(vx, 2) + 2 * ACC * (-limX - ball->pos.x))) / ACC);
        if (vx > 0) // right cushion
            CheckBallCushionCollision(ball, 2, (-vx + sqrt(pow(vx, 2) + 2 * ACC * (limX - ball->pos.x))) / ACC);
        if (vy < 0) // top cushion
            CheckBallCushionCollision(ball, 1, (-vy - sqrt(pow(vy, 2) + 2 * ACC * (-limY - ball->pos.z))) / ACC);
        if (vy > 0) // bottom cushion
            CheckBallCushionCollision(ball, 3, (-vy + sqrt(pow(vy, 2) + 2 * ACC * (limY - ball->pos.z))) / ACC);
    };

    const auto DetectBallBallCollision = [&](Ball *ball, Ball *ball2) {
        float px = ball->pos.x - ball2->pos.x, py = ball->pos.z - ball2->pos.z;
        float Vx = ball->velocity.x - ball2->velocity.x, Vy = ball->velocity.y - ball2->velocity.y;

        // 2nd grade function: the distance between the balls function of time
        float a = Vx * Vx + Vy * Vy;
        if (a == 0) return;
        float b = 2 * (px * Vx + py * Vy);
        float c = px * px + py * py - 4 * RAD * RAD;
        float d = b * b - 4 * a * c;
        if (d < 0) return;
        d = sqrt(d);
        float r1 = (-b - d) / (2 * a);

        if (0 < r1 && r1 <= deltaTime && r1 < firstCollision.t)
            firstCollision = CollisionPair(ball, ball2, r1);
    };

    const auto DetectBallPocketCollision = [&](Ball *ball, Pocket *pocket) {
        float px = ball->pos.x - pocket->pos.x, py = ball->pos.z - pocket->pos.z;
        float Vx = ball->velocity.x, Vy = ball->velocity.y;

        // 2nd grade function: the distance between the ball and the pocket
        float a = Vx * Vx + Vy * Vy;
        if (a == 0) return;
        float b = 2 * (px * Vx + py * Vy);
        float c = px * px + py * py - pow(UIConstants::Table::CORNER_RAD, 2);
        float d = b * b - 4 * a * c;
        if (d < 0) return;
        d = sqrt(d);
        float r1 = (-b - d) / (2 * a);

        if (0 < r1 && r1 <= deltaTime && r1 < firstCollision.t)
            firstCollision = CollisionPair(ball, pocket, r1);
    };

    const auto FindAllCollisions = [&]() {
        firstCollision.t = deltaTime + 1;
        for (auto &b : balls)
            DetectBallChusionCollision(b);

        for (auto it = balls.begin(); it != balls.end(); ++it) {
            auto it2 = it;
            ++it2;
            for (; it2 != balls.end(); ++it2)
                DetectBallBallCollision(*it, *it2);
        }

        for (auto &ball : balls)
            for (int i = 0; i < 6; i++)
                DetectBallPocketCollision(ball, poolTable->pockets[i]);
    };

    FindAllCollisions();
    while (firstCollision.t <= deltaTime) {
        // update game logic state
        deltaTime -= firstCollision.t;
        UpdateBalls(firstCollision.t);

        // process the first collision
        Ball *ball = firstCollision.a;
        switch (firstCollision.b->type) {
            case CUSHION:
                // ball-cushion collision
                switch (static_cast<Cushion *>(firstCollision.b)->type) {
                    case 0:
                    case 2:
                        ball->velocity.x *= -1;
                        break;
                    case 1:
                    case 3:
                        ball->velocity.y *= -1;
                        break;
                }
                break;

            case BALL: {
                // ball-ball collision
                Ball *ball2 = static_cast<Ball *>(firstCollision.b);

                glm::vec2 dv(ball->velocity - ball2->velocity);
                glm::vec2 dx(ball->pos.x - ball2->pos.x, ball->pos.z - ball2->pos.z);
                ball->velocity -= glm::dot(dv, dx) * dx / pow(glm::length(dx), 2);
                ball2->velocity -= glm::dot(-dv, -dx) * (-dx) / pow(glm::length(dx), 2);

                // check if this is the first ball-ball collision in the current turn
                if (firstTouchedBall == nullptr)
                    firstTouchedBall = (ball == cueBall) ? ball2 : ball;
            }
            break;

            case POCKET:
                ball->AnimatePocketing();
        }

        FindAllCollisions();
    }

    UpdateBalls(deltaTime); // last update (without collisions)
}

void BallPool::Update(float deltaTimeSeconds)
{
    ProcessMovements(deltaTimeSeconds);
    RenderTexturedMesh(floorMesh, shaders["TextureByPos"], glm::mat4(1), {floorTexture});

    UpdateCue(deltaTimeSeconds);
    if (gameState != IN_MOVE && gameState != START)
        RenderPullBackAnimatedMesh(cue->mesh, cue->GetModelMatrix(),
                                   cue->texture, cue->cueDir, cue->pullBackDist);

    for (auto comp : poolTable->texComps)
        RenderTexturedMesh(comp.mesh, shaders["Texture"], comp.modelMat, {comp.texture});

    for (auto comp : poolTable->colorComps)
        RenderColoredMesh(comp.mesh, comp.modelMat, comp.color);

    // render balls
    for (int i = 0; i < 7; i++) {
        RenderColoredMesh(yellowBalls[i]->mesh, yellowBalls[i]->GetModelMatrix(), glm::vec3(1, 1, 0));
        RenderColoredMesh(redBalls[i]->mesh, redBalls[i]->GetModelMatrix(), glm::vec3(1, 0, 0));
    }

    RenderColoredMesh(blackBall->mesh, blackBall->GetModelMatrix(), glm::vec3(0, 0, 0));
    RenderColoredMesh(cueBall->mesh, cueBall->GetModelMatrix(), glm::vec3(1, 1, 1));
}

void BallPool::FrameEnd() {}

void BallPool::SetShaderMVP(const Shader *shader, const glm::mat4 &modelMatrix) const
{
    // render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light & material properties
    // Bind light position
    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(UIConstants::Light::LIGHT_POS));

    // Bind eye position (camera position)
    glm::vec3 eyePosition = modelMatrix * glm::vec4(camera->position, 1);
    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

    // Bind material property uniforms (shininess, kd, ks) 
    glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), UIConstants::Light::MATERIAL_SHININESS);
    glUniform1f(glGetUniformLocation(shader->program, "material_kd"), UIConstants::Light::MATERIAL_KD);
    glUniform1f(glGetUniformLocation(shader->program, "material_ks"), UIConstants::Light::MATERIAL_KS);

    // Bind model, view and projection matrices
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
}

void BallPool::RenderTexturedMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
                                  const vector<Texture2D *> &textures) const
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    SetShaderMVP(shader, modelMatrix);

    // Bind textures
    for (unsigned i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]->GetTextureID());
        string textureName = "textureImg" + to_string(i);
        glUniform1i(glGetUniformLocation(shader->GetProgramID(), textureName.c_str()), i);
    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void BallPool::RenderColoredMesh(const Mesh *mesh, const glm::mat4 &modelMatrix, const glm::vec3 &color) const
{
    Shader *shader = shaders.at("LightColor");
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    SetShaderMVP(shader, modelMatrix);

    // Bind color vector
    glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(color));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void BallPool::RenderPullBackAnimatedMesh(const Mesh *mesh, const glm::mat4 &modelMatrix, const Texture2D *texture,
                                          const glm::vec3 &pullBackDir, float pullBackDist) const
{
    const Shader *shader = shaders.at("PullBackAnim");
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    SetShaderMVP(shader, modelMatrix);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->GetProgramID(), "textureImg0"), 0);

    // Bind pull back direction vector and distance
    glUniform3fv(glGetUniformLocation(shader->program, "pull_back_dir"), 1, glm::value_ptr(pullBackDir));
    glUniform1f(glGetUniformLocation(shader->program, "pull_back_dist"), pullBackDist);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void BallPool::OnInputUpdate(float deltaTime, int mods)
{
    // process camera movement
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && camera->type == FirstPerson)
        camera->Update(deltaTime, window->KeyHold(GLFW_KEY_W), window->KeyHold(GLFW_KEY_A), window->KeyHold(GLFW_KEY_S),
                       window->KeyHold(GLFW_KEY_D), window->KeyHold(GLFW_KEY_Q), window->KeyHold(GLFW_KEY_E));

    if ((gameState == BALL_IN_HAND || gameState == BREAK) && !window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        if (window->KeyHold(GLFW_KEY_W))
            TryMoveCueBall(glm::vec2(0, -deltaTime));
        else if (window->KeyHold(GLFW_KEY_A))
            TryMoveCueBall(glm::vec2(-deltaTime, 0));
        else if (window->KeyHold(GLFW_KEY_S))
            TryMoveCueBall(glm::vec2(0, deltaTime));
        else if (window->KeyHold(GLFW_KEY_D))
            TryMoveCueBall(glm::vec2(deltaTime, 0));
    }
}

void BallPool::Foul()
{
    cout << "FOUL\n";
    player[currentPlayer].fouls++;

    DisplayStatistics();
    gameState = BALL_IN_HAND;
}

void BallPool::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE) {
        // update game state
        if (gameState == ENDED)
            return;
        if (gameState == START)
            gameState = BREAK;
        else if (gameState == IN_MOVE) {
            if (firstTouchedBall == nullptr ||
                (firstTouchedBall && firstTouchedBall->color == player[3 - currentPlayer].color) ||
                cueBall->isPocketed)
                Foul();
            else
                gameState = TURN;

            if (cueBall->isPocketed)
                cueBall->Restore(UIConstants::Ball::initCueBallPos);

            if (!scored)
                currentPlayer = 3 - currentPlayer; // switch current player
        }

        scored = false;
        firstTouchedBall = nullptr;

        cout << "-----------------------------------------\n";
        cout << "\n-> Player " << currentPlayer << ":\n";

        // switch to <third person> camera mode
        camera->SwitchMode(CameraType::ThirdPerson);
        camera->SetTarget(cueBall->pos, UIConstants::Camera::LOOKING_DIR_THIRD_PERSON, UIConstants::Camera::DIST_TO_TARGET);
        cue->SetTarget(cueBall->pos, camera->GetPosFromCameraSpace(UIConstants::Cue::DIR_CAMERA_SPACE), UIConstants::Ball::RAD);
    }
}

void BallPool::OnKeyRelease(int key, int mods) {}

void BallPool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        // rotate the camera around OX and OY
        camera->RotateOX(-deltaY * UIConstants::Camera::SENSITIVITY);
        camera->RotateOY(-deltaX * UIConstants::Camera::SENSITIVITY);

        cue->SetTarget(cueBall->pos, camera->GetPosFromCameraSpace(UIConstants::Cue::DIR_CAMERA_SPACE), UIConstants::Ball::RAD);
    }
}

void BallPool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && (gameState == TURN || gameState == BREAK || gameState == BALL_IN_HAND))
        cueShotRunning = UIConstants::Cue::PULL_BACK_SPEED_FACTOR;
}

void BallPool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        cueShotDist = cue->pullBackDist;
        cueShotRunning = UIConstants::Cue::RETURN_SPEED_FACTOR;

        camera->SwitchMode(FirstPerson, CameraLayout::TopDown);
        gameState = IN_MOVE;
    }
}

void BallPool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void BallPool::OnWindowResize(int width, int height) {}
