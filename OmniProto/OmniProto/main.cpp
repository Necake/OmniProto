//========================================================================================
//main.cpp - Main entry point for the program, contains the main loop in which
// all objects are rendered 
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#include<GLAD\glad.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<ft2build.h>
#include FT_FREETYPE_H

#include<iostream>
#include<string>

#include "Shader.h"
#include "camera.h"
#include "model.h"
#include "ResourceManager.h"
#include "RayUtil.h"
#include "Cubemap.h"
#include "Particle.h"
#include "DiffuseShader.h"

//Function prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setupSharedMatrices(GLFWwindow* window, unsigned int UBOMatrices, glm::mat4* projection, glm::mat4* view);
void drawDeformable(glm::mat4* model, Shader* shader);
void renderScene(Shader* rayShader, Shader* modelShader, Shader* normalShader, Shader* skyboxShader, DiffuseShader* diffuseShader, Ray* projectileRay, Cubemap skybox, glm::mat4 projection, unsigned int skyboxVAO);
void renderShadowPass(Shader* simpleDepthShader, glm::mat4 projection);
void renderQuad();

//Program-wide constants
int windowWidth = 800; int windowHeight = 800;
float deltaTime = 0, lastFrame = 0.0f;
//Camera-related constants
Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true, isWireframe = false;
//Falloff related constants (see simpleFalloff shader)
float falloffRadius = 2.0f;
glm::vec3 projectilePos(0.0f, 3.0f, 0.0f);

bool simulationStarted = false;
bool blinn = false;

