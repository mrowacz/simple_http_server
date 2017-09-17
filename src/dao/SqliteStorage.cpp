//
// Created by mrowacz on 16.09.17.
//

#include "SqliteStorage.h"
#include "../HttpLib.h"

void SqliteStorage::create(const string id, stringstream& ss,
            const string contentType, Response& resp)
{
    string data = ss.str();
    if (!lib::checkObjectName(id) || contentType.empty()) {
        resp.setStatus(http_status::HTTP_STATUS_BAD_REQUEST);
        return;
    }
    if (data.length() > dao::ONE_MB) {
        resp.setStatus(http_status::HTTP_STATUS_PAYLOAD_TOO_LARGE);
        return;
    }

    try {
        *db << "INSERT OR REPLACE INTO objects "
           "(name, type, payload) values (?, ?, ?);"
           << id
           << contentType
           << data;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
    resp.setStatus(http_status::HTTP_STATUS_CREATED);
}

void SqliteStorage::del(const string id, Response& resp)
{
    try {
        *db << "DELETE FROM objects WHERE name = ?;" << id;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
    resp.setStatus(http_status::HTTP_STATUS_OK);
}

void SqliteStorage::get(const string id, Response& resp)
{
    if (id.empty()) {
        list(resp);
        return;
    }
    if (!lib::checkObjectName(id)) {
        resp.setStatus(http_status::HTTP_STATUS_BAD_REQUEST);
        return;
    }
    // FIXME - fix issues with sqlite3 - see functional tests
    try {
        *db << "SELECT name, type, payload FROM objects WHERE name = ?;"
            << id
            >> [&](string name, string type, string payload) -> void {
                resp.body += payload;
                resp.setHeader(http::HTTP_HEADER_CONTENT_TYPE, type);
                resp.setStatus(http_status::HTTP_STATUS_OK);
                return;
            };

    } catch (exception &e) {
        cout << e.what() << endl;
    }
    resp.setStatus(http_status::HTTP_STATUS_NOT_FOUND);
}

void SqliteStorage::list(Response& resp)
{
    dao::genListHttpResponse(resp, [&](auto &vec) -> void {
        try {
            *db << "SELECT name FROM objects;"
                >> [&](string name) {
                    vec.push_back(name);
                };
        } catch (exception &e) {
            cout << e.what() << endl;
        }
    });
}

SqliteStorage::SqliteStorage()
{
    try {
        db = std::make_unique<sqlite::database>("database.db");

        *db << "CREATE TABLE IF NOT EXISTS objects ("
                "name text PRIMARY KEY NOT NULL,"
                "type text NOT NULL,"
                "payload text NOT NULL);";
    } catch(exception& e) {
        cout << e.what() << endl;
    }
}

SqliteStorage::~SqliteStorage()
{
}
