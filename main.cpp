#include "include/shazam.hpp"

#define VERSION "Shazam 1.2"

int main(const int argc, const char* const* argv) {
    shazam::App app(argv[0], VERSION);
    return app.run(argc, argv);
}
