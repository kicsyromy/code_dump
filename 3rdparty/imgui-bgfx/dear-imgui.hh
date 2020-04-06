#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <imgui/imgui.h>

namespace imgui_bgfx_sdl
{
    class Context
    {
    private:
    public:
        inline Context(SDL_Window *w, float font_size = 18.f) noexcept
          : window_{ w }
        {
            imguiCreate(font_size);
        }

        inline ~Context() noexcept { imguiDestroy(); }

    public:
        bool process_event(const SDL_Event *event)
        {
            if (!(ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() ||
                    ImGui::IsAnyItemHovered()))
            { return false; }

            ImGuiIO &io = ImGui::GetIO();
            switch (event->type)
            {
            case SDL_MOUSEWHEEL:
            {
                if (event->wheel.x > 0) io.MouseWheelH += 1;
                if (event->wheel.x < 0) io.MouseWheelH -= 1;
                if (event->wheel.y > 0) io.MouseWheel += 1;
                if (event->wheel.y < 0) io.MouseWheel -= 1;
                return true;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event->button.button == SDL_BUTTON_LEFT) mouse_buttons_[0] = true;
                if (event->button.button == SDL_BUTTON_RIGHT) mouse_buttons_[1] = true;
                if (event->button.button == SDL_BUTTON_MIDDLE) mouse_buttons_[2] = true;
                return true;
            }
            case SDL_TEXTINPUT:
            {
                io.AddInputCharactersUTF8(event->text.text);
                return true;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                int key = event->key.keysym.scancode;
                IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                io.KeysDown[key] = (event->type == SDL_KEYDOWN);
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
                io.KeySuper = false;
#else
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
                return true;
            }
            }
            return false;
        }

        void new_frame()
        {
            ImGuiIO &io = ImGui::GetIO();
            IM_ASSERT(io.Fonts->IsBuilt() &&
                      "Font atlas not built! It is generally built by the renderer back-end. "
                      "Missing call "
                      "to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

            // Setup display size (every frame to accommodate for window resizing)
            int w, h;
            int display_w, display_h;
            SDL_GetWindowSize(window_, &w, &h);
            SDL_GL_GetDrawableSize(window_, &display_w, &display_h);
            io.DisplaySize = ImVec2(float(w), float(h));
            if (w > 0 && h > 0)
                io.DisplayFramebufferScale =
                    ImVec2(float(display_w) / float(w), float(display_h) / float(h));

            // Setup time step (we don't use SDL_GetTicks() because it is using millisecond
            // resolution)
            auto current_time = SDL_GetPerformanceCounter();
            io.DeltaTime =
                time_ > 0 ? float(double(current_time - time_) / double(frequency_)) : 1.0f / 60.0f;
            time_ = current_time;

            update_mouse_position_and_buttons();
            update_mouse_cursor();

            ImGui::NewFrame();
        }

        void end_frame() { imguiEndFrame(); }

    private:
        void update_mouse_position_and_buttons()
        {
            ImGuiIO &io = ImGui::GetIO();

            // Set OS mouse position if requested (rarely used, only when
            // ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
            if (io.WantSetMousePos)
                SDL_WarpMouseInWindow(window_, int(io.MousePos.x), int(io.MousePos.y));
            else
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

            int mx, my;
            Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
            io.MouseDown[0] =
                mouse_buttons_[0] ||
                (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) !=
                    0; // If a mouse press event came, always pass it as "mouse held this frame", so
                       // we don't miss click-release events that are shorter than 1 frame.
            io.MouseDown[1] =
                mouse_buttons_[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
            io.MouseDown[2] =
                mouse_buttons_[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
            mouse_buttons_[0] = mouse_buttons_[1] = mouse_buttons_[2] = false;

            if (SDL_GetWindowFlags(window_) & SDL_WINDOW_INPUT_FOCUS)
                io.MousePos = ImVec2(float(mx), float(my));
        }

        void update_mouse_cursor()
        {
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) return;

            ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
            if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
            {
                // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
                SDL_ShowCursor(SDL_FALSE);
            }
            else
            {
                SDL_ShowCursor(SDL_TRUE);
            }
        }

    private:
        std::uint64_t time_{ 0 };
        std::uint64_t frequency_{ SDL_GetPerformanceFrequency() };
        std::array<bool, 3> mouse_buttons_{ false, false, false };
        SDL_Window *window_{ nullptr };
    };

} // namespace imgui_bgfx_sdl
