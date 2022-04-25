#include "include/shazam.hpp"

#define VERSION "1.1"

int main(int argc, char * argv[]) {
    shazam::App app(argv[0], VERSION, argc, argv);
    return app.run();
}