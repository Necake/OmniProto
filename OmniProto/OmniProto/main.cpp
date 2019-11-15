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

	Shader triangleShader("../OmniProto/triangle.vert", "../OmniProto/triangle.frag");
	Shader modelShader("../OmniProto/basicModel.vert", "../OmniProto/basicModel.frag");

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	ResourceManager::loadTexture("C:/Users/Nemanja/Desktop/OpenGLAssets/awesomeface.png", "face");
	glActiveTexture(GL_TEXTURE1);
	ResourceManager::loadTexture("C:/Users/Nemanja/Desktop/OpenGLAssets/container2.png", "container");
	triangleShader.use();
	triangleShader.setInt("texture1", 0);
	triangleShader.setInt("texture2", 1);

	Model nanoSuit("C:/Users/Nemanja/Desktop/OpenGLAssets/NanoSuit/nanosuit.obj");
	//================================================================================
	//Main loop
	//================================================================================
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(.1f, .4f, .4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		view = cam.GetViewMatrix();
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

		triangleShader.use();
		triangleShader.setFloat("sizeX", sin(glfwGetTime()));
		triangleShader.setMat4("view", view);
		triangleShader.setMat4("projection", projection);


		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)glfwGetTime() + i, glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));

			triangleShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE0);
			ResourceManager::getTexture("face").bind();
			glActiveTexture(GL_TEXTURE1);
			ResourceManager::getTexture("container").bind();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)));
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		modelShader.use();
		modelShader.setMat4("model", model);
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);
		nanoSuit.draw(modelShader);

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
}