#include <stdlib.h>
#include <stdio.h>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
//#include glm

#include "GL/glut.h"
#include <assimp/cimport.h>
//#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <list>
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

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	list<aiMesh*> meshList;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{

		aiMesh* meshes = scene->mMeshes[i];
		meshList.push_back(meshes);
		//meshes->mNumVertices
	}
	//list<aiVector3D> verticies;
	//meshes.push_back(scene->mMeshes);

	list<aiVector3D*> verticiesList;

	for (int i = 0; i < meshList.size(); i++)
	{

		//aiVector3D verticies = meshList[i]->mVertices;
		if (meshList.front()->mNumVertices > 0)
		{
			aiMesh* temp = meshList.front();
			
			//verticiesList.push_front(meshList.front()->mVertices);
			for (int j = 0; j < temp->mNumVertices; j++)
			{
				verticiesList.push_front(&temp->mVertices[j]);
			}
			printf("home");
			
		}
		//float xVal = verticies->x;

		//printf("%f", xVal);
	}




	aiReleaseImport(scene);

	aiDetachAllLogStreams();



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
