#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "InputManager.h"
class InputManager;

class Window {
public:

	const char* TITLE = "Window";
	const uint32_t WIDTH = 1200;
	const uint32_t HEIGHT = 800;

	Window();
	~Window();

	void toggleFullscreen();

	void setCursorVis(bool b);
	bool getCursorVis() const { return cursorvis; }

	int getWidth();
	int getHeight();

	InputManager* inputManager = nullptr;
	GLFWwindow* wnd = nullptr;
	GLFWmonitor* monitor = nullptr;

private:

	bool fullscreen = false;
	bool cursorvis = true;
	int xpos = 0, ypos = 0;
	int width = 0, height = 0;
};