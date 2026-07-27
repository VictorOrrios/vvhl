#pragma once

#include <vvhl/Events/EventDispatcher.hpp>

namespace vvhl
{

class Window {
public:

    struct WindowSpecification{
        std::string Title = "VVHL";
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool Resizable = true;
    };

    Window(WindowSpecification spec, vvhl::EventDispatcher& dispatcher);
    ~Window(){destroy();}

    void create();
    void destroy();

    void close();
    void pollEvents();

    bool isOpen() const;
    bool isFocused() const;
    bool isMinimized() const;
    bool isFullscreen() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    VkExtent2D getFramebufferSize() const;

    std::pair<int,int> getPosition() const;

    GLFWwindow* getNativeHandle() const;

    VkSurfaceKHR createSurface(VkInstance instance) const;

    void setTitle(const std::string& title);
    void setSize(uint32_t width, uint32_t height);
    void setPosition(int x, int y);
    void setFullscreen(bool enabled);

    void maximize();
    void minimize();
    void restore();


private:

    struct WindowedState{
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };

    void registerCallbacks();

    static void WindowSizeCallback(GLFWwindow*,int,int);
    static void FramebufferSizeCallback(GLFWwindow*,int,int);
    static void WindowCloseCallback(GLFWwindow*);
    static void WindowFocusCallback(GLFWwindow*,int);
    static void WindowIconifyCallback(GLFWwindow*,int);
    static void WindowMaximizeCallback(GLFWwindow*,int);
    static void WindowPositionCallback(GLFWwindow*,int,int);
    

private:

    WindowSpecification m_spec;
    vvhl::EventDispatcher& m_dispatcher;

    GLFWwindow* m_glfwHandle = nullptr;

    bool m_isFullscreen = false;
    WindowedState m_windowedState;

};

}; // namespace vvhl