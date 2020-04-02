#pragma once
#include <vector>
#include "raybox.h"
#include <GL\glew.h>

class Model {

public:
	AABBox* box;
	std::vector<glm::vec3> points;
	std::vector<GLuint> indicies;
	Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, glm::vec3 max, glm::vec3 min);
};
