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
#include <iostream>
using namespace std;
using namespace Assimp;

/*given in blender:
dae=0
abc=? Not going to support
ply=7/8(inline/binary)
stl=5/6(inline/binary)
fbx=17/18(inline/binary)
glb/gltf=10/11(inline/binary)
obj=3/4
.x3d=6
*/
enum ExportType
{
	dae=0,
	obj = 4,//incl.mat:3
	stl=5,//bin=6
	ply=7,//bin=8
	glb=10,//bin=11 11, might be gltf?
	x3d=16,
	fbx=18,//bin=17
};

aiReturn saveScene(const aiScene* scene,ExportType ex)
{
	size_t exportNumbers = aiGetExportFormatCount();
	const char* desc;
	printf("Avaliable formats");
	for (int i = 0; i < exportNumbers; i++)
	{
		desc = aiGetExportFormatDescription(i)->description;
		printf(desc);
		printf("\n");
	}
	desc = aiGetExportFormatDescription(4)->description;
	return AI_FAILURE;
	//return aiExportScene(scene, aiGetExportFormatDescription(4)->id, "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\Line.obj", aiProcess_Triangulate);

}


int main()
{
#pragma region 
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1080, 720, "3D Model Loading", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	//glutInit();
#pragma endregion Setup

#pragma region 
	string path = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\teapot.obj";
	printf("Input File Location:\n");
//	cin >> path;

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

#pragma endregion Loading 

#pragma region
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

#pragma endregion Proccess Data

#pragma region
	///Test operation
	for (int test = 0; test < verticiesList.size(); test++)
	{
		aiVector3D* val = verticiesList[test];
		aiVector3D addTest(val->x, val->y, val->z);
		addTest = addTest + addTest;

		val->x = addTest.x;
		val->y = 0;//addTest.y;
		val->z = 0;//addTest.z;
		verticiesList[test] = val;
	}

	//scene->mMeshes[0]->mVertices;
#pragma endregion Editing Data

#pragma region 
	ExportType ex = obj;
	aiReturn status = saveScene(scene, ex);

	if (status == AI_SUCCESS)
	{
		printf("\nSuccessfully saved\n");
	}
	else
	{
		printf("\nError saving.\n");
	}
#pragma endregion Export

#pragma region
	aiReleaseImport(scene);

	aiDetachAllLogStreams();
#pragma endregion Cleanup
}


