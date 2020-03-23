#include <vector>;
#include "lib/glm/ext/vector_float3.hpp"
#include <algorithm>

using namespace std;

class BoundingBox {

private:



	bool checkUniuque(vector<float> copy, float value)
	{
		int size = copy.size();
		copy.erase(remove(copy.begin(), copy.end(), value), copy.end());
		if(copy.size()+1==size)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	//8 points: order- MaxZ,MaxY,MinX,MaxX,MinY,MinZ
	vector<glm::vec3> points;

	BoundingBox(vector<glm::vec3> pointsInsert)
	{
		vector<float> x, y, z;

		for (int i = 0; i < pointsInsert.size(); i++)
		{
			x.push_back(pointsInsert[i].x);
			y.push_back(pointsInsert[i].y);
			z.push_back(pointsInsert[i].z);
		}

		int maxX = max_element(x.begin(), x.end()) - x.begin();
		int minX = min_element(x.begin(), x.end()) - x.begin();

		int minY = max_element(y.begin(), y.end()) - y.begin();
		int maxY = min_element(y.begin(), y.end()) - y.begin();

		int maxZ = max_element(z.begin(), z.end()) - z.begin();
		int minZ = min_element(z.begin(), z.end()) - z.begin();


		//check if position lies at max

		glm::vec3 point(minX, maxY, maxZ);
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{ 
					points.push_back(point);
					point.x = maxX;
				}
				point.y = minY;
				point.x = minX;
			}
			point.y = maxY;
			point.z = minZ;
		}				
	}



};