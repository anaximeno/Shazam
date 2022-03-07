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
        File(std::string path, EFileStatus status) :
        _status{status}, _path{path} {
            //
        }

        std::string path() {
            return this->_path;
        }

        EFileStatus status() const {
            return this->_status;
        }

        bool isValid() const {
            return this->status() == VALID_FILE;
        }

        int size() {
            return this->isValid() ? fs::file_size(this->path()) : 0;
        }

        std::string explainStatus() {
            return explainFileStatus(this->status());
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
            if (!this->fileExists(filestatus)) {
                return NON_EXISTENT;
            } else if (!this->fileIsNotDirectory(filestatus)) {
                return IS_DIRECTORY;
            } else if (!this->fileIsPermissive(filestatus)) {
                return NON_PERMISSIVE;
            } else if (!this->fileIsReadable(path)) {
                return NON_READABLE;
            } else {
                return VALID_FILE;
            }
        }

    public:
        std::shared_ptr<File> create(std::string path) {
            return std::make_shared<File>(path, this->fileValidStatus(path));
        }
};


class ProgressObserver {
    public:
        void update() {
            this->activeObservables--;
            if (this->pbar != nullptr) {
                ++(*(this->pbar));
                this->pbar->display();
            }
        }

        void done() {
            if (this->pbar != nullptr) {
                this->pbar->done();
                std::cout << std::endl;
            }
        }

        void increaseObervableCounter() {
            this->activeObservables++;
        }

        int getObservablesNumber() {
            return this->activeObservables;
        }

        void init() {
            if (this->getObservablesNumber() > 0) {
                this->pbar = std::make_unique<progresscpp::ProgressBar>(
                    this->getObservablesNumber(), this->progressWidth);
            }
        }

        ProgressObserver(int progressWidth):
            progressWidth{progressWidth} {
            //
        }

    private:
        // used to count the number of observables
        int activeObservables = 0;
        // used to set the width of the progress bar
        const int progressWidth;
        // points to the progress bar instance
        std::unique_ptr<
            progresscpp::ProgressBar
        > pbar;
};

// To store hash sum in different formats
template <class T> struct SHashSum {
    bool wasCalculated;
    T value;
};


class Hash {
    SHashSum<int> intHashsum{false, 0};
    SHashSum<std::string> hexHashSum{false, ""};
    const std::string hashName;
    const std::shared_ptr<File> file;

    const std::unique_ptr<
        hashlibpp::hashwrapper
    > hasher;

    std::shared_ptr<
        ProgressObserver
    > observer;

    std::string calculateHashSum(void) {
        return this->hasher->getHashFromFile(this->file->path());
    }

    public:
        Hash(std::string hashname,
            std::unique_ptr<hashlibpp::hashwrapper> wrapper,
            std::shared_ptr<File> file_ptr
        ) : hashName{hashname}, file{file_ptr}, 
            hasher{std::move(wrapper)}
        {
            assert(file->isValid());
        }

        void registerObserver(std::shared_ptr<ProgressObserver> obs) {
            obs->increaseObervableCounter();
            this->observer = obs;
        }

        void notify(void) {
            this->observer->update();
        }

        void calculate(void) {
            if (!this->hexHashSum.wasCalculated) {
                this->hexHashSum.value = this->calculateHashSum();
                this->hexHashSum.wasCalculated = true;
            } else {
                // Do Nothing!
            }
        }

        std::string type(void) {
            return this->hashName;
        }

        std::string getStringHashSum(void) {
            if (!this->hexHashSum.wasCalculated) {
                this->calculate();
            }
            return this->hexHashSum.value;
        }

        int getIntHashSum(void) {
            if (!this->intHashsum.wasCalculated) {
                this->intHashsum.value = hexaToInt(this->getStringHashSum());
                this->intHashsum.wasCalculated = true;
                return this->getIntHashSum();
            }
            return this->intHashsum.value;
        }

        std::string getFilePath(void) {
            return this->file->path();
        }
};


class HashFactory: public hashlibpp::wrapperfactory {
    public:
        static constexpr std::array<const char*, 5>
        VALID_HASH_TYPES_ARRAY = {
            "MD5",  "SHA1",  "SHA256", "SHA384",  "SHA512"};

        std::shared_ptr<Hash> hashFile(std::string hashtype, std::shared_ptr<File> file) {
            assert(file->isValid()); // The file must be valid, since it has to be open
            auto wrapper = std::unique_ptr<hashlibpp::hashwrapper>(this->create(hashtype));
            return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
        }
};


class Checker {
    HashFactory hashFactory;
    const bool showProgressBar;

    const std::shared_ptr<
        ProgressObserver
    > progress;

    std::list<
        std::shared_ptr<Hash>
    > validFilesHashes;

    std::list<
        std::shared_ptr<File>
    > invalidFilesList;

