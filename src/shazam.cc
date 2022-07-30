#include "../include/shazam/shazam.hh"
#include "../include/shazam/basic-types.hh"
#include "../include/shazam/common.hh"
#include "../include/shazam/files.hh"

#include "../include/external/argparse.hpp"
#include "../include/external/ProgressBar.hpp"
#include "../include/external/hashlib2plus/hl_hashwrapper.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <sstream>

namespace fs = std::filesystem;
namespace pgs = progresscpp;
namespace ap = argparse;

void shazam::ProgressObserver::update() {
    activeObservables--;
    if (progressBar != nullptr) {
        ++( *progressBar );
        progressBar->display();
    }
}

void shazam::ProgressObserver::done() {
    if (progressBar != nullptr) {
        progressBar->done();
        std::cout << std::endl;
    }
}

void shazam::ProgressObserver::increaseObervableCounter() {
    activeObservables++;
}

int shazam::ProgressObserver::getObservablesNumber() {
    return activeObservables;
}

void shazam::ProgressObserver::init() {
    const int observables = getObservablesNumber();
    if (observables > 0) {
        progressBar = std::make_unique<pgs::ProgressBar>(observables, progressWidth);
    }
}

void shazam::Hash::registerObserver(std::shared_ptr<ProgressObserver> obs) {
    obs->increaseObervableCounter();
    observer = obs;
}

void shazam::Hash::notify(void) {
    observer->update();
}

void shazam::Hash::calculate(void) {
    if (!hexHashSum.wasCalculated) {
        hexHashSum.value = calculateHashSum();
        hexHashSum.wasCalculated = true;
    } else {
        // Do Nothing!
    }
}

std::string shazam::Hash::type(void) {
    return hashName;
}

std::string shazam::Hash::getStringHashSum(void) {
    if (!hexHashSum.wasCalculated) {
        calculate();
    }

    return hexHashSum.value;
}

// int shazam::Hash::getIntHashSum(void) {
//     if (!intHashsum.wasCalculated) {
//         intHashsum.value = hexaToInt(getStringHashSum());
//         intHashsum.wasCalculated = true;
//         return getIntHashSum();
//     }

//     return intHashsum.value;
// }

std::string shazam::Hash::getFilePath(void) {
    return file->path();
}

std::string shazam::Hash::calculateHashSum(void) {
    return hasher->getHashFromFile(file->path());
}

std::shared_ptr<shazam::Hash>
shazam::HashFactory::hashFile(std::string hashtype, std::shared_ptr<shazam::File> file) {
    auto wrapper = std::unique_ptr<hashwrapper>(create(hashtype));
    return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
}

void shazam::Checker::displayValidHashes() {
    if (!validFilesHashes.empty()) {
        for (auto& hash : validFilesHashes) {
            std::cout << hash->getStringHashSum() << " ";
            std::cout << hash->getFilePath() << "\n";
        }
    }
}

void shazam::Checker::displayInvalidFiles() {
    if (!invalidFilesList.empty() && showInvalidFiles) {
        std::cout << std::endl << "Invalid Files:\n";

        for (auto& file : invalidFilesList) {
            std::cout << " " << file->path() << " -> ";
            std::cout << file->explainStatus() << "\n";
        }

        std::cout << std::endl;
    }
}

void shazam::Checker::add(std::shared_ptr<shazam::File> file, std::string hashtype) {
    if (file->isValid()) {
        auto hash = hashFactory.hashFile(hashtype, file);
        hash->registerObserver(progress);
        validFilesHashes.push_front(hash);
    } else {
        invalidFilesList.push_front(file);
    }
}

void shazam::Checker::calculateHashSums() {
    if (showProgressBar) {
        // add observer for the init operation
        progress->increaseObervableCounter();
        // initialize the progress bar
        progress->init();
        // after calling init, update the progress bar
        progress->update();
    }

    std::for_each(
        validFilesHashes.begin(),
        validFilesHashes.end(),
        [](std::shared_ptr<Hash>& hash) {
            hash->calculate();
            hash->notify();
        }
    );
}

void shazam::Checker::setShowProgressBar(bool value) {
    showProgressBar = value;
}

void shazam::Checker::setShowInvalidFiles(bool value) {
    showInvalidFiles = value;
}

void shazam::Checker::displayResults() {
    if (showProgressBar) {
        progress->done();
    }
    displayValidHashes();
    displayInvalidFiles();
}

std::list<std::shared_ptr<shazam::Hash>>
shazam::Checker::getValidHashesList() {
    return validFilesHashes;
}
std::list<std::shared_ptr<shazam::File>>
shazam::Checker::getInvalidFilesList() {
    return invalidFilesList;
}


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
