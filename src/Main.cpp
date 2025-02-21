#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Window.h"
#include <cstdlib>
#include <iostream>

void createInputActions(Window* window);

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Window* window = new Window();
	createInputActions(window);

	// main loop
	while (!glfwWindowShouldClose(window->wnd)) {

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