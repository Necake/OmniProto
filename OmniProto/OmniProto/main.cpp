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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int windowWidth = 800; int windowHeight = 800;
float deltaTime = 0, lastFrame = 0.0f;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true, isWireframe = false;

float falloffRadius = 2.0f;
glm::vec3 projectilePos(0.0f, 0.0f, 0.0f);

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
	//================================================================================
	//Init
	//================================================================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	windowWidth = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	windowHeight = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "bottom text", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window!" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Loading opengl func pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //tell opengl to cull back faces
	glFrontFace(GL_CCW); //set the front faces to be counter-clockwise winded

	glLineWidth(2.5);

	//================================================================================
	//Geometry setup
	//================================================================================
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

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	Shader diffuseShader("../OmniProto/diffuse.vert", "../OmniProto/diffuse_tex.frag");
	Shader unlitShader("../OmniProto/unlit.vert", "../OmniProto/unlit.frag");
	Shader modelShader("../OmniProto/simpleFalloff.vert", "../OmniProto/simpleFalloff.frag");
	Shader rayShader("../OmniProto/ray.vert", "../OmniProto/ray.frag");
	Shader skyboxShader("../OmniProto/skybox.vert", "../OmniProto/skybox.frag");
	Shader geomTestShader("../OmniProto/deleteMe.vert", "../OmniProto/deleteMe.frag", "../OmniProto/deleteMe.geom");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/awesomeface.png", "face");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2.png", "container");
	ResourceManager::loadTexture("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/container2_specular.png", "containerSpecular");
	const char* skyboxTextures[6] = {
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/right.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/left.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/top.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/bot.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/front.png",
		"C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/Skybox/back.png"
	};
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/sphere.obj", "sphere");
	ResourceManager::loadModel("C:/Users/Nemanja/Documents/_Dev/OpenGLAssets/sphere.obj", "light");
	ResourceManager::getModel("sphere").getSpecs();
	ResourceManager::getModel("light").getSpecs();
	RayUtil::initAxes();
	Ray projectileRay(glm::vec3(0, 0, 0), glm::vec3(0, -0.01, 0), glm::vec3(1, 1, 1));

	
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	Cubemap skybox(skyboxTextures);

	float points[] = {
	-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
	 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
	-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

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


		glm::mat4 view = glm::mat4(1.0f);
		view = cam.GetViewMatrix();
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		RayUtil::renderAxes(view, model, projection, rayShader);
		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		modelShader.use();
		modelShader.setMat4("model", model);
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);
		modelShader.setVec3("projectilePos", projectilePos);
		modelShader.setFloat("time", glfwGetTime());
		modelShader.setFloat("radius", falloffRadius);
		modelShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		modelShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("lightPos", lightPos);
		modelShader.setVec3("viewPos", cam.Position);
		ResourceManager::getModel("sphere").draw(modelShader);

		unlitShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
		unlitShader.setMat4("model", model);
		unlitShader.setMat4("view", view);
		unlitShader.setMat4("projection", projection);
		ResourceManager::getModel("light").draw(unlitShader);

		diffuseShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2, 0, 0));
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
		diffuseShader.setMat4("model", model);
		diffuseShader.setMat4("view", view);
		diffuseShader.setMat4("projection", projection); 
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
		glActiveTexture(GL_TEXTURE0);
		ResourceManager::getTexture("container").bind();
		glActiveTexture(GL_TEXTURE1);
		ResourceManager::getTexture("containerSpecular").bind();
		ResourceManager::getModel("sphere").draw(diffuseShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(20,0,0));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		diffuseShader.setMat4("model", model);
		diffuseShader.setInt("skybox", 4);
		glActiveTexture(GL_TEXTURE4);
		skybox.bind();
		ResourceManager::getModel("sphere").draw(diffuseShader);


		rayShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, projectilePos);
		rayShader.setMat4("model", model);
		projectileRay.draw(rayShader);

		geomTestShader.use();
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_POINTS, 0, 4);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthFunc(GL_LEQUAL);

		glDepthMask(false);
		skyboxShader.use();
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(cam.GetViewMatrix())));
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		skybox.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);

		glDepthFunc(GL_LESS);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	float camSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
		isWireframe = true;
	else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		isWireframe = false;

	float resizeSpeed = camSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		falloffRadius += resizeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		falloffRadius -= resizeSpeed;
	}

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
}