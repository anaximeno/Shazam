#include "../include/shazam/hash.hh"

#include "../include/external/hashlib2plus/hl_hashwrapper.h"

#include <string>
#include <memory>

void shazam::Hash::registerObserver(std::shared_ptr<ProgressObserver> obs)
{
    obs->increaseObervableCounter();
    observer = obs;
}

void shazam::Hash::notify(void)
{
    observer->update();
}

void shazam::Hash::calculate(void)
{
    if (!hexHashSum.wasCalculated) {
        hexHashSum.value = calculateHashSum();
        hexHashSum.wasCalculated = true;
    } else {
        // Do Nothing!
    }
}

std::string shazam::Hash::type(void)
{
    return hashName;
}

std::string shazam::Hash::getStringHashSum(void)
{
    if (!hexHashSum.wasCalculated)
        calculate();

    return hexHashSum.value;
}

std::string shazam::Hash::getFilePath(void)
{
    return file->path();
}

std::string shazam::Hash::calculateHashSum(void)
{
    return hasher->getHashFromFile(file->path());
}

std::shared_ptr<shazam::Hash>
shazam::HashFactory::hashFile(std::string hashtype, std::shared_ptr<shazam::File> file)
{
    auto wrapper = std::unique_ptr<hashwrapper>(create(hashtype));
    return std::make_shared<Hash>(hashtype, std::move(wrapper), file);
}
