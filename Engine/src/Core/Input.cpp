#include <vvhl/Core/Input.hpp>


#include <vvhl/Events/KeyboardEvents.hpp>
#include <vvhl/Events/MouseEvents.hpp>

namespace vvhl
{

// APP
void Input::initialize(EventDispatcher& dispatcher, GLFWwindow* window){
    s_dispatcher = &dispatcher;
    s_window = window;
    registerCallbacks(window);
}

void Input::endFrame(){
    s_mouseDelta = {0.0, 0.0};
    s_scrollDelta = {0.0, 0.0};
}

//---------------------------
// SET & GET
//---------------------------

// KEYBOARD
bool Input::isKeyPressed(int key){ return s_keys[key] != ButtonState::Released; }

// MOUSE
bool Input::isMouseButtonPressed(int button){ return s_mouseButtons[button] != ButtonState::Released; }
glm::dvec2 Input::getMousePosition(){ return s_mousePosition; }
glm::dvec2 Input::getMouseDelta(){ return s_mouseDelta; }
glm::dvec2 Input::getScrollDelta(){ return s_scrollDelta; }

// CURSOR
void Input::showCursor(){
    glfwSetInputMode(s_window,
                     GLFW_CURSOR,
                     GLFW_CURSOR_NORMAL);
}

void Input::hideCursor(){
    glfwSetInputMode(s_window,
                     GLFW_CURSOR,
                     GLFW_CURSOR_HIDDEN);
}

void Input::lockCursor(){
    glfwSetInputMode(s_window,
                     GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED);
}

void Input::unlockCursor(){
    glfwSetInputMode(s_window,
                     GLFW_CURSOR,
                     GLFW_CURSOR_NORMAL);
}

void Input::setCursorPosition(double x, double y){
    glfwSetCursorPos(s_window, x, y);
}

//---------------------------
// CALLBACKS
//---------------------------

// KEYBOARD
void Input::KeyCallback(GLFWwindow*,
                        int key,
                        int,
                        int action,
                        int mods)
{
    // Invalid key or non existent dispatcher
    if (key < 0 || !s_dispatcher) return;

    // Update key
    s_keys[key] = ButtonState(GLFW_RELEASE);

    if (action == GLFW_PRESS){
        s_dispatcher->dispatch(
            KeyPressedEvent(key, mods, false));
    }else if (action == GLFW_REPEAT){
        s_dispatcher->dispatch(
            KeyPressedEvent(key, mods, true));
    }else if (action == GLFW_RELEASE){
        s_dispatcher->dispatch(
            KeyReleasedEvent(key, mods));
    }
}

void Input::CharCallback(GLFWwindow*, unsigned int codepoint){
    if (!s_dispatcher) return;

    s_dispatcher->dispatch(
        CharInputEvent(codepoint));
}

// MOUSE
void Input::MouseButtonCallback(GLFWwindow*,
                                int button,
                                int action,
                                int)
{
    if (!s_dispatcher) return;

    // Update mouse button
    s_mouseButtons[button] = ButtonState(GLFW_RELEASE);

    if (action == GLFW_PRESS){
        s_dispatcher->dispatch(
            MouseButtonPressedEvent(
                button,
                s_mousePosition.x,
                s_mousePosition.y));
    }else if (action == GLFW_RELEASE){
        s_dispatcher->dispatch(
            MouseButtonReleasedEvent(
                button,
                s_mousePosition.x,
                s_mousePosition.y));
    }
}

void Input::CursorPositionCallback(GLFWwindow*,
                                   double x,
                                   double y)
{
    if (!s_dispatcher) return;

    // Update internal values
    s_previousMousePosition = s_mousePosition;
    s_mousePosition = {x, y};
    s_mouseDelta = s_mousePosition - s_previousMousePosition;

    s_dispatcher->dispatch(
        MouseMovedEvent(x, y));
}

void Input::CursorEnterCallback(GLFWwindow*, int entered){
    if (!s_dispatcher) return;

    s_dispatcher->dispatch(
        MouseEnteredEvent(entered == GLFW_TRUE));
}

void Input::ScrollCallback(GLFWwindow*,
                           double xOffset,
                           double yOffset)
{
    if (!s_dispatcher) return;
 
    // Update internal values
    s_scrollDelta = {xOffset, yOffset};

    s_dispatcher->dispatch(
        MouseScrolledEvent(xOffset, yOffset));
}

// CALLBACK REGISTRATION
void Input::registerCallbacks(GLFWwindow* window){
    glfwSetKeyCallback(
        window,
        KeyCallback);

    glfwSetCharCallback(
        window,
        CharCallback);

    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback);

    glfwSetCursorPosCallback(
        window,
        CursorPositionCallback);

    glfwSetCursorEnterCallback(
        window,
        CursorEnterCallback);

    glfwSetScrollCallback(
        window,
        ScrollCallback);
}

}; // namespace vvhl