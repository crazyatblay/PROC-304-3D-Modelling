#include <stdlib.h>
#include <stdio.h>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
//#include glm

#include "GL/glut.h"
#include <assimp/cimport.h>
#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
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
	vector<aiMesh*> meshList;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{

		aiMesh* meshes = scene->mMeshes[i];
		meshList.push_back(meshes);
		//meshes->mNumVertices
	}
	//list<aiVector3D> verticies;
	//meshes.push_back(scene->mMeshes);

	vector<aiVector3D*> verticiesList;

	for (int i = 0; i < meshList.size(); i++)
	{

		//aiVector3D verticies = meshList[i]->mVertices;
		if (meshList.front()->mNumVertices > 0)
		{
			aiMesh* temp = meshList.front();

			//verticiesList.push_front(meshList.front()->mVertices);
			for (int j = 0; j < temp->mNumVertices; j++)
			{
				verticiesList.push_back(&temp->mVertices[j]);
				//printf("%f,%f, %f\n", verticiesList[j]->x, verticiesList[j]->y, verticiesList[j]->z);
			}
			//printf("home");

		}
		//float xVal = verticies->x;

		//printf("%f", xVal);
	}

	///Test operation
	for (int test = 0; test < verticiesList.size(); test++)
	{
		aiVector3D* val = verticiesList[test];
		aiVector3D addTest(val->x, val->y, val->z);
		addTest = addTest + addTest;

		val->x = addTest.x;
		val->y = addTest.y;
		val->z = 0;//addTest.z;
		verticiesList[test] = val;
	}

	//scene->mMeshes[0]->mVertices;

	size_t exportNumbers = aiGetExportFormatCount();
	const char* desc;
	printf("Avaliable formats");
	for (int i = 0; i < exportNumbers; i++)
	{
		desc = aiGetExportFormatDescription(i)->description;
		printf(desc);
		printf("\n");
	}
	desc= aiGetExportFormatDescription(4)->description;
	aiReturn status = aiExportScene(scene, aiGetExportFormatDescription(4)->id, "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\flat.obj", aiProcess_Triangulate);
	if (status == AI_SUCCESS)
	{
		printf("\nSuccessfully saved\n");
	}
	else
	{
		printf("\nError saving.\n");
	}
	
	
	aiReleaseImport(scene);

	aiDetachAllLogStreams();

}
