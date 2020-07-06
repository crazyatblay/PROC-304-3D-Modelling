#pragma once
#include <vector>
#include "raybox.h"
#include "lib/GLEW/glew.h"	
#include <set>
class Model {

public:
	AABBox* box;
	std::vector<glm::vec3> points;
	std::vector<GLuint> indicies;
	std::vector<glm::vec2> textureUvs;
	std::string textureFile;
	std::vector<std::set<int> > map;
	Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, std::vector<glm::vec2> textures, std::string path, glm::vec3 max, glm::vec3 min, std::vector<std::set<int> > inputMap);

	void Rotate(glm::vec3 rotation);
};
