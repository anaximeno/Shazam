#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <memory>
#include <queue>

#include "include/external/argparse.hpp"
#include "include/shazam.hpp"


class App {
    shazam::HashFactory hashFactory;
    shazam::FileFactory fileFactory;
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