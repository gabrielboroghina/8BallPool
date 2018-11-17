#include "PoolTable.h"

#include "8BallPool/MeshBuilder.h"
#include "8BallPool/UIConstants.h"
#include <GL/glew.h>
#include "Core/GPU/Texture2D.h"
#include "Core/Managers/ResourcePath.h"

using namespace std;
using namespace UIConstants::Table;

glm::mat4 reflectOZ(glm::mat4 m)
{
	return glm::mat4(1, 0, 0, 0,
	                 0, 1, 0, 0,
	                 0, 0, -1, 0,
	                 0, 0, 0, 1) * m;
}

PoolTable::PoolTable()
{
	Mesh *mesh;

	// create table top
	texComps.push_back(make_pair(
		MeshBuilder::CreateRect(glm::vec3(0), LEN, WIDTH, glm::vec3(1, 1, 1)),
		translate(glm::mat4(1), glm::vec3(0, HEIGHT, 0))));

	// create corners
	glm::mat4 cornersT = translate(glm::mat4(1), glm::vec3(WIDTH / 2, HEIGHT, LEN / 2));
	/*components.push_back(std::make_pair(
		MeshBuilder::CreateRoundedTriangle(0, CORNER_RAD, CORNER_RAD, CORNER_RAD, CORNER_RAD), cornersT));

	components.push_back(std::make_pair(
		MeshBuilder::CreateRoundedTriangle(0, CORNER_RAD, CORNER_RAD, CORNER_RAD, CORNER_RAD), reflectOZ(cornersT)));*/

	float halfL = LEN / 2 - 0.05, halfW = WIDTH / 2 - 0.05;
	for (int i = -1; i <= 1; i += 2) {
		colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, glm::vec3(i * halfW, HEIGHT + 0.01f, halfL)),
		                                 glm::vec3(0), glm::mat4(1)));
		colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, glm::vec3(i * halfW, HEIGHT + 0.01f, -halfL)),
		                                 glm::vec3(0), glm::mat4(1)));
		colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, glm::vec3(i * (halfW + 0.05), HEIGHT + 0.01f, 0)),
		                                 glm::vec3(0), glm::mat4(1)));
	}
	mesh = new Mesh("wall");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");

	float e = 0.05f;
	for (int i = -1; i <= 1; i += 2) {
		colorComps.push_back(ColoredComp(mesh, wallColor, scale(translate(glm::mat4(1), glm::vec3(i * (WIDTH / 2 + e), HEIGHT, 0)),
		                                                        glm::vec3(0.1, 0.3, LEN + 2 * CORNER_RAD))));
		colorComps.push_back(ColoredComp(mesh, wallColor, scale(translate(glm::mat4(1), glm::vec3(0, HEIGHT, i * (LEN / 2 + e))),
		                                                        glm::vec3(WIDTH, 0.3, 0.1))));
	}
	// load texture
	texture = new Texture2D();
	texture->Load2D("Resources/Textures/table.jpg", GL_REPEAT);
}


PoolTable::~PoolTable()
{
	for (auto &comp : colorComps)
		delete comp.mesh;
}
