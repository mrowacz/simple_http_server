//
// Created by l.czerwinski on 9/14/17.
//

#ifndef WP_INTERVIEW_TEMPORARYSTORAGE_H
#define WP_INTERVIEW_TEMPORARYSTORAGE_H

#include <map>
#include "Dao.h"

class EphemeralStorage : public dao::Dao {
public:
    ~EphemeralStorage() {}
    void create(const std::string& id, const std::string& payload, const std::string& type);
    void del(const std::string& id);
    std::tuple<std::string, std::string> get(const std::string& id);
    std::string list();
    void clear();
protected:
    std::map<std::string, std::tuple<std::string, std::string>> mp;
    std::map<std::string, std::tuple<std::string, std::string>>::iterator findId(std::string id);
};

#endif //WP_INTERVIEW_TEMPORARYSTORAGE_H
