#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <cstdlib>
#include <iostream>
#include <thread>
#include <atomic>

void initImgui(Window* window);
void cleanupImgui();
void registerInputActions(Window* window);
void registerCameraInput(Camera& camera, Window* window, double& deltaTime);

// TODO: refactor main.cpp into Engine.cpp/Engine.h and create Engine class

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Window* window = new Window();

	ShaderProgram shaders("shaders/model.vert", "shaders/model.frag");

	registerInputActions(window);
	window->setCursorVis(false);

	bool drawWireframes = false;
	
	// set opengl state
	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glEnable(GL_DEPTH_TEST);

	initImgui(window);

	// timing variables
	double prevTime = glfwGetTime(), simLag = 0.0, deltaTime = 0.0;
	constexpr double simDelta = 1.0 / 60.0;

	// init camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	registerCameraInput(camera, window, deltaTime);

	// load models
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window->wnd);
	Model backpack("assets/models/backpack/backpack.obj");

	// main loop
	while (!glfwWindowShouldClose(window->wnd)) {
		double currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		simLag += deltaTime;
		prevTime = currentTime;

		while (simLag >= simDelta) {
			simLag -= simDelta;
			// update sim here
		}

		// layout imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::Begin("Debug Panel", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);

		ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0);
		ImGui::Text("FPS: %.1f", deltaTime > 0.0 ? 1.0 / deltaTime : 0.0);
		ImGui::Text("Window Size: %dx%d", window->getWidth(), window->getHeight());
		ImGui::Separator();
		ImGui::Checkbox("Draw Wireframes", &drawWireframes);
		ImGui::End();

		// clear screen and set draw mode
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, drawWireframes ? GL_LINE : GL_FILL);

		shaders.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),
			(float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
		shaders.setMat4("projection", projection);
		glm::mat4 view = camera.getViewMatrix();
		shaders.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		shaders.setMat4("model", model);

		backpack.Draw(shaders);

		// render imgui on top of scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffers and poll/process events
		glfwSwapBuffers(window->wnd);
		glfwPollEvents();
		window->inputManager->processActions();
	}

	// cleanup
	cleanupImgui();
	delete window;
	return 0;
}

// register input helpers

void registerInputActions(Window* window) {
	// quit
	window->inputManager->registerAction("Quit", Binding::key(GLFW_KEY_ESCAPE, GLFW_PRESS), [window]() {
		glfwSetWindowShouldClose(window->wnd, true);
	});
	// toggle fullscreen
	window->inputManager->registerAction("ToggleFullscreen", Binding::key(GLFW_KEY_F, GLFW_PRESS), [window]() {
		window->toggleFullscreen();
	});
	// toggle cursor
	window->inputManager->registerAction("ToggleCursor", Binding::key(GLFW_KEY_T, GLFW_PRESS), [window]() {
		window->setCursorVis(!window->getCursorVis());
	});
}

void registerCameraInput(Camera& camera, Window* window, double& deltaTime) {
	static bool firstMouse = true;
	static float lastX = window->getWidth() / 2.0f;
	static float lastY = window->getHeight() / 2.0f;

	window->inputManager->registerAction("CameraForward", Binding::key(GLFW_KEY_W, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Forward, (float)deltaTime);
	});
	window->inputManager->registerAction("CameraBackward", Binding::key(GLFW_KEY_S, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Backward, (float)deltaTime);
	});
	window->inputManager->registerAction("CameraLeft", Binding::key(GLFW_KEY_A, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Left, (float)deltaTime);
	});
	window->inputManager->registerAction("CameraRight", Binding::key(GLFW_KEY_D, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Right, (float)deltaTime);
	});
	window->inputManager->registerAction("CameraUp", Binding::key(GLFW_KEY_SPACE, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Up, (float)deltaTime);
	});
	window->inputManager->registerAction("CameraDown", Binding::key(GLFW_KEY_LEFT_CONTROL, GLFW_REPEAT), [&]() {
		camera.processKeyboard(Camera::Movement::Down, (float)deltaTime);
	});

	window->inputManager->addMouseMoveCallback([&, window](double xpos, double ypos) {
		if (window->getCursorVis()) return;

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;

		camera.processMouseMovement(-xoffset, -yoffset);
	});

	window->inputManager->addMouseScrollCallback([&, window](double yoffset) {
		camera.processMouseScroll(yoffset);
	});

	window->inputManager->addActionCallback("ToggleCursor", [&]() {
		firstMouse = true;
	});
}

// imgui helpers

void initImgui(Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->wnd, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void cleanupImgui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}