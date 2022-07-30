#ifndef _SHAZAM_FILES_HEADER
#define _SHAZAM_FILES_HEADER

#include "./basic-types.hh"

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace shazam {
    /* Represents as file in the program. */
    class File {
        const EFileStatus _status;
        const std::string _path;

    public:
        File(std::string path, EFileStatus status)
        : _status(status), _path(path) { }

        /* Returns the path to the file. */
        std::string path() const;

        /* Returns the status of the file. */
        EFileStatus status() const;

        /* Explains the status of the file. */
        std::string explainStatus() const;

        /* Returns true if this file is a valid file. */
        bool isValid() const;

        /* Returns the size of the file. */
        int size();
    };


    class FileFactory {
    private:
        /* Returns true if the file exists. */
        bool fileExists(fs::file_status filestatus);

        /* Returns true if the file is not a directory. */
        bool fileIsNotDirectory(fs::file_status filestatus);

        /* Returns true if the user has permission to read the files. */
        bool fileIsPermissive(fs::file_status filestatus);

        /* Returns true if the file can be read. */
        bool fileIsReadable(std::string path);

    protected:
        /* Returns a enum value corresponding to the state of the file. */
        EFileStatus fileValidStatus(std::string path);

    public:
        /* Returns a shared pointer to a File instance. */
        std::shared_ptr<File> create(std::string path);
    };
};

#endif /* _SHAZAM_FILES_HEADER */
