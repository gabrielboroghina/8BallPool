#pragma once

#include "Core/GPU/Mesh.h"
#include "Pocket.h"
#include "Cushion.h"

struct ColoredComp
{
    Mesh *mesh;
    glm::vec3 color;
    glm::mat4 modelMat;

    ColoredComp(Mesh *mesh, glm::vec3 color, glm::mat4 modelMat) : mesh(mesh), color(color), modelMat(modelMat) {}
};

class PoolTable
{
public:
    PoolTable();
    ~PoolTable();

    std::vector<std::pair<Mesh *, glm::mat4>> texComps;
    std::vector<ColoredComp> colorComps;
    Texture2D *texture;
    Cushion *cushions[4]; // 0 - left, 1 - top, 2 - right, 3 - bottom
    Pocket *pockets[6];
};
