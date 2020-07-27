#define STB_IMAGE_IMPLEMENTATION
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

#pragma region
#include "Conversion.h"
#include "ShaderLoad.h"
#include "Model.h"

#include "Shobjidl.h"
#include "atlcomcli.h"

#include "lib/GLEW/glew.h"

#include "lib/GLFW/glfw3.h"
#include "lib/GLFW/glfw3native.h"

#include "lib/GL/glut.h"
#include "lib/glm/glm.hpp"

#include "lib/stb/stb_image.h"

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/cexport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "maths_funcs.h"

#include <vector>
#include <iostream>
#include <set>
#include <map>
#pragma endregion includes

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
	objMat = 3,
	obj = 4,
	//stl = 5,	//bin=6- Currently havng Issues: Under Review
	ply = 7,	//bin=8 loading errors?
	//glb = 10,	//bin=11 11, might be gltf?, see stl
	x3d = 16,	//Can't read, unsure if working
	fbx = 18,	//bin=17
};

#pragma region
enum VAO_IDs { Triangles, Colours, Normals, Tex, NumVAOs = 1 };
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
enum Attrib_IDs { vPosition = 0, cPosition = 1, vNormal = 2, tPosition = 3 };

GLuint VAO[NumVAOs];

GLuint Buffers[NumBuffers];
GLuint NumVertices = 0;

GLuint program;
const aiScene* scene;
vector<Model> models;
GLuint texture1;
vector<GLuint> tempTest;
vector<GLfloat> colourData;

glm::vec3 sphere_pos_wor[] = { glm::vec3(-2.0, 0.0, 0.0), glm::vec3(2.0, 0.0, 0.0), glm::vec3(-2.0, 0.0, -2.0), glm::vec3(1.5, 1.0, -1.0) };
const float sphere_radius = 1.0f;


glm::mat4 model;//local?
glm::mat4 view;//view_mat
glm::mat4 projection;//proj_mat
glm::vec3 cameraPos;//cam_pos
glm::vec3 planeSetUp;

float xRotation = 0.0f, yRotation = 0.0f, zRotation = 0.0f;//pitch,yaw,roll
float xPan = 0.0f, yPan = 0.0f;//move
float culumXPan = 0.0f, culumYPan = 0.0f;
float scroll = -10.0f, culumScroll = -10.0f;
glm::vec2 startPos;

versor quaternion;
glm::mat4 rotationMatrix;

int selectedModel = -1, selectedPoint = -1;
int moveSize = 0;
set<int> selectedPoints;
glm::vec2 movementStart;

double xPos, yPos;
float xUpdate, yUpdate, zUpdate;
bool leftPress;
bool rightPress;
bool middlePress;

bool update;
bool interaction;
bool updatePoint = false;

string filePath;
string fileName;
ExportType currentType;

#define BUFFER_OFFSET(a)((void*)(a))
#pragma endregion Vars


void ResetValues()
{

	selectedPoint = selectedModel = -1;
	xRotation = yRotation = zRotation = 0;
	cameraPos = vec3(0, 0, 0);
	planeSetUp = vec3(1, 1, 0);
	xPan = yPan = scroll = 0;
	culumXPan = culumYPan = 0;
	scroll = culumScroll = -5;
	selectedModel = selectedPoint = -1;

	updatePoint = false;
	selectedPoints.clear();
}

void getMinMax(vector<glm::vec3> glmVerticies, glm::vec3& Max, glm::vec3& Min)
{
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

	float height = y[maxY] - y[minY];
	for (int i = 0; i < glmVerticies.size(); i++)
	{
		glmVerticies[i].y -= height / 2;
	}
	//check if position lies at max

	Max.x = x[maxX];
	Max.y = y[maxY];
	Max.z = z[maxZ];

	Min.x = x[minX];
	Min.y = y[minY];
	Min.z = z[minZ];
}


void LoadTexture(Model model, vector<glm::vec2> finalTexture)
{
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Tex]);
	glBufferData(GL_ARRAY_BUFFER, finalTexture.size() * sizeof(glm::vec2), finalTexture.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(tPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));	// load and create a texture 

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);	// set the texture wrapping parameters	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// set texture filtering parameters	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// load image, create texture and generate mipmaps	

	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //tell stb_image.h to flip loaded texture's on the y-axis.	
	string path = model.textureFile;
	unsigned char* data = stbi_load(path.data(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else { std::cout << "Failed to load texture" << std::endl; }

	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(program, "texture1"), 0);// creating the model matrix
}

