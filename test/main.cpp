//
// Created by mrowacz on 18.09.17.
//

#include "gtest/gtest.h"
#include "LogEngine.h"

namespace logging = boost::log;
using namespace logging::trivial;

int main(int argc, char **argv) {
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::fatal
            );
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

