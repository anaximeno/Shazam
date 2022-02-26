#include <iostream>
#include <memory>

#include "./include/tinytest.hpp"
#include "../include/shazam.hpp"


// -------------- Testing File Factory -------------------------------------------------

void test_file_factory_non_existent_file(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("i_am_a_inexistent_file.txt");

    ASSERT("Testing FileFactory for a non existent file",
        file->status() == shazam::EFileValidStatus::NON_EXISTENT
    );

    ASSERT_EQUALS(file->isValid(), false);
}

void test_file_factory_on_directories(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("./");

    ASSERT("Testing FileFactory on directories",
        file->status() == shazam::EFileValidStatus::IS_DIRECTORY
    );

    ASSERT_EQUALS(file->isValid(), false);
}

void test_file_factory_on_non_permissive_files(void) {
    shazam::FileFactory factory;

    std::system("touch .filefortest.shazam.tmp && chmod -r .filefortest.shazam.tmp");

    std::shared_ptr<shazam::File> file = factory.create(".filefortest.shazam.tmp");

    ASSERT(
        "Testing FileFactory for non permissive files",
        file->status() == shazam::EFileValidStatus::NOT_PERMISSIVE
    );

    std::system("rm .filefortest.shazam.tmp");

    ASSERT_EQUALS(file->isValid(), false);
}


// -------------- END Testing File Factory ---------------------------------------------



// -------------- Testing Hash Sums ----------------------------------------------------

#define FILE_FOR_HASHING_NAME "static_file.txt"


void test_md5sum() {
    const string MD5SUM = "e2c1267c569e72c2365d6fab0a5167c0";

    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    auto hash = hfactory.createFileHash("MD5", ffactory.create(FILE_FOR_HASHING_NAME));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing MD5 hash sum", MD5SUM == CALCULATED);
}

void test_sha1sum() {
    const string SHA1SUM = "5652d27b9769b7e1e11af27915a421272429cade";

    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    auto hash = hfactory.createFileHash("SHA1", ffactory.create(FILE_FOR_HASHING_NAME));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA1 hash sum", SHA1SUM == CALCULATED);
}

void test_sha256sum() {
    const string SHA256SUM = "5e093ff408286e5d57c1b4bbb0f4aad439ec070ed42796946e41bea3ac2286be";

    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    auto hash = hfactory.createFileHash("SHA256", ffactory.create(FILE_FOR_HASHING_NAME));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA256 hash sum", SHA256SUM == CALCULATED);
}

void test_sha384sum() {
    const string SHA384SUM = "2bad85d5481290e1c756504882d6eff9a5915d17df625955d2d485a120ccfda1736722cb0a097ae6607560c52cf1d88e";

    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    auto hash = hfactory.createFileHash("SHA384", ffactory.create(FILE_FOR_HASHING_NAME));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA384 hash sum", SHA384SUM == CALCULATED);
}

void test_sha512sum() {
    const string SHA512SUM = "eeabc1e01e9572a3cba059289486caedd372ef8bfc357c6e3b90290896c3f7eb3e940a3d5d88349533133d5c4f89482d3a9648d55228beef0c45b269f814faa9";

    shazam::HashFactory hfactory;
    shazam::FileFactory ffactory;

    auto hash = hfactory.createFileHash("SHA512", ffactory.create(FILE_FOR_HASHING_NAME));
    const string CALCULATED = hash->getStringHashSum();

    ASSERT("Testing SHA512 hash sum", SHA512SUM == CALCULATED);
}

// -------------- END Hash Sums --------------------------------------------------------


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

    return TEST_REPORT();
}
