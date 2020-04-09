#if defined(NANOGUI_GLAD)
#if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
#define GLAD_GLAPI_EXPORT
#endif
#prgama message("ProjDir == " ProjDir)
#include <glad/glad.h>
#else
#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GL_GLEXT_PROTOTYPES
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#endif
#include "Conversion.h"
#include "ShaderLoad.h"
#include "Model.h"

//#include <stdlib.h>
//#include <stdio.h>

#include "lib/GLEW/glew.h"
//#include "GL/freeglut.h"
#include "lib/GLFW/glfw3.h"
#include "lib/GLFW/glfw3native.h"


//#include "lib/GL/glut.h" should be able to remove this folder and glut ofolder in lib?
#include "lib/GL/glut.h"
#include "lib/glm/glm.hpp"
#include <assimp/cimport.h>
#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <vector>
#include <iostream>


using namespace std;
using namespace Assimp;

/*given in blender:
dae=0
abc=? Not going to support
ply=7/8(inline/binary)
stl=5/6(inline/binary) not in current use
fbx=17/18(inline/binary)
glb/gltf=10/11(inline/binary) not in current use
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
vector<Model> models;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
aiMatrix4x4 matrix;
float xRotation = 0.0f, yRotation = 0.0f, zRotation = 0.0f;
double xPos, yPos;
bool leftPress;
string filePath;
string fileName;
ExportType currentType;

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

	vector<glm::vec3> glmVerticies;

	vector<GLuint> indicies = parseFaces(facesList);

	for (int i = 0; i < (int)verticiesList.size(); i++)
	{
		glmVerticies.push_back(Conversion::Vec3ConversionAi(verticiesList[i]));
	}

	vector<float> x, y, z;

	for (int i = 0; i < (int)glmVerticies.size(); i++)
	{
		x.push_back(glmVerticies[i].x);
		y.push_back(glmVerticies[i].y);
		z.push_back(glmVerticies[i].z);
	}

	int maxX = max_element(x.begin(), x.end()) - x.begin();
	int minX = min_element(x.begin(), x.end()) - x.begin();

	int maxY = max_element(y.begin(), y.end()) - y.begin();
	int minY = min_element(y.begin(), y.end()) - y.begin();

	int maxZ = max_element(z.begin(), z.end()) - z.begin();
	int minZ = min_element(z.begin(), z.end()) - z.begin();


	//check if position lies at max

	glm::vec3 pointMax(x[maxX], y[maxY], z[maxZ]);
	glm::vec3 pointMin(x[minX], y[minY], z[minZ]);

	Model newModel(glmVerticies, indicies, pointMax, pointMin);
	models.push_back(newModel);
	indicies.clear();
	glmVerticies.clear();

}

void ParseModels()
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

	vector<vec3> finalPoints;

	for (int j = 0; j < (int)models.size(); j++)
	{
		for (int i = 0; i < (int)models[j].indicies.size(); i++)
		{
			finalPoints.push_back(models[j].points[models[j].indicies[i]]);
		}
	}

	NumVertices = (GLuint)finalPoints.size();

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

void SplitInput(string input)
{
	string fileTypeLoc = input.substr(input.find_last_of('.') + 1, input.size() - input.find_last_of('.'));
	string fileNameLoc = input.substr(input.find_last_of('\\') + 1, input.size() - input.find_last_of('.') + 2);
	string filePathLoc = input.substr(0, input.length() - (fileTypeLoc.length() + fileNameLoc.length()) - 1);
	fileName = fileNameLoc;
	filePath = filePathLoc;
	currentType = compareInput(fileTypeLoc);
	}

//fileName+path
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

	//hacky work around 
	string output = FileName;
	/*= "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\";
	output.append(FileName);*/


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
	//need to seperate actions, only on press
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		double localXPos, localYPos;
		int width, height;
		localXPos = localYPos = 0;
		width = height = 0;

		glfwGetWindowSize(window, &width, &height);
		glfwGetCursorPos(window, &localXPos, &localYPos);

