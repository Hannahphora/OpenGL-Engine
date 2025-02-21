#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Window.h"
#include <cstdlib>
#include <stdio.h>

void BindInputActions(Window* window);

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Window* window = new Window();
	BindInputActions(window);

	// main loop
	while (!glfwWindowShouldClose(window->wnd)) {
		
		glClearColor(.0f, .0f, .0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window->wnd);
		glfwPollEvents();
	}

	// cleanup
	window->cleanup();
	delete window;
	return 0;
}

void BindInputActions(Window* window) {

	window->inputManager.bindKeyAction(GLFW_KEY_ESCAPE, GLFW_PRESS, [window]() {
		glfwSetWindowShouldClose(window->wnd, true);
	});

	window->inputManager.bindKeyAction(GLFW_KEY_F, GLFW_PRESS, [window]() {
		window->toggleFullscreen();
	});

	window->inputManager.bindMouseButtonAction(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, [window]() {
		window->toggleFullscreen();
	});
}