#ifndef _SHAZAM_HEADER
#define _SHAZAM_HEADER

#include "./basic-types.hh"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>

#include "../external/argparse.hpp"
#include "../external/ProgressBar.hpp"
#include "../external/hashlib2plus/hl_hashwrapper.h"
#include "../external/hashlib2plus/hl_wrapperfactory.h"

namespace fs = std::filesystem;
namespace pgs = progresscpp;
namespace ap = argparse;


namespace shazam {
    std::string explainFileStatus(const EFileStatus status);

    int hexaToInt(std::string hexadecimalString);

    std::string toUpperCase(std::string str);

    std::string toLowerCase(std::string str);

    void printErrMessage(const std::string& message);

    class File {
        const EFileStatus _status;
        const std::string _path;

    public:
        File(std::string path, EFileStatus status)
        : _status(status), _path(path) {  }
        std::string path() const;
        EFileStatus status() const;
        std::string explainStatus() const;
        bool isValid() const;
        int size();
    };


    class FileFactory {
    private:
        bool fileExists(fs::file_status filestatus);
        bool fileIsNotDirectory(fs::file_status filestatus);
        bool fileIsPermissive(fs::file_status filestatus);
        bool fileIsReadable(std::string path);

    protected:
        EFileStatus fileValidStatus(std::string path);

    public:
        std::shared_ptr<File> create(std::string path);
    };

    class ProgressObserver {
    private:
        const int progressWidth;
        int activeObservables;
        std::unique_ptr<pgs::ProgressBar> progressBar;

    public:
        void update();
        void done();
        void increaseObervableCounter();
        int getObservablesNumber();
        void init();

        ProgressObserver(int progressWidth)
        : progressWidth(progressWidth), activeObservables(0) {  }
    };

    class Hash {
        // stores hash sum in different formats
        template <class T> struct SHashSum {
            bool wasCalculated;
            T value;
        };

        // SHashSum<int> intHashsum{false, 0};
        SHashSum<std::string> hexHashSum{false, ""};
        const std::string hashName;
        const std::shared_ptr<File> file;
        const std::unique_ptr<hashwrapper> hasher;
        std::shared_ptr<ProgressObserver> observer;

    public:
        Hash(std::string hashname, std::unique_ptr<hashwrapper> wrapper, std::shared_ptr<File> file_ptr)
        : hashName(hashname), file(file_ptr), hasher(std::move(wrapper)) {
            //
        }

        void registerObserver(std::shared_ptr<ProgressObserver> obs);
        void notify(void);
        void calculate(void);
        std::string type(void);
        std::string getStringHashSum(void);
        // int getIntHashSum(void);
        std::string getFilePath(void);

    private:
        std::string calculateHashSum(void);
    };

    class HashFactory: protected wrapperfactory {
    public:
        std::shared_ptr<Hash> hashFile(std::string hashtype, std::shared_ptr<File> file);
    };

    class Checker {
        bool showProgressBar;
        bool showInvalidFiles;
        const std::shared_ptr<ProgressObserver> progress;
        std::list<std::shared_ptr<Hash>> validFilesHashes;
        std::list<std::shared_ptr<File>> invalidFilesList;
        HashFactory hashFactory;

    public:
        Checker(bool showProgressBar, bool showInvalidFiles)
        : showProgressBar(showProgressBar), showInvalidFiles(showInvalidFiles),
        progress(std::make_shared<ProgressObserver>(40)) {  }
        Checker(): Checker(false, true) {  }
        std::list<std::shared_ptr<Hash>> getValidHashesList();
        std::list<std::shared_ptr<File>> getInvalidFilesList();
        void add(std::shared_ptr<File> file, std::string hashtype);
        void calculateHashSums();
        void setShowProgressBar(bool value);
        void setShowInvalidFiles(bool value);
        void displayResults();

    private:
        void displayValidHashes();
        void displayInvalidFiles();
    };


    class App {
        const std::string name;
        const std::string version;
        std::unique_ptr<ap::ArgumentParser> args;
        std::unique_ptr<Checker> checker;
        FileFactory fileFactory;

    public:
        App(std::string name, std::string ver)
        : name(name), version(ver), args(std::make_unique<ap::ArgumentParser>(name, ver)),
        checker(std::make_unique<Checker>()) {
            this->setupArgparser();
        }

        int run(const int& argc, const char* const*& argv);

    private:
        void setupArgparser();

        std::string getHashType();

        void parseArguments(const int& argc, const char* const*& argv);

        void getAndRegisterInputFiles(std::string hashType);
    };
}

#endif
