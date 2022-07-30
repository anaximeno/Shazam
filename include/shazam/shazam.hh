#ifndef _SHAZAM_HEADER
#define _SHAZAM_HEADER

#include "./basic-types.hh"
#include "./files.hh"
#include "./hash.hh"
#include "./common.hh"
#include "./checker.hh"

#include "../external/argparse.hpp"

#include <iostream>
#include <cassert>
#include <memory>

namespace ap = argparse;

namespace shazam {
    class App {
        const std::string name;
        const std::string version;
        std::unique_ptr<ap::ArgumentParser> args;
        std::unique_ptr<Checker> checker;
        FileFactory fileFactory;

    public:
        App(std::string name, std::string ver)
        : name(name), version(ver), args(std::make_unique<ap::ArgumentParser>(name, ver)),
        checker(std::make_unique<Checker>()) {
            this->setupArgparser();
        }

        int run(const int& argc, const char* const*& argv);

    private:
        void setupArgparser();

        std::string getHashType();

        void parseArguments(const int& argc, const char* const*& argv);

        void getAndRegisterInputFiles(std::string hashType);
    };
}

#endif
