#include "Camera.h"

Camera::Camera(Window* window, glm::vec3 position) : window(window), Position(position) {

    window->inputManager->registerAction("Foward", Binding::key(GLFW_KEY_W, GLFW_REPEAT), [&]() {
        Position += Front * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("Backward", Binding::key(GLFW_KEY_S, GLFW_REPEAT), [&]() {
        Position -= Front * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("Left", Binding::key(GLFW_KEY_A, GLFW_REPEAT), [&]() {
        Position -= Right * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("Right", Binding::key(GLFW_KEY_D, GLFW_REPEAT), [&]() {
        Position += Right * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("Up", Binding::key(GLFW_KEY_SPACE, GLFW_REPEAT), [&]() {
        Position += Up * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("Down", Binding::key(GLFW_KEY_LEFT_CONTROL, GLFW_REPEAT), [&]() {
        Position -= Up * MovementSpeed * (float)deltaTime;
        updateVectors();
    });

    window->inputManager->registerAction("CameraLook", Binding::mouseMove(), [&, window]() {
        if (window->cursorvis) return;

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

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity * -1;

        Yaw += xoffset;
        Pitch += yoffset;

        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateVectors();
    });

    window->inputManager->registerAction("ZoomOut", Binding::scrollDown(), [&]() {
        Zoom += 1.0;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
        updateVectors();
    });

    window->inputManager->registerAction("ZoomIn", Binding::scrollUp(), [&]() {
        Zoom -= 1.0;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
        updateVectors();
    });

    window->inputManager->addActionCallback("ToggleCursor", [&]() {
        firstMouse = true;
    });

    int width, height;
    glfwGetWindowSize(window->wnd, &width, &height);
    lastX = width / 2.0f;
    lastY = height / 2.0f;

    updateVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::updateVectors() {
    // calculate new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // re-calculate right and up vectors
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize vectors
    Up = glm::normalize(glm::cross(Right, Front));
}
