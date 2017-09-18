//
// Created by mrowacz on 20.09.17.
//

#include <iostream>

#include "dao/SqliteStorage.h"
#include "gtest/gtest.h"

TEST(SqliteStorage, test_ordinary_usage) {
    SqliteStorage ss;
    ss.clear();

    string type;
    string payload;

    ss.create("file1", "payload_1", "text");
    ss.create("file2", "payload_2", "json");
    ss.create("file3", "payload_3", "pdf");

    tie (type, payload) = ss.get("file1");
    EXPECT_EQ("text", type);
    EXPECT_EQ("payload_1", payload);

    tie (type, payload) = ss.get("file2");
    EXPECT_EQ("json", type);
    EXPECT_EQ("payload_2", payload);

    tie (type, payload) = ss.get("file3");
    EXPECT_EQ("pdf", type);
    EXPECT_EQ("payload_3", payload);
}

TEST(SqliteStorage, test_exceptions_del) {
    SqliteStorage ss;
    ss.clear();

    string type;
    string payload;

    ss.create("object", "content", "text");
    tie(type, payload) = ss.get("object");

    EXPECT_EQ("text", type);
    EXPECT_EQ("content", payload);

    try {
        ss.del("object");
    } catch (exception &e)
    {
        cout << e.what() << endl;
        FAIL() << "NOKOOOO";
    }

    try {
        ss.get("object");
        FAIL() << "Expected not_found exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::not_found)
            FAIL() << "wrong exception" << endl;
    }

    try {
        ss.del("object");
        FAIL() << "Expected not_found exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::not_found)
            FAIL() << "wrong exception" << endl;
    }

    try {
        ss.create("two", string(dao::ONE_MB * 2, 'a'), "text");
        FAIL() << "Expected object_too_large exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::object_too_large)
            FAIL() << "wrong exception" << endl;
    }
}

TEST(SqliteStorage, test_object_sizes)
{
    SqliteStorage ss;
    ss.clear();

    try {
        ss.create("one", string(dao::ONE_MB, 'a'), "text");
        // 1kb size
        ss.create("two", string(1024, 'a'), "text");
        ss.create("three", string(128, 'a'), "text");

    } catch (dao::dao_exception& e) {
        FAIL() << "It shouldn't happen" << endl;
    }

    try {
        ss.create("four", string(dao::ONE_MB + 1, 'a'), "text");
        FAIL() << "Expected object_too_large exception" << endl;
    } catch (dao::dao_exception& e) {
        if (e.code() != dao::dao_error::object_too_large)
            FAIL() << "wrong exception" << endl;
    }
}

TEST(SqliteStorage, test_list) {
    SqliteStorage ss;

    ss.clear();
    EXPECT_EQ("[]", ss.list());

    ss.create("file1", "payload_1", "text");
    ss.create("file2", "payload_2", "json");
    ss.create("file3", "payload_3", "pdf");

    EXPECT_EQ("[\"file1\", \"file2\", \"file3\"]", ss.list());

    ss.clear();
    EXPECT_EQ("[]", ss.list());
}