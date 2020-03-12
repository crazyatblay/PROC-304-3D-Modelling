#pragma once

#ifndef CONVERT_H
#define CONVERT_H


#include "lib/glm/glm.hpp"
#include"lib/glm/gtc/type_ptr.hpp"
#include <assimp/scene.h>

using namespace Assimp;
using namespace glm;

class Conversion
{
public:
	static vec3 Vec3ConversionAi(aiVector3D* vecIn);
	static aiVector3D Vec3ConversionGLM(vec3 vecIn);
	static aiMatrix4x4 MatrixConversionGLM(mat4 matIn);
	static mat4 MatrixConversionAi(aiMatrix4x4 matIn);
};
#endif