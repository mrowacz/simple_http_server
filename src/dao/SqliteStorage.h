//
// Created by mrowacz on 16.09.17.
//

#ifndef WP_INTERVIEW_SQLITESTORAGE_H
#define WP_INTERVIEW_SQLITESTORAGE_H

#include <sqlite_modern_cpp.h>
#include "Dao.h"

using namespace std;
using namespace http;

class SqliteStorage : public dao::Dao {
public:
    SqliteStorage();
    ~SqliteStorage();
    void create(const string id, stringstream& ss,
                const string contentType, Response& resp);
    void del(const string id, Response& resp);
    void get(const string id, Response& resp);
    void list(Response& resp);

private:
    std::unique_ptr<sqlite::database> db;
};

#endif //WP_INTERVIEW_SQLITESTORAGE_H
