#include "BallPool.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>

using namespace std;

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
	InitBalls();
	cue = new Cue();
	poolTable = new PoolTable();

	// init camera
	camera = new Camera(60, window->props.aspectRatio);
	camera->SwitchMode(FirstPerson, CameraLayout::TopDown);

	cueBall->AttachObserver(camera);
	cueBall->AttachObserver(cue);

	gameState = IN_MOVE;
}

void BallPool::InitBalls()
{
	for (int i = 0; i < 7; i++) {
		yellowBalls[i] = new Ball(UIConstants::Ball::initYellowBallPos[i]);
		redBalls[i] = new Ball(UIConstants::Ball::initRedBallPos[i]);
	}

	blackBall = new Ball(UIConstants::Ball::initBlackBallPos);
	cueBall = new Ball(UIConstants::Ball::initCueBallPos);
}

void BallPool::tryMoveCueBall(const glm::vec2 &move)
{
	// TODO check collisions
	using namespace UIConstants::Ball;
	cueBall->Move(glm::vec3(move.x * BALL_IN_HAND_SPEED, 0, move.y * BALL_IN_HAND_SPEED));
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
			cueBall->ReceiveVelocity(-glm::vec2(cue->cueDir.x, cue->cueDir.z) * (cueShotDist * 2.5f));
		}
	}
}

void BallPool::Update(float deltaTimeSeconds)
{
	RenderTexturedMesh(floorMesh, shaders["TextureByPos"], glm::mat4(1), {floorTexture});

	UpdateCue(deltaTimeSeconds);
	if (gameState != GameState::IN_MOVE)
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

	cueBall->Update(deltaTimeSeconds);
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
			tryMoveCueBall(glm::vec2(0, -deltaTime));
		else if (window->KeyHold(GLFW_KEY_A))
			tryMoveCueBall(glm::vec2(-deltaTime, 0));
		else if (window->KeyHold(GLFW_KEY_S))
			tryMoveCueBall(glm::vec2(0, deltaTime));
		else if (window->KeyHold(GLFW_KEY_D))
			tryMoveCueBall(glm::vec2(deltaTime, 0));
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
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && gameState == TURN || gameState == BREAK)
		cueShotRunning = UIConstants::Cue::PULL_BACK_SPEED_FACTOR;
}

void BallPool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		cueShotDist = cue->pullBackDist;
		cueShotRunning = UIConstants::Cue::RETURN_SPEED_FACTOR;
	}
}

void BallPool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void BallPool::OnWindowResize(int width, int height) {}
