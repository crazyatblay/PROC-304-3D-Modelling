//#include "BoundingBox.cpp"
#include "Model.h"

	//BoundingBox boundBox;
AABBox* box;
std::vector<glm::vec3> points;
std::vector<GLuint> indicies;

Model::Model(std::vector<glm::vec3> input, std::vector<GLuint> ind, glm::vec3 max, glm::vec3 min)
{
	indicies = ind;
	points = input;
	//boundBox = BoundingBox(input);
	box = new AABBox(max, min);
};


