#include "PoolTable.h"

#include "8BallPool/MeshBuilder.h"
#include "8BallPool/UIConstants.h"
#include <GL/glew.h>
#include "Core/GPU/Texture2D.h"
#include "Core/Managers/ResourcePath.h"

using namespace std;
using namespace UIConstants::Table;

PoolTable::PoolTable()
{
    Mesh *mesh;

    // create table top
    texComps.push_back(make_pair(MeshBuilder::CreateRect(glm::vec3(0), LEN, WIDTH, glm::vec3(1, 1, 1)),
                                 translate(glm::mat4(1), glm::vec3(0, HEIGHT, 0))));

    // create pockets
    int pocketIndex = -1;
    for (int i = -1; i <= 1; i += 2) {
        pockets[++pocketIndex] = new Pocket(glm::vec3(i * POCKET_X, HEIGHT + 0.01f, POCKET_Z));
        colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, pockets[pocketIndex]->pos),
                                         glm::vec3(0), glm::mat4(1)));

        pockets[++pocketIndex] = new Pocket(glm::vec3(i * POCKET_X, HEIGHT + 0.01f, -POCKET_Z));
        colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, pockets[pocketIndex]->pos),
                                         glm::vec3(0), glm::mat4(1)));

        pockets[++pocketIndex] = new Pocket(glm::vec3(i * POCKET_X_CENTER, HEIGHT + 0.01f, 0));
        colorComps.push_back(ColoredComp(MeshBuilder::CreateDisk(CORNER_RAD, pockets[pocketIndex]->pos),
                                         glm::vec3(0), glm::mat4(1)));
    }
    mesh = new Mesh("wall");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");

    // create table margins
    float e = MARGIN_W / 2;
    for (int i = -1; i <= 1; i += 2) {
        colorComps.push_back(ColoredComp(mesh, wallColor, scale(translate(glm::mat4(1), glm::vec3(i * (WIDTH / 2 + e), HEIGHT, 0)),
                                                                glm::vec3(MARGIN_W, MARGIN_H, LEN + 2 * MARGIN_W))));
        colorComps.push_back(ColoredComp(mesh, wallColor, scale(translate(glm::mat4(1), glm::vec3(0, HEIGHT, i * (LEN / 2 + e))),
                                                                glm::vec3(WIDTH, MARGIN_H, MARGIN_W))));
    }

    for (int i = 0; i < 4; i++)
        cushions[i] = new Cushion(i);

    // load texture
    texture = new Texture2D();
    texture->Load2D("Resources/Textures/table.jpg", GL_REPEAT);
}

PoolTable::~PoolTable()
{
    for (auto &comp : colorComps)
        delete comp.mesh;

    delete texture;
}
