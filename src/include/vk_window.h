#ifndef VK_WINDOW_H
#define VK_WINDOW_H

#include "support.h"

namespace vk
{
    class window_t
    {
    private:
        static constexpr const auto WIDTH  { 800 * 2 };
        static constexpr const auto HEIGHT { 600 * 2 };

    public:
        inline window_t()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            handle_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
        }

        inline ~window_t()
        {
            glfwDestroyWindow(handle_);
            glfwTerminate();
        }

    public:
        inline void runMainLoop()
        {
            while (!glfwWindowShouldClose(handle_))
            {
                glfwPollEvents();
            }
        }

    public:
        inline operator GLFWwindow *() const { return handle_; }

    private:
        GLFWwindow *handle_;

    private:
        DISABLE_COPY(window_t)
        DISABLE_MOVE(window_t)
    };
}

#endif // !VK_WINDOW_H
