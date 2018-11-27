#pragma once

#include "Core/GPU/Mesh.h"

struct ColoredComp
{
    Mesh *mesh;
    glm::vec3 color;
    glm::mat4 modelMat;

    ColoredComp(Mesh *mesh, glm::vec3 color, glm::mat4 modelMat) :
        mesh(mesh),
        color(color),
        modelMat(modelMat) {}
};

class PoolTable
{
public:
    PoolTable();
    ~PoolTable();

    std::vector<std::pair<Mesh *, glm::mat4>> texComps;
    std::vector<ColoredComp> colorComps;
    Texture2D *texture;
};