extern "C"
{ //Use the discrete GPU instead of the onboard GPU (AMD only, for NVidia see http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf)
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
	//================================================================================
	//Init
	//================================================================================
	glfwInit(); //Initialize glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Set OpenGL version and core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	windowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width; //Get window size
	windowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "O M N I", NULL, NULL); //Create window
	if (window == NULL) //Fail check
	{
		std::cout << "Failed to create window!" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window); //Render to current window

	//Loading opengl func pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}
	glViewport(0, 0, windowWidth, windowHeight); //Set gl viewport to according window size
	
	//Callback setup
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST); //Enable depth testing
	glEnable(GL_CULL_FACE); //Enable face culling
	glCullFace(GL_BACK); //Tell opengl to cull back faces
	glFrontFace(GL_CCW); //Set the front faces to be counter-clockwise winded
	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set according blend mode
	glLineWidth(2.5); //Set wireframe line width a bit thicker than normal
	glEnable(GL_FRAMEBUFFER_SRGB);

	//================================================================================
	//Geometry setup
	//================================================================================
	//Vertices for the skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f); //Position of the point light (represented by the light bulb)

	//Loading and compiling shaders (TODO: move to resource manager)
	DiffuseShader diffuseShader(true);
	diffuseShader.matAmbient = glm::vec3(0.34f, 1.0f, 0.75f);
	diffuseShader.matDiffuse = glm::vec3(1.0f, 0.34f, 0.95f);
	diffuseShader.matSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	diffuseShader.matShine = 32.0f;
	diffuseShader.plPosition = lightPos;
	diffuseShader.plAmbient = glm::vec3(0.4f, 0.4f, 0.4f);
	diffuseShader.plDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	diffuseShader.plSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	diffuseShader.plConst = 1.0f;
	diffuseShader.plLinear = 0.09f;
	diffuseShader.plQuadratic = 0.032f;
	diffuseShader.dlDirection = glm::normalize(glm::vec3(2.0f, -4.0f, 1.0f));
	diffuseShader.dlAmbient = glm::vec3(0.2f, 0.1f, 0.05f);
	diffuseShader.dlDiffuse = glm::vec3(0.4f, 0.2f, 0.1f);
	diffuseShader.dlSpecular = glm::vec3(1.0f, 0.8f, 0.5f);
	diffuseShader.flAmbient = glm::vec3(0, 0, 0);
	diffuseShader.flDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	diffuseShader.flSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	diffuseShader.flConst = 1.0f;
	diffuseShader.flLinear = 0.09f;
	diffuseShader.flQuadratic = 0.032f;
	diffuseShader.flCutoff = glm::cos(glm::radians(12.5f));
	diffuseShader.flOuterCutoff = glm::cos(glm::radians(17.5f));

	//Shader diffuseShaderInstanced("../OmniProto/diffuseInstanced.vert", "../OmniProto/diffuse_tex.frag", "../OmniProto/diffuse.geom");
	Shader normalShader("../OmniProto/drawNormals.vert", "../OmniProto/drawNormals.frag", "../OmniProto/calcNormals.geom");
	Shader normalCalcShader("../OmniProto/simpleFalloff.vert", "../OmniProto/drawNormals.frag", "../OmniProto/simpleFalloff.geom");
	Shader unlitShader("../OmniProto/unlit.vert", "../OmniProto/unlit.frag");
	Shader modelShader("../OmniProto/simpleFalloff.vert", "../OmniProto/simpleFalloff.frag", "../OmniProto/simpleFalloff.geom");
	Shader rayShader("../OmniProto/ray.vert", "../OmniProto/ray.frag");
	Shader skyboxShader("../OmniProto/skybox.vert", "../OmniProto/skybox.frag");
	Shader simpleDepthShader("../OmniProto/simpleDepthShader.vert", "../OmniProto/simpleDepthShader.frag");
	Shader debugQuad("../OmniProto/debugQuad.vert", "../OmniProto/debugQuad.frag");
	
	Shader instanceShader("../OmniProto/instancing.vert", "../OmniProto/instancing.frag");

	//Loading textures
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/awesomeface.png", "face");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2.png", "container");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2_specular.png", "containerSpecular");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/wood.jpg", "wood");
	const char* skyboxTextures[6] = { //Skybox textures for cubemap setup
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/right.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/left.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/top.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/bot.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/front.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/back.png"
	};
	Cubemap skybox(skyboxTextures);
	//Loading models and logging their sizes
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/sphereMapped.obj", "sphere");
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/sphereLowRes.obj", "sphereLowRes");
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/bulb.obj", "light");
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Planet/planet.obj", "planet");
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/floorPlane.obj", "floor");
	ResourceManager::getModel("sphere").getSpecs();
	ResourceManager::getModel("light").getSpecs();
	ResourceManager::getModel("floor").getSpecs();
	RayUtil::initAxes(); //Allocating memory and initializing axis rays
	Ray projectileRay(glm::vec3(0, 0, 0), glm::vec3(0, -0.01, 0), glm::vec3(1, 0, 1)); //Projectile ray for affecting falloff (see simpleFalloff shader)

	//VAO and VBO for the skybox
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	unsigned int amount = 5000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 30;
	float offset = 10.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	unsigned int modelInstanceBuffer;
	glGenBuffers(1, &modelInstanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, modelInstanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	int asteroidMeshesSize = ResourceManager::getModel("asteroid").meshes.size();
	for (unsigned int i = 0; i < asteroidMeshesSize; i++)
	{
		unsigned int VAO = ResourceManager::getModel("asteroid").meshes[i].VAO;
		glBindVertexArray(VAO);
		// vertex Attributes
		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	glUniformBlockBinding(diffuseShader.ID, glGetUniformBlockIndex(diffuseShader.ID, "Matrices"), 0);
	glUniformBlockBinding(unlitShader.ID, glGetUniformBlockIndex(unlitShader.ID, "Matrices"), 0);
	glUniformBlockBinding(modelShader.ID, glGetUniformBlockIndex(modelShader.ID, "Matrices"), 0);
	glUniformBlockBinding(rayShader.ID, glGetUniformBlockIndex(rayShader.ID, "Matrices"), 0);
	glUniformBlockBinding(normalShader.ID, glGetUniformBlockIndex(normalShader.ID, "Matrices"), 0);
	//glUniformBlockBinding(diffuseShaderInstanced.ID, glGetUniformBlockIndex(diffuseShaderInstanced.ID, "Matrices"), 0);
	//Init uniform buffer that holds the projection and view matrix (shared across multiple shaders)
	unsigned int UBOMatrices;
	glGenBuffers(1, &UBOMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, 2 * sizeof(glm::mat4));

	Particle particle(ResourceManager::getModel("sphereLowRes"), diffuseShader, 3, glm::vec3(0, 0, 0), glm::vec3(10, 0, 0), 10);

	//================================================================================
	//SHADOW MAPPING
	//================================================================================
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 1.0f, far_plane = 15.0f;

	//================================================================================
	//Main loop
	//================================================================================
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);


		glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glm::mat4 projection;
		glm::mat4 view;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		setupSharedMatrices(window, UBOMatrices, &projection, &view);
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glCullFace(GL_FRONT);
		renderShadowPass(&simpleDepthShader, projection);
		glCullFace(GL_BACK);
		renderShadowPass(&simpleDepthShader, projection);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(.05f, 0.05f, .05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setupSharedMatrices(window, UBOMatrices, &projection, &view);

		diffuseShader.use();
		diffuseShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		diffuseShader.setInt("shadowMap", 5);

		renderScene(&rayShader, &modelShader, &normalShader, &skyboxShader, &diffuseShader, &projectileRay, skybox, projection, skyboxVAO);
		debugQuad.use();
		debugQuad.setFloat("near_plane", near_plane);
		debugQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		if(!blinn)
			renderQuad();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ResourceManager::Clear();
	glfwTerminate();
	return 0;
}

void renderScene(Shader* rayShader, Shader* modelShader, Shader* normalShader, Shader* skyboxShader, DiffuseShader* diffuseShader, Ray* projectileRay, Cubemap skybox, glm::mat4 projection, unsigned int skyboxVAO)
{

	//render XYZ axes
	glm::mat4 model = glm::mat4(1.0f);
	RayUtil::renderAxes(model, *rayShader);

	if (isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Draw everything in wireframe from this point if told so (see processInput)

	drawDeformable(&model, modelShader);

	//Draw projectile ray
	rayShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, projectilePos);
	rayShader->setMat4("model", model);
	projectileRay->draw(*rayShader);

	//Render the two textured spheres, same idea as above, just with more light-related shader calls (see diffuse / diffuse_tex shader)
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2, 0, 0));
	model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));

	diffuseShader->use();
	diffuseShader->setInt("blinn", blinn);
	diffuseShader->setupGeneral(cam.Position, model, 4);
	diffuseShader->updateMaterial();
	diffuseShader->updatePointLight();
	diffuseShader->updateDirecitonalLight();
	diffuseShader->setupFlashLight(cam.Position, cam.Front);
	diffuseShader->setInt("skybox", 4);
	glActiveTexture(GL_TEXTURE4); //Bind skybox to the diffuse shader for reflections
	skybox.bind();

	diffuseShader->setInt("material.texture_diffuse1", 0);
	diffuseShader->setInt("material.texture_specular1", 1);
	diffuseShader->setFloat("material.reflectiveness", 0.5);
	ResourceManager::getModel("sphere").draw(*diffuseShader);

	diffuseShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5, 0, 0));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	diffuseShader->setMat4("model", model);
	ResourceManager::getModel("sphere").draw(*diffuseShader);

	normalShader->use();
	normalShader->setMat4("model", model);
	ResourceManager::getModel("sphere").draw(*normalShader);

	//draw floor
	diffuseShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5, -0.5f, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	diffuseShader->setMat4("model", model);
	diffuseShader->setFloat("material.reflectiveness", 0.2);
	ResourceManager::getModel("floor").draw(*diffuseShader);

	//if (simulationStarted)
	//	particle.update(deltaTime);

	//Draw skybox, disable wireframe and change depth testing for faster drawing
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(false);
	skyboxShader->use();
	skyboxShader->setMat4("view", glm::mat4(glm::mat3(cam.GetViewMatrix()))); //View matrix is a bit different here, since we disregard translations
	skyboxShader->setMat4("projection", projection);
	glBindVertexArray(skyboxVAO);
	skybox.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);

	//Reenable wireframe if needed and return depth func to normal
	glDepthFunc(GL_LESS);
	if (isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Draw light, half transparent
	/*unlitShader.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
	unlitShader.setMat4("model", model);
	ResourceManager::getModel("light").draw(unlitShader);*/
}
void renderShadowPass(Shader* simpleDepthShader, glm::mat4 projection)
{

	glm::mat4 model = glm::mat4(1.0f);

	if (isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Draw everything in wireframe from this point if told so (see processInput)

	drawDeformable(&model, simpleDepthShader);


	//Render the two textured spheres, same idea as above, just with more light-related shader calls (see diffuse / diffuse_tex shader)
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2, 0, 0));
	model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));

	simpleDepthShader->setMat4("model", model);
	ResourceManager::getModel("sphere").draw(*simpleDepthShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5, 0, 0));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	simpleDepthShader->setMat4("model", model);
	ResourceManager::getModel("sphere").draw(*simpleDepthShader);


	//draw floor
	simpleDepthShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5, -0.5f, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	simpleDepthShader->setMat4("model", model);
	ResourceManager::getModel("floor").draw(*simpleDepthShader);


	//Draw light, half transparent
	/*unlitShader.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
	unlitShader.setMat4("model", model);
	ResourceManager::getModel("light").draw(unlitShader);*/
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
//Callback function for window resizing
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Callback function for mouse movement handling
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	cam.ProcessMouseMovement(xoffset, yoffset);
}

