#include "harbor_compositor.hh"
#include "harbor_logger.hh"

using namespace harbor;

int main(int argc, const char *argv[])
{
    LOG_INFO("Starting compositor...");
    compositor::run(argc, argv);
}
