#include "Window.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>

// callbacks

void error_callback(int error, const char* description) {
	fprintf_s(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// window constructor

Window::Window() {

	// init glfw
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf_s(stderr, "Failed to init glfw\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// get monitor & create window
	monitor = glfwGetPrimaryMonitor();
	wnd = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
	if (!wnd) {
		fprintf_s(stderr, "Failed to create window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// register window & callbacks
	glfwSetWindowUserPointer(wnd, this);
	glfwMakeContextCurrent(wnd);
	glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf_s(stderr, "Failed to init glad\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// init InputManager
	inputManager = new InputManager(wnd);
}

// window funcs

void Window::cleanup() {
	delete inputManager;
	glfwTerminate();
}

void Window::toggleFullscreen() {
	if (!fullscreen) {
		glfwGetWindowSize(wnd, &width, &height);
		glfwGetWindowPos(wnd, &xpos, &ypos);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(wnd, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else glfwSetWindowMonitor(wnd, nullptr, xpos, ypos, width, height, 0);
	fullscreen = !fullscreen;
}

void Window::toggleShowCursor() {
	if (!cursorvis) glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	cursorvis = !cursorvis;
}

void Window::setShowCursor(bool b) {
	cursorvis = b;
	if (cursorvis) glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}