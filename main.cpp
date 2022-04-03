#include "include/shazam.hpp"


int main(int argc, char * argv[]) {
    shazam::App app(argv[0], argc, argv);
    return app.run();
}