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
        }
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
    string calculatedHash;
    hashwrapper* hashWrapper;
    bool hashsumWasCalculated = false;
    ValidFile* file;
 
    void calculate(void) {
        this->calculatedHash = this->hashWrapper->getHashFromFile(this->file->getPath());
        this->hashsumWasCalculated = true;
    }

    public:
        Hash(hashwrapper* hashWrapper, ValidFile* file) {
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
        Hash* hashFile(string hashtype, ValidFile* file) {
            return new Hash(this->create(hashtype), file);
        }
};


// class Shazam {
//     public:
// };



int main() {
    HashFactory hf;
    ValidFile* f = new ValidFile("compile.sh");
    Hash* h = hf.hashFile("MD5", f);
    cout << h->getStringHashSum() << endl;
    delete f, h;
	return 0;
}
