#include "BoundingBox.cpp"
#include <GL\glew.h>

class Model {




public:
	BoundingBox boundBox;
	vector<glm::vec3> points;
	vector<GLuint> indicies;

	Model(vector<glm::vec3> input, vector<GLuint> ind)
	{
		indicies = ind;
		points = input;
		boundBox = BoundingBox(input);
	}


};