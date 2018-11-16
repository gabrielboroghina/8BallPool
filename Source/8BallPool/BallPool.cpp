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
		Mesh *mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a simple cube
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(1, 0, 1), glm::vec3(0.5f, 1, 1), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(0, 1, 1), glm::vec3(0, 0.6f, 1), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0.3f, 1, 0.1f), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 0.3f, 0.4f), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0, 0.7f, 1), glm::vec3(0), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(1, 1, 0), glm::vec3(0.9f, 1, 0.2f), glm::vec3(0), glm::vec2(0, 0)),
			// TODO: Complete the information for the cube
		};

		vector<unsigned short> indices =
		{
			0, 1, 2, // indices for first triangle
			1, 3, 2, // indices for second triangle
			2, 3, 7,
			2, 7, 6,
			1, 7, 3,
			1, 5, 7,
			6, 7, 4,
			7, 5, 4,
			0, 4, 1,
			1, 4, 5,
			2, 6, 4,
			0, 2, 4,
			0, 2, 4,
			// TODO: Complete indices data
		};

		MeshBuilder::CreateMesh("cube", vertices, indices);
	}

	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 0)),
			VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 2)),
		};

		vector<unsigned short> indices = {
			0, 1, 2,
		};

		MeshBuilder::CreateMesh("rect", vertices, indices);
	}

	{
		// floor
		float floorLen = 10;
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-floorLen, 0, -floorLen), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(-floorLen, 0, floorLen), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(floorLen, 0, floorLen), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(floorLen, 0, -floorLen), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
		};

		vector<unsigned short> indices = {
			0, 1, 2,
			0, 2, 3
		};

		floorMesh = MeshBuilder::CreateMesh("floor", vertices, indices);
	}

	// Load shader for texture by position
	Shader *shader = new Shader("TextureByPosShader");
	shader->AddShader("Source/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	shader->AddShader("Source/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;

	floorTexture = new Texture2D();
	floorTexture->Load2D("Resources/Textures/floor.jpg", GL_REPEAT);

	tex = new Texture2D();
	tex->Load2D("Resources/Textures/dark_wood.jpg", GL_REPEAT);

	cue = new Cue();
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
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		//RenderSimpleMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);
	}

	RenderSimpleMesh(floorMesh, shaders["TextureByPosShader"], glm::mat4(1), {floorTexture, tex});

	RenderSimpleMesh(cue->smallEndMesh, shaders["VertexColor"], glm::mat4(1), {});
	RenderSimpleMesh(cue->bigEndMesh, shaders["VertexColor"], glm::mat4(1), {});
}

void BallPool::FrameEnd()
{
	//DrawCoordinatSystem();
}

void BallPool::RenderSimpleMesh(const Mesh *mesh, const Shader *shader, const glm::mat4 &modelMatrix,
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

void BallPool::OnInputUpdate(float deltaTime, int mods) {}

void BallPool::OnKeyPress(int key, int mods) {}

void BallPool::OnKeyRelease(int key, int mods) {}

void BallPool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}

void BallPool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void BallPool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}

void BallPool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void BallPool::OnWindowResize(int width, int height) {}
