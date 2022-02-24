#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

#include <hashlibpp.h>

using std::cout, std::endl, std::string;
namespace fs = std::filesystem;


// This enum contains a set of possible status of the files
// that where received to calculate the hashsums.
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
            return _path;
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

    /* This functions takes the path of one file and classifies it as a:
     * NON_EXISTENT    -> file that wasn't found at the given path
     * IS_DIRECTORY    -> the path given points to a directory
     * NON_PERMISSIVE  -> The user don't have permissions to read the file
     * NOT_READABLE    -> file that cannot be read by some reason
     * IS_VALID        -> valid file that can be used withoud any problems
    */
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
    bool hashsumWasCalculated = false;
    std::unique_ptr<File> const file;

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
    public:
        std::unique_ptr<Hash> createFileHash(string hashtype, std::unique_ptr<File> file) {
            assert(file->isValid());
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_unique<Hash>(std::move(wrapper), std::move(file));
        }
};


int main(int argc, char * argv[]) {
    HashFactory hashFactory;
    FileFactory fileFactory;

    // const std::array<string, 6> hashTypes = {
    //    "MD5", "SHA1", "SHA256", "SHA384", "SHA512"
    //};

    string filepath = "./compile.sh";
    if (argc == 2) {
        filepath = string(argv[1]);
    }

    cout << "Looking for: " << filepath << endl;
    auto file = fileFactory.createFileType(filepath);

    if (file->isValid()) {
        auto hash = hashFactory.createFileHash("SHA1", std::move(file));
        cout << "The SHA1SUM for '" << filepath << "' is: ";
        cout << hash->getStringHashSum() << endl;
    } else {
        std::cerr << "Error: ";
        switch (file->status())
        {
        case EFileValidStatus::NON_EXISTENT:
            std::cerr << "File Not Found: ";
            break;
        case EFileValidStatus::IS_DIRECTORY:
            std::cerr << "Directory given as file: ";
            break;
        case EFileValidStatus::NOT_PERMISSIVE:
            std::cerr << "Refused permission: ";
            break;
        case EFileValidStatus::NOT_READABLE:
            std::cerr << "Could not read: ";
            break;
        default:
            std::cerr << "Unknown file status: ";
            break;
        }
        std::cerr << file->path() << endl;
    }
	return 0;
}
