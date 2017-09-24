//
// Created by mrowacz on 20.09.17.
//

#include <iostream>

#include "dao/EphemeralStorage.h"
#include "gtest/gtest.h"

using namespace std;

TEST(EphemeralStorage, test_ordinary_usage) {
    EphemeralStorage ep;

    string type;
    string payload;

    ep.create("file1", "payload_1", "text");
    ep.create("file2", "payload_2", "json");
    ep.create("file3", "payload_3", "pdf");

    tie (type, payload) = ep.get("file1");
    EXPECT_EQ("text", type);
    EXPECT_EQ("payload_1", payload);

    tie (type, payload) = ep.get("file2");
    EXPECT_EQ("json", type);
    EXPECT_EQ("payload_2", payload);

    tie (type, payload) = ep.get("file3");
    EXPECT_EQ("pdf", type);
    EXPECT_EQ("payload_3", payload);
}

TEST(EphemeralStorage, test_exceptions_del) {
    EphemeralStorage ep;

    string type;
    string payload;

    ep.create("object", "content", "text");
    tie(type, payload) = ep.get("object");

    EXPECT_EQ("text", type);
    EXPECT_EQ("content", payload);

    ep.del("object");

    try {
        ep.get("object");
        FAIL() << "Expected not_found exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::not_found)
            FAIL() << "wrong exception" << endl;
    }

    try {
        ep.del("object");
        FAIL() << "Expected not_found exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::not_found)
            FAIL() << "wrong exception" << endl;
    }

    try {
        ep.create("two", string(dao::ONE_MB * 2, 'a'), "text");
        FAIL() << "Expected object_too_large exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::object_too_large)
            FAIL() << "wrong exception" << endl;
    }

    try {
        ep.create("two", "", "text");
        FAIL() << "Expected empty payload exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::empty_payload)
            FAIL() << "wrong exception" << endl;
    }
}

TEST(EphemeralStorage, test_object_sizes)
{
    EphemeralStorage ep;

    try {
        ep.create("one", string(dao::ONE_MB, 'a'), "text");
        // 1kb size
        ep.create("two", string(1024, 'a'), "text");
        ep.create("three", string(128, 'a'), "text");

    } catch (dao::dao_exception& e) {
        FAIL() << "It shouldn't happen" << endl;
    }

    try {
        ep.create("four", string(dao::ONE_MB + 1, 'a'), "text");
        FAIL() << "Expected object_too_large exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::object_too_large)
            FAIL() << "wrong exception" << endl;
    }
}

TEST(EphemeralStorage, test_list) {
    EphemeralStorage ep;

    EXPECT_EQ("[]", ep.list());

    ep.create("file1", "payload_1", "text");
    ep.create("file2", "payload_2", "json");
    ep.create("file3", "payload_3", "pdf");

    EXPECT_EQ("[\"file1\", \"file2\", \"file3\"]", ep.list());

    ep.clear();
    EXPECT_EQ("[]", ep.list());
}