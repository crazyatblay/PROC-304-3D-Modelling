#pragma once
#include <vector>
#include "raybox.h"
#include "lib/GLEW/glew.h"	
class Model {

public:
	AABBox* box;
	std::vector<glm::vec3> points;
	std::vector<GLuint> indicies;
	Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, std::string path, glm::vec3 max, glm::vec3 min);
	std::string textureFile;
	void Rotate(glm::vec3 rotation);
};
