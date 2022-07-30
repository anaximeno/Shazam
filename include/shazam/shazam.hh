#ifndef _SHAZAM_HEADER
#define _SHAZAM_HEADER

#include "./basic-types.hh"
#include "./files.hh"
#include "./hash.hh"
#include "./common.hh"

#include "../external/argparse.hpp"

#include <iostream>
#include <cassert>
#include <memory>

namespace ap = argparse;

namespace shazam {
    class Checker {
        bool showProgressBar;
        bool showInvalidFiles;
        const std::shared_ptr<ProgressObserver> progress;
        std::list<std::shared_ptr<Hash>> validFilesHashes;
        std::list<std::shared_ptr<File>> invalidFilesList;
        HashFactory hashFactory;

    public:
        Checker(bool showProgressBar, bool showInvalidFiles)
        : showProgressBar(showProgressBar), showInvalidFiles(showInvalidFiles),
        progress(std::make_shared<ProgressObserver>(40)) {  }
        Checker(): Checker(false, true) {  }
        std::list<std::shared_ptr<Hash>> getValidHashesList();
        std::list<std::shared_ptr<File>> getInvalidFilesList();
        void add(std::shared_ptr<File> file, std::string hashtype);
        void calculateHashSums();
        void setShowProgressBar(bool value);
        void setShowInvalidFiles(bool value);
        void displayResults();

    private:
        void displayValidHashes();
        void displayInvalidFiles();
    };


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
