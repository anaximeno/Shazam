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

using std::string;
using std::cout, std::endl, std::string;
using hashlibpp::hashwrapper,
      hashlibpp::wrapperfactory;
using argparse::ArgumentParser;

// ---- Enums -------------------------------------------------------

enum EFileValidStatus {
    IS_VALID,
    NON_EXISTENT,
    IS_DIRECTORY,
    NOT_PERMISSIVE,
    NOT_READABLE
};

const std::array<const string, 5> VALID_HASH_TYPES_ARRAY = {
    "MD5", "SHA1", "SHA256", "SHA384", "SHA512"
};

// ---- Some useful functions-----------------------------------------

int hexaToInt(string hexadecimalString) {
    return std::stoi(hexadecimalString, 0, 16);
}

string toUpperCase(string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

string toLowerCase(string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---- Classes -----------------------------------------------------
class File {
    const EFileValidStatus _status;
    const string _path;
    string reasonForStatus;
    public:
        File(string path, EFileValidStatus status): _status{status}, _path{path} {
            switch (status)
            {
            case NON_EXISTENT:
                this->reasonForStatus = "File Not Found! ";
                break;
            case IS_DIRECTORY:
                this->reasonForStatus = "Directory given as file! ";
                break;
            case NOT_PERMISSIVE:
                this->reasonForStatus = "Permission refused! ";
                break;
            case NOT_READABLE:
                this->reasonForStatus = "Can't read the file!";
                break;
            case IS_VALID:
                this->reasonForStatus = "File is valid.";
            default:
                this->reasonForStatus = "Unknown file status!";
                break;
            }
        }

        string path() {
            return this->_path;
        }

        EFileValidStatus status() {
            return this->_status;
        }

        bool isValid() {
            return this->status() == EFileValidStatus::IS_VALID;
        }

        int size() {
            if (this->isValid()) {
                return fs::file_size(this->path());
            } else {
                return 0;
            }
        }

        string explain() {
            return this->reasonForStatus;
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

    bool fileIsReadable(string path) {
        char byt;
        bool isReadable = false;

        std::fstream f;
        f.open(path, std::ios::in | std::ios::binary);

        if(f) {
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
        EFileValidStatus fileValidStatus(string path) {
            fs::file_status filestatus = fs::status(path);
            if (!this->fileExists(filestatus))
                return EFileValidStatus::NON_EXISTENT;
            else if (!this->fileIsNotDirectory(filestatus))
                return EFileValidStatus::IS_DIRECTORY;
            else if (!this->fileIsPermissive(filestatus))
                return EFileValidStatus::NOT_PERMISSIVE;
            else if (!this->fileIsReadable(path))
                return EFileValidStatus::NOT_READABLE;
            else
                return EFileValidStatus::IS_VALID;
        }

    public:
        std::shared_ptr<File> create(string path) {
            return std::make_shared<File>(path, this->fileValidStatus(path));
        }
};


class ProgressObserver {
    const int progressWidth;
    int observables = 0;
    std::unique_ptr<progresscpp::ProgressBar> pbar;
    
    public:
        void update() {
            if (this->pbar != nullptr) {
                ++(*(this->pbar));
                this->pbar->display();
            }
        }

        void done() {
            if (this->pbar != nullptr) {
                this->pbar->done();
                cout << endl;
            }
        }

        void increaseObervableCounter() {
            this->observables++;
        }

        int getObservablesNumber() {
            return this->observables;
        }

        void init() {
            this->pbar = std::make_unique<progresscpp::ProgressBar>(
                this->observables, this->progressWidth
            );
        }

        ProgressObserver(int progressWidth):
            progressWidth{progressWidth} {
            //
        }
};



class Hash {
    string calculatedHash;
    bool hashsumWasCalculated = false;
    const string _type;
    const std::unique_ptr<hashwrapper> hasher;
    const std::shared_ptr<File> file;
    
    std::shared_ptr<ProgressObserver> observer;

    void _calculate(void) {
        this->calculatedHash = this->hasher->getHashFromFile(this->file->path());
        this->hashsumWasCalculated = true;
    }

    public:
        Hash(string hashname, std::unique_ptr<hashwrapper> wrapper, std::shared_ptr<File> file_ptr) : 
            _type{hashname}, hasher{std::move(wrapper)}, file{file_ptr} {
            assert(file->isValid());
        }

        void registerObserver(std::shared_ptr<ProgressObserver> pObs) {
            this->observer = pObs;
            this->observer->increaseObervableCounter();
        }

        void notify() {
            this->observer->update();
        }

        void calculateSum() {
            if (!this->hashsumWasCalculated) {
                this->_calculate();
            }
        }

        string type() {
            return this->_type;
        }

        string getStringHashSum() {
            if (this->hashsumWasCalculated) {
                return this->calculatedHash;
            } this->_calculate();
            return this->getStringHashSum();
        }

        int getIntHashSum() {
            return hexaToInt(this->getStringHashSum());
        }

        string getFilePath() {
            return this->file->path();
        }
};


class HashFactory: public wrapperfactory {
    public:
        std::shared_ptr<Hash>
        createFileHash(string hashtype, std::shared_ptr<File> file) {
            assert(file->isValid());
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
        }
};


class Checker {
    HashFactory hashFactory;
    const string hashtype;
    const std::shared_ptr<ProgressObserver> progress;
    std::list<std::shared_ptr<Hash>> validFilesHashes;
    std::list<std::shared_ptr<File>> invalidFilesList;

    public:
        Checker(string hashtype):
            hashtype{hashtype},
            progress{std::make_shared<ProgressObserver>(40)} {
        }

    void add(std::shared_ptr<File> file) {
        if (file->isValid()) {
            auto hash = this->hashFactory.createFileHash(this->hashtype, file);
            hash->registerObserver(this->progress);
            this->validFilesHashes.push_front(hash);
        } else {
            this->invalidFilesList.push_front(file);
        }
    }

    void calculateHashSums() {
        this->progress->init();

        auto lambda = [](std::shared_ptr<Hash>& hash) {
            hash->calculateSum();
            hash->notify();
        };

        std::for_each(this->validFilesHashes.begin(), this->validFilesHashes.end(), lambda);
    }

    void displayResults() {
        this->progress->done();
        cout << this->hashtype << "SUM:" << endl;
        if (!this->validFilesHashes.empty()) {
            for (auto& hash : this->validFilesHashes) {
                cout << hash->getStringHashSum() << " ";
                cout << hash->getFilePath() << "\n";
            }
        }

        if (!this->invalidFilesList.empty()) {
            cout << endl;
            cout << "Invalid Files:\n";
            for (auto& file : this->invalidFilesList) {
                cout << " " << file->path() << " -> " << file->explain();
                cout << "\n";
            }
            cout << endl;
        }
    }

};


class App {
    const string name;
    string hashType;
    FileFactory fileFactory;
    std::unique_ptr<ArgumentParser> args;

    void setupArgparser() {
        // Must create the args first!
        assert(this->args != nullptr);

        this->args->add_argument("files").remaining()
                        .help("the name of the file");

        for (auto& htype : VALID_HASH_TYPES_ARRAY) {
            const string lower = toLowerCase(htype);
            this->args->add_argument("-" + lower, "--" + lower + "sum")
                            .help("Use this to calculate the " + lower + " hash sum")
                            .default_value(false).implicit_value(true);
        }
    }

    void getHashType() {
        int counter = 0;
        for (auto& htype : VALID_HASH_TYPES_ARRAY) {
            if (this->args->is_used("-" + toLowerCase(htype))) {
                this->hashType = htype;
                counter++;
            }
        }

        if (counter == 0) {
            this->printErrMessage(
                "Must specify the type of hash sum!"
            );
        } else if (counter > 1) {
            this->printErrMessage(
                "You can chose only one hash type each time!"
            );
        } else {
            // DO NOTHING!
        }
    }

    void parse(int argc, char** argv) {
        try {
            this->args->parse_args(argc, argv);
        } catch (const std::runtime_error& err) {
            std::cerr << err.what() << endl;
            std::cerr << *(this->args) << endl;
            std::exit(1);
        }
    }

    std::unique_ptr<Checker> analizeInputFiles(string hashType) {
        std::unique_ptr<Checker> check = std::make_unique<Checker>(hashType); 

        try {
            auto files = this->args->get<std::vector<string>>("files");
            for (auto& file : files) {
                check->add(this->fileFactory.create(file));
            }
        } catch (const std::logic_error& err) {
            this->printErrMessage("No files were provided");
        }

        return check;
    }

    public:
        App(string name): name{name}, args{std::make_unique<ArgumentParser>(name)} {
            this->setupArgparser();
        }

        void printErrMessage(const string& message) {
            std::cerr << "Err: " << message << endl;
            cout << endl << this->args->help().str() << endl;
            std::exit(1);
        }

        int run(int argc, char** argv) {
            this->parse(argc, argv);
            this->getHashType();
            auto check = this->analizeInputFiles(this->hashType);
            check->calculateHashSums();
            check->displayResults();
            return 0;
        }
};
}

#endif