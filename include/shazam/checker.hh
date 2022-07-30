#ifndef _SHAZAM_CHECKER_HEADER
#define _SHAZAM_CHECKER_HEADER

#include "./common.hh"
#include "./files.hh"
#include "./hash.hh"

#include <list>
#include <string>
#include <memory>

namespace shazam {
    /* The hash checker. */
    class Checker {
        bool showProgressBar;
        bool showInvalidFiles;
        const std::shared_ptr<ProgressObserver> progress;
        std::list<std::shared_ptr<HashCalculator>> validFilesHashes;
        std::list<std::shared_ptr<File>> invalidFilesList;
        HashFactory hashFactory;

    public:
        Checker(bool showProgressBar, bool showInvalidFiles)
        : showProgressBar(showProgressBar), showInvalidFiles(showInvalidFiles),
        progress(std::make_shared<ProgressObserver>(40)) {  }

        Checker(): Checker(false, true) {  }

        /* Get the list of valid hashed files. */
        std::list<std::shared_ptr<HashCalculator>> getValidHashesList();

        /* Get the list of invalid files. */
        std::list<std::shared_ptr<File>> getInvalidFilesList();

        /* Adds a new file to the checker. */
        void add(std::shared_ptr<File> file, std::string hashtype);

        /* Calcultes the hash sums.  */
        void calculateHashSums();

        /* Changes the showProgressBar attr definition.
         * If set to true, the progress bar will be shown to the
         * user during the execution, if false, it won't be shown.
         * */
        void setShowProgressBar(bool value);

        /* Changes the showInvalidFiles attr definition.
         * If set to true, invalid files will be shown to the
         * user during the execution, if false, they won't be shown.
         * */
        void setShowInvalidFiles(bool value);

        /* Displays the result of the hash check. */
        void displayResults();

    private:
        /* Displays the valid hashes as a result. */
        void displayValidHashes();

        /* Displays invalid files, if showInvalidFiles is true. */
        void displayInvalidFiles();
    };
};

#endif /* _SHAZAM_CHECKER_HEADER */
