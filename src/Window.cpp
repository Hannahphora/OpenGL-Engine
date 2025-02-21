#include "Window.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>

// GENERAL CALLBACKS

void error_callback(int error, const char* description) {
	fprintf_s(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// INPUT CALLBACKS

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* wnd = (Window*)glfwGetWindowUserPointer(window);
	wnd->inputManager.updateKeyState(key, action);
	wnd->inputManager.processKeyActions(); // should i call processActions here, or in main loop?
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	Window* wnd = (Window*)glfwGetWindowUserPointer(window);
	wnd->inputManager.updateMouseButtonState(button, action);
	wnd->inputManager.processMouseButtonActions();
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	Window* wnd = (Window*)glfwGetWindowUserPointer(window);
	wnd->inputManager.updateMousePosition(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Window* wnd = (Window*)glfwGetWindowUserPointer(window);
	wnd->inputManager.updateMouseScrollOffset(xoffset, yoffset);
}

// WINDOW FUNCTIONS

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

	// register input callbacks
	glfwSetKeyCallback(wnd, key_callback);
	glfwSetMouseButtonCallback(wnd, mouse_button_callback);
	glfwSetCursorPosCallback(wnd, cursor_position_callback);
	glfwSetScrollCallback(wnd, scroll_callback);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf_s(stderr, "Failed to init glad\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void Window::cleanup() {
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
