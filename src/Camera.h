#pragma once
#include "Window.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:

    enum class Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    Window* window;

    double deltaTime = 0.0;
    float lastX;
    float lastY;
    bool firstMouse = true;

    // attributes
    glm::vec3 Position;
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Right;
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // euler angles
    float Yaw = -90.0f;
    float Pitch = 0.0f;
    // camera options
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    Camera(Window* window, glm::vec3 position);

    glm::mat4 GetViewMatrix();

private:
    void updateVectors();
};