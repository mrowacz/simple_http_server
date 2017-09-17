//
// Created by l.czerwinski on 9/14/17.
//

#ifndef WP_INTERVIEW_TEMPORARYSTORAGE_H
#define WP_INTERVIEW_TEMPORARYSTORAGE_H

#include "Dao.h"

using namespace std;
using namespace http;

class EphemeralStorage : public dao::Dao {
public:
    ~EphemeralStorage() {}
    void create(const string id, stringstream& ss,
                const string contentType, Response& resp);
    void del(const string id, Response& resp);
    void get(const string id, Response& resp);
    void list(Response& resp);

private:
    map<string, tuple<string, string>> mp;
    map<string, tuple<string, string>>::iterator findId(string id);
};

#endif //WP_INTERVIEW_TEMPORARYSTORAGE_H
