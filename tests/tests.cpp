#include <iostream>
#include <memory>

#include "./include/tinytest.hpp"
#include "../include/shazam.hpp"


#define VALID_FILE_S_PATH       "static_file.txt"
#define VALID_FILE_S_MD5SUM     "e2c1267c569e72c2365d6fab0a5167c0"
#define VALID_FILE_S_SHA1SUM    "5652d27b9769b7e1e11af27915a421272429cade"
#define VALID_FILE_S_SHA256SUM  "5e093ff408286e5d57c1b4bbb0f4aad439ec070ed42796946e41bea3ac2286be"
#define VALID_FILE_S_SHA384SUM  "2bad85d5481290e1c756504882d6eff9a5915d17df625955d2d485a120ccfda1736722cb0a097ae6607560c52cf1d88e"
#define VALID_FILE_S_SHA512SUM  "eeabc1e01e9572a3cba059289486caedd372ef8bfc357c6e3b90290896c3f7eb3e940a3d5d88349533133d5c4f89482d3a9648d55228beef0c45b269f814faa9"



// -------------- Testing File Factory -------------------------------------------------

void test_file_factory_non_existent_file(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("i_dont_exist.txt");

    ASSERT("Testing FileFactory for a non existent file",
        file->status() == shazam::EFileStatus::NON_EXISTENT
    );

    ASSERT_EQUALS(file->isValid(), false);
}

void test_file_factory_on_directories(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("./");

    ASSERT("Testing FileFactory on directories",
        file->status() == shazam::EFileStatus::IS_DIRECTORY
    );

    ASSERT_EQUALS(file->isValid(), false);
}

void test_file_factory_on_non_permissive_files(void) {
    shazam::FileFactory factory;

    std::system("touch .filefortest.shazam.tmp && chmod -r .filefortest.shazam.tmp");

    std::shared_ptr<shazam::File> file = factory.create(".filefortest.shazam.tmp");

    ASSERT(
        "Testing FileFactory for non permissive files",
        file->status() == shazam::EFileStatus::NON_PERMISSIVE
    );

    std::system("rm .filefortest.shazam.tmp");

    ASSERT_EQUALS(file->isValid(), false);
}


// -------------- END Testing File Factory ---------------------------------------------



// -------------- Testing Hash Sums ----------------------------------------------------

void test_md5sum() {
    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    const auto hash = hfactory.hashFile("MD5", ffactory.create(VALID_FILE_S_PATH));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing MD5 hash sum", VALID_FILE_S_MD5SUM == CALCULATED);
}

void test_sha1sum() {
    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    const auto hash = hfactory.hashFile("SHA1", ffactory.create(VALID_FILE_S_PATH));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA1 hash sum", VALID_FILE_S_SHA1SUM == CALCULATED);
}

void test_sha256sum() {
    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    const auto hash = hfactory.hashFile("SHA256", ffactory.create(VALID_FILE_S_PATH));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA256 hash sum", VALID_FILE_S_SHA256SUM == CALCULATED);
}

void test_sha384sum() {
    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    const auto hash = hfactory.hashFile("SHA384", ffactory.create(VALID_FILE_S_PATH));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA384 hash sum", VALID_FILE_S_SHA384SUM == CALCULATED);
}

void test_sha512sum() {
    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    const auto hash = hfactory.hashFile("SHA512", ffactory.create(VALID_FILE_S_PATH));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA512 hash sum", VALID_FILE_S_SHA512SUM == CALCULATED);
}

// -------------- END Hash Sums --------------------------------------------------------

// -------------- File Size ------------------------------------------------------------

void test_file_size() {
    // previously calculated
    const int SIZE = 17;

    shazam::FileFactory ffactory;
    const auto file = ffactory.create(VALID_FILE_S_PATH);
    const auto not_existent_file = ffactory.create("i_dont_exist.txt");

    ASSERT("Testing file.size", SIZE == file->size());
    ASSERT_EQUALS(not_existent_file->size(), 0);
}

// -------------- END File Size --------------------------------------------------------

