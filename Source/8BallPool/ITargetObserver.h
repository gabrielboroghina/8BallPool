#pragma once

#include <glm/detail/type_vec3.hpp>

class ITargetObserver
{
public:
	virtual ~ITargetObserver() = 0 {};

	virtual void Update(glm::vec3 movement) = 0;
};
