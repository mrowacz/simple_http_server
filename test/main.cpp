//
// Created by mrowacz on 18.09.17.
//

#include "gtest/gtest.h"
#include "LogEngine.h"


int main(int argc, char **argv) {
    // display only server version
    slog::init();
    slog::disable();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

