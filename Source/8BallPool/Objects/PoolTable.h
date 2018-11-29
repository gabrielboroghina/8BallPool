#pragma once

#include "Core/GPU/Mesh.h"
#include "Pocket.h"
#include "Cushion.h"

struct ColoredComp
{
    Mesh *mesh;
    glm::vec3 color;
    glm::mat4 modelMat;

    ColoredComp(Mesh *mesh, glm::vec3 color, const glm::mat4 modelMat)
        : mesh(mesh),
          color(color),
          modelMat(modelMat) {}
};

struct TexturedComp
{
    Mesh *mesh;
    Texture2D *texture;
    glm::mat4 modelMat;

    TexturedComp(Mesh *mesh, Texture2D *texture, const glm::mat4 &model_mat)
        : mesh(mesh),
          texture(texture),
          modelMat(model_mat) {}
};

class PoolTable
{
public:
    PoolTable();
    ~PoolTable();

    std::vector<TexturedComp> texComps;
    std::vector<ColoredComp> colorComps;
    Cushion *cushions[4]; // 0 - left, 1 - top, 2 - right, 3 - bottom
    Pocket *pockets[6];
};
