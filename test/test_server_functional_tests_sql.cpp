//
// Created by mrowacz on 17.09.17.
//

#include <memory>
#include <iostream>

#include "dao/Dao.h"
#include "http/Http.h"
#include "gtest/gtest.h"
#include "ServerError.h"
#include "router/HttpRouter.h"
#include "dao/SqliteStorage.h"
#include "restclient-cpp/restclient.h"

#include "task.h"

using namespace std;
using namespace http;
using namespace router;

class ServerTest_1 : public ::testing::Test {
protected:

    ServerTest_1() :
            router(),
            daoPtr(make_unique<SqliteStorage>()),
            path_1(make_unique<HttpPath>("/api/objects/:id")),
            path_2(make_unique<HttpPath>("/api/objects"))
    {
        server = make_unique<Server>([&](auto& req, auto& res) {
            try {
                router.route(req, res);
            } catch (router_exception& e) {
                ServerError::handleRouterException(e, res);
            }
        });

        path_1->set(router::Method::GET, [&](auto& req, auto& res, auto pHandler) -> void {
            string id = (*pHandler)["id"];

            try {
                string payload;
                string type;
                tie(type, payload) = daoPtr->get(id);
                res.headers.insert({"Content-Type", type});
                res.setStatus(http_status::HTTP_STATUS_OK);
                res.end(payload);
            } catch (dao::dao_exception& e) {
                ServerError::handleDaoException(e, res);
            }
        });

        path_1->set(router::Method::PUT, [&](auto& req, auto& res, auto pHandler) -> void {
            string id = (*pHandler)["id"];
            try {
                daoPtr->create(id, req.body.str(), req.headers.at("Content-Type"));
                res.setStatus(http_status::HTTP_STATUS_CREATED);
                res.end();
            } catch (dao::dao_exception& e) {

                ServerError::handleDaoException(e, res);
            }
        });

        path_1->set(router::Method::DELETE, [&](auto& req, auto& res, auto pHandler) -> void {
            string id = (*pHandler)["id"];

            try {
                daoPtr->del(id);
                res.setStatus(http_status::HTTP_STATUS_OK);
                res.end();
            } catch (dao::dao_exception& e) {
                ServerError::handleDaoException(e, res);
            }
        });

        auto path_2 = make_unique<HttpPath>("/api/objects");
        path_2->set(router::Method::GET, [&](auto& req, auto& res, auto pHandler) {
            string payload;
            try {
                payload = daoPtr->list();
                res.headers.insert({"Content-Type", "application/json"});
                res.setStatus(http_status::HTTP_STATUS_OK);
                res.end(payload);
            } catch (dao::dao_exception& e) {
                ServerError::handleDaoException(e, res);
            }
        });
        router.addPath(path_1);
        router.addPath(path_2);
        server->run();
    }

    virtual ~ServerTest_1() {
        server->stop();
        MAKE_VALGRIND_HAPPY();
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    std::string path_1_str;
    unique_ptr<Server> server;
    Router router;
    unique_ptr<dao::Dao> daoPtr;
    unique_ptr<HttpPath> path_1;
    unique_ptr<HttpPath> path_2;
};

TEST_F(ServerTest_1, CRUD_basic_tests_with_sql)
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