#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

#include <hashlibpp.h>

using std::cout, std::endl, std::string;
namespace fs = std::filesystem;


class File {
    public:
        const string path;
        const bool isValid;
        File(string path, bool isValid): isValid{isValid}, path{path} {
            //
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
            }
        }

        f.close();
        return isReadable;
    }

    bool fileIsValid(string path) {
        fs::file_status filestatus = fs::status(path);
        return this->fileExists(filestatus)         && 
               this->fileIsNotDirectory(filestatus) &&
               this->fileIsPermissive(filestatus)   &&
               this->fileIsReadable(path);
    }

    public:
        std::unique_ptr<File> createFileType(string path) {
            return std::make_unique<File>(path, this->fileIsValid(path));
        }
};


class Hash {
    string calculatedHash;
    std::unique_ptr<hashwrapper> const hasher;
    bool hashsumWasCalculated = false;
    std::unique_ptr<File> const file;

    void calculate(void) {
        this->calculatedHash = this->hasher->getHashFromFile(this->file->path);
        this->hashsumWasCalculated = true;
    }

    public:
        Hash(std::unique_ptr<hashwrapper> hashWrapper, std::unique_ptr<File> file_ptr):
            hasher{std::move(hashWrapper)}, file{std::move(file_ptr)} {
            assert(file->isValid);
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
            assert(file->isValid);
            auto wrapper = std::unique_ptr<hashwrapper>(this->create(hashtype));
            return std::make_unique<Hash>(std::move(wrapper), std::move(file));
        }
};


int main(int argc, char const* argv[]) {
    HashFactory hashFactory;
    FileFactory fileFactory;
    string filepath = "./compile.sh";

    auto file = fileFactory.createFileType(filepath);

    if (file->isValid) {
        auto hash = hashFactory.createFileHash("SHA1", std::move(file));
        cout << "The SHA1SUM for '" << filepath << "' is: ";
        cout << hash->getStringHashSum() << endl;
    } else {
        cout << "Could not calculate hash for '" << filepath << "'!" << endl;
        cout << "File is invalid!" << endl;
    }

	return 0;
}
