#ifndef _SHAZAM_HASH_HEADER
#define _SHAZAM_HASH_HEADER

#include "./common.hh"
#include "./files.hh"

#include "../external/hashlib2plus/hl_hashwrapper.h"
#include "../external/hashlib2plus/hl_wrapperfactory.h"

#include <string>
#include <memory>

namespace shazam {
    /* Calcultes the hash sum. */
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
        std::string getFilePath(void);

    private:
        std::string calculateHashSum(void);
    };

    class HashFactory: protected wrapperfactory {
    public:
        std::shared_ptr<Hash> hashFile(std::string hashtype, std::shared_ptr<File> file);
    };
};

#endif /* _SHAZAM_HASH_HEADER */
