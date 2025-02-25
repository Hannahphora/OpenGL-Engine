#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Window.h"
#include "Shader.h"
#include <cstdlib>
#include <iostream>

void createInputActions(Window* window);

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// init window & default input actions
	Window* window = new Window();
	createInputActions(window);

	// create shader program
	ShaderProgram shaders("shaders\\base.vert", "shaders\\base.frag");

	// vertex data
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {
		0, 1, 3,   // first tri
		1, 2, 3    // second tri
	};

	// create vao, vbo, and ebo
	uint32_t VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind vao
	glBindVertexArray(VAO);
	// bind and set vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// bind and set ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// set vert attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// unbind objects
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// opengl state
	glClearColor(.0f, .0f, .0f, 1.f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// timing variables
	double prevTime = glfwGetTime(), lag = 0.0;
	constexpr double simDelta = 1.0 / 60.0;

	// main loop
	while (!glfwWindowShouldClose(window->wnd)) {
		double currentTime = glfwGetTime();
		lag += currentTime - prevTime;
		prevTime = currentTime;

		while (lag >= simDelta) {
			// this updates at fixed sim rate
			lag -= simDelta;
		}

		// rendering
		glClear(GL_COLOR_BUFFER_BIT);

		shaders.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// swap buffers and process events
		glfwSwapBuffers(window->wnd);
		glfwPollEvents();
		window->inputManager->processActions();
	}

	// cleanup
	window->cleanup();
	delete window;
	return 0;
}

void createInputActions(Window* window) {

	// quit
	window->inputManager->registerAction("Quit", Binding{ .type = Binding::Type::Key, .code = GLFW_KEY_ESCAPE, .event = GLFW_PRESS }, [window]() {
		glfwSetWindowShouldClose(window->wnd, true);
	});

	// toggle fullscreen
	window->inputManager->registerAction("ToggleFullscreen", Binding{ .type = Binding::Type::Key, .code = GLFW_KEY_F, .event = GLFW_PRESS }, [window]() {
		window->toggleFullscreen();
	});

}