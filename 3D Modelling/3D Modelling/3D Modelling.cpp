// 3D Modelling.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "lib/glut/GL/glut.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;

//#include "lib//assimp/Importer.hpp"

int main()
{    

    glutInit();

    const aiScene* scene = NULL;
    
    string path = "";

    scene = aiImportFile(path.c_str,aiProcessPreset_TargetRealtime_MaxQuality);
        

}
