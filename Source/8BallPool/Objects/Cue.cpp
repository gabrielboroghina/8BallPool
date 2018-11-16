#include "Cue.h"

#include "8BallPool/MeshBuilder.h"
#include <GL/glew.h>
#include <Core/GPU/Texture2D.h>

Cue::Cue()
{
	// create mesh
	std::vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 0)),
		VertexFormat(glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 0)),
		VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec2(0, 2)),
	};

	std::vector<unsigned short> indices;

	smallEndMesh = MeshBuilder::CreateDisk(UIConstants::Cue::SMALL_RADIUS, glm::vec3(0, UIConstants::Cue::LENGTH, 0));
	bigEndMesh = MeshBuilder::CreateDisk(UIConstants::Cue::BIG_RADIUS, glm::vec3(0, 1, 0));

	// import textures
	textures.push_back(new Texture2D());
	textures.back()->Load2D("Resources/Textures/dark_wood.jpg", GL_REPEAT);
}

Cue::~Cue()
{
	delete smallEndMesh;
	delete bigEndMesh;
}

std::vector<Texture2D*> Cue::GetTextures() const
{
	return textures;
}
