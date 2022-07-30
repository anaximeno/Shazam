#ifndef _SHAZAM_COMMON_HEADER
#define _SHAZAM_COMMON_HEADER

#include "../external/ProgressBar.hpp"

#include <string>
#include <memory>

namespace pgs = progresscpp;

namespace shazam {
    /* Converts and hexadecimal value to integer. */
    int hexaToInt(std::string hexadecimalString);

    /* Returns the input str as an uppercase output. */
    std::string toUpperCase(std::string str);

    /* Returns the input str as an lowercase output. */
    std::string toLowerCase(std::string str);

    /* Prints an error message and exits. */
    void printErrMessage(const std::string& message);

    /* Observes the progress of the tasks being completed. */
    class ProgressObserver {
    private:
        const int progressWidth;

        int activeObservables;

        std::unique_ptr<pgs::ProgressBar>
        progressBar;

    public:
        /* Initializes the progress bar. */
        void init();

        /* Updates the progress bar. */
        void update();

        /* Terminates the progress bar. */
        void done();

        /* Increases the number of observables tasks. */
        void increaseObervableCounter();

        /* Returns the current number of observables tasks. */
        int getObservablesNumber();

        /* Receives the `progressWidth` param which represents
         * the width of the progress bar that is shown to the user.
         * */
        ProgressObserver(int progressWidth)
        : progressWidth(progressWidth), activeObservables(0) {  }

    private:
        /* Decreases the number of observables tasks. */
        void decreaseObervableCounter();
    };

    class IAmObservable {
        std::shared_ptr<ProgressObserver> observer;
    public:
        /* Sets the observer for this observable class. */
        virtual void setObserver(std::shared_ptr<ProgressObserver> observer);

        /* Notifies the observer about a change on the state. */
        virtual void notifyObserver(void);
    };
};

#endif /* _SHAZAM_COMMON_HEADER */
