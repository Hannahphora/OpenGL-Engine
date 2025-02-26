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
#include <cstdlib>
#include <iostream>

unsigned int LoadTexture(const char* path);

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Window* window = new Window();

	// global input actions
	{
		window->inputManager->registerAction("Quit", Binding::key(GLFW_KEY_ESCAPE, GLFW_PRESS), [window]() {
			glfwSetWindowShouldClose(window->wnd, true);
		});
		window->inputManager->registerAction("ToggleFullscreen", Binding::key(GLFW_KEY_F, GLFW_PRESS), [window]() {
			window->toggleFullscreen();
		});
		window->inputManager->registerAction("ToggleCursor", Binding::key(GLFW_KEY_T, GLFW_PRESS), [window]() {
			window->setCursorVis(!window->getCursorVis());
		});
	}

	// create shader program
	ShaderProgram shaders("shaders\\base.vert", "shaders\\base.frag");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	// world space positions of cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	uint32_t texture1 = LoadTexture("assets\\textures\\container.jpg");

	shaders.use();
	shaders.setInt("texture1", 0);

	bool drawWireframes = false;
	float bgColor[3] = { 0.0f, 0.0f, 0.0f };
	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.f);
	glEnable(GL_DEPTH_TEST);
	window->setCursorVis(false);

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->wnd, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// timing variables
	double prevTime = glfwGetTime(), simLag = 0.0, deltaTime = 0.0;
	constexpr double simDelta = 1.0 / 60.0;

	// create camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	bool firstMouse = true;
	float lastX = window->getWidth() / 2.0f;
	float lastY = window->getHeight() / 2.0f;

	// register camera input actions
	{
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

		window->inputManager->registerAction("CameraLook", Binding::mouseMove(), [&]() {
			if (window->getCursorVis()) return;

			float xpos = (float)window->inputManager->getMouseX();
			float ypos = (float)window->inputManager->getMouseY();

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

		window->inputManager->registerAction("CameraScroll", Binding::scrollUp(), [&]() {
			float scrollY = window->inputManager->getScrollY();
			camera.processMouseScroll(scrollY);
		});
		window->inputManager->addActionBinding("CameraScroll", Binding::scrollDown());

		window->inputManager->addActionCallback("ToggleCursor", [&]() {
			firstMouse = true;
		});
	}

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
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// debug panel
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
			ImGui::Begin("Debug Panel", nullptr,
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav);

			// debug info
			ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0);
			ImGui::Text("FPS: %.1f", deltaTime > 0.0 ? 1.0 / deltaTime : 0.0);
			ImGui::Text("Window Size: %dx%d", window->getWidth(), window->getHeight());
			ImGui::Separator();

			// controls
			ImGui::Checkbox("Draw Wireframes", &drawWireframes);
			ImGui::ColorEdit3("Clear Color", bgColor);
			ImGui::End();
		}

		// clear screen and set draw mode
		glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, drawWireframes ? GL_LINE : GL_FILL);

		// bind textures & use shader program
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		shaders.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),
			(float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
		shaders.setMat4("projection", projection);
		glm::mat4 view = camera.getViewMatrix();
		shaders.setMat4("view", view);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shaders.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// render imgui on top of scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffers and process events
		glfwSwapBuffers(window->wnd);
		glfwPollEvents();
		window->inputManager->processActions();
	}

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	window->cleanup();
	delete window;
	return 0;
}

unsigned int LoadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		fprintf(stderr, "Error loading texture from: %s\n", path);
	}

	return textureID;
}