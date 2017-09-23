//
// Created by mrowacz on 18.09.17.
//

#include "gtest/gtest.h"
#include "LogEngine.h"


int main(int argc, char **argv) {
    slog::init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

