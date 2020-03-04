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
	none = -1,	//Used for Unrecognised formats
	dae = 0,	//Can't read, unsure if working
	obj = 4,	//incl.mat:3
	//stl = 5,	//bin=6- Currently havng Issues: Under Review
	ply = 7,	//bin=8
	//glb = 10,	//bin=11 11, might be gltf?, see stl
	x3d = 16,	//Can't read, unsure if working
	fbx = 18,	//bin=17
};

ExportType compareInput(string typeName)
{
	if (typeName == "dae")
	{
		return ExportType(0);
	}
	else if (typeName == "obj")
	{
		return ExportType(4);
	}
	else if (typeName == "stl")
	{
		return ExportType(5);
	}
	else if (typeName == "ply")
	{
		return ExportType(7);
	}
	else if (typeName == "glb")
	{
		return ExportType(10);
	}
	else if (typeName == "x3d")
	{
		return ExportType(16);
	}
	else if (typeName == "fbx")
	{
		return ExportType(18);
	}
	return ExportType(-1);
}

//need to include fileName options
aiReturn saveScene(const aiScene* scene, string FileName, ExportType ex)
{
	/*Gets a list of all avaliable formats*/
	const char* desc;
	/*size_t exportNumbers = aiGetExportFormatCount();
	std::printf("Avaliable formats");
	for (int i = 0; i < exportNumbers; i++)
	{
		desc = aiGetExportFormatDescription(i)->description;
		printf(desc);
		printf("%d\n", i);
	}*/

	try {
		desc = aiGetExportFormatDescription(ex)->description;
		std::printf(desc);
	}
	catch (exception e)
	{
		std::printf("File Type unavaliable");
		return AI_FAILURE;
	}

	string output = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\";
	output.append(FileName);


#pragma region 	
	switch (ex)
	{
	case 0:
		output.append(".dae");
		break;
	case 3:
	case 4:
	default:
		output.append(".obj");
		break;
	case 5:
	case 6:
		output.append(".stl");
		break;
	case 7:
	case 8:
		output.append(".ply");
		break;
	case 10:
	case 11:
		output.append(".glb");
		break;
	case 16:
		output.append(".x3d");
		break;
	case 17:
	case 18:
		output.append(".fbx");
		break;
	}

#pragma endregion Switch

	return aiExportScene(scene, aiGetExportFormatDescription(ex)->id, output.c_str(), NULL);
}


int main()
{
#pragma region 
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1080, 720, "3D Model Loading", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	printf("Finished Initilisation");
	//glutInit();
#pragma endregion Setup

#pragma region 
	string path = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\Sphericon.stl";
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
		val->y = addTest.y;
		val->z = 0;//addTest.z;
		verticiesList[test] = val;
	}

	//scene->mMeshes[0]->mVertices;
#pragma endregion Editing Data

#pragma region 
	string fileName;
	ExportType type;
	bool validType = false;

	printf("Enter File name:");
	cin >> fileName;

	printf("Enter Export file Type:");		
	while (validType != true)
	{
		string typeString;
		printf("\nAvailable types:\ndae\nobj\nstl\nply\nglb\nx3d\nfbx\n");		
		cin >> typeString;

		type = compareInput(typeString);

		if (type != ExportType(-1))
		{
			validType = true;
		}
		else
		{
			printf("The file type specified either is not suupported by the system");
		}
	}

	aiReturn status = saveScene(scene, fileName, type);

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


