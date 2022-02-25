#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

// TODO: turn this into a module inside the include path!!
#include <hashlibpp.h>
#include "external/argparse.hpp"


namespace shazam {
    namespace fs = std::filesystem;
    using std::string;

    /* Enum FileValidStatus Values:
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

            int getIntHashSum();
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

    class Checker {
        
    };
}