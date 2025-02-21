#include "InputManager.h"

// glfw input callbacks

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* wnd = (Window*)glfwGetWindowUserPointer(window);
    wnd->inputManager->updateKeyState(key, action);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* wnd = (Window*)glfwGetWindowUserPointer(window);
    wnd->inputManager->updateMouseButtonState(button, action);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window* wnd = (Window*)glfwGetWindowUserPointer(window);
    wnd->inputManager->updateMousePos(xpos, ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* wnd = (Window*)glfwGetWindowUserPointer(window);
    wnd->inputManager->updateMouseScroll(xoffset, yoffset);
}

// constructor

InputManager::InputManager(GLFWwindow* wnd) : window(wnd) {

    // register glfw input callbacks
    glfwSetKeyCallback(wnd, key_callback);
    glfwSetMouseButtonCallback(wnd, mouse_button_callback);
    glfwSetCursorPosCallback(wnd, cursor_position_callback);
    glfwSetScrollCallback(wnd, scroll_callback);
}

//  update state funcs (called from glfw callbacks)

void InputManager::updateKeyState(int key, int action) {
    prevKeyState[key] = keyState[key];
    keyState[key] = (action != GLFW_RELEASE);
}

void InputManager::updateMouseButtonState(int button, int action) {
    prevMouseButtonState[button] = mouseButtonState[button];
    mouseButtonState[button] = (action != GLFW_RELEASE);
}

void InputManager::updateMousePos(double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

void InputManager::updateMouseScroll(double xoffset, double yoffset) {
    // accumulate scroll delta
    scrollX += xoffset;
    scrollY += yoffset;
}

// action/binding funcs

int InputManager::registerAction(const std::string& actionID) {
    if (!actions.count(actionID)) actions[actionID] = InputAction{ actionID };
    else return 1;
}

int InputManager::registerAction(const std::string& actionID, const Binding& binding, std::function<void()> callback) {
    if (!actions.count(actionID)) actions[actionID] = InputAction{ actionID };
    else return 1;

    addActionBinding(actionID, binding);
    addActionCallback(actionID, callback);
}

int InputManager::addActionBinding(const std::string& actionID, const Binding& binding) {
    if (!actions.count(actionID)) return 1;
    actions[actionID].bindings.emplace_back(binding);
}

int InputManager::addActionCallback(const std::string& actionID, std::function<void()> callback) {
    if (!actions.count(actionID)) return 1;
    actions[actionID].callbacks.emplace_back(callback);
}

void InputManager::processActions() {
    // lambda to check trigger for key and mouse buttons
    auto checkTrigger = [this](bool current, bool previous, int event) -> bool {
        switch (event) {
        case GLFW_PRESS:   return current && !previous;
        case GLFW_REPEAT:  return current && previous;
        case GLFW_RELEASE: return !current && previous;
        default:           return false;
        }
    };

    // compute deltas for mouse pos/scroll
    double mouseDeltaX = mouseX - prevMouseX;
    double mouseDeltaY = mouseY - prevMouseY;
    double scrollDeltaX = scrollX; // already accumulated per frame
    double scrollDeltaY = scrollY;

    // iterate over all actions and bindings
    for (auto& [actionID, action] : actions) {
        bool triggered = std::any_of(action.bindings.begin(), action.bindings.end(), [&](const Binding& binding) {
            switch (binding.type) {
            case Binding::Type::Key: return checkTrigger(keyState[binding.code], prevKeyState[binding.code], binding.event);
            case Binding::Type::MouseButton: return checkTrigger(mouseButtonState[binding.code], prevMouseButtonState[binding.code], binding.event);
            case Binding::Type::MousePos:  return (std::abs(mouseDeltaX) >= binding.thresholdX || std::abs(mouseDeltaY) >= binding.thresholdY);
            case Binding::Type::MouseScroll: return (std::abs(scrollDeltaX) >= binding.thresholdX || std::abs(scrollDeltaY) >= binding.thresholdY);
            default: return false;
            }
        });

        if (triggered)
            for (auto& callback : action.callbacks)
                callback();
    }

    // update prev states
    memcpy(prevKeyState, keyState, sizeof(keyState));
    memcpy(prevMouseButtonState, mouseButtonState, sizeof(mouseButtonState));
    prevMouseX = mouseX;
    prevMouseY = mouseY;
    prevScrollX = scrollX;
    prevScrollY = scrollY;
    scrollX = 0.0;
    scrollY = 0.0;
}

// getters for input states

bool InputManager::isKeyPressed(int key) const {
    return keyState[key] && !prevKeyState[key];
}

bool InputManager::isKeyHeld(int key) const {
    return keyState[key] && prevKeyState[key];
}

bool InputManager::isKeyReleased(int key) const {
    return !keyState[key] && prevKeyState[key];
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

double InputManager::getMouseX() const {
    return mouseX;
}

double InputManager::getMouseY() const {
    return mouseY;
}

double InputManager::getScrollX() const {
    return scrollX;
}

double InputManager::getScrollY() const {
    return scrollY;
}
