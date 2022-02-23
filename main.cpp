#include <iostream>
#include <queue>
#include <hashlibpp.h>
using namespace std;


class AbstractFile {
    protected:
        string path;
        bool isValidFile;
    public:
        AbstractFile(string path) {
            this->path = path;
        }
        bool isValid() {
            return this->isValidFile;
        };
        string getPath() {
            return this->path;
        }
};


class InvalidFile: public AbstractFile {
    string reason;
    public:
        InvalidFile(string path, string reason): AbstractFile(path) {
            this->reason = reason;
        }
};


class ValidFile: public AbstractFile {
    public:
        ValidFile(string path): AbstractFile(path) {
            isValidFile = true;
        }
};


class FileFactory {
    public:
        // TODO
        static AbstractFile getFile(string path);
};


class Hash {
    string calculatedHash = "";
    hashwrapper* hashWrapperInstance;
    ValidFile* file;
    void calculate();

    public:
        Hash(hashwrapper* hashWrapperInstance, ValidFile* file) {
            this->hashWrapperInstance = hashWrapperInstance;
            this->file = file;
        }

        string getStringHashSum();
        int getIntHashSum();
};


class HashFactory: public wrapperfactory {
    public:
        Hash* hashFile(string hashtype, ValidFile* file) {
            return new Hash(this->create(hashtype), file);
        }
};


class Shazam {
    public:
};



int main() {
    hashwrapper *h = new md5wrapper();
    h->test();
    std::string md5 = h->getHashFromString("hello world");
    std::cout << md5 << std::endl;
    delete h;

	cout << "hello world" << endl; 
	return 0;
}
