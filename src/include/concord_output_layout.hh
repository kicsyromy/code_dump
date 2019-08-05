#ifndef CONCORD_OUTPUT_HH
#define CONCORD_OUTPUT_HH

#include <vector>

#include <sigc++/sigc++.h>

extern "C"
{
#define static
#include <wlr/types/wlr_output_layout.h>
#undef static
}

#include "concord_global.hh"
#include "concord_signal.hh"
#include "concord_wayland_signal.hh"

namespace concord
{
    struct server;

    class output_layout
    {
    private:
        class output
        {
        public:
            output(wlr_output *handle);

        public:
            events::signal<output &> output_frame_requested;

        private:
            void on_output_frame_requested();

        private:
            wlr_output *handle;
            wayland::signal<> output_frame;

        public:
            DECLARE_CONVERT_FROM_RAW_PTR(handle)
        };

    public:
        output_layout(server &server);

    private:
        void on_new_output_added(wlr_output &output);
        void on_output_frame_requested(output &output);

    private:
        wayland_ptr<wlr_output_layout> handle_;

    private:
        server &server_;

    private:
        std::vector<output> outputs_{};
        wayland::signal<wlr_output> new_output;

    public:
        DECLARE_CONVERT_FROM_SMART_PTR(handle_)
    };
} // namespace concord

#endif /* !CONCORD_OUTPUT_HH */
