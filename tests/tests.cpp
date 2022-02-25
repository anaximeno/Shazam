#include <iostream>
#include <memory>

#include "tinytest.h"
#include "../include/shazam.hpp"


// -------------- Testing File Factory -------------------------------------------------

void test_file_factory_non_existent_file(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("i_am_a_inexistent_file.txt");

    ASSERT(
        "Testing FileFactory for a non existent file",
        file->status() == shazam::EFileValidStatus::NON_EXISTENT
    );

    ASSERT_EQUALS(file->isValid(), false);
}

void test_file_factory_on_directories(void) {
    shazam::FileFactory factory;
    std::shared_ptr<shazam::File> file = factory.create("./");

    ASSERT(
        "Testing FileFactory on directories",
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



int main(int argc, char** argv) {
    RUN(test_file_factory_non_existent_file);
    RUN(test_file_factory_on_directories);
    RUN(test_file_factory_on_non_permissive_files);
    return TEST_REPORT();
}