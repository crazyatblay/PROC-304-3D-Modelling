//[header]
// A simple program to compute the intersection of rays with AA boxes
//[/header]
//[compile]
// Download the raybox.cpp and geometry.h files to a folder.
// Open a shell/terminal, and run the following command where the files is saved:
//
// c++ -o raybox raybox.cpp -O3 -std=c++11
//
// Run with: ./raybox.
//[/compile]
//[ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//[/ignore]

//from https://www.scratchapixel.com/code.php?id=10&origin=/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes

#include "lib/glm/glm.hpp"
#include <cstdlib>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

class Ray
{
public:
    Ray(const glm::vec3 orig, const glm::vec3 dir) : orig(orig), dir(dir)
    {

        invdir = glm::vec3(1 / dir.x,1/dir.y,1/dir.z);
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }
    glm::vec3 orig, dir; // ray orig and dir
    glm::vec3 invdir;
    int sign[3];
};

class AABBox
{
public:
    AABBox(const glm::vec3 b0, const glm::vec3 b1) { bounds[0] = b0, bounds[1] = b1; }
    bool intersect(const Ray& r, float& t) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
        tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
        tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
        tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
        tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        t = tmin;

        if (t < 0) {
            t = tmax;
            if (t < 0) return false;
        }

        return true;
    }
    glm::vec3 bounds[2];
};

int main(int argc, char** argv)
{
    AABBox box(glm::vec3(-1), glm::vec3(1));
    gen.seed(0);
    for (uint32_t i = 0; i < 16; ++i) {
        glm::vec3 randDir(2 * dis(gen) - 1, 2 * dis(gen) - 1, 2 * dis(gen) - 1);
        normalize(randDir);
        Ray ray(glm::vec3(0), randDir);
        float t;
        if (box.intersect(ray, t)) {
            glm::vec3 Phit = ray.orig + ray.dir * t;
            //std::cerr << ray.orig << " " << Phit << std::endl;
        }
    }
    return 0;
}
