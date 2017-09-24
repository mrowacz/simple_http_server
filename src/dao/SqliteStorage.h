//
// Created by mrowacz on 16.09.17.
//

#ifndef WP_INTERVIEW_SQLITESTORAGE_H
#define WP_INTERVIEW_SQLITESTORAGE_H

#include <memory>
#include <string>
#include <sqlite_modern_cpp.h>

#include "Dao.h"

class SqliteStorage : public dao::Dao {
public:
    SqliteStorage();
    ~SqliteStorage();
    void create(const std::string& id, const std::string& payload, const std::string& type);
    void del(const std::string& id);
    std::tuple<std::string, std::string> get(const std::string& id);
    std::string list();
    void clear();

protected:
    std::unique_ptr<sqlite::database> db;
};

#endif //WP_INTERVIEW_SQLITESTORAGE_H
