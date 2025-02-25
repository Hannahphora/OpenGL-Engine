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

	// create shaders
	ShaderProgram shaders("shaders\\base.vert", "shaders\\base.frag");


	// vertex data
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	uint32_t VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// timing
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
		glClearColor(.0f, .0f, .0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

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