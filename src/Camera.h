#pragma once
#include "Window.h"

#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
public:
    enum class Movement { Forward, Backward, Left, Right, Up, Down };

    Camera(const glm::vec3& position);

    glm::mat4 getViewMatrix() const;
    void processKeyboard(Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    glm::vec3 getPosition() const { return position; }
    float getZoom() const { return zoom; }

private:
    void updateOrientation();

    glm::vec3 position;
    glm::vec3 worldUp;

    glm::quat orientation;
    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};