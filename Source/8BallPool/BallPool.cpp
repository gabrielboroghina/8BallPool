#include "BallPool.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>

using namespace std;

BallPool::BallPool() {}

BallPool::~BallPool() {}

void BallPool::Init()
{
	{
		Mesh *mesh = new Mesh("ball");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

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
	cue = new Cue();
	poolTable = new PoolTable();
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

void BallPool::Update(float deltaTimeSeconds)
{
	RenderTexturedMesh(floorMesh, shaders["TextureByPos"], glm::mat4(1), {floorTexture});

	RenderTexturedMesh(cue->mesh, shaders["Texture"], glm::mat4(1), cue->GetTextures());

	for (auto comp : poolTable->texComps)
		RenderTexturedMesh(comp.first, shaders["Texture"], comp.second, {poolTable->texture});

	for (auto comp : poolTable->colorComps)
		RenderColoredMesh(comp.mesh, shaders["Color"], comp.modelMat, comp.color);
}

void BallPool::FrameEnd()
{
	//DrawCoordinatSystem();
}

void BallPool::RenderTexturedMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
                                const vector<Texture2D *> &textures) const
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	GLint loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	GLint loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

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

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	GLint loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	GLint loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind color vector
	glUniform3fv(glGetUniformLocation(shader->program, "color"), 1, glm::value_ptr(color));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void BallPool::OnInputUpdate(float deltaTime, int mods) {}

void BallPool::OnKeyPress(int key, int mods) {}

void BallPool::OnKeyRelease(int key, int mods) {}

void BallPool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}

void BallPool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void BallPool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}

void BallPool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void BallPool::OnWindowResize(int width, int height) {}