void LoadColour()
{
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Colours]);

	glBufferStorage(GL_ARRAY_BUFFER, colourData.size() * sizeof(GLfloat), colourData.data(), 0);

	glVertexAttribPointer(cPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

set<int> RecursiveMap(int selectedPoint, int remainingDistance)
{
	vector<set<int> > group = models[selectedModel].map;
	set<int> returnSet = group[selectedPoint];
	set<int> combineSet;

	if (remainingDistance <= 0 || returnSet.size() > 8)
	{
		return { selectedPoint };
	}
	if (remainingDistance == 1)
	{
		returnSet.insert(selectedPoint);
		return returnSet;
	}

	std::set<int>::iterator it;
	it = returnSet.begin();

	while (it != returnSet.end())
	{
		set<int> localSet = RecursiveMap(*it, remainingDistance - 1);
		combineSet.insert(localSet.begin(), localSet.end());
		it++;
	}
	returnSet.insert(combineSet.begin(), combineSet.end());;
	return returnSet;
}

void UpdateColour()
{
	set<int> group;

	group = RecursiveMap(selectedPoint, moveSize);
	selectedPoints.insert(group.begin(), group.end());
	std::set<int>::iterator it = group.begin();

	for (int i = 0; i < models[selectedModel].indicies.size(); i++)
	{
		int base = i * 4;
		bool match = false;
		while (it != group.end())
		{
			if (models[selectedModel].indicies[i] == *it)
			{

				colourData[base + 1] = 0.0f;
				colourData[base + 2] = 0.0f;
				colourData[base + 3] = 0.9f;
				it = group.end();
				it--;
				match = true;
			}
			it++;
		}
		if (!match)
		{

			colourData[base + 1] = 1.0f;
			colourData[base + 2] = 1.0f;
			colourData[base + 3] = 1.0f;
		}
		it = group.begin();
	}
	LoadColour();
	update = true;
}

vector<set<int> > MapGeneration(int size, vector<GLuint> inds)
{
	vector<set<int> > vals;

	vals.resize(size);
	vector<unsigned int> indicies = inds;

	for (int i = 0; i < indicies.size(); i += 3)
	{

		vals[indicies[i]].insert(indicies[i + 1]);
		vals[indicies[i]].insert(indicies[i + 2]);

		vals[indicies[i + 1]].insert(indicies[i]);
		vals[indicies[i + 1]].insert(indicies[i + 2]);

		vals[indicies[i + 2]].insert(indicies[i]);
		vals[indicies[i + 2]].insert(indicies[i + 1]);
	}

	return vals;
}

void LoadModel()
{
	vector<aiMesh*> meshList;
	vector<aiFace*> facesList;
	vector<aiMaterial*> textureList;
	vector<glm::vec2> texCords;
	string path;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{

		aiMesh* meshes = scene->mMeshes[i];
		meshList.push_back(meshes);

		aiMaterial* materials = scene->mMaterials[i];
		textureList.push_back(materials);
	}

	vector<aiVector3D*> verticiesList;

	for (int i = 0; i < meshList.size(); i++)
	{
		if (meshList.front()->mNumVertices > 0)
		{
			aiMesh* temp = meshList.front();

			aiMaterial* mat = textureList.front();
			aiString texture;
			aiString loc;
			path = filePath;
			GLint width, height, nrChannels;

			if (AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &loc, NULL, NULL, NULL, NULL, NULL))
			{
				path.append(loc.C_Str());
			}
			else
			{
				path = "media/3DFillerBoxes.png";
				aiString locConv = (aiString)path;
				aiTextureMapping map;
				GLuint result;
				glGenTextures(1, &result);

			}

			/*	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}*/

			bool black = false;
			unsigned int numVerticiesLocal = temp->mNumVertices;
			for (unsigned int j = 0; j < numVerticiesLocal; j++)
			{
				verticiesList.push_back(&temp->mVertices[j]);
				if (&temp->mTextureCoords[0][0] != NULL)
				{
					aiVector3D* convert = &temp->mTextureCoords[0][j];

					texCords.push_back(Conversion::Vec3ConversionAi(convert));
					black = true;
				}
			}
			if (!black)
			{
				vec2 val(0.1, 0.1);
				texCords.push_back(val);

				val = vec2(0.9, 0.9);
				texCords.push_back(val);
			}
			for (unsigned int j = 0; j < temp->mNumFaces; j++)
			{
				facesList.push_back(&temp->mFaces[j]);
			}
		}
	}

	vector<glm::vec3> lookupList, glmVerticies;
	bool repeated = false;

	for (int i = 0; i < (int)verticiesList.size(); i++)
	{
		lookupList.push_back(Conversion::Vec3ConversionAi(verticiesList[i]));
		if (lookupList[i] == lookupList[0] && i == (verticiesList.size() / 3))
		{
			repeated = true;
		}
		if (i < (verticiesList.size() / 3) + 1)
		{
			glmVerticies.push_back(lookupList[i]);
			if ((i == verticiesList.size() / 3) && (lookupList[i] == glmVerticies[0]))
			{
				glmVerticies.pop_back();
			}
		}
	}

	verticiesList.clear();
	vector<GLuint> indicies = Conversion::parseAIFaces(facesList);
	tempTest = indicies;
	if (repeated)
	{
		indicies = Conversion::lookupSort(lookupList, indicies);
	}
	else
	{
		glmVerticies = lookupList;
	}
	//check if position lies at max

	glm::vec3 pointMin, pointMax;
	getMinMax(glmVerticies, pointMin, pointMax);
	//float height =   pointMin.y-pointMax.y;// y[maxY] - y[minY];

	colourData.resize(indicies.size() * 4);
	std::fill(colourData.begin(), colourData.end(), 1.0f);
	//for (int i = 0; i < indicies.size(); i++)
	//{
	//	//glmVerticies[i].y -= height / 2;
	//	colourData.push_back(1.0f);
	//	colourData.push_back(1.0f);
	//	colourData.push_back(1.0f);
	//	colourData.push_back(1.0f);
	//}

	vector<set<int> > map = MapGeneration(glmVerticies.size(), indicies);
	Model newModel(glmVerticies, indicies, texCords, path, pointMax, pointMin, map);

	models.push_back(newModel);

	indicies.clear();
	glmVerticies.clear();
	meshList.clear();
	facesList.clear();
	textureList.clear();

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

	vector<glm::vec3> finalPoints;
	vector<glm::vec2> finalTexture;
	bool existingTexture = false;
	bool flip = false;

	for (int j = 0; j < (int)models.size(); j++)
	{
		for (int i = 0; i < (int)models[j].indicies.size(); i++)
		{
			finalPoints.push_back(models[j].points[models[j].indicies[i]]);
			if (models[j].textureUvs.size() > 2)
			{

				existingTexture = true;
				finalTexture.push_back(models[j].textureUvs[tempTest[i]]);
			}
			else/* if (i < models[j].indicies.size()/4)*/
			{
				/*if (j % 4 == 0)
				{*/
				flip = !flip;
				//}

				existingTexture = true;
				if (flip)
				{
					finalTexture.push_back(models[j].textureUvs[0]);
				}
				else
				{
					finalTexture.push_back(models[j].textureUvs[1]);
				}
			}
		}
	}

	//vector<vec2> textCopy = finalTexture;
	//finalTexture.insert(finalTexture.begin(), textCopy.begin(), textCopy.end());

	//vector<vec3> reversePoints = finalPoints;
	//std::reverse(finalPoints.begin(), finalPoints.end());
	//finalPoints.insert(finalPoints.end(),reversePoints.begin(),reversePoints.end());
	NumVertices = (GLuint)finalPoints.size();


	glGenBuffers(NumBuffers, Buffers);

	//Verticies Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	glBufferData(GL_ARRAY_BUFFER, finalPoints.size() * sizeof(glm::vec3), finalPoints.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	if (existingTexture)
	{
		LoadTexture(models[0], finalTexture);
	}

	LoadColour();


	//MVP creation
	model = glm::mat4(1.0f);

	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -3.0f));// creating the view matrix	

	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));// creating the projection matrix

	projection = glm::ortho(45.0f, 4.0f / 3, 0.1f, 20.0f);// Adding all matrices up to create combined matrix

	glm::mat4 mv = view * model;

	int mvLoc = glGetUniformLocation(program, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(program, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//enables vertex arrays for verticies, colours, and normals
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(cPosition);
	if (existingTexture)
	{
		glEnableVertexAttribArray(tPosition);
	}
	//glEnableVertexAttribArray(vNormal);

	finalPoints.clear();
	finalTexture.clear();
}

string compareOutput(ExportType type)
{
	switch (type)
	{
	case 0:
		return(".dae");
		break;
	case 3:
	case 4:
		return(".obj");
		break;
	case 5:
	case 6:
		return(".stl");
		break;
	case 7:
	case 8:
		return(".ply");
		break;
	case 10:
	case 11:
		return(".glb");
		break;
	case 16:
		return(".x3d");
		break;
	case 17:
	case 18:
		return(".fbx");
		break;
	default:
		return ("");
	}
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
	string fileTypeLoc = input.substr(input.find_last_of('.') + 1, input.length() - input.find_last_of('.'));
	string fileNameLoc = input.substr(input.find_last_of('\\') + 1, ((input.length() - input.find_last_of('\\')) - fileTypeLoc.length()) - 2);
	string filePathLoc = input.substr(0, input.length() - (input.length() - input.find_last_of('\\')) + 1);

	fileName = fileNameLoc;
	filePath = filePathLoc;
	currentType = compareInput(fileTypeLoc);
}

void LoadSetup(GLFWwindow* window)
{
	models.clear();

	IFileDialog* pfd = NULL;
	HRESULT hr;
	HWND hWnd = glfwGetWin32Window(window);
	CComPtr<IFileOpenDialog> dlg;
	COMDLG_FILTERSPEC aFileTypes[] = {
		{ L"Object", L"*.obj" },
		{ L"Stero", L"*.stl" },
		//{ L"Polygon File", L"*.ply"},
		{ L"Digital Asset", L"*.dae"},
		{ L"Extensible 3D", L"*.x3d"},
		//{ L"Filmbox", L"*.fbx"},
		{ L"Global Module", L"*.glb"},
		{ L"All files", L"*.*" }
	};

	hr = dlg.CoCreateInstance(__uuidof(FileOpenDialog));
	if (FAILED(hr))
	{
		//need to implement
		//status = AI_FAILURE;
	}
	dlg->SetFileTypes(countof(aFileTypes), aFileTypes);
	dlg->SetTitle(L"Load construct");
	dlg->SetOkButtonLabel(L"Load ");
	dlg->SetDefaultExtension(L".obj");
	hr = dlg->Show(hWnd);

	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pItem;
		hr = dlg->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{
			LPOLESTR pwsz = NULL;

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

			if (SUCCEEDED(hr))
			{
				wchar_t* poleConv = pwsz;
				wstring wCharConv(poleConv);
				string wStringConv(wCharConv.begin(), wCharConv.end());
				SplitInput(wStringConv);
				string fileExt = compareOutput(currentType);
				if (fileExt == "")
				{
					MessageBox(nullptr, TEXT("Loading Error"), TEXT("File corrupt/not found!"), MB_OK);
					return;
				}
				string path = filePath + fileName + fileExt;

				Assimp::Importer importer;
				scene = importer.ReadFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
				if (scene == nullptr)
				{
					MessageBox(nullptr, TEXT("Loading Error"), TEXT("File corrupt/not found!"), MB_OK);
				}
				else
				{
					LoadModel();
					ParseModels();
					ResetValues();
				}

			}
		}
	}
}

