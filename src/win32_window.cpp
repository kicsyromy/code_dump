#ifdef VK_USE_PLATFORM_WIN32_KHR
#include "window.h"

#include <windows.h>

#include "logger.h"

namespace vulkan_tutorial
{
    struct window_handle_t
    {
        HINSTANCE hInstance;
        HWND hWnd;

        inline ~window_handle_t() noexcept
        {
            DestroyWindow(hWnd);
            UnregisterClass("VulkanTutorial", hInstance);
        }
    };
}

namespace
{
    enum class user_message_t
    {
        RESIZE = WM_USER + 1,
        QUIT,
        MOUSE_CLICK,
        MOUSE_MOVE,
        MOUSE_WHEEL
    };

    LRESULT CALLBACK WindowProcedure(
            HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        case WM_LBUTTONDOWN:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_CLICK),
                        0, 1);
            break;
        case WM_LBUTTONUP:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_CLICK),
                        0, 0);
            break;
        case WM_RBUTTONDOWN:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_CLICK),
                        1, 1);
            break;
        case WM_RBUTTONUP:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_CLICK),
                        1, 0);
            break;
        case WM_MOUSEMOVE:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_MOVE),
                        LOWORD(lParam),
                        HIWORD(lParam));
            break;
        case WM_MOUSEWHEEL:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::MOUSE_WHEEL),
                        HIWORD(wParam),
                        0);
            break;
        case WM_SIZE:
        case WM_EXITSIZEMOVE:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::RESIZE),
                        wParam, lParam);
            break;
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
            {
                PostMessage(hWnd,
                            static_cast<UINT>(user_message_t::QUIT),
                            wParam, lParam);
            }
            break;
        case WM_CLOSE:
            PostMessage(hWnd,
                        static_cast<UINT>(user_message_t::QUIT),
                        wParam, lParam);
            break;
        }
        return 0;
    }
}

using namespace vulkan_tutorial;

window_t::window_t(
            std::string_view window_title,
            int x, int y,
            int width, int height) noexcept
  : handle_ { new window_handle_t }
{
    handle_->hInstance = GetModuleHandle(nullptr);
    LOG_ERROR("Failed to create window. Error: %d", GetLastError());

    WNDCLASSEX window_class {
        sizeof(WNDCLASSEX),               // UINT         cbSize
        /* Win 3.x */
        CS_HREDRAW | CS_VREDRAW,          // UINT         style
        &WindowProcedure,                 // WNDPROC      lpfnWndProc
        0,                                // int          cbClsExtra
        0,                                // int          cbWndExtra
        handle_->hInstance,               // HINSTANCE    hInstance
        nullptr,                          // HICON        hIcon
        LoadCursor(nullptr, IDC_ARROW),   // HCURSOR      hCursor
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),       // HBRUSH       hbrBackground
        nullptr,                          // LPCSTR       lpszMenuName
        "VulkanTutorial",                 // LPCSTR       lpszClassName
        /* Win 4.0 */
        nullptr                           // HICON        hIconSm
    };

    if(!RegisterClassEx(&window_class))
    {
        LOG_ERROR("Failed to create window. Error: %d", GetLastError());
        return;
    }

    handle_->hWnd = CreateWindow(
        "VulkanCookbook",
        window_title.data(),
        WS_OVERLAPPEDWINDOW,
        x, y,
        width, height,
        nullptr, nullptr,
        handle_->hInstance,
        nullptr
    );

    if (!handle_->hWnd)
    {
        LOG_ERROR("Failed to create window. Error: %d", GetLastError());
    }
}

window_t::~window_t() noexcept
{
    delete handle_;
}

void window_t::run_render_loop() const noexcept
{
    ShowWindow(handle_->hWnd, SW_SHOWNORMAL);
    UpdateWindow(handle_->hWnd);

    MSG message;
    bool loop = true;

    while (loop)
    {
        if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE ) )
        {
            auto m = static_cast<user_message_t>(message.message);
            switch (m)
            {
            case user_message_t::MOUSE_CLICK:
                break;
            case user_message_t::MOUSE_MOVE:
                break;
            case user_message_t::MOUSE_WHEEL:
                break;
            case user_message_t::RESIZE:
                break;
            case user_message_t::QUIT:
                loop = false;
                break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
        }
    }
}

#endif // VK_USE_PLATFORM_WIN32_KHR
