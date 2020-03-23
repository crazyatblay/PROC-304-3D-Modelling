#if defined(NANOGUI_GLAD)
#if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
#define GLAD_GLAPI_EXPORT
#endif

#include <glad/glad.h>
#else
#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GL_GLEXT_PROTOTYPES
#endif
#endif

#include <stdlib.h>
#include <stdio.h>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"


//#include "lib/GL/glut.h" should be able to remove this folder and glut ofolder in lib?
#include "GL/glut.h"
#include "lib/glm/glm.hpp"
#include <assimp/cimport.h>
#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <nanogui/nanogui.h>
#include <vector>
#include <iostream>

#include "Conversion.h"
#include "ShaderLoad.h"
using namespace std;
using namespace Assimp;
using namespace nanogui;

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

enum VAO_IDs { Triangles, Colours, Normals, NumVAOs = 1 };
enum Buffer_IDs { ArrayBuffer, NumBuffers = 3 };
enum Attrib_IDs { vPosition = 0, cPosition = 1, vNormal = 2 };


GLuint VAO[NumVAOs];

GLuint Buffers[NumBuffers];
GLuint NumVertices = 0;

GLuint program;
const aiScene* scene;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
aiMatrix4x4 matrix;
float xRotation = 0.0f, yRotation = 0.0f, zRotation = 0.0f;
double xPos, yPos;
bool leftPress;

Screen* screenCurrent = nullptr;


#define BUFFER_OFFSET(a)((void*)(a))

vector<GLuint> parseFaces(vector<aiFace*> faces)
{
	vector<GLuint> ind;
	for (int i = 0; i < faces.size(); i++)
	{
		vector<unsigned int> locInd;
		for (unsigned int j = 0; j < faces[i]->mNumIndices; j++)
		{
			unsigned int locInd = faces[i]->mIndices[j];
			ind.push_back(locInd);
		}
	}
	return ind;
}

void LoadModel()
{
	//Memory Management
	glDeleteBuffers(NumBuffers, Buffers);
	glDeleteProgram(program);

	//generates vertex array data
	glGenVertexArrays(NumVAOs, VAO);
	glBindVertexArray(VAO[Triangles]);

	ShaderInfo  shaders[] =
	{
		{ GL_VERTEX_SHADER, "media/triangles.vert" },
		{ GL_FRAGMENT_SHADER, "media/triangles.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	glUseProgram(program);


#pragma region
	vector<aiMesh*> meshList;
	vector<aiFace*> facesList;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
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
			for (unsigned int j = 0; j < temp->mNumVertices; j++)
			{
				verticiesList.push_back(&temp->mVertices[j]);
			}
			for (unsigned int j = 0; j < temp->mNumFaces; j++)
			{
				facesList.push_back(&temp->mFaces[j]);
			}

		}
	}


	vector<GLuint> indicies = parseFaces(facesList);
#pragma endregion Proccess Data


#pragma region/*
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

	//scene->mMeshes[0]->mVertices;*/
#pragma endregion Editing Data

	vector<glm::vec3> glmVerticies;


	for (int i = 0; i < verticiesList.size(); i++)
	{
		glmVerticies.push_back(Conversion::Vec3ConversionAi(verticiesList[i]));
	}

	vector<vec3> finalPoints;
	for (int i = 0; i < indicies.size(); i++)
	{
		finalPoints.push_back(glmVerticies[indicies[i]]);
	}

	NumVertices = finalPoints.size();

	glGenBuffers(NumBuffers, Buffers);

	//Verticies Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	glBufferData(GL_ARRAY_BUFFER, finalPoints.size() * sizeof(glm::vec3), finalPoints.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//MVP creation
	model = glm::mat4(1.0f);

	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -3.0f));// creating the view matrix	

	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));// creating the projection matrix

	projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 20.0f);// Adding all matrices up to create combined matrix

	glm::mat4 mv = view * model;

	int mvLoc = glGetUniformLocation(program, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(program, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//enables vertex arrays for verticies, colours, and normals
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(cPosition);
	//glEnableVertexAttribArray(vNormal);

	indicies.clear();
	finalPoints.clear();
}


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

void display()
{
	//clears existing objects
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	//culls back faces
	glEnable(GL_CULL_FACE);//keep only this

	//binds VAO
	glBindVertexArray(VAO[Triangles]);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));


	model = rotate(model, radians(xRotation), vec3(0.0f, 1.0f, .0f));
	model = rotate(model, radians(yRotation), vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, radians(zRotation), vec3(0.0f, 0.0f, 1.0f));


	glm::mat4 mv = view * model;
	projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 20.0f);

	int mvLoc = glGetUniformLocation(program, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(program, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void saveSetUp()
{
	string fileName = "teapot";
	ExportType type = ExportType(4);
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
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		double localXPos, localYPos;
		int width, height;
		localXPos = localYPos = 0;
		width = height = 0;

		glfwGetWindowSize(window, &width, &height);
		glfwGetCursorPos(window, &localXPos, &localYPos);

		float x = (2.0f * localXPos) / width - 1.0f;
		float y = 1.0f - (2.0f * localYPos) / height;
		float z = 1.0f;

		vec4 ray_clip(x, y, -1.0, 1.0);
		vec4 ray_eye = inverse(projection) * ray_clip;
		ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		vec3 ray_wor = (inverse(view) * ray_eye);
		ray_wor = normalize(ray_wor);

		vec3 nearPoint(x, y, z);
		vec3 farPoint(x, y, -1.0f);
		vec3 direction = farPoint - nearPoint;
		direction=normalize(direction);
		

		if (action == GLFW_PRESS)
		{
			leftPress = true;
		}
		else if (action == GLFW_RELEASE)
		{
			leftPress = false;
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_S && mods == GLFW_MOD_CONTROL)
	{
		saveSetUp();
	}
}


void CheckEvents(GLFWwindow* window)
{
	if (leftPress == true)
	{
		double localXPos, localYPos;
		localXPos = localYPos = 0;

		//glfwSetCursorPos(window, 100, 100);
		glfwGetCursorPos(window, &localXPos, &localYPos);
		if (!(xPos == 0 && yPos == 0))
		{
			xRotation -= xPos - (float)localXPos;
			yRotation -= yPos - (float)localYPos;

		}
		xPos = localXPos;
		yPos = localYPos;
	}
}

int main()
{
#pragma region 

	glfwInit();

	GLFWwindow* window = nullptr;
	window = glfwCreateWindow(1080, 720, "3D Model Loading", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	printf("Finished Initilisation");

#pragma endregion Setup

#pragma region 
	string path = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\teapot.obj";
	printf("Input File Location:\n");
	//	cin >> path;

	scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == nullptr)
	{
		printf("Scene not loaded");
	}
	else
	{
		printf("Success");
	}
	LoadModel();
#pragma endregion Loading

#pragma region

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glPolygonMode(GL_FRONT, GL_FILL);

	while (!glfwWindowShouldClose(window))
	{
		//screenCurrent->drawContents();

		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
		CheckEvents(window);


		//displayFabric.UpdateFabric();
		//UpdateModel();
	}

#pragma endregion Display

#pragma region
	aiReleaseImport(scene);

	aiDetachAllLogStreams();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
#pragma endregion Cleanup
}


