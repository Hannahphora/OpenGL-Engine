#include "Camera.h"

Camera::Camera(const glm::vec3& position)
    : position(position),
    worldUp(0.0f, 1.0f, 0.0f),
    yaw(0.0f),
    pitch(0.0f),
    movementSpeed(2.5f),
    mouseSensitivity(0.1f),
    zoom(45.0f) {
    updateOrientation();
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));
    glm::vec3 up = glm::normalize(glm::rotate(orientation, worldUp));
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 front = glm::normalize(glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)));
    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 up = glm::normalize(glm::rotate(orientation, worldUp));

    switch (direction) {
    case Movement::Forward:
        position += front * velocity;
        break;
    case Movement::Backward:
        position -= front * velocity;
        break;
    case Movement::Left:
        position -= right * velocity;
        break;
    case Movement::Right:
        position += right * velocity;
        break;
    case Movement::Up:
        position += up * velocity;
        break;
    case Movement::Down:
        position -= up * velocity;
        break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateOrientation();
}

void Camera::processMouseScroll(float yoffset) {
    zoom = glm::clamp(zoom - yoffset, 1.0f, 45.0f);
}

void Camera::updateOrientation() {
    glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    orientation = qYaw * qPitch;
}