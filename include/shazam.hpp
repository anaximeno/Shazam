// ------ Include Libs ----------------------------------------------

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>

#include "external/hashlibpp.hpp"
#include "external/argparse.hpp"
#include "external/ProgressBar.hpp"

#ifndef __SHAZAM_HPP
#define __SHAZAM_HPP

// ------------------------------------------------------------------

namespace shazam { // -- MAIN NAMESPACE: shazam --

// ---- Commonly used namespaces ------------------------------------

namespace fs = std::filesystem;

// ---- Enums -------------------------------------------------------

enum EFileStatus {
    VALID_FILE,
    NON_EXISTENT,
    IS_DIRECTORY,
    NON_PERMISSIVE,
    NON_READABLE
};


std::string explainFileStatus(const EFileStatus status) {
    std::string resonForStatus;

    switch (status) {
        case NON_EXISTENT:
            resonForStatus = "Was not found.";
            break;
        case IS_DIRECTORY:
            resonForStatus = "Its a directory.";
            break;
        case NON_PERMISSIVE:
            resonForStatus = "No permissions to read.";
            break;
        case NON_READABLE:
            resonForStatus = "Could not read!";
            break;
        case VALID_FILE:
            resonForStatus = "File is valid.";
            break;
        default:
            resonForStatus = "Unknown file status!";
            break;
    }

    return resonForStatus;
}

// ---- Some useful functions-----------------------------------------

int hexaToInt(std::string hexadecimalString) {
    return std::stoi(hexadecimalString, 0, 16);
}

std::string toUpperCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---- Classes -----------------------------------------------------
class File {
    const EFileStatus _status;
    const std::string _path;

    public:
        File(std::string path, EFileStatus status)
            : _status(status), _path(path) {}

        std::string path() const {
            return _path;
        }

        EFileStatus status() const {
            return _status;
        }

        bool isValid() const {
            return status() == VALID_FILE;
        }

        int size() {
            return isValid() ? fs::file_size(path()) : 0;
        }

        std::string explainStatus() const {
            return explainFileStatus(status());
        }
};


class FileFactory {
    bool fileExists(fs::file_status filestatus) {
        return fs::exists(filestatus);
    }

    bool fileIsNotDirectory(fs::file_status filestatus) {
        return !fs::is_directory(filestatus);
    }

    bool fileIsPermissive(fs::file_status filestatus) {
        return (filestatus.permissions() & fs::perms::owner_read) != fs::perms::none;
    }

    bool fileIsReadable(std::string path) {
        bool isReadable = false;

        std::fstream f;
        f.open(path, std::ios::in | std::ios::binary);

        if(f) {
            char byt;
            try {
                f.read(&byt, 1);
                isReadable = true;
            } catch(const std::exception& e) {
                // TODO: do something here!
                // NOTE: IDEA: Maybe add this err to some log file.
            }
        }

        f.close();
        return isReadable;
    }

    protected:
        EFileStatus fileValidStatus(std::string path) {
            fs::file_status filestatus = fs::status(path);
            if (!fileExists(filestatus)) {
                return NON_EXISTENT;
            } else if (!fileIsNotDirectory(filestatus)) {
                return IS_DIRECTORY;
            } else if (!fileIsPermissive(filestatus)) {
                return NON_PERMISSIVE;
            } else if (!fileIsReadable(path)) {
                return NON_READABLE;
            } else {
                return VALID_FILE;
            }
        }

    public:
        std::shared_ptr<File> create(std::string path) {
            return std::make_shared<File>(path, fileValidStatus(path));
        }
};


class ProgressObserver {
    private:
        int activeObservables;

        // used to set the width of the progress bar
        const int progressWidth;

        // points to the progress bar instance
        std::unique_ptr<
            progresscpp::ProgressBar
        > progressBar;
    
    public:
        void update() {
            activeObservables--;
            if (progressBar != nullptr) {
                ++(*progressBar);
                progressBar->display();
            }
        }

