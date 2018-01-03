#ifndef VK_WINDOW_H
#define VK_WINDOW_H

#include "support.h"

namespace vk
{
    class window_t
    {
    private:
        static constexpr std::uint32_t WIDTH  { 800 * 2 };
        static constexpr std::uint32_t HEIGHT { 600 * 2 };

    public:
        inline window_t()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            handle_ = glfwCreateWindow(
                        WIDTH,
                        HEIGHT,
                        "Vulkan Window",
                        nullptr,
                        nullptr);
        }

        inline ~window_t()
        {
            glfwDestroyWindow(handle_);
            /* Remove this for now since it causes a crash on Windows */
            /* glfwTerminate(); */
        }

    public:
        std::uint32_t width() const  { return WIDTH; }
        std::uint32_t height() const { return HEIGHT; }

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
