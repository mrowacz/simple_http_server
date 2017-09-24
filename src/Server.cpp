// Copyright 2017 Lukasz Czerwinski mrowacz@gmail.com
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.

#include <signal.h>
#include <iostream>

#include "Options.h"
#include "LogEngine.h"
#include "http/Http.h"
#include "Templates.h"
#include "ServerError.h"
#include "router/HttpRouter.h"
#include "dao/SqliteStorage.h"
#include "dao/EphemeralStorage.h"

using std::string;
using std::unique_ptr;
using std::make_unique;
using http::Server;
using router::Router;
using router::HttpPath;

volatile bool signal_flag = false;
void sigint(int a) {
    signal_flag = true;
}

int main(int argc, char *argv[]) {
    int port = 8080;
    bool quietFlag = false;
    server_options::dbtype dbMode("sqlite3");
    unique_ptr<dao::Dao> daoPtr = nullptr;

    slog::init();
    std::tie(port, dbMode, quietFlag) = server_options::process_program_options(argc, argv);

    switch (dbMode.md) {
        case server_options::dbtype::mode::MEMORY:
            daoPtr = make_unique<EphemeralStorage>();
            break;
        case server_options::dbtype::mode::SQLITE3:
            daoPtr = make_unique<SqliteStorage>();
            break;
        default:
            throw std::invalid_argument("Wrong db mode!");
            break;
    }

    Router router;
    auto path_1 = make_unique<HttpPath>("/api/objects/:id");

    path_1->set(router::Method::GET,
                [&](auto& req, auto& res, auto pHandler) -> void {
        string id = (*pHandler)["id"];
        INFO() << "id: " << id;

        string payload;
        string type;
        tie(type, payload) = daoPtr->get(id);
        INFO() << "read from db " << id
               << " " << payload.size() << " b"
               << " " << type;
        res.headers.insert({"Content-Type", type});
        res.setStatus(http_status::HTTP_STATUS_OK);
        res.end(payload);
    });

    path_1->set(router::Method::PUT,
                [&](auto& req, auto& res, auto pHandler) -> void {
        string id = (*pHandler)["id"];

        INFO() << "PUT id: " << id;
        daoPtr->create(id, req.body.str(), req.headers.at("Content-Type"));
        res.setStatus(http_status::HTTP_STATUS_CREATED);
        res.end();
    });

    path_1->set(router::Method::DELETE,
                [&](auto& req, auto& res, auto pHandler) -> void {
        string id = (*pHandler)["id"];
        INFO() << "DELETE id: " << id;
        daoPtr->del(id);
        res.setStatus(http_status::HTTP_STATUS_OK);
        res.end();
    });

    auto path_2 = make_unique<HttpPath>("/api/objects");
    path_2->set(router::Method::GET,
                [&](auto& req, auto& res, auto pHandler) {
        string payload;
        payload = daoPtr->list();
        res.headers.insert({"Content-Type", "application/json"});
        res.setStatus(http_status::HTTP_STATUS_OK);
        res.end(payload);
    });

    // server setup and run
    router.addPath(path_1);
    router.addPath(path_2);
    Server server([&](auto& req, auto& res) {
        try {
            INFO() << req.method << " " << req.url;
            router.route(req, res);
        } catch (router::router_exception& e) {
            FATAL() << "router_exception "
                    << templates::toUType(e.code());
            ServerError::handleRouterException(e, res);
        }
    });

    // catch ctrl + c
    signal(SIGINT, sigint);

    // start server
    server.run();
    while (!signal_flag) {}
    server.stop();

    return 0;
}
