#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

#include <hashlibpp.h>
#include "include/external/argparse.hpp"


using std::cout, std::endl, std::string;
using argparse::ArgumentParser;

namespace fs = std::filesystem;



/* Values:
     NON_EXISTENT    -> file that wasn't found at the given path

     IS_DIRECTORY    -> the path given points to a directory

     NON_PERMISSIVE  -> The user don't have permissions to read the file

     NOT_READABLE    -> file that cannot be read by some reason

     IS_VALID        -> valid file that can be used withoud any problems
*/
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
        std::unique_ptr<File> createFileType(string path) {
            return std::make_unique<File>(path, this->fileValidStatus(path));
        }
};


class Hash {
    string calculatedHash;
    std::unique_ptr<hashwrapper> const hasher;
    std::unique_ptr<File> const file;
    bool hashsumWasCalculated = false;

    protected:
        void calculate(void) {
            this->calculatedHash = this->hasher->getHashFromFile(this->file->path());
            this->hashsumWasCalculated = true;
        }

    public:
        Hash(std::unique_ptr<hashwrapper> hashWrapper, std::unique_ptr<File> file_ptr):
            hasher{std::move(hashWrapper)}, file{std::move(file_ptr)} {
            assert(file->isValid());
        }

        string getStringHashSum() {
            if (this->hashsumWasCalculated) {
                return this->calculatedHash;
            } this->calculate();
            return this->getStringHashSum();
        }

        int getIntHashSum();
};


class HashFactory: public wrapperfactory {
    const std::array<string, 6> validHashTypes = {
        "MD5", "SHA1", "SHA256", "SHA384", "SHA512"
    };

    public:
        std::unique_ptr<Hash> createFileHash(string hashtype, std::unique_ptr<File> file) {
            assert(file->isValid());
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_unique<Hash>(std::move(wrapper), std::move(file));
        }
};


class Checker {
    // TODO: implement
};


class App {
    HashFactory hashFactory;
    FileFactory fileFactory;
    std::unique_ptr<ArgumentParser> argparser;
    const string name;

    void setupArgparser() {
        // Must create the argparser first!
        assert(this->argparser != nullptr);

        this->argparser->add_argument("filename")
                        .required()
                        .help("the name of the file");
        this->argparser->add_argument("-md5", "--md5sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the md5 hash sum");
        this->argparser->add_argument("-sha1", "--sha1sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha1 hash sum");
        this->argparser->add_argument("-sha256", "--sha256sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha256 hash sum");
        this->argparser->add_argument("-sha384", "--sha384sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the md5 hash sum");
        this->argparser->add_argument("-sha512", "--sha512sum")
                        .default_value(false)
                        .implicit_value(true)
                        .help("Use this to calculate the sha512 hash sum");
    }

    string evaluateFileErr(EFileValidStatus errStatus) {
        string err;

        switch (errStatus)
        {
        case EFileValidStatus::NON_EXISTENT:
            err = "File Not Found! ";
            break;
        case EFileValidStatus::IS_DIRECTORY:
            err = "Directory given as file! ";
            break;
        case EFileValidStatus::NOT_PERMISSIVE:
            err = "Permission was refused to read the file! ";
            break;
        case EFileValidStatus::NOT_READABLE:
            err = "Can't read the file!";
            break;
        default:
            err = "Unknown file status!";
            break;
        }

        return err;
    }

    // INFO: For tests! Will be improved!
    string exhaustiveGetHashType() {
        if (this->argparser->is_used("-md5")) {
            return "MD5";
        } else if (this->argparser->is_used("-sha1")) {
            return "SHA1";
        } else if (this->argparser->is_used("-sha256")) {
            return "SHA256";
        } else if (this->argparser->is_used("-sha384")) {
            return "SHA384";
        } else if (this->argparser->is_used("-sha512")) {
            return "SHA512";
        } else {
            return "none";
        }
    }

    void parse(int argc, char** argv) {
        try {
            this->argparser->parse_args(argc, argv);
        } catch (const std::runtime_error& err) {
            std::cerr << err.what() << endl;
            std::cerr << *(this->argparser) << endl;
            std::exit(1);
        }
    }

    public:
        App(string name): name{name} {
            this->argparser = std::make_unique<ArgumentParser>(name);
            this->setupArgparser();
        }

        int run(int argc, char** argv) {
            this->parse(argc, argv);

            string filepath = this->argparser->get<string>("filename");
            string hashtype = this->exhaustiveGetHashType();
            hashtype = hashtype == "none" ? "SHA256" : hashtype;

            cout << "Checking " << hashtype << "SUM for: " << filepath << endl;

            auto file = this->fileFactory.createFileType(filepath);

            if (file->isValid()) {
                auto hash = this->hashFactory.createFileHash(hashtype, std::move(file));
                cout << hashtype << "SUM: " << hash->getStringHashSum() << endl;
            } else {
                std::cerr << this->evaluateFileErr(file->status()) << endl;
            }

            return 0;
        }
};


int main(int argc, char * argv[]) {
	return std::make_unique<App>(argv[0])->run(argc, argv);
}


// TODO: change the type of pointer of file from unique to shared!