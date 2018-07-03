#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

//==================================DEFINE=====================================
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int WIN_WIDTH = 1000;
const unsigned int WIN_HEIGHT = 750;

const char* modelVSPath = "Shader/colors.vs";
const char* modelFSPath = "Shader/colors.fs";
const char* depth_testVSPath = "Shader/depth_test.vs";
const char* depth_testFSPath = "Shader/depth_test.fs";
const char* modelPath = "Model/UE4ShaderBall.obj";
//const char* modelPath = "Model/nanosuit.obj";

float angle;

Camera mainCamera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = WIN_WIDTH / 2;
float lastY = WIN_HEIGHT / 2;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
//glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

//================================Main==========================================
int main() {
	//Base Setting & Window Setting========================================================
	//GLFW Init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create Window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//GLAD Init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//END==========================================================================
	
	glEnable(GL_DEPTH_TEST);

	//Setup Shader
	Shader modelShader(modelVSPath, modelFSPath);
	Shader depthTestShader(depth_testVSPath, depth_testFSPath);

	//load model
	Model ourModel(modelPath);

	//Transformation Matrix
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;

	//============================================================================
	while (!glfwWindowShouldClose(window)) {
		glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
		
		//frame-time set
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//get input
		processInput(window);

		//render vireport
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// camera/view transformation
		model = glm::mat4();
		view = mainCamera.GetViewMatrix();
		projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);	

		depthTestShader.use();
		depthTestShader.setVec3("viewPos", mainCamera.Position);
		depthTestShader.setMat4("projection", projection);
		depthTestShader.setMat4("view", view);
		
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		depthTestShader.setMat4("model", model);

		ourModel.Draw(depthTestShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, -3.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		depthTestShader.setMat4("model", model);
		ourModel.Draw(depthTestShader);
        //===================  second viewport  ==========================
		glEnable(GL_SCISSOR_TEST);//enable scissor test for second viewport
		glViewport(WIN_WIDTH - 400, WIN_HEIGHT - 300, 400, 300);
		glScissor(WIN_WIDTH - 400, WIN_HEIGHT - 300, 400, 300);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear area of scissor test
		//render vireport
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		model = glm::mat4();

		modelShader.use();
		modelShader.setVec3("viewPos", mainCamera.Position);
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);

		modelShader.setFloat("material.shininess", 32.0f);

		modelShader.setVec3("dirLight.direction", lightDirection);
		modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
		modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		modelShader.setMat4("model", model);
		ourModel.Draw(modelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, -3.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		modelShader.setMat4("model", model);
		ourModel.Draw(modelShader);
		glDisable(GL_SCISSOR_TEST);//disable scissor test

		//swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Clean resource
	glfwTerminate();
	return 0;
}

//Make sure viewport match the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(width - 400, 0, 400, 300);
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	mainCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	mainCamera.ProcessMouseScroll(yoffset);
}

//Input function
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(RIGHT, deltaTime);
}