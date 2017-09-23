//
// Created by mrowacz on 16.09.17.
//

#include <http_parser.h>
#include "SqliteStorage.h"
#include "LogEngine.h"

using namespace dao;
using namespace std;

void SqliteStorage::create(const std::string& id, const std::string& payload, const std::string& type)
{
    if (type.empty())
        throw dao_exception(dao_error::empty_content);

    if (payload.empty())
        throw dao_exception(dao_error::empty_payload);
    if (payload.length() > ONE_MB)
        throw dao_exception(dao_error::object_too_large);

    try {
        *db << "INSERT OR REPLACE INTO objects "
           "(name, type, payload) values (?, ?, ?);"
           << id
           << type
           << payload;
    } catch (exception &e) {
        throw dao_exception(dao_error::internal_error, e.what());
    }
}

void SqliteStorage::del(const string& id)
{
    try {
        // if doesn't exists throw exception
        auto data = get(id);
        *db << "DELETE FROM objects WHERE name = ?;" << id;
    } catch (sqlite::sqlite_exception &e) {
        throw dao_exception(dao_error::internal_error, e.what());
    }
}

std::tuple<std::string, std::string> SqliteStorage::get(const string& id)
{
    tuple<string,string> tp;
    if (id.empty())
        return make_tuple<string, string>(list(), "");

    try {
        *db << "SELECT name, type, payload FROM objects WHERE name = ?;"
            << id
            >> [&](string name, string type, string payload) -> void {
                INFO() << "Read " << id << " type: " <<
                                              type << " " << payload.size();

                tp = make_tuple<>(type, payload);
            };
    } catch (exception& e) {
        throw dao_exception(dao_error::internal_error, e.what());
    }

    if (std::get<0>(tp).empty() || std::get<1>(tp).empty())
        throw dao_exception(dao_error::not_found);

    return tp;
};

std::string SqliteStorage::list()
{
    return dao::genListStr([&](auto &vec) -> void {
        try {
            *db << "SELECT name FROM objects;"
                >> [&](string name) {
                    vec.push_back(name);
                };

        } catch (exception &e) {
            throw dao_exception(dao_error::internal_error, e.what());
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
    } catch(sqlite::sqlite_exception& e) {
        cerr << e.what() << e.get_code() <<  endl;
    }
}

void SqliteStorage::clear()
{
    try {
        *db << "DELETE FROM objects;";
    } catch (sqlite::sqlite_exception& e) {
        throw dao_exception(dao_error::internal_error, e.what());
    }
}

SqliteStorage::~SqliteStorage()
{

}

