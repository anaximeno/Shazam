#include "../include/shazam/shazam.hh"
#include "../include/shazam/basic-types.hh"
#include "../include/shazam/common.hh"
#include "../include/shazam/files.hh"
#include "../include/shazam/hash.hh"
#include "../include/shazam/checker.hh"

#include "../include/external/argparse.hpp"

#include <iostream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <sstream>

namespace fs = std::filesystem;
namespace ap = argparse;


void shazam::App::setupArgparser() {
    args->add_argument("files")
            .help("path of the files to perform hash sums.")
            .remaining();

    for (auto& htype : HASH_TYPES) {
        const std::string lower = toLowerCase(htype);
        args->add_argument("-" + lower, "--" + lower + "sum")
                .help("use this to calculate the " + lower + " hash sum")
                .default_value(false)
                .implicit_value(true);
    }

    args->add_argument("--progress", "-P")
            .help("show progress bars when calculating hashsums")
            .default_value(false)
            .implicit_value(true);

    args->add_argument("--hide-invalid", "-H")
            .help("hide Invalid files, instead of showing them.")
            .default_value(false)
            .implicit_value(true);

    args->add_argument("-c", "--check")
            .help("Use this to check the hash sum")
            .default_value(false)
            .implicit_value(true);
}

std::string shazam::App::getHashType() {
    std::string hashType;

    int counter = 0;
    // Searching for all hash types to see if one was used
    for (auto& htype : HASH_TYPES) {
        if (args->is_used("-" + toLowerCase(htype)) && ++counter) {
            hashType = htype;
        }
        if (counter > 1) {
            // If more than one were used show this error message
            printErrMessage("You can chose only one hash type each time!");
        }
    }

    // If no hash sum was indicated them print err message
    if (counter == 0) {
        printErrMessage("Must specify the type of hash sum!\n\n" + args->help().str() + "\n");
    }

    return hashType;
}

void shazam::App::parseArguments(const int& argc, const char* const*& argv) {
    try {
        args->parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::stringstream sstrm;
        sstrm << *( args );
        std::string out = sstrm.str();
        shazam::printErrMessage(std::string(err.what()) + "\n" + out + "\n");
    }
}

void shazam::App::getAndRegisterInputFiles(std::string hashType) {
    try {
        const auto files = args->get<std::vector<std::string>>("files");
        for (auto& file : files) {
            checker->add(fileFactory.create(file), hashType);
        }
    } catch (const std::logic_error &err) {
        printErrMessage("No files were provided" + args->help().str() + "\n");
    }
}

int shazam::App::run(const int& argc, const char* const*& argv) {
    this->parseArguments(argc, argv);
    this->getAndRegisterInputFiles(this->getHashType());
    checker->setShowProgressBar(args->get<bool>("--progress"));
    checker->setShowInvalidFiles(!args->get<bool>("--hide-invalid"));
    checker->calculateHashSums();
    checker->displayResults();
    return 0;
}
