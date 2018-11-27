#pragma once

#include "Object.h"
#include <glm/glm.hpp>

class Pocket : public Object
{
public:
	glm::vec3 pos;

    Pocket(glm::vec3 pos);
    ~Pocket();
};