        void done() {
            if (progressBar != nullptr) {
                progressBar->done();
                std::cout << std::endl;
            }
        }

        void increaseObervableCounter() {
            activeObservables++;
        }

        int getObservablesNumber() {
            return activeObservables;
        }

        void init() {
            const int observables = getObservablesNumber();
            if (observables > 0) {
                progressBar = std::make_unique<progresscpp::ProgressBar>(
                    observables, progressWidth);
            }
        }

        ProgressObserver(int progressWidth)
            : progressWidth(progressWidth) {
                activeObservables = 0;
            }
};

using hashlibpp::hashwrapper;

class Hash {
    // stores hash sum in different formats
    template <class T> struct SHashSum {
        bool wasCalculated;
        T value;
    };

    SHashSum<int> intHashsum{false, 0};
    SHashSum<std::string> hexHashSum{false, ""};
    const std::string hashName;
    const std::shared_ptr<File> file;
    const std::unique_ptr<hashwrapper> hasher;
    std::shared_ptr<ProgressObserver> observer;

    public:
        Hash(std::string hashname, std::unique_ptr<hashwrapper> wrapper, std::shared_ptr<File> file_ptr)
        : hashName(hashname), file(file_ptr), hasher(std::move(wrapper)) {
            //
        }

        void registerObserver(std::shared_ptr<ProgressObserver> obs) {
            obs->increaseObervableCounter();
            observer = obs;
        }

        void notify(void) {
            observer->update();
        }

        void calculate(void) {
            if (!hexHashSum.wasCalculated) {
                hexHashSum.value = calculateHashSum();
                hexHashSum.wasCalculated = true;
            } else {
                // Do Nothing!
            }
        }

        std::string type(void) {
            return hashName;
        }

        std::string getStringHashSum(void) {
            if (!hexHashSum.wasCalculated) {
                calculate();
            }

            return hexHashSum.value;
        }

        int getIntHashSum(void) {
            if (!intHashsum.wasCalculated) {
                intHashsum.value = hexaToInt(getStringHashSum());
                intHashsum.wasCalculated = true;
                return getIntHashSum();
            }

            return intHashsum.value;
        }

        std::string getFilePath(void) {
            return file->path();
        }

    private:
        std::string calculateHashSum(void) {
            return hasher->getHashFromFile(file->path());
        }
};


class HashFactory: protected hashlibpp::wrapperfactory {
    public:
        static constexpr std::array<const char*, 5> HASH_TYPES = {
            "MD5",  "SHA1",  "SHA256", "SHA384",  "SHA512"};

        std::shared_ptr<Hash> hashFile(std::string hashtype, std::shared_ptr<File> file) {
            auto wrapper = std::unique_ptr<hashwrapper>(create(hashtype));
            return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
        }
};


class Checker {
    const bool showProgressBar;
    const bool showInvalidFiles;
    const std::shared_ptr<ProgressObserver> progress;
    std::list<std::shared_ptr<Hash>> validFilesHashes;
    std::list<std::shared_ptr<File>> invalidFilesList;
    HashFactory hashFactory;

    void displayValidHashes() {
        if (!validFilesHashes.empty()) {
            for (auto& hash : validFilesHashes) {
                std::cout << hash->getStringHashSum() << " ";
                std::cout << hash->getFilePath() << "\n";
            }
        }
    }

    void displayInvalidFiles() {
        if (!invalidFilesList.empty() && showInvalidFiles) {
            std::cout << std::endl << "Invalid Files:\n";

            for (auto& file : invalidFilesList) {
                std::cout << " " << file->path() << " -> ";
                std::cout << file->explainStatus() << "\n";
            }

            std::cout << std::endl;
        }
    }

    public:
        Checker(bool showProgressBar, bool showInvalidFiles)
        : showProgressBar(showProgressBar), showInvalidFiles(showInvalidFiles),
          progress(std::make_shared<ProgressObserver>(40)) {
            //
        }

