#include "BallPool.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>
#include <set>

using namespace std;

const auto cmpTime = [](const CollisionPair *x, const CollisionPair *y) { return x->t < y->t; };
std::set<CollisionPair *, decltype(cmpTime)> collisions(cmpTime); // collisions sorting (by time) set

BallPool::BallPool() : cueShotRunning(false) {}

BallPool::~BallPool() {}

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
		floorTexture->Load2D((RESOURCE_PATH::TEXTURES + "floor.jpg").c_str(), GL_REPEAT);
	}

	// Load shader for texture by position
	LoadShader("TextureByPos");
	LoadShader("Texture");

	// initialize objects
	for (int i = 0; i < 4; i++)
		cushion[i] = new Cushion(i);

	InitBalls();
	cue = new Cue();
	poolTable = new PoolTable();

	// init camera
	camera = new Camera(60, window->props.aspectRatio);
	camera->SwitchMode(FirstPerson, CameraLayout::TopDown);

	cueBall->AttachObserver(camera);
	cueBall->AttachObserver(cue);

	gameState = START;

	// compute some helpful constants
	limX = UIConstants::Table::WIDTH / 2 - UIConstants::Ball::RAD;
	limY = UIConstants::Table::LEN / 2 - UIConstants::Ball::RAD;
}

void BallPool::InitBalls()
{
	for (int i = 0; i < 7; i++) {
		yellowBalls[i] = new Ball(UIConstants::Ball::initYellowBallPos[i]);
		redBalls[i] = new Ball(UIConstants::Ball::initRedBallPos[i]);

		gameBalls.insert(yellowBalls[i]);
		gameBalls.insert(redBalls[i]);
	}

	blackBall = new Ball(UIConstants::Ball::initBlackBallPos);
	gameBalls.insert(blackBall);
	cueBall = new Ball(UIConstants::Ball::initCueBallPos);

	balls = gameBalls;
	balls.insert(cueBall);

	// init collisions pairs
	for (auto &ball : balls) {
		for (int i = 0; i < 4; i++)
			pairs[make_pair(ball, cushion[i])] = new CollisionPair(ball, cushion[i]);

		for (auto &ball2 : balls)
			if (ball != ball2)
				pairs[make_pair(ball, ball2)] = new CollisionPair(ball, ball2);
	}
}

void BallPool::TryMoveCueBall(const glm::vec2 &move)
{
	using namespace UIConstants::Ball;
	using namespace UIConstants::Table;
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
		float a = distX * distX + distY * distY;
		float rx = ball->pos.x - cueBall->pos.x, ry = ball->pos.z - cueBall->pos.z;
		float b = -2 * (rx * distX + ry * distY);
		float c = pow(rx, 2) + pow(ry, 2) - 4 * RAD * RAD;
		float d = sqrt(b * b - 4 * a * c);
		float r1 = (-b - d) / (2 * a);
		if (0 < r1 && r1 < 1)
			fmax = min(fmax, r1);
		else if (r1 <= 0 && 0 <= -b / (2 * a))
			fmax = 0;
	}

	cueBall->Move(glm::vec3(fmax * distX, 0, /*max*/(fmax * distY/* LEN / 6 + RAD - cueBall->pos.z*/)));
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
			cueBall->ReceiveVelocity(-glm::vec2(cue->cueDir.x, cue->cueDir.z) * (cueShotDist * 4));
		}
	}
}

void BallPool::ProcessMovements(float deltaTime)
{
	using namespace UIConstants::Ball;
	collisions.clear();

	float ttc; // time to collision
	const auto InsertBallCushionCollision = [&](Ball *ball, int cushionIndex) {
		if (ttc <= deltaTime) {
			pairs[{ball, cushion[cushionIndex]}]->t = ttc;
			collisions.insert(pairs[{ball, cushion[cushionIndex]}]);
		}
	};

	const auto DetectBallChusionCollision = [&](Ball *ball) {
		float vx = ball->velocity.x, vy = ball->velocity.y;
		// left cushion
		if (vx < 0) {
			ttc = (-vx - sqrt(pow(vx, 2) + 2 * ACC * (-limX - ball->pos.x))) / ACC;
			InsertBallCushionCollision(ball, 0);
		}
		// top cushion
		if (vx > 0) {
			ttc = (-vx + sqrt(pow(vx, 2) + 2 * ACC * (limX - ball->pos.x))) / ACC;
			InsertBallCushionCollision(ball, 2);
		}
		// right cushion
		if (vy < 0) {
			ttc = (-vy - sqrt(pow(vy, 2) + 2 * ACC * (-limY - ball->pos.z))) / ACC;
			InsertBallCushionCollision(ball, 1);
		}
		// bottom cushion
		if (vy > 0) {
			ttc = (-vy + sqrt(pow(vy, 2) + 2 * ACC * (limY - ball->pos.z))) / ACC;
			InsertBallCushionCollision(ball, 3);
		}
	};

	const auto DetectBallBallCollision = [&](Ball *ball, Ball *ball2) {
		float px = ball->pos.x - ball2->pos.x, py = ball->pos.z - ball2->pos.z;
		float Vx = ball->velocity.x - ball2->velocity.x, Vy = ball->velocity.y - ball2->velocity.y;

		// 2nd grade function: the distance between the balls function of time
		float a = Vx * Vx + Vy * Vy;
		if (a == 0) return;
		float b = 2 * (px * Vx + py * Vy);
		float c = px * px + py * py - 4 * RAD * RAD;
		float d = sqrt(b * b - 4 * a * c);
		float r1 = (-b - d) / (2 * a);

		if (0 < r1 && r1 <= deltaTime) {
			pairs[{ball, ball2}]->t = r1;
			collisions.insert(pairs[{ball, ball2}]);
		}
	};

	for (auto &ball : balls)
		DetectBallChusionCollision(ball);

	// detect balls collisions
	for (auto it = balls.begin(); it != balls.end(); ++it) {
		auto it2 = it;
		++it2;
		for (; it2 != balls.end(); ++it2)
			DetectBallBallCollision(*it, *it2);
	}

	while (!collisions.empty()) {
		// process the first collision
		CollisionPair *cp = *collisions.begin();
		deltaTime -= cp->t;

		for (auto &ball : balls) ball->Update(cp->t);

		Ball *ball = cp->a;
		if (cp->b->isCushion) {
			// ball-cushion collision
			switch (static_cast<Cushion *>(cp->b)->type) {
				case 0:
				case 2:
					ball->velocity.x *= -1;
					break;
				case 1:
				case 3:
					ball->velocity.y *= -1;
					break;
			}

			collisions.erase(collisions.begin());

			// TODO update elements from set
		}
		else {
			// ball-ball collision
			Ball *ball2 = static_cast<Ball *>(cp->b);

			glm::vec2 dv(ball->velocity - ball2->velocity);
			glm::vec2 dx(ball->pos.x - ball2->pos.x, ball->pos.z - ball2->pos.z);
			ball->velocity -= glm::dot(dv, dx) * dx / pow(glm::length(dx), 2);
			ball2->velocity -= glm::dot(-dv, -dx) * (-dx) / pow(glm::length(dx), 2);

			for (auto it = collisions.begin(); it != collisions.end();)
				if ((*it)->a == ball || (*it)->a == ball2 || (*it)->b == ball || (*it)->b == ball2)
					it = collisions.erase(it);
				else ++it;

			for (auto &ball3 : balls) {
				if (ball3 != ball) DetectBallBallCollision(ball, ball3);
				if (ball3 != ball2) DetectBallBallCollision(ball2, ball3);
			}
		}
	}

	for (auto &ball : balls)
		ball->Update(deltaTime); // bring the balls to present (move without collisions)
}

