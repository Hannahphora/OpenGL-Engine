#include "InputManager.h"

// key funcs

void InputManager::updateKeyState(int key, int action) {
    prevKeyState[key] = keyState[key];
    keyState[key] = (action != GLFW_RELEASE);
}

bool InputManager::isKeyPressed(int key) const {
    return key < MAX_KEYS ? (keyState[key] && !prevKeyState[key]) : false;
}

bool InputManager::isKeyHeld(int key) const {
    return key < MAX_KEYS ? keyState[key] : false;
}

bool InputManager::isKeyReleased(int key) const {
    return key < MAX_KEYS ? (!keyState[key] && prevKeyState[key]) : false;
}

void InputManager::bindKeyAction(int key, int action, std::function<void()> callback) {
    if (key < 0 || key >= MAX_KEYS || action < 0 || action >= NUM_ACTIONS) return;
    keyCallbacks[key][action] = callback;
}

void InputManager::processKeyActions() {
    for (int i = 0; i < MAX_KEYS; i++) {
        for (int action = 0; action < NUM_ACTIONS; action++) {

            if (keyCallbacks[i][action] &&
                ((action == GLFW_PRESS && isKeyPressed(i))
                || (action == GLFW_REPEAT && isKeyHeld(i))
                || (action == GLFW_RELEASE && isKeyReleased(i))))
            {
                keyCallbacks[i][action]();
            }
        }
    }
}

// mouse button funcs

void InputManager::updateMouseButtonState(int button, int action) {
    prevMouseButtonState[button] = mouseButtonState[button];
    mouseButtonState[button] = (action != GLFW_RELEASE);
}

bool InputManager::isMouseButtonPressed(int button) const {
    return mouseButtonState[button] && !prevMouseButtonState[button];
}

bool InputManager::isMouseButtonHeld(int button) const {
    return mouseButtonState[button] && prevMouseButtonState[button];
}

bool InputManager::isMouseButtonReleased(int button) const {
    return !mouseButtonState[button] && prevMouseButtonState[button];
}

void InputManager::bindMouseButtonAction(int button, int action, std::function<void()> callback) {
    mouseButtonCallbacks[button][action] = callback;
}

void InputManager::processMouseButtonActions() {
    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) {
        for (int action = 0; action < NUM_ACTIONS; action++) {

            if (mouseButtonCallbacks.find(i)[action] &&
                ((action == GLFW_PRESS && isMouseButtonPressed(i))
                || (action == GLFW_REPEAT && isMouseButtonHeld(i))
                || (action == GLFW_RELEASE && isMouseButtonReleased(i))))
            {
                mouseButtonCallbacks[i][action]();
            }
        }
    }
}

// mouse movement funcs

void InputManager::updateMousePosition(double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

double InputManager::getMouseX() const {
    return mouseX;
}

double InputManager::getMouseY() const {
    return mouseY;
}

void InputManager::processMouseMove() {

}

// mouse scroll funcs
void InputManager::updateMouseScrollOffset(double xoffset, double yoffset) {
    mouseScrollOffsetX = xoffset;
    mouseScrollOffsetY = yoffset;
}

double InputManager::getMouseScrollOffsetX() const {
    return mouseScrollOffsetX;
}

double InputManager::getMouseScrollOffsetY() const {
    return mouseScrollOffsetY;
}

void InputManager::processMouseScroll() {

}