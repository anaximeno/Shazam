#ifndef _SHAZAM_HEADER
#define _SHAZAM_HEADER

#include "./common.hh"
#include "./checker.hh"

#include "../external/argparse.hpp"

#include <iostream>
#include <cassert>
#include <memory>

namespace ap = argparse;

namespace shazam {
    /* Represents the program abstraction. */
    class App {
        const std::string name;
        const std::string version;

        const std::unique_ptr<ap::ArgumentParser> args;
        const std::unique_ptr<Checker> checker;

        FileFactory fileFactory;

    public:
        App(std::string name, std::string ver)
        : name(name), version(ver), args(std::make_unique<ap::ArgumentParser>(name, ver)),
        checker(std::make_unique<Checker>()) {
            this->setupArgparser();
        }

        /* Runs the program given the argument count and the
         * argument vector. */
        int run(const int& argc, const char* const*& argv);

    private:
        /* Setups the argument parser. */
        void setupArgparser();

        /* Returns the hash sum type chosen by the user. */
        std::string getHashType();

        /* Activates the argument parser to parse the arguments. */
        void parseArguments(const int& argc, const char* const*& argv);

        /* Gets the files given by the user and adds them to the checker. */
        void getAndRegisterInputFiles(std::string hashType);
    };
}

#endif
