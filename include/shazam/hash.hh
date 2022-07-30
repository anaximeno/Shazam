#ifndef _SHAZAM_HASH_HEADER
#define _SHAZAM_HASH_HEADER

#include "./basic-types.hh"
#include "./common.hh"
#include "./files.hh"

#include "../external/hashlib2plus/hl_hashwrapper.h"
#include "../external/hashlib2plus/hl_wrapperfactory.h"

#include <string>
#include <memory>

namespace shazam {
    /* Calcultes the hash sum. */
    class HashCalculator: public IAmObservable {
        const std::string hashName;
        const std::shared_ptr<File> file;
        const std::unique_ptr<hashwrapper> hasher;
        std::string hashSum = "";

    public:
        HashCalculator(std::string hashname, std::unique_ptr<hashwrapper> wrapper, std::shared_ptr<File> file_ptr)
        : hashName(hashname), file(file_ptr), hasher(std::move(wrapper)) {}

        /* Calculates the hash sum. */
        void calculate(void);

        /* Returns the type of hash sum being calculated. */
        std::string type(void);

        /* Returns the calculated hash sum. */
        HashSum get(void);

        /* Returns the path of the file being used. */
        std::string getFilePath(void);

    private:
        /* Makes the calculation of the hash sum and returns the result. */
        std::string calculateHashSum(void);
    };

    /* Compares the hashes and returns the result. */
    FileHashSumComparationResult compareHashes(HashSum originalHashSum, HashSum currentHashSum);

    class HashFactory: protected wrapperfactory {
    public:
        /* Creates an hash calculator class for the given file, depending on the given hash type. */
        std::shared_ptr<HashCalculator> hashFile(std::string hashtype, std::shared_ptr<File> file);
    };
};

#endif /* _SHAZAM_HASH_HEADER */
