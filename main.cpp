#include "include/shazam/app.hh"

#define VERSION "Shazam 2.0"

int main(const int argc, const char* const* argv) {
    shazam::App app(argv[0], VERSION);
    return app.run(argc, argv);
}
