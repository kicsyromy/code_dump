#include "harbor_compositor.hh"

#include <miral/runner.h>
#include <miral/set_window_management_policy.h>

#include "harbor_window_manager.hh"

#include "harbor_panel.hh"

#include "harbor_logger.hh"

int harbor::compositor::run(int argc, const char *argv[]) noexcept
{
    using namespace harbor;

    LOG_INFO("Starting up...");

    miral::MirRunner runner{ argc, argv };
    runner.add_start_callback(&panel::panel_run);
    runner.add_stop_callback(&panel::panel_quit);
    return runner.run_with({ miral::set_window_management_policy<harbor::WindowManagerPolicy>() });
}
