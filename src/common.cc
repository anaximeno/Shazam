#include "../include/shazam/common.hh"

#include "../include/external/ProgressBar.hpp"

#include <string>
#include <algorithm>
#include <memory>
#include <iostream>

namespace pgs = progresscpp;

int shazam::hexaToInt(std::string hexadecimalString)
{
    return std::stoi(hexadecimalString, 0, 16);
}

std::string shazam::toUpperCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string shazam::toLowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void shazam::printErrMessage(const std::string& message)
{
    std::cerr << "Shazam: Err: " << message << std::endl;
    std::exit(1);
}

void shazam::ProgressObserver::init()
{
    const int observables = getObservablesNumber();
    if (observables > 0) {
        progressBar = std::make_unique<pgs::ProgressBar>(observables, progressWidth);
    }
}

void shazam::ProgressObserver::update()
{
    decreaseObervableCounter();
    if (progressBar != nullptr) {
        ++( *progressBar );
        progressBar->display();
    }
}

void shazam::ProgressObserver::done()
{
    if (progressBar != nullptr) {
        progressBar->done();
        std::cout << std::endl;
    }
}

void shazam::ProgressObserver::increaseObervableCounter()
{
    activeObservables++;
}

void shazam::ProgressObserver::decreaseObervableCounter()
{
    activeObservables--;
}

int shazam::ProgressObserver::getObservablesNumber()
{
    return activeObservables;
}

void shazam::IAmObservable::setObserver(std::shared_ptr<ProgressObserver> observer)
{
    this->observer = observer;
    this->observer->increaseObervableCounter();
}

void shazam::IAmObservable::notifyObserver()
{
    if (observer != nullptr)
        observer->update();
}
