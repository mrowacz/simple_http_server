//
// Created by mrowacz on 17.09.17.
//

#include <memory>
#include <iostream>

#include "HttpLib.h"
#include "dao/Dao.h"
#include "http/Http.h"
#include "gtest/gtest.h"
#include "dao/EphemeralStorage.h"
#include "restclient-cpp/restclient.h"

using namespace std;
using namespace http;

class ServerTest : public ::testing::Test {
protected:

    ServerTest() :
            server(),
            daoPtr(make_unique<EphemeralStorage>()),
            path_1(make_unique<RestPath>()),
            path_1_str("/api/objects/")
    {
        path_1->setPathStr(path_1_str)
                ->addMethod(http::RestMethod::PUT, [this](auto &req,  auto &res) -> void {
                    daoPtr->create(
                            RestPath::objectName(path_1_str, req.url),
                            req.body,
                            lib::getContentHeader(req.headers),
                            res
                    );
                })
                ->addMethod(http::RestMethod::GET, [this](auto &req,  auto &res) -> void {
                    daoPtr->get(
                            RestPath::objectName(path_1_str, req.url),
                            res
                    );
                })
                ->addMethod(http::RestMethod::DELETE, [this](auto &req,  auto &res) -> void {
                    daoPtr->del(
                            RestPath::objectName(path_1_str, req.url),
                            res
                    );
                });

        server
                .port(8080)
                .enableStdLogs(false)
                .registerPath(path_1)
                .run();
    }

    virtual ~ServerTest() {
        server.stop();
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    std::string path_1_str;
    Server server;
    unique_ptr<dao::Dao> daoPtr;
    unique_ptr<RestPath> path_1;
};

TEST_F(ServerTest, CRUD_basic_tests)
{
    std::string str("{\"foo\": \"bla\"}");
    std::string url("http://localhost:8080/api/objects/abc");

    // first make standard query
    auto r = RestClient::put(
            url,
            "text/json",
            str
    );
    EXPECT_EQ(201, r.code);

    r = RestClient::get(url);
    EXPECT_EQ(200, r.code);
    EXPECT_EQ(str, r.body);

    // make too big object
    r = RestClient::put(
            url,
            "text/json",
            std::string(2*1024*1024, 'a')
    );
    EXPECT_EQ(413, r.code);

    // get wrong object and not existing object
    std::string url_2("http://localhost:8080/api/objects/abc--");
    std::string url_3("http://localhost:8080/api/objects/niema");

    r = RestClient::get(url_2);
    EXPECT_EQ(400, r.code);

    r = RestClient::get(url_3);
    EXPECT_EQ(404, r.code);

    // test delete
    r = RestClient::del(url);
    r = RestClient::get(url);

    EXPECT_EQ(404, r.code);

    // test listing
    std::string output = "[\"key1\", \"key2\", \"key3\"]";
    RestClient::put("http://localhost:8080/api/objects/key1", "text/json", str);
    RestClient::put("http://localhost:8080/api/objects/key2", "text/json", str);
    RestClient::put("http://localhost:8080/api/objects/key3", "text/json", str);
    r = RestClient::get("http://localhost:8080/api/objects");
    EXPECT_EQ(output, r.body);
}