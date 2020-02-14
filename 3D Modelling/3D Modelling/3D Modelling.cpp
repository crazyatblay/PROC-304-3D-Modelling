//#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//#include "lib/glut/GL/glut.h" CHECK THIS?
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#pragma comment(lib, "assimp-vc140-mt")

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
//#include <assimp/cimport.h>
//#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <list>

using namespace std;
//using namespace Assimp;


int main()
{    
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1080, 720, "Fabirc Physics", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    Assimp::Importer importer;
    
    //const aiScene* scene = NULL;
    
    string path = "";

    //scene = importer.ReadFile(path,aiProcess_Triangulate);
        
   // Exporter exporter;
    //size_t formatCount = aiGetExportFormatCount();
   // const list<aiExportFormatDesc>* types;
   // const aiExportFormatDesc* format;
   // for (int i = 0; i < (int)formatCount; i++)
  //  {
  //    format = aiGetExportFormatDescription(i);
  //  }

    //exporter.Export(scene, format->description, "testName", aiPostProcessSteps::aiProcess_Triangulate, NULL);

//    aiExportScene(scene,aiGetExportFormatDescription() ,"textFile");
    
}
