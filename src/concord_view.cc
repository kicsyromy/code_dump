#include "concord_view.hh"

using namespace concord;

surface::surface(wlr_xdg_surface &surface)
  : xdg_surface_{ surface }, xdg_surface_map{ surface.events.map },
    xdg_surface_unmap{ surface.events.unmap }, xdg_surface_destroy{ surface.events.destroy },
    xdg_toplevel_request_move{ surface.toplevel->events.request_move }, xdg_toplevel_request_resize{
        surface.toplevel->events.request_resize
    }
{
}

void surface::on_xdg_surface_map()
{
    mapped_ = true;
    focus_requested.emit(*this);
}

void surface::on_xdg_surface_unmap() { mapped_ = false; }

void surface::on_xdg_surface_destroy() { destroyed.emit(*this); }

void surface::on_xdg_toplevel_request_move() { move_requested.emit(*this); }

void surface::on_xdg_toplevel_request_resize(wlr_xdg_toplevel_resize_event &event)
{
    resize_requested.emit(*this, static_cast<const std::uint32_t>(event.edges));
}