// -------------- Hexa to Int converter ------------------------------------------------

void test_hexa_to_int_converter() {
    const string HEXA = "beaf"; // :-´)
    const int RESULT = 48815;
    ASSERT("Testing Hexa to Int Converter", shazam::hexaToInt(HEXA) == RESULT);
}

// -------------- END Hexa to Int converter ---------------------------------------------


// -------------- Upper and Lower case converters ---------------------------------------

void test_upper_case_converter() {
    string lower_case_sentence = "oke dokie";
    string upper_case_sentence = "OKE DOKIE";

    ASSERT("Uppercase converter",
        shazam::toUpperCase(lower_case_sentence) == upper_case_sentence
    );
}

void test_lower_case_converter() {
    string lower_case_sentence = "oke dokie";
    string upper_case_sentence = "OKE DOKIE";

    ASSERT("Lowercase converter",
        shazam::toLowerCase(upper_case_sentence) == lower_case_sentence
    );
}

// -------------- END Upper and Lower case converters ------------------------------------


// -------------- Testing Checker --------------------------------------------------------

void test_checker_on_invalid_files() {
    shazam::Checker checker;
    shazam::FileFactory ffactory;
    
    checker.add(ffactory.create("i_dont_exist.txt"), "SHA1");

    ASSERT("Checker.validHashesList should be empty", checker.getValidHashesList().empty());
    ASSERT("Checker.invalidFilesList should not be empty", !checker.getInvalidFilesList().empty());
}

void test_checker_on_valid_files() {
    shazam::Checker checker;
    shazam::FileFactory ffactory;
    
    checker.add(ffactory.create(VALID_FILE_S_PATH), "SHA1");

    ASSERT("Checker.validHashesList should not be empty", !checker.getValidHashesList().empty());
    ASSERT("Checker.invalidFilesList should be empty", checker.getInvalidFilesList().empty());
}

void test_checker_on_valid_and_invalid_files() {
    shazam::Checker checker;
    shazam::FileFactory ffactory;

    checker.add(ffactory.create(VALID_FILE_S_PATH), "SHA1");
    checker.add(ffactory.create("i_dont_exist.txt"), "SHA1");

    ASSERT("Checker.validHashesList should not be empty", !checker.getValidHashesList().empty());
    ASSERT("Checker.invalidFilesList should not be empty", !checker.getInvalidFilesList().empty());
}

void test_checker_hash_sum_calculation() {
    shazam::Checker checker;
    shazam::FileFactory ffactory;

    checker.add(ffactory.create(VALID_FILE_S_PATH), "SHA1");
    checker.add(ffactory.create(VALID_FILE_S_PATH), "SHA256");

    const auto list = checker.getValidHashesList();

    const auto sha256 = list.front();
    const auto sha1 = list.back();
    ASSERT("Testing Checker sha1sum result", sha1->getStringHashSum() == VALID_FILE_S_SHA1SUM);
    ASSERT("Testing Checker sha256sum result", sha256->getStringHashSum() == VALID_FILE_S_SHA256SUM);
}

// -------------- END Testing Checker ----------------------------------------------------



int main(int argc, char** argv) {
    // ---- File Factory
    RUN(test_file_factory_non_existent_file);
    RUN(test_file_factory_on_directories);
    RUN(test_file_factory_on_non_permissive_files);

    // ---- Hash Sums
    RUN(test_md5sum);
    RUN(test_sha1sum);
    RUN(test_sha256sum);
    RUN(test_sha384sum);
    RUN(test_sha512sum);

    // ---- File Size
    RUN(test_file_size);

    // ---- Hexa to Int converter
    RUN(test_hexa_to_int_converter);

    // ---- Lower and Upper case converter
    RUN(test_upper_case_converter);
    RUN(test_lower_case_converter);

    // ---- Checker
    RUN(test_checker_on_invalid_files);
    RUN(test_checker_on_valid_files);
    RUN(test_checker_on_valid_and_invalid_files);
    RUN(test_checker_hash_sum_calculation);

    return TEST_REPORT();
}
