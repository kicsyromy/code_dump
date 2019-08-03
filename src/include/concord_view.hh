#ifndef CONCORD_VIEW_HH
#define CONCORD_VIEW_HH

#include <cstdint>

#include <wayland-server.h>

extern "C"
{
#define static
#include <wlr/types/wlr_xdg_shell.h>
#undef static
}

#include "concord_cursor.hh"
#include "concord_global.hh"
#include "concord_signal.hh"
#include "concord_wayland_signal.hh"

namespace concord
{
    class surface
    {
    public:
        surface(wlr_xdg_surface &surface);
        surface(surface &&) = default;

    public:
        inline bool is_mapped() const noexcept { return mapped_; }

    public:
        bool operator==(const surface &other) const noexcept
        {
            return &xdg_surface_ == &other.xdg_surface_;
        }

    public:
        events::signal<surface &> focus_requested;
        events::signal<surface &> move_requested;
        events::signal<surface &, const std::uint32_t> resize_requested;
        events::signal<surface &> destroyed;

    private:
        void on_xdg_surface_map();
        void on_xdg_surface_unmap();
        void on_xdg_surface_destroy();
        void on_xdg_toplevel_request_move();
        void on_xdg_toplevel_request_resize(wlr_xdg_toplevel_resize_event &event);

    private:
        wlr_xdg_surface &xdg_surface_;

        int x_{ 0 };
        int y_{ 0 };
        bool mapped_{ false };

        wayland::signal<> xdg_surface_map;
        wayland::signal<> xdg_surface_unmap;
        wayland::signal<> xdg_surface_destroy;
        wayland::signal<> xdg_toplevel_request_move;
        wayland::signal<wlr_xdg_toplevel_resize_event> xdg_toplevel_request_resize;

    private:
        DISABLE_COPY(surface);

    public:
        DECLARE_CONVERT_FROM_RAW_PTR(xdg_surface_)
    };
} // namespace concord

#endif /* !CONCORD_VIEW_HH */
