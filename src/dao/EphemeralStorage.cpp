// Copyright 2017 Lukasz Czerwinski mrowacz@gmail.com
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.

#include <iostream>
#include <algorithm>

#include "EphemeralStorage.h"

using namespace dao;
using namespace std;

void EphemeralStorage::create(const std::string& id, const std::string& payload, const std::string& type)
{
    if (type.empty())
        throw dao_exception(dao_error::empty_content);

    if (payload.empty())
        throw dao_exception(dao_error::empty_payload);
    if (payload.length() > ONE_MB)
        throw dao_exception(dao_error::object_too_large);

    // if exists -- overwrite object
    if (mp.count(id) == 1)
        del(id);

    mp.insert(std::pair<string, tuple<string, string>>
                      (id, make_tuple<string, string>(string(type), string(payload))));
}

void EphemeralStorage::del(const string& id)
{
    auto it = findId(id);
    if (it != mp.end())
        mp.erase(it);
    else throw dao_exception(dao_error::not_found);
}

std::tuple<std::string, std::string> EphemeralStorage::get(const string& id)
{
    if (id.empty())
        return make_tuple<string, string>(list(), "");

    auto it = findId(id);
    if (it != mp.end())
    {
        return it->second;
    } else throw dao_exception(dao_error::not_found);
}

map<string, tuple<string, string>>::iterator EphemeralStorage::findId(string id)
{
    return find_if(mp.begin(), mp.end(), [&](auto &ref) -> bool {
        if (ref.first == id)
            return true;
        return false;
    });
}

std::string EphemeralStorage::list()
{
    return dao::genListStr([&](auto& vec) -> void {
        for_each(mp.begin(), mp.end(), [&](auto& it)->void {
            vec.push_back(it.first);
        });
    });
}

void EphemeralStorage::clear()
{
    mp.clear();
}