        Checker()
        : Checker(false, true) {
            //
        }

        void add(std::shared_ptr<File> file, std::string hashtype) {
            if (file->isValid()) {
                auto hash = hashFactory.hashFile(hashtype, file);
                hash->registerObserver(progress);
                validFilesHashes.push_front(hash);
            } else {
                invalidFilesList.push_front(file);
            }
        }

        void calculateHashSums() {
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

        void displayResults() {
            if (showProgressBar) {
                progress->done();
            }

            displayValidHashes();
            displayInvalidFiles();
        }

        auto getValidHashesList() {
            return validFilesHashes;
        }

        auto getInvalidFilesList() {
            return invalidFilesList;
        }
};

using argparse::ArgumentParser;

class App {
    std::string hashType;
    const std::string name;

    struct CommandArguments {
        const int argc;
        const char* const* argv;
    };

    std::unique_ptr<Checker> checker;
    std::unique_ptr<ArgumentParser> args;
    CommandArguments commandArgs;
    FileFactory fileFactory;

    void setupArgparser() {
        args->add_argument("files")
                .help("path of the files to perform hash sums.")
                .remaining();

        for (auto& htype : HashFactory::HASH_TYPES) {
            const std::string lower = toLowerCase(htype);
            args->add_argument("-" + lower, "--" + lower + "sum")
                    .help("use this to calculate the " + lower + " hash sum")
                    .default_value(false)
                    .implicit_value(true);
        }

        args->add_argument("--progress")
                .help("show progress bars when calculating hashsums")
                .default_value(false)
                .implicit_value(true);

        args->add_argument("--hide-invalid")
                .help("side Invalid files, instead of showing them.")
                .default_value(false)
                .implicit_value(true);
    }

    void getHashType() {
        int counter = 0;
        for (auto& htype : HashFactory::HASH_TYPES) {
            if (args->is_used("-" + toLowerCase(htype)) && ++counter) {
                hashType = htype;
            }
        }

        if (counter == 0) {
            printErrMessage(
                "Must specify the type of hash sum!");
        } else if (counter > 1) {
            printErrMessage(
                "You can chose only one hash type each time!");
        } else {
            // DO NOTHING!
        }
    }

    void parseArguments() {
        try {
            args->parse_args(commandArgs.argc, commandArgs.argv);
        } catch (const std::runtime_error &err) {
            const auto arguments = *( args );

            std::cerr << err.what()
                      << std::endl
                      << arguments
                      << std::endl;

            std::exit(1);
        }
    }

    void getAndRegisterInputFiles() {
        try {
            const auto files = args->get<std::vector<std::string>>("files");
            for (auto& file : files) {
                checker->add(this->fileFactory.create(file), hashType);
            }
        } catch (const std::logic_error &err) {
            printErrMessage("No files were provided");
        }
    }

    public:
        App(std::string name, int& argc, char**& argv)
        : name(name), args(std::make_unique<ArgumentParser>(name)),
          commandArgs(CommandArguments{.argc = argc, .argv = argv}) {
            // ---- space comment ----
            ;
            setupArgparser();
            parseArguments();

            const bool showProgressBar = args->get<bool>("--progress");
            const bool showInvalidFiles = !args->get<bool>("--hide-invalid");

            checker = std::make_unique<Checker>(showProgressBar, showInvalidFiles);
        }

        void printErrMessage(const std::string& message, const int errNum = 1) {
            const auto helpMessage = args->help().str();

            std::cerr << "Err: "
                      << message
                      << std::endl;
            std::cerr << std::endl
                      << helpMessage
                      << std::endl;

            std::exit(errNum);
        }

        int run() {
            getHashType();
            getAndRegisterInputFiles();
            checker->calculateHashSums();
            checker->displayResults();
            return 0;
        }
};
}

#endif
