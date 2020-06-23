//#include "BoundingBox.cpp"
#include "Model.h"

	//BoundingBox boundBox;
AABBox* box;
std::vector<glm::vec3> points;
std::vector<GLuint> indicies;

Model::Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, std::string path, glm::vec3 max, glm::vec3 min)
{
	indicies = ind;
	points = input;
	//boundBox = BoundingBox(input);
	box = new AABBox(max, min);
	textureFile = path;
};

void Model::Rotate(glm::vec3 rotation)
{
	//points = glRotatef(90, 0, 1, 0);
}
;
