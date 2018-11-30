#include <signal.h>

#include <miral/runner.h>
#include <miral/set_window_management_policy.h>

#include "harbor_window_manager.hh"

#include "harbor_logger.hh"

int main(int argc, const char *argv[])
{
    static miral::MirRunner *display_server = nullptr;
    const auto terminate_handler = [](int signo) noexcept
    {
        if (signo != SIGTERM)
        {
            return;
        }

        display_server->stop();
        std::exit(EXIT_SUCCESS);
    };

    if (signal(SIGTERM, terminate_handler) == SIG_ERR)
    {
        std::exit(EXIT_FAILURE);
    }

    miral::MirRunner runner{ argc, argv };
    display_server = &runner;
    return runner.run_with({ miral::set_window_management_policy<harbor::WindowManagerPolicy>() });
}
