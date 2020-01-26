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

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "ResourceManager.h"
#include "RayUtil.h"
#include "Cubemap.h"
#include "Particle.h"

//Function prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

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
glm::vec3 projectilePos(0.0f, 0.0f, 0.0f);

bool simulationStarted = false;

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
	Shader diffuseShader("../OmniProto/diffuse.vert", "../OmniProto/diffuse_tex.frag", "../OmniProto/diffuse.geom");
	//Shader diffuseShaderInstanced("../OmniProto/diffuseInstanced.vert", "../OmniProto/diffuse_tex.frag", "../OmniProto/diffuse.geom");
	Shader normalShader("../OmniProto/drawNormals.vert", "../OmniProto/drawNormals.frag", "../OmniProto/calcNormals.geom");
	Shader normalCalcShader("../OmniProto/simpleFalloff.vert", "../OmniProto/drawNormals.frag", "../OmniProto/simpleFalloff.geom");
	Shader unlitShader("../OmniProto/unlit.vert", "../OmniProto/unlit.frag");
	Shader modelShader("../OmniProto/simpleFalloff.vert", "../OmniProto/simpleFalloff.frag", "../OmniProto/simpleFalloff.geom");
	Shader rayShader("../OmniProto/ray.vert", "../OmniProto/ray.frag");
	Shader skyboxShader("../OmniProto/skybox.vert", "../OmniProto/skybox.frag");

	Shader instanceShader("../OmniProto/instancing.vert", "../OmniProto/instancing.frag");

	//Loading textures
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/awesomeface.png", "face");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2.png", "container");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2_specular.png", "containerSpecular");
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
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Asteroid/rock.obj", "asteroid");
	ResourceManager::getModel("sphere").getSpecs();
	ResourceManager::getModel("light").getSpecs();
	RayUtil::initAxes(); //Allocating memory and initializing axis rays
	Ray projectileRay(glm::vec3(0, 0, 0), glm::vec3(0, -0.01, 0), glm::vec3(1, 1, 1)); //Projectile ray for affecting falloff (see simpleFalloff shader)

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
	//Main loop
	//================================================================================
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(.05f, 0.05f, .05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Filling the first half of the uniform buffer with the projection matrix
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		//Filling the second half of the uniform buffer with the view matrix
		glm::mat4 view = glm::mat4(1.0f);
		view = cam.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//render XYZ axes
		glm::mat4 model = glm::mat4(1.0f);
		RayUtil::renderAxes(model, rayShader);

		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Draw everything in wireframe from this point if told so (see processInput)
		

		//Render deformable model
		//model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f))); //model matrix setup, rotation changes based on time
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		modelShader.use(); //Shader calls
		modelShader.setMat4("model", model);
		modelShader.setVec3("projectilePos", projectilePos);
		modelShader.setFloat("time", glfwGetTime());
		modelShader.setFloat("radius", falloffRadius);
		modelShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		modelShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("lightPos", lightPos);
		modelShader.setVec3("viewPos", cam.Position);
		ResourceManager::getModel("sphereLowRes").draw(modelShader); //Render with appropriate shader

		//Render the two textured spheres, same idea as above, just with more light-related shader calls (see diffuse / diffuse_tex shader)
		diffuseShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2, 0, 0));
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
		diffuseShader.setMat4("model", model);
		diffuseShader.setVec3("material.ambient", 0.34f, 1.0f, 0.75f);
		diffuseShader.setVec3("material.diffuse", 1.0f, 0.34f, 0.95f);
		diffuseShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		diffuseShader.setFloat("material.shininess", 32.0f);
		diffuseShader.setVec3("pointLight.position", lightPos);
		diffuseShader.setVec3("pointLight.ambient", 0.4f, 0.4f, 0.4f);
		diffuseShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		diffuseShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		diffuseShader.setFloat("pointLight.constant", 1.0f);
		diffuseShader.setFloat("pointLight.linear", 0.09f);
		diffuseShader.setFloat("pointLight.quadratic", 0.032f);
		diffuseShader.setVec3("dirLight.direction", 1.0f, 1.0f, 0);
		diffuseShader.setVec3("dirLight.ambient", 0.2f, 0.1f, 0.05f);
		diffuseShader.setVec3("dirLight.diffuse", 0.4f, 0.2f, 0.1f);
		diffuseShader.setVec3("dirLight.specular", 1.0f, 0.8f, 0.5f);
		diffuseShader.setVec3("flashLight.position", cam.Position);
		diffuseShader.setVec3("flashLight.direction", cam.Front);
		diffuseShader.setFloat("flashLight.cutoff", glm::cos(glm::radians(12.5f)));
		diffuseShader.setFloat("flashLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		diffuseShader.setFloat("flashLight.constant", 1.0f);
		diffuseShader.setFloat("flashLight.linear", 0.09f);
		diffuseShader.setFloat("flashLight.quadratic", 0.032f);
		diffuseShader.setVec3("flashLight.ambient", 0, 0, 0);
		diffuseShader.setVec3("flashLight.diffuse", 0.8f, 0.8f, 0.8f);
		diffuseShader.setVec3("flashLight.specular", 1.0f, 1.0f, 1.0f);
		diffuseShader.setVec3("viewPos", cam.Position);
		diffuseShader.setInt("material.texture_diffuse1", 0);
		diffuseShader.setInt("material.texture_specular1", 1);
		//diffuseShader.setFloat("time", (float)glfwGetTime());
		glActiveTexture(GL_TEXTURE0); //Bind externally loaded textures to model
		ResourceManager::getTexture("container").bind();
		glActiveTexture(GL_TEXTURE1);
		ResourceManager::getTexture("containerSpecular").bind();
		ResourceManager::getModel("sphere").draw(diffuseShader);

		normalShader.use();
		normalShader.setMat4("model", model);
		ResourceManager::getModel("sphere").draw(normalShader);
		
		diffuseShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(20,0,0));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		diffuseShader.setMat4("model", model);
		diffuseShader.setInt("skybox", 4);
		glActiveTexture(GL_TEXTURE4); //Bind skybox to the diffuse shader for reflections
		skybox.bind();
		ResourceManager::getModel("sphere").draw(diffuseShader);

		normalShader.use();
		normalShader.setMat4("model", model);
		ResourceManager::getModel("sphere").draw(normalShader);

		if(simulationStarted)
			particle.update(deltaTime);

		//Draw projectile ray
		rayShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, projectilePos);
		rayShader.setMat4("model", model);
		projectileRay.draw(rayShader);

		//Draw skybox, disable wireframe and change depth testing for faster drawing
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthFunc(GL_LEQUAL);

		glDepthMask(false);
		skyboxShader.use();
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(cam.GetViewMatrix()))); //View matrix is a bit different here, since we disregard translations
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		skybox.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);

		//Reenable wireframe if needed and return depth func to normal
		glDepthFunc(GL_LESS);
		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Draw light, half transparent
		unlitShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
		unlitShader.setMat4("model", model);
		ResourceManager::getModel("light").draw(unlitShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ResourceManager::Clear();
	glfwTerminate();
	return 0;
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
}