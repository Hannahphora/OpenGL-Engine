#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>
#include <map>
#include <vector>

#define MAX_KEYS 350
#define MAX_MOUSE_BUTTONS 8
#define NUM_ACTIONS 3 // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT

// TODO: use hashmap instead of full arrays for storing state
// TODO: allow for multiple actions to be bound to the same callback

struct InputAction {
    const char* id = nullptr;


};

class InputManager {
public:
    
    // key funcs
    void updateKeyState(int key, int action);
    bool isKeyPressed(int key) const;
    bool isKeyHeld(int key) const;
    bool isKeyReleased(int key) const;
    void bindKeyAction(int key, int action, std::function<void()> callback);
    void processKeyActions();

    // mouse button funcs
    void updateMouseButtonState(int button, int action);
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonHeld(int button) const;
    bool isMouseButtonReleased(int button) const;
    void bindMouseButtonAction(int button, int action, std::function<void()> callback);
    void processMouseButtonActions();

    // mouse movement funcs
    void updateMousePosition(double xpos, double ypos);
    double getMouseX() const;
    double getMouseY() const;
    void processMouseMove();

    // mouse scroll funcs
    void updateMouseScrollOffset(double xoffset, double yoffset);
    double getMouseScrollOffsetX() const;
    double getMouseScrollOffsetY() const;
    void processMouseScroll();

private:

    // key state arrays
    bool keyState[MAX_KEYS] = { false };
    bool prevKeyState[MAX_KEYS] = { false };
    std::map<int, std::vector<std::function<void()>>[NUM_ACTIONS]> keyCallbacks = {};

    // mouse button state arrays
    bool mouseButtonState[MAX_MOUSE_BUTTONS] = { false };
    bool prevMouseButtonState[MAX_MOUSE_BUTTONS] = { false };
    std::map<std::pair<int, int>, std::vector<std::function<void()>>> mouseButtonCallbacks = {};

    double mouseX = 0.0, mouseY = 0.0;
    double mouseScrollOffsetX = 0.0, mouseScrollOffsetY = 0.0;

};