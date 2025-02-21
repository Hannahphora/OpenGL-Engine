#pragma once

#include "Window.h"
class Window;

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

#define MAX_KEYS 350
#define MAX_MOUSE_BUTTONS 8

// TODO: add composite bindings, that allow actions to be run
//       only when multiple binding conditions are met

struct Binding {
    enum class Type { Key, MouseButton, MousePos, MouseScroll } type;
    int code;
    int event; // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    
    // trigger thresholds for mouse pos/scroll
    // if thresholds are zero, then they are not considered
    double thresholdX = 0.0;
    double thresholdY = 0.0;
};

struct InputAction {
    bool active = true; // TODO: add ability to toggle active
    std::string id;
    std::vector<Binding> bindings;
    std::vector<std::function<void()>> callbacks;
};

class InputManager {
public:
    
    InputManager(GLFWwindow* wnd);

    // update state funcs (called from glfw callbacks)

    void updateKeyState(int key, int action);
    void updateMouseButtonState(int button, int action);
    void updateMousePos(double xpos, double ypos);
    void updateMouseScroll(double xoffset, double yoffset);

    // action/binding funcs

    void processActions();
    int registerAction(const std::string& actionID);
    int registerAction(const std::string& actionID, const Binding& binding, std::function<void()> callback);
    int addActionBinding(const std::string& actionID, const Binding& binding);
    int addActionCallback(const std::string& actionID, std::function<void()> callback);

    // getters for input states

    bool isKeyPressed(int key) const;
    bool isKeyHeld(int key) const;
    bool isKeyReleased(int key) const;

    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonHeld(int button) const;
    bool isMouseButtonReleased(int button) const;

    double getMouseX() const;
    double getMouseY() const;

    double getScrollX() const;
    double getScrollY() const;

private:

    GLFWwindow* window;
    std::unordered_map<std::string, InputAction> actions;

    // TODO: pack these bool arrays (if thats a good idea)
    bool keyState[MAX_KEYS] = { false };
    bool prevKeyState[MAX_KEYS] = { false };
    bool mouseButtonState[MAX_MOUSE_BUTTONS] = { false };
    bool prevMouseButtonState[MAX_MOUSE_BUTTONS] = { false };

    // mouse pos tracking
    double mouseX = 0.0, mouseY = 0.0;
    double prevMouseX = 0.0, prevMouseY = 0.0;

    // scroll tracking (accumulated per frame)
    double scrollX = 0.0, scrollY = 0.0;
    double prevScrollX = 0.0, prevScrollY = 0.0;

};