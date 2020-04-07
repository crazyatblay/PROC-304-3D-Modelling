#include <vector>
#include "lib/glm/ext/vector_float3.hpp"
#include <algorithm>

using namespace std;

class BoundingBox {

private:
	void UpdateMaxMin()
	{
		maxX = minX = points[0].x;
		maxY = minY = points[0].y;
		maxZ = minY = points[0].z;

		for (int i = 1; i < points.size(); i++)
		{
			if (points[i].x < minX)
			{
				minX = points[i].x;
			}
			else if (points[i].x > minX)
			{
				maxX = points[i].x;
			}

			if (points[i].y < minY)
			{
				minY = points[i].y;
			}
			else if (points[i].y > minY)
			{
				maxY = points[i].y;
			}

			if (points[i].z < minZ)
			{
				minZ = points[i].z;
			}
			else if (points[i].z > minZ)
			{
				maxZ = points[i].z;
			}
		}

	}
	//ray direction should be normalized
	

	bool isWithinBox(glm::vec3 point)
	{
		if (point.x <= maxX && point.x >= minX &&
			point.y <= maxY && point.y >= minY &&
			point.z <= maxZ && point.z >= minZ)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool isWithinaBox(BoundingBox box)
	{
		if (minX <= box.maxX && maxX >= box.minX &&
			minY <= box.maxY && maxY >= box.minY &&
			minZ <= box.maxZ && maxZ >= box.minZ)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool checkUniuque(vector<float> copy, float value)
	{
		int size = (int)copy.size();
		copy.erase(remove(copy.begin(), copy.end(), value), copy.end());
		if (copy.size() + 1 == size)
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

	float maxX, maxY, maxZ, minX, minY, minZ;


	bool rayIntersects(glm::vec3 rayStart, glm::vec3 rayDirection)
	{
		float tmin = (minX - rayStart.x) / rayDirection.x;
		float tmax = (maxX - rayStart.x) / rayDirection.x;

		if (tmin > tmax) swap(tmin, tmax);

		float tymin = (minY - rayStart.y) / rayDirection.y;
		float tymax = (maxY - rayStart.y) / rayDirection.y;

		if (tymin > tymax) swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (minZ - rayStart.z) / rayDirection.z;
		float tzmax = (maxZ - rayStart.z) / rayDirection.z;

		if (tzmin > tzmax) swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;

	}

	BoundingBox()
	{

	}

	BoundingBox(vector<glm::vec3> pointsInsert)
	{
		vector<float> x, y, z;

		for (int i = 0; i < pointsInsert.size(); i++)
		{
			x.push_back(pointsInsert[i].x);
			y.push_back(pointsInsert[i].y);
			z.push_back(pointsInsert[i].z);
		}

		int maxX = (int)(max_element(x.begin(), x.end()) - x.begin());
		int minX = (int)(min_element(x.begin(), x.end()) - x.begin());

		int minY = (int)(max_element(y.begin(), y.end()) - y.begin());
		int maxY = (int)(min_element(y.begin(), y.end()) - y.begin());

		int maxZ = (int)(max_element(z.begin(), z.end()) - z.begin());
		int minZ = (int)(min_element(z.begin(), z.end()) - z.begin());


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
		UpdateMaxMin();
	}



};