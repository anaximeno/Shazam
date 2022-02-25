#include "include/external/argparse.hpp"
#include "include/shazam.hpp"


using std::cout, std::endl, std::string;
using argparse::ArgumentParser;


class App {
    shazam::HashFactory hashFactory;
    shazam::FileFactory fileFactory;
    std::unique_ptr<ArgumentParser> args;
    const string name;

    void setupArgparser() {
        // Must create the args first!
        assert(this->args != nullptr);

        this->args->add_argument("filename")
                        .required()
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

    // INFO: For tests! Will be improved!
    string exhaustiveGetHashType() {
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

    public:
        App(string name) :
            name{name},
            args{std::make_unique<ArgumentParser>(name)}
        {
            this->setupArgparser();
        }

        int run(int argc, char** argv) {
            this->parse(argc, argv);

            auto file = this->fileFactory.create(this->args->get<string>("filename"));

            cout << "Locating: " << file->path() << endl;

            if (file->isValid()) {
                auto hash = this->hashFactory.createFileHash(this->exhaustiveGetHashType(), file);
                cout << hash->type() << "SUM: " << hash->getStringHashSum() << endl;
            } else {
                std::cerr << this->evaluateFileErr(file->status()) << endl;
            }

            return 0;
        }
};


int main(int argc, char * argv[]) {
	return std::make_unique<App>(argv[0])->run(argc, argv);
}