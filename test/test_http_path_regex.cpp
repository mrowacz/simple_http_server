//
// Created by l.czerwinski on 9/20/17.
//

#include <map>
#include <regex>
#include <iostream>
#include "gtest/gtest.h"

#include "router/HttpRouter.h"

using namespace std;

class HttpPathTest : public router::HttpPath {
public:
    HttpPathTest(std::string in) : router::HttpPath(in)
    {

    }

    bool test(string url) {
        return match(url);
    }
};


TEST(HttpPathRegex, basic_patterns) {
    HttpPathTest hp("/api/objects/:id");

    EXPECT_EQ(true, hp.test("/api/objects/abc"));
    EXPECT_EQ("abc", hp["id"]);

    EXPECT_EQ(false, hp.test("/api/objects/_abc"));
    EXPECT_EQ(false, hp.test("/api/objects/abc_abc"));
    EXPECT_EQ(false, hp.test("/api/objects"));
    EXPECT_EQ(true, hp.test("/api/objects/" + string(100, 'a')));
    EXPECT_EQ(false, hp.test("/api/objects/" + string(0, 'a')));
    EXPECT_EQ(false, hp.test("/api/objects/" + string(101, 'a')));

    HttpPathTest hp2("/api/objects");
    EXPECT_EQ(false, hp2.test("/api/objects/abc"));
    EXPECT_EQ("", hp["id"]);
    EXPECT_EQ("", hp["unknown_key"]);
    EXPECT_EQ(false, hp2.test("/api/objects/_abc"));
    EXPECT_EQ(false, hp2.test("/api/objects/abc_abc"));
    EXPECT_EQ(true, hp2.test("/api/objects"));
    EXPECT_EQ(false, hp2.test("/api/objects/" + string(100, 'a')));
    EXPECT_EQ(false, hp2.test("/api/objects/" + string(0, 'a')));
    EXPECT_EQ(false, hp2.test("/api/objects/" + string(101, 'a')));
    EXPECT_EQ(false, hp2.test("/api/objectsdsjgfjds" + string(101, 'a')));
}

TEST(HttpPathRegex, advanced_patterns) {
    HttpPathTest hp("/api/objects/:id/types/:type");

    EXPECT_EQ(true, hp.test("/api/objects/abc/types/json"));
    EXPECT_EQ("abc", hp["id"]);
    EXPECT_EQ("json", hp["type"]);

    EXPECT_EQ(false, hp.test("/api/objects/abc/types/"));
    EXPECT_EQ(false, hp.test("/api/objects/abc/types/_json"));
    EXPECT_EQ(false, hp.test("/api/objects/abc"));
}