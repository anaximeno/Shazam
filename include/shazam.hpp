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

using namespace hashlibpp;
using std::string;


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
    public:
        File(string path, EFileValidStatus status):
            _status{status}, _path{path} {
            //
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
    const std::unique_ptr<hashwrapper> hasher;
    const std::shared_ptr<File> file;
    const string _type;
    bool hashsumWasCalculated = false;

    protected:
        void calculate(void) {
            this->calculatedHash = this->hasher->getHashFromFile(this->file->path());
            this->hashsumWasCalculated = true;
        }

    public:
        Hash(string hashname,
            std::unique_ptr<hashwrapper> wrapper,
            std::shared_ptr<File> file_ptr
        ) : hasher{std::move(wrapper)},
            file{file_ptr},
            _type{hashname}
        {
            assert(file->isValid());
        }

        string getStringHashSum() {
            if (this->hashsumWasCalculated) {
                return this->calculatedHash;
            } this->calculate();
            return this->getStringHashSum();
        }

        string type() {
            return this->_type;
        }

        int getIntHashSum() {
            return std::stoi(this->getStringHashSum(), 0, 16);
        }
};


class HashFactory: public wrapperfactory {
    const std::array<string, 6> validHashTypes = {
        "MD5", "SHA1", "SHA256", "SHA384", "SHA512"
    };

    public:
        std::unique_ptr<Hash>
        createFileHash(string hashtype, std::shared_ptr<File> file) {
            assert(file->isValid());
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_unique<Hash>(hashtype, std::move(wrapper), file);
        }
};

class ProgressObserver {
    // TODO: implement this class
};

class Checker {
    const string hashType;
    std::queue<std::shared_ptr<Hash>> hashedFilesQueue;

    public:
        Checker(string hashType): hashType{hashType} {

        }
    
};

class App {
    const string name;
    shazam::HashFactory hashFactory;
    shazam::FileFactory fileFactory;
    std::list<std::shared_ptr<File>> invalidFilesQueue;
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

    string evaluateFileErr(shazam::EFileValidStatus errStatus) {
        string err;

        switch (errStatus)
        {
        case shazam::EFileValidStatus::NON_EXISTENT:
            err = "File Not Found! ";
            break;
        case shazam::EFileValidStatus::IS_DIRECTORY:
            err = "Directory given as file! ";
            break;
        case shazam::EFileValidStatus::NOT_PERMISSIVE:
            err = "Permission was refused to read the file! ";
            break;
        case shazam::EFileValidStatus::NOT_READABLE:
            err = "Can't read the file!";
            break;
        default:
            err = "Unknown file status!";
            break;
        }

        return err;
    }

    string getHashType() {
        std::array<string, 6> hashes;
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

    void getFiles() {
        try {
            auto files = this->args->get<std::vector<string>>("files");
            for (auto& file : files)
                std::cout << file << std::endl;
        } catch (const std::logic_error& err) {
            std::cout << "No files provided" << std::endl;
        }
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
            this->getFiles();
            std::exit(0);
            
            auto file = this->fileFactory.create(this->args->get<string>("filename"));

            cout << "Locating: " << file->path() << endl;

            if (file->isValid()) {
                auto hash = this->hashFactory.createFileHash(this->getHashType(), file);
                cout << hash->type() << "SUM: " << hash->getStringHashSum() << endl;
            } else {
                std::cerr << this->evaluateFileErr(file->status()) << endl;
            }

            return 0;
        }
};
}


