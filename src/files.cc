#include "../include/shazam/files.hh"
#include "../include/shazam/basic-types.hh"
#include "../include/shazam/common.hh"

#include <string>
#include <algorithm>
#include <memory>

std::string shazam::File::path() const
{
    return _path;
}

shazam::EFileStatus shazam::File::status() const
{
    return _status;
}

bool shazam::File::isValid() const
{
    return status() == VALID_FILE;
}

int shazam::File::size()
{
    return isValid() ? fs::file_size(path()) : 0;
}

std::string shazam::File::explainStatus() const
{
    switch (status()) {
        case NON_EXISTENT:
            return "Was not found.";
        case IS_DIRECTORY:
            return "Its a directory.";
        case NON_PERMISSIVE:
            return "No permissions to read.";
        case NON_READABLE:
            return "Could not read!";
        case VALID_FILE:
            return "File is valid.";
        default:
            return "Unknown file status!";
    }
}

bool shazam::FileFactory::fileExists(fs::file_status filestatus)
{
    return fs::exists(filestatus);
}

bool shazam::FileFactory::fileIsNotDirectory(fs::file_status filestatus)
{
    return !fs::is_directory(filestatus);
}

bool shazam::FileFactory::fileIsPermissive(fs::file_status filestatus)
{
    return (filestatus.permissions() & fs::perms::owner_read) != fs::perms::none;
}

bool shazam::FileFactory::fileIsReadable(std::string path)
{
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
        }
    }

    f.close();
    return isReadable;
}

shazam::EFileStatus shazam::FileFactory::fileValidStatus(std::string path)
{
    fs::file_status filestatus = fs::status(path);

    if (fileExists(filestatus) == false) {
        return NON_EXISTENT;
    } else if (fileIsNotDirectory(filestatus) == false) {
        return IS_DIRECTORY;
    } else if (fileIsPermissive(filestatus) == false) {
        return NON_PERMISSIVE;
    } else if (fileIsReadable(path) == false) {
        return NON_READABLE;
    } else {
        return VALID_FILE;
    }
}

std::shared_ptr<shazam::File> shazam::FileFactory::create(std::string path)
{
    return std::make_shared<shazam::File>(path, fileValidStatus(path));
}