    void displayValidHashes(const std::string& hashtype) {
        if (!this->validFilesHashes.empty()) {
            if (hashtype != "") {
                // Only display the hashtype if one is send
                std::cout << hashtype << "SUM:" << std::endl;
            }
            for (auto& hash : this->validFilesHashes) {
                std::cout << hash->getStringHashSum() << " ";
                std::cout << hash->getFilePath() << "\n";
            }
        }
    }

    void displayInvalidFiles() {
        if (!this->invalidFilesList.empty()) {
            std::cout << std::endl << "Invalid Files:\n";
            for (auto& file : this->invalidFilesList) {
                std::cout << " " << file->path() << " -> " << file->explainStatus();
                std::cout << "\n";
            }
            std::cout << std::endl;
        }
    }

    public:
        Checker(bool showProgressBar = false):
            showProgressBar{showProgressBar},
            progress{std::make_shared<ProgressObserver>(40)} {
                //
        }

        void add(std::shared_ptr<File> file, std::string hashtype) {
            if (file->isValid()) {
                auto hash = this->hashFactory.hashFile(hashtype, file);
                hash->registerObserver(this->progress);
                this->validFilesHashes.push_front(hash);
            } else {
                this->invalidFilesList.push_front(file);
            }
        }

        void calculateHashSums() {
            if (this->showProgressBar) {
                // add observer for the init operation
                this->progress->increaseObervableCounter();
                // initialize the progress bar
                this->progress->init();
                // after calling init, update the progress bar
                this->progress->update();
            }
            std::for_each(
                this->validFilesHashes.begin(),
                this->validFilesHashes.end(),
                [](std::shared_ptr<Hash>& hash) {
                    hash->calculate();
                    hash->notify();
                }
            );
        }

        void displayResults(const std::string hashtype = "") {
            if (this->showProgressBar) {
                this->progress->done();
            }
            this->displayValidHashes(hashtype);
            this->displayInvalidFiles();
        }

        auto getValidHashesList() {
            return this->validFilesHashes;
        }

        auto getInvalidFilesList() {
            return this->invalidFilesList;
        }
};


class App {
    const std::string name;
    std::string hashType;
    FileFactory fileFactory;
    std::unique_ptr<Checker> check;
    std::unique_ptr<
        argparse::ArgumentParser
    > args;

    void setupArgparser() {
        assert(this->args != nullptr);
        this->args->add_argument("files").remaining()
                        .help("the name of the file");
        this->args->add_argument("--progress")
                    .help("Show progress bars")
                    .default_value(false)
                    .implicit_value(true);
        for (auto& htype : HashFactory::VALID_HASH_TYPES_ARRAY) {
            const std::string lower = toLowerCase(htype);
            this->args->add_argument("-" + lower, "--" + lower + "sum")
                        .default_value(false).implicit_value(true)
                        .help("Use this to calculate the " + lower + " hash sum");
        }
    }

    void getHashType() {
        int counter = 0;
        for (auto& htype : HashFactory::VALID_HASH_TYPES_ARRAY) {
            if (this->args->is_used("-" + toLowerCase(htype))) {
                this->hashType = htype;
                counter++;
            }
        }

        if (counter == 0) {
            this->printErrMessage(
                "Must specify the type of hash sum!");
        } else if (counter > 1) {
            this->printErrMessage(
                "You can chose only one hash type each time!");
        } else {
            // DO NOTHING!
        }
    }

    void parse(int argc, char** argv) {
        try {
            this->args->parse_args(argc, argv);
        } catch (const std::runtime_error& err) {
            const auto arguments = *(this->args);
            std::cerr << err.what() << std::endl;
            std::cerr << arguments << std::endl;
            std::exit(1);
        }
    }

    void getAndRegisterInputFiles() {
        assert(this->args != nullptr);///// -> Must setup and parse the args first
        assert(this->check != nullptr);//// -> Must initialize the checker first
        try {
            // Getting the file from the argument parser
            auto files = this->args->get<std::vector<std::string>>("files");
            for (auto& file : files) {
                this->check->add(this->fileFactory.create(file), this->hashType);
            }
        } catch (const std::logic_error& err) {
            // If err when parsing the files, the message below
            // will be shown to the user:
            this->printErrMessage("No files were provided");
        }
    }

    void initializeChecker() {
        assert(this->args != nullptr);//// -> Must setup and parse the args first
        this->check = std::make_unique<Checker>(this->args->get<bool>("--progress"));
    } 

    public:
        App(std::string name): name{name},
            args{std::make_unique<argparse::ArgumentParser>(name)} {
            this->setupArgparser();
        }

        void printErrMessage(const std::string& message) {
            std::cerr << "Err: " << message;
            std::cout << "\n" << std::endl;
            std::cout << this->args->help().str();
            std::cout << std::endl;
            std::exit(1);
        }

        int run(int argc, char** argv) {
            this->parse(argc, argv);
            this->getHashType();
            this->initializeChecker();
            this->getAndRegisterInputFiles();
            this->check->calculateHashSums();
            this->check->displayResults();
            return 0;
        }
};
}

#endif
