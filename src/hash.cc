#include "../include/shazam/hash.hh"

#include "../include/external/hashlib2plus/hl_hashwrapper.h"

#include <string>
#include <memory>

void shazam::HashCalculator::calculate(void)
{
    if (hashSum == "")
        hashSum = calculateHashSum();
}

std::string shazam::HashCalculator::type(void)
{
    return hashName;
}

std::string shazam::HashCalculator::getStringHashSum(void)
{
    if (hashSum == "")
        calculate();
    // TODO: update to return the HashSum struct
    return hashSum;
}

std::string shazam::HashCalculator::getFilePath(void)
{
    return file->path();
}

std::string shazam::HashCalculator::calculateHashSum(void)
{
    return hasher->getHashFromFile(file->path());
}

std::shared_ptr<shazam::HashCalculator>
shazam::HashFactory::hashFile(std::string hashtype, std::shared_ptr<shazam::File> file)
{
    auto wrapper = std::unique_ptr<hashwrapper>(create(hashtype));
    return std::make_shared<HashCalculator>(hashtype, std::move(wrapper), file);
}
