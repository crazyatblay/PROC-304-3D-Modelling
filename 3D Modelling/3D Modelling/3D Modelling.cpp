#include <stdlib.h>
#include <stdio.h>

//#include "lib/glut/GL/glut.h" need to re-add glut/GLFW

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
using namespace Assimp;


int main()
{
	/*glfwInit();
	GLFWwindow* window = glfwCreateWindow(1080, 720, "Fabirc Physics", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();*/
	//glutInit();

	const aiScene* scene = NULL;

	string path = "";
	Importer importer;
	scene = importer.ReadFile(path, aiProcess_Triangulate);
	//scene = importer.GetScene();

	//scene = aiImportFile(path.c_str,aiProcessPreset_TargetRealtime_MaxQuality);




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
