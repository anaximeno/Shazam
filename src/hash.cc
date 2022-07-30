#include "../include/shazam/common.hh"
#include "../include/shazam/basic-types.hh"
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

shazam::HashSum shazam::HashCalculator::get(void)
{
    if (hashSum == "")
        calculate();

    return HashSum {
        .filename = file->path(),
        .hashType = hashName,
        .hashSum = hashSum
    };
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

shazam::FileHashSumComparationResult shazam::HashComparator::compareHashes()
{
    const std::string original = originalHashSum.hashSum;
    const std::string current = currentHashSum.hashSum;
    const ComparationResult result = makeComparation(original, current);

    return FileHashSumComparationResult {
        .filename = currentHashSum.filename,
        .hashType = originalHashSum.hashType,
        .originalHashSum = original,
        .currentHashSum = current,
        .result = result
    };
}

shazam::ComparationResult
shazam::HashComparator::makeComparation(std::string original, std::string current)
{
    const auto _orig_ull = hexaToInt(original);
    const auto _curr_ull = hexaToInt(current);

    if (_orig_ull == _curr_ull)
        return MATCH;
    else
        return NOT_MATCH;
}