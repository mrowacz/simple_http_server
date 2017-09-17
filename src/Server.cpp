#include <signal.h>
#include <iostream>

#include "HttpLib.h"
#include "http/Http.h"
#include "dao/SqliteStorage.h"
#include "dao/EphemeralStorage.h"
#include "Options.h"

using namespace http;

volatile bool signal_flag = false;
void sigint(int a)
{
    signal_flag = true;
}

int main(int argc, char *argv[])
{
    int port = 8080;
    bool quietFlag = false;
    server_options::dbtype dbMode;

    auto path_1 = make_unique<RestPath>();
    auto path_1_str = string("/api/objects/");
    unique_ptr<dao::Dao> daoPtr = nullptr;

    signal(SIGINT, sigint);
    std::tie(port, dbMode, quietFlag) = server_options::process_program_options(argc, argv);

    // TODO add exception!!!
    switch (dbMode.md) {
        case server_options::dbtype::mode::MEMORY:
            daoPtr = make_unique<EphemeralStorage>();
            break;
        case server_options::dbtype::mode::SQLITE3:
            daoPtr = make_unique<SqliteStorage>();
            break;
        default:
            throw std::invalid_argument("Wrong choosen db mode!");
            break;
    }
    Server server;

    path_1->setPathStr(path_1_str)
            ->addMethod(http::RestMethod::PUT, [&](auto &req,  auto &res) -> void {
                daoPtr->create(
                        RestPath::objectName(path_1_str, req.url),
                        req.body,
                        lib::getContentHeader(req.headers),
                        res
                );
            })
            ->addMethod(http::RestMethod::GET, [&](auto &req,  auto &res) -> void {
                daoPtr->get(
                        RestPath::objectName(path_1_str, req.url),
                        res
                );
            })
            ->addMethod(http::RestMethod::DELETE, [&](auto &req,  auto &res) -> void {
                daoPtr->del(
                        RestPath::objectName(path_1_str, req.url),
                        res
                );
            });

    server
            .port(port)
            .enableStdLogs(!quietFlag)
            .registerPath(path_1)
            .run();

    while(!signal_flag) {}
    server.stop();

    return 0;
}