//fileName+path
aiReturn saveScene(string FileName, ExportType ex)
{
	/*Gets a list of all avaliable formats*/
	const char* desc;
	size_t exportNumbers = aiGetExportFormatCount();
	std::printf("Avaliable formats");
	for (int i = 0; i < exportNumbers; i++)
	{
		desc = aiGetExportFormatDescription(i)->description;
		printf(desc);
		printf("%d\n", i);
	}

	try
	{
		desc = aiGetExportFormatDescription(ex)->description;
		std::printf(desc);
	}
	catch (exception e)
	{
		MessageBox(nullptr, TEXT("File Type Unavaliable"), TEXT("File Type unavaliable"), MB_OK);

		return AI_FAILURE;
	}

	string output = FileName;
	if (output == "" || scene == NULL)
	{
		return aiReturn_FAILURE;
	}

	string fileType = compareOutput(ex);
	if (fileType == "")
	{
		return AI_FAILURE;
	}
	output.append(compareOutput(ex));

	try
	{
		vector<aiFace> facesList;
		vector<aiVector3D> vecsList;

		for (int i = 0; i < models.size(); i++)
		{
			facesList = Conversion::parseGLMIndicies(models[i].indicies);

			for (int j = 0; j < models[i].points.size(); j++)
			{
				aiVector3D convert = Conversion::Vec3ConversionGLM(models[i].points[j]);
				vecsList.push_back(convert);
			}

			for (int k = 0; k < vecsList.size(); k++)
			{
				scene->mMeshes[i]->mVertices[k] = vecsList[k];
			}

			for (int m = 0; m < facesList.size(); m++)
			{
				scene->mMeshes[i]->mFaces[m].mNumIndices = 3;
				scene->mMeshes[i]->mFaces[m].mIndices = facesList[m].mIndices;
			}
			vecsList.clear();
			//facesList.clear();doens't like clearing faces? unsure why but whatever.
		}

		return aiExportScene(scene, aiGetExportFormatDescription(ex)->id, output.c_str(), NULL);
	}
	catch (exception e)
	{
		return aiReturn_FAILURE;
	}
}

void SaveSetup(GLFWwindow* window)
{
	aiReturn status = AI_FAILURE;

	IFileDialog* pfd = NULL;
	HRESULT hr;
	HWND hWnd = glfwGetWin32Window(window);
	CComPtr<IFileSaveDialog> dlg;
	COMDLG_FILTERSPEC aFileTypes[] = {
		{ L"Object", L"*.obj" },
		{ L"Stero", L"*.stl" },
		//{ L"Polygon File", L"*.ply"},
		{ L"Digital Asset", L"*.dae"},
		{ L"Extensible 3D", L"*.x3d"},
		//{ L"Filmbox", L"*.fbx"},
		{ L"Global Module", L"*.glb"},
		{ L"All files", L"*.*" }
	};

	hr = dlg.CoCreateInstance(__uuidof(FileSaveDialog));
	if (FAILED(hr))
	{
		status = AI_FAILURE;
	}
	dlg->SetFileTypes(countof(aFileTypes), aFileTypes);
	dlg->SetTitle(L"Save construct");
	dlg->SetOkButtonLabel(L"Save ");
	dlg->SetDefaultExtension(L".obj");

	//taken from stackoverflow, might have problems?
	wstring convert(fileName.begin(), fileName.end());
	dlg->SetFileName(convert.c_str());

	hr = dlg->Show(hWnd);

	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pItem;
		hr = dlg->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{
			LPOLESTR pwsz = NULL;

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

			if (SUCCEEDED(hr))
			{
				wchar_t* poleConv = pwsz;
				wstring wCharConv(poleConv);
				string wStringConv(wCharConv.begin(), wCharConv.end());
				SplitInput(wStringConv);

				status = saveScene(filePath + fileName, currentType);
			}
		}
	}

	if (status == AI_SUCCESS)
	{
		MessageBox(nullptr, TEXT("Saved"), TEXT("Successfully saved!"), MB_OK);
	}
	else
	{
		MessageBox(nullptr, TEXT("Saving Failure"), TEXT("Failed to save file."), MB_OK);
	}
}

