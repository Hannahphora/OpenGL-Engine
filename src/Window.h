#pragma once

#include "InputManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Window {
public:

	const char* TITLE = "WindOwO";
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	Window();

	void cleanup();
	void toggleFullscreen();

	InputManager inputManager = {};
	GLFWwindow* wnd = nullptr;
	GLFWmonitor* monitor = nullptr;

private:

	bool fullscreen = false;
	int xpos = 0, ypos = 0;
	int width = 0, height = 0;

};