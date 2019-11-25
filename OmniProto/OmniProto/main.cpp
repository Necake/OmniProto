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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int windowWidth = 800; int windowHeight = 800;
float deltaTime = 0, lastFrame = 0.0f;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

float falloffRadius = 2.0f;
glm::vec3 projectilePos(0.0f, 0.0f, 0.0f);

int main()
{
	//================================================================================
	//Init
	//================================================================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	//================================================================================
	//Geometry setup
	//================================================================================
	float vertices[] = {
	// Back face
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
   // Front face
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
   // Left face
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
   // Right face
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
   // Bottom face
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
   // Top face
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left   
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

	Shader diffuseShader("../OmniProto/diffuse.vert", "../OmniProto/diffuse.frag");
	Shader unlitShader("../OmniProto/unlit.vert", "../OmniProto/unlit.frag");
	Shader modelShader("../OmniProto/simpleFalloff.vert", "../OmniProto/simpleFalloff.frag");
	Shader rayShader("../OmniProto/ray.vert", "../OmniProto/ray.frag");

	ResourceManager::loadTexture("C:/Users/Nemanja/Desktop/OpenGLAssets/awesomeface.png", "face");
	ResourceManager::loadTexture("C:/Users/Nemanja/Desktop/OpenGLAssets/container2.png", "container");
	ResourceManager::loadModel("E:/Epski projekat dva tacka nula/OpenGLAssets/testModels/testSphere.obj", "sphere");
	ResourceManager::loadModel("E:/Epski projekat dva tacka nula/OpenGLAssets/testModels/testCube.obj", "light");
	ResourceManager::getModel("sphere").getSpecs();
	ResourceManager::getModel("light").getSpecs();
	RayUtil::initAxes();
	Ray projectileRay(glm::vec3(0, 0, 0), glm::vec3(0, -0.01, 0), glm::vec3(1, 1, 1));

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
		
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(.3f, .3f, .3f));
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
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		unlitShader.setMat4("model", model);
		unlitShader.setMat4("view", view);
		unlitShader.setMat4("projection", projection);
		ResourceManager::getModel("light").draw(unlitShader);

		diffuseShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2, 0, 0));
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(.3f, .3f, .3f));
		diffuseShader.setMat4("model", model);
		diffuseShader.setMat4("view", view);
		diffuseShader.setMat4("projection", projection);
		diffuseShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		diffuseShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		diffuseShader.setVec3("lightPos", lightPos);
		diffuseShader.setVec3("viewPos", cam.Position);
		ResourceManager::getModel("sphere").draw(diffuseShader);

		rayShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, projectilePos);
		rayShader.setMat4("model", model);
		projectileRay.draw(rayShader);

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
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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