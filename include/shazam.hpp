// ------ Include Libs ----------------------------------------------

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

#include "external/hashlibpp.hpp"
#include "external/argparse.hpp"
#include "external/progressbar.hpp"


// ------------------------------------------------------------------

namespace shazam {

// ---- Commonly used namespaces ------------------------------------
namespace fs = std::filesystem;

using std::string;
using std::cout, std::endl, std::string;
using hashlibpp::hashwrapper, hashlibpp::wrapperfactory;
using argparse::ArgumentParser;


enum EFileValidStatus {
    IS_VALID,
    NON_EXISTENT,
    IS_DIRECTORY,
    NOT_PERMISSIVE,
    NOT_READABLE
};


class File {
    const EFileValidStatus _status;
    const string _path;
    string reasonForStatus;
    public:
        File(string path, EFileValidStatus status):
            _status{status}, _path{path} {
        switch (status)
            {
            case NON_EXISTENT:
                this->reasonForStatus = "File Not Found! ";
                break;
            case IS_DIRECTORY:
                this->reasonForStatus = "Directory given as file! ";
                break;
            case NOT_PERMISSIVE:
                this->reasonForStatus = "Permission was refused to read the file! ";
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

        string explain() {
            return this->reasonForStatus;
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


class Hash {
    string calculatedHash;
    bool hashsumWasCalculated = false;
    const string _type;
    const std::unique_ptr<hashwrapper> hasher;
    const std::shared_ptr<File> file;

    void _calculate(void) {
        this->calculatedHash = this->hasher->getHashFromFile(this->file->path());
        this->hashsumWasCalculated = true;
    }

    public:

        Hash(string hashname,
            std::unique_ptr<hashwrapper> wrapper,
            std::shared_ptr<File> file_ptr
        ) : _type{hashname},
            hasher{std::move(wrapper)},
            file{file_ptr}
        {
            assert(file->isValid());
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
            return std::stoi(this->getStringHashSum(), 0, 16);
        }

        string getFilePath() {
            return this->file->path();
        }
};


class HashFactory: public wrapperfactory {
    const std::array<string, 6> validHashTypes = {
        "MD5", "SHA1", "SHA256", "SHA384", "SHA512"
    };

    public:
        std::shared_ptr<Hash>
        createFileHash(string hashtype, std::shared_ptr<File> file) {
            assert(file->isValid());
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
        }
};

class ProgressObserver {
    // TODO: implement this class
};

class Checker {
    const string hashtype;
    HashFactory hashFactory;
    std::list<std::shared_ptr<Hash>> validFilesHashes;
    std::list<std::shared_ptr<File>> invalidFilesList;

    public:
        Checker(string hashtype): hashtype{hashtype} {
            // 
        }
    
    void add(std::shared_ptr<File> file) {
        if (file->isValid()) {
            this->validFilesHashes.push_front(
                this->hashFactory.createFileHash(this->hashtype, file)
            );
        } else {
            this->invalidFilesList.push_front(file);
        }
    }

    void calculateHashSums() {
        auto lambda = [](std::shared_ptr<Hash>& hash) {
            hash->calculateSum();
        };

        std::for_each(this->validFilesHashes.begin(), this->validFilesHashes.end(), lambda);
    }

    void displayResults() {
        cout << this->hashtype << "SUM:" << endl;
        if (!this->validFilesHashes.empty()) {
            for (auto& hash : this->validFilesHashes) {
                cout << hash->getStringHashSum() << " ";
                cout << hash->getFilePath() << "\n";
            }
            cout << endl;
        }

        if (!this->invalidFilesList.empty()) {
            cout << "Not found: ";
            for (auto& file : this->invalidFilesList) {
                cout << file->path() << ", ";
            }
            cout << endl;
        }
    }

};


class App {
    const string name;
    FileFactory fileFactory;
    std::unique_ptr<ArgumentParser> args;

    void setupArgparser() {
        // Must create the args first!
        assert(this->args != nullptr);

        this->args->add_argument("files")
                        .remaining()
                        .help("the name of the file");
        this->args->add_argument("-md5", "--md5sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the md5 hash sum");
        this->args->add_argument("-sha1", "--sha1sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha1 hash sum");
        this->args->add_argument("-sha256", "--sha256sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha256 hash sum");
        this->args->add_argument("-sha384", "--sha384sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the md5 hash sum");
        this->args->add_argument("-sha512", "--sha512sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha512 hash sum");
    }

    string getHashType() {
        if (this->args->is_used("-md5")) {
            return "MD5";
        } else if (this->args->is_used("-sha1")) {
            return "SHA1";
        } else if (this->args->is_used("-sha256")) {
            return "SHA256";
        } else if (this->args->is_used("-sha384")) {
            return "SHA384";
        } else if (this->args->is_used("-sha512")) {
            return "SHA512";
        } else { // Default, TODO: Maybe add to a config file,
                // and give the user an option to change.
            return "SHA256";
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
            cout << "No files were provided" << std::endl;
            cout << this->args->help().str() << endl;
            std::exit(0);
        }

        return check;
    }

    public:
        App(string name) :
            name{name},
            args{std::make_unique<ArgumentParser>(name)}
        {
            this->setupArgparser();
        }

        int run(int argc, char** argv) {
            this->parse(argc, argv);
            const string hashType = this->getHashType();
            auto check = this->analizeInputFiles(hashType);
            check->calculateHashSums();
            check->displayResults();
            return 0;
        }
};
}