#pragma region
		float x = (2.0f * localXPos) / width - 1.0f;
		float y = 1.0f - (2.0f * localYPos) / height;
		float z = 1.0f;

		GLint viewport[4]; //var to hold the viewport info
		GLdouble modelview[16]; //var to hold the modelview info
		GLdouble projection[16]; //var to hold the projection matrix info
		GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
		GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
		glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
		glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		winZ = -100;

		//get the world coordinates from the screen coordinates
		gluUnProject(x, y, z, modelview, projection, viewport, &worldX, &worldY, &worldZ);
		vec3 worldPos(worldX, worldY, worldZ);



		//GLdouble pos3D_x, pos3D_y, pos3D_z;// arrays to hold matrix informationGL
		//double model_view[16];glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
		//GLdouble projection[16];glGetDoublev(GL_PROJECTION_MATRIX, projection);
		//GLint viewport[4];glGetIntegerv(GL_VIEWPORT, viewport);
		//								   // get 3D coordinates based on window coordinates
		//gluUnProject(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.01,	model_view, projection, viewport,	&pos3D_x, &pos3D_y, &pos3D_z);

#pragma endregion

		vec3 screenPos(x, y, 0.0f);
		vec3 dir(0, 0, -1.0f);
		//vec3 randDir(2 * dis(gen) - 1, 2 * dis(gen) - 1, 2 * dis(gen) - 1);
		Ray ray(screenPos, dir);

		int intersectModel = -1;
		for (int i = 0; i < (int)models.size(); i++)
		{
			//bounding box is roughtly right height, X dimens seem off?
			//raypicking needs to be redone, should be able to just check if its in bounds now.
			//also box need to rotate with model
			//models[i].boundBox.rayIntersects(nearPoint, direction)
			//GLUnproject getting contant point regalress of position
			if (models[i].box->intersect(ray))
			{
				intersectModel = i;
				break;
			}
		}

		vec3 nearPoint(x, y, z);
		if (intersectModel != -1)
		{
			int closest = 0;
			for (int i = 1; i < (int)models[intersectModel].points.size(); i++)
			{
				if (distance(nearPoint, models[intersectModel].points[i]) < distance(nearPoint, models[intersectModel].points[closest]))
				{
					closest = i;
				}

			}

			models[intersectModel].points[closest] += vec3(1, 1, 1);
		}

		if (action == GLFW_PRESS && intersectModel == -1)
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

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);

	printf("Finished Initilisation");
#pragma endregion Setup

#pragma region 
	string path = "C:\\Users\\crazy\\OneDrive\\Documents\\Assimp\\teapot.obj";
	string dir = "";
	//GetCurrentDirectory(dir.length(), &dir.c_str);
	string fileName = "teapot";
	ExportType type = ExportType(3);
	printf("Input File Location:\n");
	//	cin >> path;

	SplitInput(path);

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
	ParseModels();
#pragma endregion Loading

#pragma region

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glPolygonMode(GL_FRONT, GL_FILL);

	while (!glfwWindowShouldClose(window))
	{
		//screenCurrent->drawContents();

		display();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
								 
			ImGui::InputText("File Path", &filePath, IM_ARRAYSIZE(filePath.c_str()));
			ImGui::InputText("File Name", &fileName, IM_ARRAYSIZE(fileName.c_str()));
			ExportType type = ExportType(0);
			const char* items[] = { "obj", "dae", "ply", "x3d", "fbx" };
			static int item_current = 0;
			ImGui::Combo("File Type", &item_current, items, IM_ARRAYSIZE(items));
			if (ImGui::Button("Save"))
			{
				aiReturn status =saveScene(scene, filePath + fileName, compareInput(items[item_current]));
				

				if (status == AI_SUCCESS)
				{
					printf("\nSuccessfully saved\n");
				}
				else
				{
					printf("\nError saving.\n");
				}
			}
				ImGui::End();
		}
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(100, 100, display_w, display_h);
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//UpdateModel();
		glfwSwapBuffers(window);
		glfwPollEvents();
		CheckEvents(window);
	}

#pragma endregion Display

#pragma region
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	aiReleaseImport(scene);

	aiDetachAllLogStreams();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
#pragma endregion Cleanup
}