void BallPool::Update(float deltaTimeSeconds)
{
	ProcessMovements(deltaTimeSeconds);
	RenderTexturedMesh(floorMesh, shaders["TextureByPos"], glm::mat4(1), {floorTexture});

	UpdateCue(deltaTimeSeconds);
	if (gameState != IN_MOVE && gameState != START)
		RenderTexturedMesh(cue->mesh, shaders["Texture"], cue->GetModelMatrix(), cue->GetTextures());

	for (auto comp : poolTable->texComps)
		RenderTexturedMesh(comp.first, shaders["Texture"], comp.second, {poolTable->texture});

	for (auto comp : poolTable->colorComps)
		RenderColoredMesh(comp.mesh, shaders["Color"], comp.modelMat, comp.color);

	// render balls
	for (int i = 0; i < 7; i++)
		RenderColoredMesh(yellowBalls[i]->mesh, shaders["Color"], yellowBalls[i]->GetModelMatrix(), glm::vec3(1, 1, 0));
	for (int i = 0; i < 7; i++)
		RenderColoredMesh(redBalls[i]->mesh, shaders["Color"], redBalls[i]->GetModelMatrix(), glm::vec3(1, 0, 0));

	RenderColoredMesh(blackBall->mesh, shaders["Color"], blackBall->GetModelMatrix(), glm::vec3(0, 0, 0));
	RenderColoredMesh(cueBall->mesh, shaders["Color"], cueBall->GetModelMatrix(), glm::vec3(1, 1, 1));
}

void BallPool::FrameEnd()
{
	// DrawCoordinatSystem();
}

void BallPool::RenderTexturedMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
                                  const vector<Texture2D *> &textures) const
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model, view and projection matrices
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

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

void BallPool::RenderColoredMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color) const
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model, view and projection matrices
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	// Bind color vector
	glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(color));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}


void BallPool::OnInputUpdate(float deltaTime, int mods)
{
	// Process camera movement
	if (window->MouseHold(GLFW_MOUSE_BUTTON_MIDDLE) && camera->type == FirstPerson)
		camera->Update(deltaTime, window->KeyHold(GLFW_KEY_W), window->KeyHold(GLFW_KEY_A), window->KeyHold(GLFW_KEY_S),
		               window->KeyHold(GLFW_KEY_D), window->KeyHold(GLFW_KEY_Q), window->KeyHold(GLFW_KEY_E));

	if (gameState == BALL_IN_HAND && !window->MouseHold(GLFW_MOUSE_BUTTON_MIDDLE)) {
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


void BallPool::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) // switch between First person and third person camera modes
	{
		camera->SwitchMode(CameraType::ThirdPerson);
		camera->SetTarget(cueBall->pos, UIConstants::Camera::LOOKING_DIR_THIRD_PERSON, UIConstants::Camera::DIST_TO_TARGET);
		cue->SetTarget(cueBall->pos, camera->GetPosFromCameraSpace(UIConstants::Cue::DIR_CAMERA_SPACE), UIConstants::Ball::RAD);

		if (gameState == START)
			gameState = BREAK;
		else if (gameState == IN_MOVE)
			gameState = TURN;
	}
}

void BallPool::OnKeyRelease(int key, int mods) {}

void BallPool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_MIDDLE)) {
		// rotate the camera around OX and OY
		camera->RotateOX(-deltaY * UIConstants::Camera::SENSITIVITY);
		camera->RotateOY(-deltaX * UIConstants::Camera::SENSITIVITY);

		cue->SetTarget(cueBall->pos, camera->GetPosFromCameraSpace(UIConstants::Cue::DIR_CAMERA_SPACE), UIConstants::Ball::RAD);
	}
}

void BallPool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && (gameState == TURN || gameState == BREAK))
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
