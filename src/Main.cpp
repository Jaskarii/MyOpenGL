#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Road.h"
#include "Car.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Main.h"

glm::mat4 proj;
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 MVP;

void setCameraTo(glm::vec2 position)
{
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 0.0f));
	MVP = proj * view; // Combine the translation with the MVP
}

void checkGLError()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error in " << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	float fWidth = width / 2;
	float fHeight = height / 2;
	proj = glm::ortho(-fWidth, fWidth, -fHeight, fHeight, -1.0f, 1.0f);
	glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float fwidth = width;
		float fheight = height;
		ypos = height - ypos;
		// Convert cursor position to OpenGL normalized coordinates
		// road->SetNewPath(Vector2(xpos, ypos));
	}
}

int main()
{
	if (!glfwInit())
	{
		return -1;
	}

	// Create a window and OpenGL context
	GLFWwindow *window = glfwCreateWindow(800, 800, "MyOpenGL", NULL, NULL);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float fWidth = width / 2;
	float fHeight = height / 2;
	proj = glm::ortho(-fWidth, fWidth, -fHeight, fHeight, -1.0f, 1.0f);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the OpenGL context associated with the window current
	glfwMakeContextCurrent(window);

	// Initialize GLEW or load OpenGL function pointers (if using GLEW/Glad)
	// glewInit(); // For GLEW initialization
	gladLoadGL();

	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Check for framebuffer size and set the viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	Renderer renderer;

	car = new Car();
	road = new Road();

	while (!glfwWindowShouldClose(window))
	{
		checkGLError();
		renderer.Clear();
		CheckKeyPress(window);

		setCameraTo(car->GetPosition());
		road->Render(MVP);

		car->SetCrashed(road->IsOffRoad(car->GetPosition()));

		car->Render(proj);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void CheckKeyPress(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		car->Rotate(0.015f);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		car->Rotate(-0.015f);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		car->Accelerate(0.015f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		car->Accelerate(-0.015f);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		scale += 0.05;
		scale = std::min(scale, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		scale -= 0.05;
		scale = std::max(scale, 0.1f);
	}
}