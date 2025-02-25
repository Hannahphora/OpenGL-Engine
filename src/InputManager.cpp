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

// constructor/initialisation

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

int InputManager::registerAction(const std::string& id) {
    if (actions.count(id)) return 1;
    actions[id] = InputAction{ .id = id };
    return 0;
}

int InputManager::registerAction(const std::string& id, const Binding& binding, std::function<void()> callback) {
    if (actions.count(id)) return 1;
    actions[id] = InputAction{ .id = id };
    addActionBinding(id, binding);
    addActionCallback(id, callback);
    return 0;
}

int InputManager::addActionBinding(const std::string& id, const Binding& binding) {
    if (!actions.count(id)) return 1;
    actions[id].bindings.emplace_back(binding);
    return 0;
}

int InputManager::addActionCallback(const std::string& id, std::function<void()> callback) {
    if (!actions.count(id)) return 1;
    actions[id].callbacks.emplace_back(callback);
    return 0;
}

void InputManager::processActions() {

    // check trigger for keys and mouse buttons
    auto checkTrigger = [](bool current, bool previous, int event) -> bool {
        switch (event) {
        case GLFW_PRESS:   return current && !previous;
        case GLFW_REPEAT:  return current && previous;
        case GLFW_RELEASE: return !current && previous;
        default:           return false;
        }
    };

    // check thresholds for mouse pos/scroll
    auto checkThreshold = [](double thresholdX, double deltaX, double thresholdY, double deltaY) -> bool {
        return ((std::abs(thresholdX) > std::numeric_limits<double>::epsilon() * std::abs(thresholdX)) && (std::abs(deltaX) >= thresholdX))
            || ((std::abs(thresholdY) > std::numeric_limits<double>::epsilon() * std::abs(thresholdY)) && (std::abs(deltaY) >= thresholdY));
    };

    // iterate over actions
    for (const auto& [actionID, action] : actions) {
        if (!action.active) continue;

        // iterate over bindings, if any are met then run callbacks
        if ([&]() -> bool {
            for (const auto& binding : action.bindings) {
                switch (binding.type) {
                case Binding::Type::Key:
                    if (checkTrigger(keyState[binding.code], prevKeyState[binding.code], binding.event))
                        return true;
                    break;
                case Binding::Type::MouseButton:
                    if (checkTrigger(mouseButtonState[binding.code], prevMouseButtonState[binding.code], binding.event))
                        return true;
                    break;
                case Binding::Type::MousePos:
                    if (checkThreshold(binding.thresholdX, mouseX - prevMouseX, binding.thresholdY, mouseY - prevMouseY))
                        return true;
                    break;
                case Binding::Type::MouseScroll:
                    if (checkThreshold(binding.thresholdX, scrollX, binding.thresholdY, scrollY))
                        return true;
                    break;
                default:
                    break;
                }
            }
            return false;
        }()) {
            for (const auto& callback : action.callbacks)
                callback();
        }
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
