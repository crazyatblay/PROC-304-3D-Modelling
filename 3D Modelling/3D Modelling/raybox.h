#pragma once
#include "lib/glm/glm.hpp"
#include <cstdlib>
#include <random>

class Ray
{
public:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;

	glm::vec3 orig, dir; // ray orig and dir
	glm::vec3 invdir;
	int sign[3];

	Ray(const glm::vec3 orig, const glm::vec3 dir);
};

class AABBox
{
public:
	glm::vec3 bounds[2];
	AABBox(const glm::vec3 b0, const glm::vec3 b1);
	bool intersect(const Ray& r);
	bool ContainsPoint(glm::vec3 point);
};