//Callback function for mouse scroll handling
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
}

//Keyboard input callback function
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true); //Quit on esc press
	}
	
	float camSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //Camera movement
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);

	//Model wireframe toggle
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
		isWireframe = true;
	else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		isWireframe = false;

	//Resizing for the model falloff param (See simpleFalloff shader)
	float resizeSpeed = camSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		falloffRadius += resizeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		falloffRadius -= resizeSpeed;
	}

	//Movement of the effector for falloff calculation (see simpleFalloff shader)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		projectilePos.y += resizeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		projectilePos.y -= resizeSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		projectilePos.x += resizeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		projectilePos.x -= resizeSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		simulationStarted = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		blinn = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		blinn = false;
	}
}

void setupSharedMatrices(GLFWwindow* window, unsigned int UBOMatrices, glm::mat4* projection, glm::mat4* view)
{

	//Filling the first half of the uniform buffer with the projection matrix
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	*projection = glm::perspective(glm::radians(cam.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(*projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//Filling the second half of the uniform buffer with the view matrix
	*view = glm::mat4(1.0f);
	*view = cam.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(*view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void drawDeformable(glm::mat4* model, Shader* shader)
{
	//Render deformable model
		//model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f))); //model matrix setup, rotation changes based on time
	*model = glm::scale(*model, glm::vec3(0.01f, 0.01f, 0.01f));
	shader->use(); //Shader calls
	shader->setMat4("model", *model);
	shader->setVec3("projectilePos", projectilePos);
	shader->setFloat("time", glfwGetTime());
	shader->setFloat("radius", falloffRadius);
	shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	//modelShader.setVec3("lightPos", lightPos);
	shader->setVec3("viewPos", cam.Position);
	ResourceManager::getModel("sphereLowRes").draw(*shader); //Render with appropriate shader
}