#pragma once
#include <vector>
#include "raybox.h"
#include "lib/GLEW/glew.h"	
class Model {

public:
	AABBox* box;
	std::vector<glm::vec3> points;
	std::vector<GLuint> indicies;
	std::vector<glm::vec2> textureUvs;
	std::string textureFile;
	Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, std::vector<glm::vec2> textures, std::string path, glm::vec3 max, glm::vec3 min);

	void Rotate(glm::vec3 rotation);
};
