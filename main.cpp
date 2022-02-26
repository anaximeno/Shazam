#include "include/shazam.hpp"


int main(int argc, char * argv[]) {
	return std::make_unique<shazam::App>(argv[0])->run(argc, argv);
}