void display()
{
	//clears existing objects
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.3f, 0.8f, 0.8f, 1.0f);

	//culls back faces	
	glEnable(GL_CULL_FACE);

	//binds VAO
	glBindVertexArray(VAO[Triangles]);

	model = glm::mat4(1.0f);

	//model = scale(model, glm::vec3(0.25f, 0.25f, 0.25f));

	//if (rightPress)
	//{
	//	model = glm::translate(model, glm::vec3(xPan, yPan, zPan));
	//}
	//model = glm::translate(model, glm::vec3(xPan, yPan, zPan));
	////if rotation >180, appears inverted, "bug"
	//model = rotate(model, radians(xRotation), glm::vec3(0.0f, 1.0f, .0f));
	//model = rotate(model, radians(yRotation), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = rotate(model, radians(zRotation), glm::vec3(0.0f, 0.0f, 1.0f));



	if (models.size() > 0)
	{//need to iterate
		getMinMax(models[0].points, models[0].box->bounds[0], models[0].box->bounds[1]);
		models[0].box->bounds[0] = model * glm::vec4(models[0].box->bounds[0], 1);
		models[0].box->bounds[1] = model * glm::vec4(models[0].box->bounds[1], 1);
	}
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, (-4.0f * scroll)));
	/*cameraPos.x += xPan;
	cameraPos.y += yPan;*/
	cameraPos.z += scroll;
	planeSetUp.z += scroll;

	view = lookAt(cameraPos,
		vec3(culumXPan, culumYPan, 0),
		vec3(0, 1, 0));

	scroll = xPan = yPan = 0.0f;

	glm::mat4 mv = view * model;

	//window size?
	projection = glm::perspective(glm::radians(45.0f), 4.0f / 3, 0.1f, 20.0f);

	int mvLoc = glGetUniformLocation(program, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(program, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glBindTexture(GL_TEXTURE_2D, texture1);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

}

/* check if a ray and a sphere intersect. if not hit, returns false. it rejects
intersections behind the ray caster's origin, and sets intersection_distance to
the closest intersection */
bool ray_sphere(glm::vec3 ray_origin_wor, glm::vec3 ray_direction_wor, glm::vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {
	// work out components of quadratic
	glm::vec3 dist_to_sphere = ray_origin_wor - sphere_centre_wor;
	float b = dot(ray_direction_wor, dist_to_sphere);
	float c = dot(dist_to_sphere, dist_to_sphere) - sphere_radius * sphere_radius;
	float b_squared_minus_c = b * b - c;
	// check for "imaginary" answer. == ray completely misses sphere
	if (b_squared_minus_c < 0.0f) { return false; }
	// check for ray hitting twice (in and out of the sphere)
	if (b_squared_minus_c > 0.0f) {
		// get the 2 intersection distances along ray
		float t_a = -b + sqrt(b_squared_minus_c);
		float t_b = -b - sqrt(b_squared_minus_c);
		*intersection_distance = t_b;
		// if behind viewer, throw one or both away
		if (t_a < 0.0) {
			if (t_b < 0.0) { return false; }
		}
		else if (t_b < 0.0) {
			*intersection_distance = t_a;
		}

		return true;
	}
	// check for ray hitting once (skimming the surface)
	if (0.0f == b_squared_minus_c) {
		// if behind viewer, throw away
		float t = -b + sqrt(b_squared_minus_c);
		if (t < 0.0f) { return false; }
		*intersection_distance = t;
		return true;
	}
	// note: could also check if ray origin is inside sphere radius
	return false;
}

glm::vec3 get_ray_from_mouse(float mouse_x, float mouse_y, GLFWwindow* window) {

	int g_gl_window_width, g_gl_window_height;
	glfwGetWindowSize(window, &g_gl_window_width, &g_gl_window_height);
	// screen space (viewport coordinates)
	float x = (2.0f * mouse_x) / g_gl_window_width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / g_gl_window_height;
	float z = 1.0f;
	// normalised device space
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	// clip space
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space

	//vec4 ray_eye = inverse(proj_mat) * ray_clip;
	glm::vec4 ray_eye = inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	//replaced view
	glm::vec3 ray_wor = glm::vec3(inverse(view) * ray_eye);
	// don't forget to normalise the vector at some point
	//normalise->lize
	ray_wor = normalize(ray_wor);
	return ray_wor;
}

//
//double SolveZPos(double xInput, double yInput)
//{
//	/*a(x−x0)+b(y−y0)+c(z−z0)=0
//	(x+2)+3(y−3)−7(z−4)=0
//	x+3y-7z+21=0
//	culumScroll*Xinput
//	*/
//	double CamDiffX = planeSetUp.x - xInput;//cameraPos.x;
//	double CamDiffY = planeSetUp.y - yInput;// cameraPos.y;
//	double CamDiffZ = planeSetUp.z - cameraPos.z;
//	vec3 norm = cameraPos - vec3(culumXPan, culumYPan, 0);
//	norm = normalize(norm);
//	//vec3 norm2 = planeSetUp - vec3(culumXPan, culumYPan, 0);
//	//norm2 = normalize(norm2);
//	double res = (norm.x * CamDiffX) + (norm.y * CamDiffY) + (norm.z * CamDiffZ);
//	double resFinal = (norm.x * CamDiffX) + (norm.y * CamDiffY) + (norm.z * cameraPos.z);
//	return 0;
//}


glm::vec3 setUpPlane(vec3 setupPoint)
{
	vec3 startPos(0, 0, culumScroll);
	vec3 currentPos = cameraPos;
	vec3 startOffset = vec3(setupPoint.x, setupPoint.y, culumScroll);
	vec3 currOffset = startOffset;

	float distFromCentre = distance(currentPos, vec3(0, 0, 0));

	float distanceXZ = distance(vec2(currentPos.x, currentPos.z), vec2(0, 0));
	float distanceYZ = distance(vec2(currentPos.y, currentPos.z), vec2(0, 0));

	double xDiff = currentPos.x;
	double yDiff = currentPos.y;
	double zDiff = culumScroll;
	double XZDiff, YZDiff;

	//at 0,0,-z, no change to z
	//at 0,0,+z, offset x by diffent bbetween offset and currentPos

	//rad or degree?
	if (distanceXZ > 8)
	{
		distanceXZ /= 2;
	}
	float dotX = dot((vec2(currentPos.x, currentPos.z)), vec2(startPos.x, startPos.z));
	float dotCheck = dot((vec2(0, 5)), vec2(0, 5));
	float angleXZ = dotX / (abs(culumScroll) * abs(distanceXZ));
	if (angleXZ <= 1)
	{
		angleXZ = acos(angleXZ);
	}
	else
	{
		angleXZ = 0;
	}
	printf("X:%f\n", degrees(angleXZ));
	if (currentPos.x < 0)
	{
		angleXZ = -angleXZ;
	}

	//might need to change for pan
	float rotatedX = cos(-angleXZ) * (startOffset.x - 0) - sin(-angleXZ) * (startOffset.z - 0) + 0;
	float rotatedZX = sin(-angleXZ) * (startOffset.x - 0) + cos(-angleXZ) * (startOffset.z - 0) + 0;

	currOffset.x = -rotatedX;// -setupPoint.x;//final X (correct)	
	currOffset.z += rotatedZX - startPos.z;


	float dotY = dot((vec2(currentPos.y, currentPos.z)), (vec2(startPos.y, startPos.z)));
	float angleYZ = dotY / (abs(culumScroll) * abs(distanceYZ));

	angleYZ = acos(angleYZ);
	if (culumScroll > 0)
	{
		angleYZ = -angleYZ;
	}

	printf("Y:%f\n", degrees(angleYZ));
	if (currentPos.y < 0)
	{
		angleYZ = -angleYZ;
	}

	//same note about pan, see above
	float rotatedY = cos(angleYZ) * (startOffset.y - 0) - sin(angleYZ) * (startOffset.z - 0) + 0;//1
	float rotatedZY = sin(-angleYZ) * (startOffset.y - 0) + cos(-angleYZ) * (startOffset.z - 0) + 0;//-5

	currOffset.y = rotatedY;// -setupPoint.y;//final X (correct)
	currOffset.z += rotatedZY - startPos.z;

	//glRotatef(angleXZ, currOffset.x, 0, currOffset.z);
	//at 5,0,-10, offset by 0.5?
	//at 0,5,-10, offset by 0,5?

	//double zChange = (yDiff * 0.5) + (xDiff * 0.5);
	//
	//camPos should be at screen centre, planeSetup should be at -0.5,0.5 screen
	if (culumScroll > 0)
	{
		currOffset.x = -currOffset.x;
	}
	//printf("CAM:%f,%f,%f\nOff:%f,%f,%f\n", cameraPos.x, cameraPos.y, cameraPos.z, planeSetUp.x, planeSetUp.y, planeSetUp.z);

	return currOffset;
}

vector<int> closestPoints(vec3 startPoint, vec3 startNormal)
{
	vector<glm::vec3> pointList = models[selectedModel].points;

	vector<double> points(pointList.size());

	vec3 normalised = normalize(startNormal);
	double normalX, normalY, normalZ;

	for (int i = 0; i < pointList.size(); i++)
	{
		vec3 point = normalize(pointList[i]);
		normalX = abs(normalised.x - point.x);
		normalY = abs(normalised.y - point.y);
		normalZ = abs(normalised.z - point.z);
		points[i] = normalX + normalY + normalZ;
	}
	int checkVal = points.size();
	vector<double>::iterator max = max_element(points.begin(), points.end());
	int maxLoc = std::distance(points.begin(), max);
	double maxVal = points[maxLoc];
	vector<int>finalPoints;
	for (int i = 0; i <= 5; i++)
	{
		vector<double>::iterator min = min_element(points.begin(), points.end());
		int val = std::distance(points.begin(), min);

		finalPoints.push_back(val);

		points[val] = points[val] + maxVal;

	}
	return finalPoints;
}

void GetPostion(double propX, double propY)
{
	const double DIST = 0.03;
	//0.1=0.054,0.1=0.04
	double xPos, yPos;
	double AB = abs(culumScroll);
	double totalWidth = (AB * 0.03) + 2.33;
	double totalHeight = (AB * 0.03) + 1.64;
	xPos = propX * totalWidth;
	yPos = propY * totalHeight;
	//invert?
	planeSetUp.x = xPos;
	planeSetUp.y = yPos;
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	int width, height;
	double localXPos, localYPos;
	//localXPos = localYPos = 0;
	//width = height = 0;
	glfwGetCursorPos(window, &localXPos, &localYPos);
	glfwGetWindowSize(window, &width, &height);
	float x = (2.0f * localXPos) / width - 1.0f;
	float y = 1.0f - (2.0f * localYPos) / height;
	float z = 1.0f;

	float size = 3.0f;

	glm::vec4 ray_clip(x, y, -1, 1);
	glm::vec4 ray_eye = inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1, 0);
	glm::vec3 rayWorld = glm::vec3(inverse(view) * ray_eye);
	rayWorld = normalize(rayWorld);
	//Ray(glm::vec3(x, y, -1), rayWorld);

#pragma region
	//if there is a selected model or point
	if (selectedModel != -1 && selectedPoint != -1)
	{
		interaction = false;
		updatePoint = (selectedModel != -1 && selectedPoint != -1);//????

		/*ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Render();*/
		//glm::vec3 screenPos(x + x * culumScroll, y + y * culumScroll, 0.0f);
		//glm::vec4 movement = glm::vec4(screenPos - vec3(movementStart, 0), 0);
		//movement = view * movement;
		//models[selectedModel].points[selectedPoint] += glm::vec3(model * movement);

		//movementStart = screenPos;
		////update = true;
		//glm::vec3 printPoint(models[selectedModel].points[selectedPoint].x, models[selectedModel].points[selectedPoint].y, models[selectedModel].points[selectedPoint].z);
		//printf("\nEnd Point:%f,%f,%f\n",
		//	printPoint.x,
		//	printPoint.y,
		//	printPoint.z);
	}

	if (!interaction && localYPos > 17.5)
	{
		//middle
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
		{

			GLint viewport[4]; //var to hold the viewport info
			GLdouble modelview[16]; //var to hold the modelview info
			GLdouble projectionLoc[16]; //var to hold the projection matrix info

			glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
			glGetDoublev(GL_PROJECTION_MATRIX, projectionLoc); //get the projection matrix info
			double invertedY = viewport[3] - localYPos;


			double windowPointX, windowPointY, windowPointZ;
			gluUnProject(localXPos, invertedY, 1, modelview, projectionLoc, viewport, &windowPointX, &windowPointY, &windowPointZ);

			GetPostion(windowPointX, windowPointY);
			vec3 posStart;
			printf("PlaneStart:%f,%f,%f\n", planeSetUp.x, planeSetUp.y, planeSetUp.z);
			posStart = setUpPlane(planeSetUp);
			//glm::vec3 rayWor = get_ray_from_mouse(posStart.x, posStart.y, window);

			//double mouseX = ((width / xPos) / (width / 2)) * cameraPos.x;
			//double mouseY = ((height / yPos) / (height / 2)) * cameraPos.y;
			//vec4 mousePos = vec4(mouseX, mouseY, cameraPos.z, 1);

			printf("PosStart:%f,%f,%f\n", posStart.x, posStart.y, posStart.z);
			Ray rayDir(posStart, normalize(-posStart));
			rayDir.dir = normalize(-cameraPos);
			//Ray rayDir(vec3(view * mousePos), rayWor);

			int intersectModel = -1;
			for (int i = 0; i < (int)models.size(); i++)
			{
				if (models[i].box->intersect(rayDir))
				{
					intersectModel = i;
					break;
				}
			}
			intersectModel = 0;
			//glm::vec4 nearPoint(x, y, z, 1);
			//nearPoint = view * nearPoint;
			//nearPoint = nearPoint * glm::vec4(xRotation, yRotation, culumScroll, 0);//zRotate
			//vector<glm::vec3> pointsList;
			int closest = 0;

			//Since the rendered model matches the actual model data, the data of the rendered model is never referenced.
			if (intersectModel != -1)
			{
				selectedModel = intersectModel;

				interaction = true;
				vec3 mousePoint;
				vec2 relationCentre = vec2(width / 2, height / 2);//relative start position
				relationCentre.x = (localXPos - relationCentre.x) / width;
				relationCentre.y = (localYPos - relationCentre.y) / height;

				//planeSetUp= -0.5,0.5
				//double xMov = planeSetUp.x - cameraPos.x;//1 local pos, 0.5 screen pos
				//double yMov = cameraPos.y - planeSetUp.y;//1 local pos, 0.5 screen pos
				//double xPlace = (worldPointBX / 0.5) * xMov;
				//double yPlace = (worldPointBY / 0.5) * yMov;
				//double zPlace2 = SolveZPos(xPlace, yPlace);;
				//double zPlane = (xPlace * planeSetUp.z) - (yPlace * planeSetUp.z);

				//vec4 res = vec4(worldPointBX, worldPointBY, -worldPointBZ, 1) + vec4(cameraPos, 1);
				//vec4 resTrue = projection * res;
				//vec3 invertZ = vec3(resTrue.x, resTrue.y, -resTrue.z);

				//tryinng camZ, prob need multiply by view
				//vec3 mouseMove = vec3(pointOne.x, pointOne.y, pointOne.z) * vec3(relationCentre, 0);//pointOne * vec3(localXPos, localYPos, 0) * vec3(0.1, 0.1, 0.1);

				//vec4 checkVal = vec4(localXPos, localYPos, 0, 1) + vec4(cameraPos, 1);
				//vec4 mouseMatrix = projection * vec4(mouseMove, 1);
				vec3 normal = posStart - vec3(0, 0, 0);
				vector<int> resultTest;
				resultTest = closestPoints(posStart, normal);
				int countPos = 1;
				for (int i = 1; i < (int)resultTest.size(); i++)
				{
					if (distance(posStart, models[intersectModel].points[resultTest[i]]) < distance(posStart, models[intersectModel].points[resultTest[closest]]))
					{
						closest = i;
					}
				}
				closest = resultTest[closest];
				/*for (int i = 1; i < (int)models[intersectModel].points.size(); i++)
				{
					if (distance(posStart, models[intersectModel].points[i]) < distance(posStart, models[intersectModel].points[closest]))
					{
						closest = i;
					}

				}*/

				if (intersectModel != -1 && closest != -1)
				{
					if (mods == -GLFW_MOD_CONTROL)
					{
						selectedPoint = closest;
						UpdateColour();
					}


					selectedPoint = closest;
					movementStart = vec2(localXPos, localYPos);
					vec3 pointData = models[selectedModel].points[selectedPoint];

					UpdateColour();


					xUpdate = posStart.x;
					yUpdate = posStart.y;
					zUpdate = posStart.z;

				}
			}

			if (action == GLFW_PRESS && intersectModel != -1)
			{
				middlePress = true;
			}
			else if (action == GLFW_RELEASE)
			{
				rightPress = false;
				xPos = yPos = 0;
				movementStart = glm::vec2(0, 0);

			}
		}
#pragma region
		//			glm::vec3 rayWor = get_ray_from_mouse(x, y, window);
		//
		//			/*	int closest_sphere_clicked = -1;
		//				float closest_intersection = 0.0f;
		//				for (int i = 0; i < 4; i++)
		//				{
		//					float t_dist = 0.0f;
		//					if (ray_sphere(cameraPos, rayWor, sphere_pos_wor[i], sphere_radius, &t_dist))
		//					{
		//						if (-1 == closest_sphere_clicked || t_dist < closest_intersection) {
		//							closest_sphere_clicked = i;
		//							closest_intersection = t_dist;
		//						}
		//					}
		//				}*/
		//
		//#pragma region
		//				/*
		//
		//
		//							GLint viewport[4]; //var to hold the viewport info
		//							GLdouble modelview[16]; //var to hold the modelview info
		//							GLdouble projection[16]; //var to hold the projection matrix info
		//							GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
		//							GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
		//
		//							glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info
		//							glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
		//							glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
		//
		//							winX = (float)localXPos;
		//							winY = (float)viewport[3] - (float)localYPos;
		//
		//							glm::vec3 screenPos, farPos, dodgy;
		//							//near plane
		//							gluUnProject(winX, winY, 0, modelview, projection, viewport, &worldX, &worldY, &worldZ);
		//							screenPos = glm::vec3(worldX, worldY, worldZ);
		//							//farPlane
		//							gluUnProject(winX, winY, 1, modelview, projection, viewport, &worldX, &worldY, &worldZ);
		//							farPos = glm::vec3(worldX, worldY, worldZ);
		//
		//							vector<glm::vec3>screen;
		//							if (models.size() >= 1)
		//							{
		//								for (int i = 0; i < models[0].points.size(); i++)
		//								{
		//									gluProject(models[0].points[i].x, models[0].points[i].y, models[0].points[i].z,
		//										modelview, projection, viewport,
		//										&worldX, &worldY, &worldZ);
		//									screen.push_back(glm::vec3(worldX, worldY, worldZ));
		//								}
		//							}
		//							glm::vec3 screenPoint(winX, winY, -100);
		//
		//				*/
		//#pragma endregion
		//				//vec3 screenPos(x + x * culumScroll, y + y * culumScroll, 0.0f);
		//				//glm::vec3 dir(farPos - screenPos);
		//			//localxPos/width, localYpos/height;
		//			double mouseX = ((width/xPos)/(width / 2))*cameraPos.x;
		//			double mouseY = ((height / yPos) / (height / 2)) * cameraPos.y;
		//			vec4 mousePos = vec4(mouseX, mouseY, cameraPos.z,1);
		//			Ray rayDir(vec3(view * mousePos), rayWor);
		//			//vec4 camerapos,1
		//
		//			int intersectModel = -1;
		//			for (int i = 0; i < (int)models.size(); i++)
		//			{
		//				//bounding box is roughtly right height, X dimens seem off?
		//				//raypicking needs to be redone, should be able to just check if its in bounds now.
		//				//also box need to rotate with model
		//				//models[i].boundBox.rayIntersects(nearPoint, direction)
		//				//GLUnproject getting contant point regalress of position
		//				if (models[i].box->intersect(rayDir))
		//				{
		//					intersectModel = i;
		//					break;
		//				}
		//			}
		//
		//			glm::vec4 nearPoint(x, y, z, 1);
		//			nearPoint = view * nearPoint;
		//			nearPoint = nearPoint * glm::vec4(xRotation, yRotation, zRotation, 0);
		//			vector<glm::vec3> pointsList;
		//			int closest = 0;
		//			if (intersectModel != -1)
		//			{
		//
		//				for (int i = 1; i < (int)models[intersectModel].points.size(); i++)
		//				{
		//					if (distance(glm::vec3(cameraPos), models[intersectModel].points[i]) < distance(glm::vec3(cameraPos), models[intersectModel].points[closest]))
		//					{
		//						closest = i;
		//					}
		//
		//				}
		//
		//
		//				/*printf("\nSelected Point:%f,%f,%f\n",
		//					models[intersectModel].points[closestScreen].x,
		//					models[intersectModel].points[closestScreen].y,
		//					models[intersectModel].points[closestScreen].z);*/
		//				if (intersectModel != -1 && closest != -1)
		//				{
		//					selectedModel = intersectModel;
		//					selectedPoint = closest;
		//					movementStart = vec2(localXPos, localYPos);
		//				}
		//				//else
		//				//{
		//				//	vec3 movement = screenPos - movementStart;
		//				//	models[intersectModel].points[closest] += vec3(model * vec4(movement, 0));
		//				//	movementStart = screenPos;
		//				//	update = true;
		//
		//				//	printf("\n%f,%f,%f\n", models[intersectModel].points[closest].x, models[intersectModel].points[closest].y, models[intersectModel].points[closest].z);
		//				//	/*vec3 upper = models[intersectModel].box->bounds[0];
		//				//	printf("\n%f,%f,%f\n", upper.x, upper.y, upper.z
		//				//}
		//			}
#pragma endregion UNDER REFACTORING

		//left
		else if (button == GLFW_MOUSE_BUTTON_1)
		{

			if (action == GLFW_PRESS)
			{
				leftPress = true;
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				startPos = vec2(x, y);
			}
			else //if (action == GLFW_RELEASE)
			{
				leftPress = false;
				rightPress = false;
				xPos = yPos = 0;
				//selectedModel = selectedPoint = -1;
				movementStart = glm::vec2(0, 0);

				//}

			}
		}
		else if (button == GLFW_MOUSE_BUTTON_2)
		{
			if (action == GLFW_PRESS)
			{
				rightPress = true;
			}
			else if (action == GLFW_RELEASE)
			{
				rightPress = false;
				xPos = yPos = 0;
			}
		}


	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_S && mods == GLFW_MOD_CONTROL)
	{
		SaveSetup(window);
	}

	if (key == GLFW_KEY_L && mods == GLFW_MOD_CONTROL)
	{
		LoadSetup(window);
	}

	if (key == GLFW_KEY_W)
	{
		cameraPos.y += 0.1;
		planeSetUp.y += 0.1;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos.y -= 0.1;
		planeSetUp.y -= 0.1;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos.x -= 0.1;
		planeSetUp.x -= 0.1;
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos.x += 0.1;
		planeSetUp.x += 0.1;
	}

	if (key == GLFW_KEY_Y && action == GLFW_RELEASE)
	{
		setUpPlane(planeSetUp);
	}

	if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE)
	{
		moveSize++;
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE)
	{
		moveSize--;
	}

	if (key == GLFW_KEY_SPACE)
	{
		cameraPos = glm::vec3(0, 0, 0);
		scroll = culumScroll = 0;
		planeSetUp = glm::vec3(1, 1, -1);
		xRotation = yRotation = 0;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	int total = xoffset + yoffset;
	if (total > 0)
	{
		scroll -= 0.1f;
		culumScroll -= 0.1f;
	}
	else if (total < 0)
	{
		scroll += 0.1f;
		culumScroll += 0.1f;
	}
}

void CheckEvents(GLFWwindow* window)
{
	POINT point;
	if (!(ScreenToClient(glfwGetWin32Window(window), &point)))
	{
		rightPress = leftPress = false;
	}

	double localXRotate, localYRotate;//pitch, yaw
	double localXPan, localYPan;//move.x,move.y
	if (leftPress && !middlePress)
	{
		rightPress = false;
		localXRotate = localYRotate = 0;

		glfwGetCursorPos(window, &localXRotate, &localYRotate);
		if (xPos != 0 && yPos != 0)
		{
			xRotation -= xPos - (float)localXRotate;
			yRotation -= yPos - (float)localYRotate;
		}

		if (selectedModel == -1)
		{
			if (culumScroll < 0)
			{
				cameraPos.x -= (startPos.x - (float)localXRotate) / 100;
				planeSetUp.x -= (startPos.x - (float)localXRotate) / 100;
			}
			else
			{
				cameraPos.x += (startPos.x - (float)localXRotate) / 100;
				planeSetUp.x += (startPos.x - (float)localXRotate) / 100;
			}
			cameraPos.y -= (startPos.y - (float)localYRotate) / 100;
			planeSetUp.y -= (startPos.y - (float)localYRotate) / 100;
			startPos = vec2(localXRotate, localYRotate);
		}

		if (xRotation > 360)
		{
			xRotation -= 360;
		}
		else if (xRotation < -360)
		{
			xRotation += 360;
		}

		if (yRotation > 360)
		{
			yRotation -= 360;
		}
		else if (yRotation < -360)
		{
			yRotation += 360;
		}
		xPos = localXRotate;
		yPos = localYRotate;
	}
	else if (rightPress && !middlePress)
	{
		leftPress = false;
		localXPan = localYPan = 0;

		glfwGetCursorPos(window, &localXPan, &localYPan);
		if (xPos != 0 && yPos != 0)
		{
			xPan += (xPos - (float)localXPan) / 100;
			yPan -= (yPos - (float)localYPan) / 100;
			if (culumScroll < 0)
			{
				culumXPan -= xPan;
			}
			else
			{
				culumXPan += xPan;
			}
			culumYPan += yPan;
		}
		xPos = localXPan;
		yPos = localYPan;
	}

	if (rightPress || leftPress)
	{
		glm::vec3 movement(xRotation / 100, yRotation / 100, scroll);

		versor pitch = quat_from_axis_deg(xRotation, 1, 0, 0);
		versor yaw = quat_from_axis_deg(yRotation, 0, 1, 0);
		quaternion = yaw * quaternion;
		quaternion = pitch * quaternion;
		maT4 convertVal = quat_to_maT4(quaternion);
		rotationMatrix = Conversion::convertMat4(convertVal) * view;
		glm::vec4 fwd = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0);
		glm::vec4 rgt = rotationMatrix * glm::vec4(1.0f, 0.0f, -1.0f, 0);
		glm::vec4 up = rotationMatrix * glm::vec4(0.0f, 1.0f, -1.0f, 0);

		cameraPos += vec3(rgt * -movement.x);
		cameraPos += vec3(up * movement.y);
		cameraPos += vec3(fwd * -movement.z);
		planeSetUp += vec3(rgt * -movement.x);
		planeSetUp += vec3(up * movement.y);
		planeSetUp += vec3(fwd * -movement.z);


		cameraPos.x = min(max(cameraPos.x, -5), 5);
		cameraPos.y = min(max(cameraPos.y, -5), 5);
		cameraPos.z = min(max(cameraPos.z, -5), 5);
		planeSetUp.x = min(max(planeSetUp.x, -5), 5);
		planeSetUp.y = min(max(planeSetUp.y, -5), 5);
		planeSetUp.z = min(max(planeSetUp.z, -5), 5);
		xRotation = yRotation = 0;
		glm::mat4 Tin = translate(mat4(1.0f), cameraPos);
	}
}


