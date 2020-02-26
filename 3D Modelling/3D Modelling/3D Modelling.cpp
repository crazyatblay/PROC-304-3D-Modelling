#include <stdlib.h>
#include <stdio.h>

//#include "GL/glew.h"
//#include "GL/freeglut.h"
//#include "GLFW/glfw3.h"
//include glm

#include "GL/glut.h"
#include <assimp/cimport.h>
//#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
using namespace Assimp;


int main()
{
	//glfwInit();
	//GLFWwindow* window = glfwCreateWindow(1080, 720, "Fabirc Physics", NULL, NULL);
	//glfwMakeContextCurrent(window);
	//glewInit();
	//glutInit();

	string path = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\teapot.obj";
	const char h = 'h';
	const char* input = "er";
	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	

	//const aiScene* scene = NULL;

	//string path = "";

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

    //aiExportScene(scene,aiGetExportFormatDescription() ,"textFile");


}
