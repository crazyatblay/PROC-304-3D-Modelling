//#include "BoundingBox.cpp"
#include "Model.h"

	//BoundingBox boundBox;
AABBox* box;
std::vector<glm::vec3> points;
std::vector<GLuint> indicies;
std::vector<glm::vec2> textureUvs;
std::vector<std::set<int> > map;


Model::Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, std::vector<glm::vec2> textures, std::string path, glm::vec3 max, glm::vec3 min, std::vector<std::set<int> > inputMap)
{
	indicies = ind;
	points = input;
	textureUvs = textures;

	//boundBox = BoundingBox(input);
	box = new AABBox(max, min);
	textureFile = path;
	map = inputMap;
};

void Model::Rotate(glm::vec3 rotation)
{
	//points = glRotatef(90, 0, 1, 0);
}
;
