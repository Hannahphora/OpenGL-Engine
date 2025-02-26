#include "InputManager.h"

// binding constructors

Binding Binding::key(int code, int event) {
    return Binding{ Type::Key, code, event };
}

Binding Binding::mouseButton(int code, int event) {
    return Binding{ Type::MouseButton, code, event };
}

Binding Binding::mouseMove() {
    return Binding{ Type::MouseMove };
}

Binding Binding::scrollUp() {
    return Binding{ Type::MouseScrollUp };
}

Binding Binding::scrollDown() {
    return Binding{ Type::MouseScrollDown };
}

Binding Binding::composite(std::initializer_list<Binding> bindings) {
    Binding b;
    b.type = Type::Composite;
    b.subBindings.assign(bindings.begin(), bindings.end());
    return b;
}

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
    prevMBState[button] = mbState[button];
    mbState[button] = (action != GLFW_RELEASE);
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

int InputManager::setActionActive(const std::string& id, bool active) {
    if (!actions.count(id)) return 1;
    actions[id].active = active;
    return 0;
}

void InputManager::processActions() {
    double epsilon = std::numeric_limits<double>::epsilon();

    // check trigger for keys and mouse buttons
    auto checkTrigger = [](bool current, bool previous, int event) -> bool {
        switch (event) {
        case GLFW_PRESS:   return current && !previous;
        case GLFW_REPEAT:  return current && previous;
        case GLFW_RELEASE: return !current && previous;
        default:           return false;
        }
    };

    // recursive lambda to check binding, including composites
    std::function<bool(const Binding&)> checkBinding;
    checkBinding = [&](const Binding& binding) -> bool {
        if (binding.type == Binding::Type::Composite) {
            for (const auto& sub : binding.subBindings) 
                if (!checkBinding(sub))
                    return false;
            return true;
        }

        switch (binding.type) {
        case Binding::Type::Key:
            return checkTrigger(keyState[binding.code], prevKeyState[binding.code], binding.event);
        case Binding::Type::MouseButton:
            return checkTrigger(mbState[binding.code], prevMBState[binding.code], binding.event);
        case Binding::Type::MouseMove:
            return (std::abs(mouseX - prevMouseX) > epsilon) || (std::abs(mouseY - prevMouseY) > epsilon);
        case Binding::Type::MouseScrollUp:
            return scrollY > epsilon;
        case Binding::Type::MouseScrollDown:
            return scrollY < -epsilon;
        default:
            return false;
        }
    };

    // iterate over actions
    for (const auto& [actionID, action] : actions) {
        if (!action.active) continue;

        // iterate over bindings, if any are met then run callbacks
        if ([&]() -> bool {
            for (const auto& binding : action.bindings)
                if (checkBinding(binding))
                    return true;
            return false;
        }()) {
            for (const auto& callback : action.callbacks)
                callback();
        }
    }

    // update prev states
    memcpy(prevKeyState, keyState, sizeof(keyState));
    memcpy(prevMBState, mbState, sizeof(mbState));
    prevMouseX = mouseX;
    prevMouseY = mouseY;
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
    return mbState[button] && !prevMBState[button];
}

bool InputManager::isMouseButtonHeld(int button) const {
    return mbState[button] && prevMBState[button];
}

bool InputManager::isMouseButtonReleased(int button) const {
    return !mbState[button] && prevMBState[button];
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