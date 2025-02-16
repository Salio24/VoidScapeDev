#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <iostream>
#include "HelperStructs.hpp"

struct Sprite {
	Sprite();

	~Sprite();

	Box mVertexData;
};
