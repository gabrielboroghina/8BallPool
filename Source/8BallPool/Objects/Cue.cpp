#include "Cue.h"

#include "8BallPool/MeshBuilder.h"
#include <GL/glew.h>
#include <Core/GPU/Texture2D.h>
#include "include/math.h"

using namespace UIConstants::Cue;

Cue::Cue()
{
	// create mesh
	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	float yBottom = 0;
	float texSy = (1.0f - SMALL_RADIUS / BIG_RADIUS) / 2, ratio = SMALL_RADIUS / BIG_RADIUS / 360;

	// Insert center
	vertices.push_back(VertexFormat(glm::vec3(0, yBottom, 0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0.5))); // center
	vertices.push_back(
		VertexFormat(glm::vec3(SMALL_RADIUS, LENGTH, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0, texSy)));
	vertices.push_back(VertexFormat(glm::vec3(BIG_RADIUS, yBottom, 0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0)));

	int last;
	for (int u = 1; u <= 360; u++) {
		last = u == 1 ? 1 : 4 * u - 5;
		float rad = RADIANS(u);
		vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(rad), LENGTH, SMALL_RADIUS * sin(rad)),
		                                glm::vec3(0), glm::vec3(0), glm::vec2(0, texSy + ratio * u)));
		indices.push_back(0);
		indices.push_back(last);
		indices.push_back(vertices.size() - 1);

		vertices.push_back(VertexFormat(glm::vec3(BIG_RADIUS * cos(rad), yBottom, BIG_RADIUS * sin(rad)), glm::vec3(0),
		                                glm::vec3(0), glm::vec2(1, u / 360)));
		indices.push_back(0);
		indices.push_back(last + 1);
		indices.push_back(vertices.size() - 1);


		// build trapeze
		vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(RADIANS(u - 1)), yBottom,
		                                          SMALL_RADIUS * sin(RADIANS(u - 1))), glm::vec3(0), glm::vec3(0),
		                                glm::vec2(1, texSy + ratio * (u - 1) / 360)));
		vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(rad), yBottom, SMALL_RADIUS * sin(rad)),
		                                glm::vec3(0), glm::vec3(0), glm::vec2(1, texSy + ratio * u)));
		// right triangle
		indices.push_back(last);
		indices.push_back(vertices.size() - 2);
		indices.push_back(last + 1);

		// left triangle
		indices.push_back(vertices.size() - 4);
		indices.push_back(vertices.size() - 3);
		indices.push_back(vertices.size() - 1);

		// rect
		indices.push_back(vertices.size() - 2);
		indices.push_back(last);
		indices.push_back(vertices.size() - 4);

		indices.push_back(vertices.size() - 4);
		indices.push_back(vertices.size() - 1);
		indices.push_back(vertices.size() - 2);
	}

	mesh = MeshBuilder::CreateMesh("cue", vertices, indices);

	// import textures
	textures.push_back(new Texture2D());
	textures.back()->Load2D("Resources/Textures/cue.png", GL_REPEAT);
}

Cue::~Cue()
{
	delete mesh;
}

std::vector<Texture2D*> Cue::GetTextures() const
{
	return textures;
}