int main()
{
	//hides console window
	/*WCHAR path[265];
	GetModuleFileName(NULL, path, 265);
	HWND console = FindWindow(L"ConsoleWindowClass", path);
	ShowWindow(console, SW_HIDE);*/

#pragma region 
	update = false;

	cameraPos = glm::vec3(0, 0, 0);
	planeSetUp = glm::vec3(1, 1, 0);
	glm::mat4 T = translate(mat4(), glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));
	rotationMatrix = Conversion::convertMat4(maT4(rotate_y_deg(identity_maT4(), -0.0f)));
	quaternion = quat_from_axis_deg(0.0f, 0, 1, 0);
	view = rotationMatrix * T;

	int WindowWidth = 1280, WindowHeight = 960;
	//base 2.481width, 1.79 height.
	glfwInit();

	GLFWwindow* window = nullptr;
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Mod3D", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);
#pragma endregion Setup	

#pragma region

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glPointSize(30);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	static float xMove, yMove, zMove = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		if (update)
		{
			ParseModels();
			if (leftPress == false)
			{
				update = false;
			}

		}

		display();

#pragma region 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					//using window, need to add all options to filter.
					if (ImGui::MenuItem("Load"))
					{
						LoadSetup(window);

					}
					if (ImGui::MenuItem("Save"))
					{
						SaveSetup(window);
					}
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}

			if (updatePoint) {

				ImGui::Begin("Move Point", &updatePoint);
				{
					ImGui::Columns(3, "layout", true);

					vec3 locl = models[selectedModel].points[selectedPoint];
					ImGui::Text("Current point:\nX:%f\nY:%f\nZ:%f", locl.x, locl.y, locl.z);
					ImGui::Separator();
					ImGui::InputFloat("X", &xUpdate, 0.01f, 0.1f, "%.8f");
					ImGui::InputFloat("Y", &yUpdate, 0.01f, 0.1f, "%.8f");
					ImGui::InputFloat("Z", &zUpdate, 0.01f, 0.1f, "%.8f");

					bool setPoint = ImGui::Button("Confirm");
					if (setPoint)
					{
						models[selectedModel].points[selectedPoint] = vec3(xUpdate, yUpdate, zUpdate);
						selectedModel = selectedPoint = -1;
						selectedPoints = {};
						updatePoint = false;
						update = true;
						middlePress = false;
					}
					ImGui::NextColumn();

					ImGui::Text("Number of Selected Points:%d\n", selectedPoints.size());
					ImGui::InputFloat("XMove", &xMove, 0.01f, 0.1f, "%.8f");
					ImGui::InputFloat("YMove", &yMove, 0.01f, 0.1f, "%.8f");
					ImGui::InputFloat("ZMove", &zMove, 0.01f, 0.1f, "%.8f");
					bool movePoint = ImGui::Button("Move");
					if (movePoint)
					{
						std::set<int>::iterator it;
						it = selectedPoints.begin();
						while (it != selectedPoints.end())
						{
							vec3 selectedLocal = models[selectedModel].points[*it];
							selectedLocal.x = selectedLocal.x + xMove;
							selectedLocal.y = selectedLocal.y + yMove;
							selectedLocal.z = selectedLocal.z + zMove;
							models[selectedModel].points[*it] = selectedLocal;
							it++;
						}
						selectedModel = selectedPoint = -1;
						selectedPoints = {};
						updatePoint = false;
						update = true;
						middlePress = false;
					}
					ImGui::NextColumn();
					bool cancel = ImGui::Button("Cancel");
					if (cancel)
					{
						selectedModel = selectedPoint = -1;
						selectedPoints = {};
						updatePoint = false;
						middlePress = false;
					}
				}

				ImGui::End();
			}

		}

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion Imgui Saving/Loading

		glfwSwapBuffers(window);
		glfwPollEvents();
		CheckEvents(window);
	}

#pragma endregion Display

#pragma region
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	program = NULL;
	aiReleaseImport(scene);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
#pragma endregion Cleanup
}


