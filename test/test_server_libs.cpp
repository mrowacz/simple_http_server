//
// Created by mrowacz on 17.09.17.
//

#include <map>
#include <iostream>
#include "gtest/gtest.h"

#include "http/Http.h"
#include "HttpLib.h"

TEST(LibMethods, CheckObjectName_basic)
{
    EXPECT_EQ(true, lib::checkObjectName("abc"));
    EXPECT_EQ(false, lib::checkObjectName("_abc"));
    EXPECT_EQ(false, lib::checkObjectName("wrong_name"));
}

TEST(LibMethods, CheckObjectName_too_long)
{
    EXPECT_EQ(false, lib::checkObjectName(std::string(101, 'a')));
}

TEST(LibMethods, CheckObjectName_equal_100)
{
    EXPECT_EQ(true, lib::checkObjectName(std::string(100, 'a')));
}

TEST(LibMethods, CheckObjectName_less_100)
{
    EXPECT_EQ(true, lib::checkObjectName(std::string(99, 'a')));
}

TEST(LibMethods, CheckObjectName_empty)
{
    EXPECT_EQ(false, lib::checkObjectName(std::string("")));
}

TEST(LibMethods, getContentHeader_basic)
{
    std::map<const std::string, const std::string> mp;
    mp.insert(std::pair<const std::string, const std::string>("Content-Type", "text/plain"));
    EXPECT_EQ("text/plain", lib::getContentHeader(mp));
}

TEST(LibMethods, getContentHeader_empty)
{
    std::map<const std::string, const std::string> mp;
    EXPECT_EQ("", lib::getContentHeader(mp));
}

TEST(RestPathMethods, objectName_basic)
{
    std::string str = http::RestPath::objectName("/api/objects/", "/api/objects/abc");
    EXPECT_EQ("abc", str);
}
