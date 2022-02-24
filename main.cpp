#include <iostream>
#include <fstream>
#include <cassert>
#include <queue>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#if __cplusplus >= 201703L && __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>)
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#endif
#endif

#include <hashlibpp.h>

using namespace std;


class File {
    public:
        const string path;
        const bool isValid;
        File(string path, bool isValid): isValid{isValid}, path{path} {}
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

        fstream f;
        f.open(path, ios::in | ios::binary);

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
        File* createFileType(string path) {
            return new File(path, this->fileIsValid(path));
        }
};


class Hash {
    string calculatedHash;
    hashwrapper* hashWrapper;
    bool hashsumWasCalculated = false;
    File* file;
 
    void calculate(void) {
        this->calculatedHash = this
                        ->hashWrapper
                        ->getHashFromFile(this->file->path);
        this->hashsumWasCalculated = true;
    }

    public:
        Hash(hashwrapper* hashWrapper, File* file) {
            assert(file->isValid);
            this->hashWrapper = hashWrapper;
            this->file = file;
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
        Hash* createFileHash(string hashtype, File* file) {
            assert(file->isValid);
            return new Hash(this->create(hashtype), file);
        }
};


// class Shazam {
//     public:
// };



int main() {
    HashFactory hashFactory;
    FileFactory fileFactory;
    string filepath = "./compile.sh";

    File* file = fileFactory.createFileType(filepath);

    if (file->isValid) {
        Hash* hash = hashFactory.createFileHash("SHA1", file);
        cout << "The SHA1SUM for '" << filepath << "' is: ";
        cout << hash->getStringHashSum() << endl;
        delete hash;
    } else {
        cout << "Could not calculate hash for '" << filepath << "'!" << endl;
        cout << "File is invalid!" << endl;
    }

    delete file;
	return 0;
}
