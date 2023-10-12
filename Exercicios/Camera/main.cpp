#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Common/include/Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int setupGeometry();

bool firstMouse = true;
float lastX = 0.0, lastY = 0.0;
float yaw = -90.0, pitch = 0.0;

const GLuint WIDTH = 1000, HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
float cameraSpeed = 0.05;

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicios", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION); 
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../../Common/shaders/shader.vs", "../../Common/shaders/shader.fs");

	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	glm::mat4 model = glm::mat4(1); GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1); 

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 42);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	const float speed = 0.05f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_1:
			cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
			cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
			break;
		case GLFW_KEY_2:
			cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
			cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		case GLFW_KEY_3:
			cameraPos = glm::vec3(-3.0f, 0.0f, 0.0f);
			cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
			cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case GLFW_KEY_4:
			cameraPos = glm::vec3(3.0f, 0.0f, 0.0f);
			cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
			cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case GLFW_KEY_5:
			cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
			cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
			cameraUp = glm::vec3(-1.0f, 0.0f, 0.0f);
			break;
		case GLFW_KEY_W:
			cameraPos += cameraFront * speed;
			break;
		case GLFW_KEY_S:
			cameraPos -= cameraFront * speed;
			break;
		case GLFW_KEY_A:
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
			break;
		case GLFW_KEY_D:
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
			break;
		}
	}
}

int setupGeometry()
{
	GLfloat vertices[] = {
		// Triângulo 1 (base da pirâmide) - Azul
		-0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 
		-0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 
		0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 

		// Triângulo 2 (base da pirâmide) - Azul
		0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 
		-0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 
		0.5, -0.5, 0.5, 0.0, 0.0, 1.0, 

		// Triângulo 3 (lado 1) - Vermelho
		0.5, -0.5, 0.5, 1.0, 0.0, 0.0, 
		0.5, -0.5, -0.5, 1.0, 0.0, 0.0, 
		0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 

		// Triângulo 4 (lado 1) - Vermelho
		0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 
		0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 
		0.5, -0.5, 0.5, 1.0, 0.0, 0.0, 

		// Triângulo 5 (lado 2) - Verde
		-0.5, -0.5, 0.5, 0.0, 1.0, 0.0, 
		0.5, -0.5, 0.5, 0.0, 1.0, 0.0, 
		0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 

		// Triângulo 6 (lado 2) - Verde
		0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 
		-0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 
		-0.5, -0.5, 0.5, 0.0, 1.0, 0.0, 

		// Triângulo 7 (lado 3) - Amarelo
		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 
		-0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 

		// Triângulo 8 (lado 3) - Amarelo
		-0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 
		-0.5, 0.5, 0.5, 1.0, 1.0, 0.0, 
		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,

		// Triângulo 9 (lado 4) - Roxo
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0, 
		-0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 
		-0.5, 0.5, 0.5, 1.0, 0.0, 1.0, 

		// Triângulo 10 (lado 4) - Roxo
		-0.5, 0.5, 0.5, 1.0, 0.0, 1.0, 
		-0.5, 0.5, -0.5, 1.0, 0.0, 1.0,
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		//"Chão" - Cinza
		-5.0, -0.5, -5.0, 0.5, 0.5, 0.5, // ponto V
		-5.0, -0.5, 5.0, 0.5, 0.5, 0.5, // ponto W
		 5.0, -0.5, -5.0, 0.5, 0.5, 0.5, // ponto X
	
		-5.0, -0.5, 5.0, 0.5, 0.5, 0.5, // ponto W
		5.0, -0.5, 5.0, 0.5, 0.5, 0.5, // ponto Y
		-5.0, -0.5, 5.0, 0.5, 0.5, 0.5, // ponto W

		-5.0, -0.5, -5.0, 0.5, 0.5, 0.5, // ponto V
		5.0, -0.5, -5.0, 0.5, 0.5, 0.5, // ponto X
		5.0, -0.5, 5.0, 0.5, 0.5, 0.5  // ponto Y
	};
	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	offsetx *= cameraSpeed;
	offsety *= cameraSpeed;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}
