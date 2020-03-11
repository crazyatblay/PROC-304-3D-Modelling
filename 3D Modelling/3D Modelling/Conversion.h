#pragma once

#include "lib/glm/glm.hpp"
#include"lib/glm/gtc/type_ptr.hpp"
#include <assimp/scene.h>

using namespace Assimp;
using namespace glm;

static class Conversion 
{
public:
	static vec3 Vec3Conversion(aiVector3D* vecIn);
	static aiVector3D Vec3Conversion(vec3 vecIn);
	static aiMatrix4x4 MatrixConversion(mat4 matIn);
	static mat4 MatrixConversion(aiMatrix4x4 matIn);
};
