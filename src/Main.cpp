#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"
#include "Shader.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// init window & input actions
	Window* window = new Window();

	// create input actions
	{
		// quit
		window->inputManager->registerAction("Quit", Binding{ .type = Binding::Type::Key, .code = GLFW_KEY_ESCAPE, .event = GLFW_PRESS }, [window]() {
			glfwSetWindowShouldClose(window->wnd, true);
		});

		// toggle fullscreen
		window->inputManager->registerAction("ToggleFullscreen", Binding{ .type = Binding::Type::Key, .code = GLFW_KEY_F, .event = GLFW_PRESS }, [window]() {
			window->toggleFullscreen();
		});
	}

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
	bool drawOutlines = false;
	float bgColor[3] = { 0.0f, 0.0f, 0.0f };
	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.f);

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->wnd, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// timing variables
	double prevTime = glfwGetTime(), lag = 0.0;
	constexpr double simDelta = 1.0 / 60.0;

	// main loop
	while (!glfwWindowShouldClose(window->wnd)) {
		double currentTime = glfwGetTime();
		double frametime = currentTime - prevTime;
		lag += frametime;
		prevTime = currentTime;

		while (lag >= simDelta) {
			// update sim at fixed rate
			lag -= simDelta;
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
			ImGui::Text("Frame Time: %.3f ms", frametime * 1000.0);
			ImGui::Text("FPS: %.1f", frametime > 0.0 ? 1.0 / frametime : 0.0);
			int width, height;
			glfwGetWindowSize(window->wnd, &width, &height);
			ImGui::Text("Window Size: %dx%d", width, height);
			ImGui::Separator();

			// controls
			ImGui::Checkbox("Draw Outlines", &drawOutlines);
			ImGui::ColorEdit3("Clear Color", bgColor);
			ImGui::End();
		}

		// set clear col
		glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// set poly mode based on drawOutlines
		if (drawOutlines) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shaders.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// draw imgui on top of scene
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
