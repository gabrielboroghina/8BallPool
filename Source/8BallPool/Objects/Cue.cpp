#include "Cue.h"

#include <algorithm>
#include "../UIConstants.h"
#include "8BallPool/MeshBuilder.h"
#include <GL/glew.h>
#include <Core/GPU/Texture2D.h>
#include "include/math.h"

using namespace UIConstants::Cue;

Cue::Cue(): translateMatrix(glm::mat4(1)), rotateMatrix(glm::mat4(1))
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned short> indices;

    float yBottom = -LENGTH;
    float texSy = (1.0f - SMALL_RADIUS / BIG_RADIUS) / 2, ratio = SMALL_RADIUS / BIG_RADIUS / 360;

    // Insert center
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 0.5)));
    vertices.push_back(VertexFormat(glm::vec3(0, yBottom, 0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0.5)));
    vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0, texSy)));
    vertices.push_back(VertexFormat(glm::vec3(BIG_RADIUS, yBottom, 0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0)));

    int last;
    for (int u = 1; u <= 360; u++) {
        last = (u == 1) ? 2 : 4 * u - 4;
        float rad = RADIANS(u);
        vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(rad), 0, SMALL_RADIUS * sin(rad)),
                                        glm::vec3(0), glm::vec3(0), glm::vec2(0, texSy + ratio * u)));
        indices.push_back(0);
        indices.push_back(last);
        indices.push_back((unsigned short)vertices.size() - 1);

        vertices.push_back(VertexFormat(glm::vec3(BIG_RADIUS * cos(rad), yBottom, BIG_RADIUS * sin(rad)), glm::vec3(0),
                                        glm::vec3(0), glm::vec2(1, u / 360)));
        indices.push_back(1);
        indices.push_back(last + 1);
        indices.push_back((unsigned short)vertices.size() - 1);

        // build trapeze
        vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(RADIANS(u - 1)), yBottom, SMALL_RADIUS * sin(RADIANS(u - 1))),
                                        glm::vec3(0), glm::vec3(0), glm::vec2(1, texSy + ratio * (u - 1) / 360)));
        vertices.push_back(VertexFormat(glm::vec3(SMALL_RADIUS * cos(rad), yBottom, SMALL_RADIUS * sin(rad)),
                                        glm::vec3(0), glm::vec3(0), glm::vec2(1, texSy + ratio * u)));

        unsigned short nr = (unsigned short)vertices.size();
        // right triangle
        indices.push_back(last);
        indices.push_back(nr - 2);
        indices.push_back(last + 1);

        // left triangle
        indices.push_back(nr - 4);
        indices.push_back(nr - 3);
        indices.push_back(nr - 1);

        // rect
        indices.push_back(nr - 2);
        indices.push_back(last);
        indices.push_back(nr - 4);

        indices.push_back(nr - 4);
        indices.push_back(nr - 1);
        indices.push_back(nr - 2);
    }

    mesh = MeshBuilder::CreateMesh("cue", vertices, indices);

    // import textures
    texture = new Texture2D();
    texture->Load2D("Resources/Textures/cue.png", GL_MIRRORED_REPEAT);

    cueDir = initCueDir;
    pullBackDist = 0;
}

Cue::~Cue()
{
    delete mesh;
}

glm::mat4 Cue::GetModelMatrix() const
{
    return translateMatrix * rotateMatrix * glm::scale(glm::mat4(1), glm::vec3(1.5f, 1.5f, 1.5f));
}

void Cue::PullBack(float dist)
{
    pullBackDist += dist;
    if (pullBackDist > UIConstants::Cue::MAX_PULLBACK_DIST)
        pullBackDist = 0;
}

void Cue::UpdatePos(glm::vec3 movement)
{
    translateMatrix = glm::translate(translateMatrix, movement);
}

void Cue::SetTarget(glm::vec3 targetPos, glm::vec3 dir, float distToTarget)
{
    dir = glm::normalize(dir);
    cueDir = dir;
    translateMatrix = glm::translate(glm::mat4(1), glm::vec3(targetPos + dir * distToTarget));

    glm::vec3 N = glm::cross(initCueDir, dir); // compute the normal
    float angle = acos(glm::dot(initCueDir, dir)); // compute the rotation angle
    rotateMatrix = glm::rotate(glm::mat4(1), angle, N);
}
