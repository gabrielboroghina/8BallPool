#pragma once

#include "../UIConstants.h"
#include "Core/GPU/Mesh.h"

class Cue
{
public:
	Cue();
	~Cue();

	std::vector<Texture2D *> GetTextures() const;

//private:
	Mesh *mesh;
	std::vector<Texture2D *> textures;
};
