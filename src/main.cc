#include "harbor_logger.hh"

extern "C" int run(int, char *[]);

int main(int argc, char *argv[])
{
    LOG_INFO("Starting compositor...");
    run(argc, argv);
    //    compositor::run(argc, argv);
}
