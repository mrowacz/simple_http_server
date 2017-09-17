//
// Created by l.czerwinski on 9/14/17.
//

#include <regex>
#include <iostream>

#include "http_parser.h"
#include "../HttpLib.h"
#include "EphemeralStorage.h"

using namespace dao;
using namespace std;

void EphemeralStorage::create(const string id, stringstream& ss,
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
    if (mp.count(id) == 1)
        del(id, resp);
    mp.insert(std::pair<string, tuple<string, string>>
                      (id, make_tuple<string, string>(string(contentType), ss.str())));
    resp.setStatus(http_status::HTTP_STATUS_CREATED);
}

void EphemeralStorage::del(const string id, Response& resp)
{
    auto it = findId(id);
    if (it != mp.end())
        mp.erase(it);
    resp.setStatus(http_status::HTTP_STATUS_OK);
}

void EphemeralStorage::get(const string id, Response& resp)
{
    if (id.empty()) {
        list(resp);
        return;
    }
    if (!lib::checkObjectName(id)) {
        resp.setStatus(http_status::HTTP_STATUS_BAD_REQUEST);
        return;
    }

    auto it = findId(id);
    if (it != mp.end())
    {
        string cntType;
        string payload;

        tie(cntType, payload) = it->second;
        resp.body += payload;
        resp.setHeader(http::HTTP_HEADER_CONTENT_TYPE, cntType);
        resp.setStatus(http_status::HTTP_STATUS_OK);
        return;
    }

    resp.setStatus(http_status::HTTP_STATUS_NOT_FOUND);
}

map<string, tuple<string, string>>::iterator EphemeralStorage::findId(string id)
{
    return find_if(mp.begin(), mp.end(), [&](auto &ref) -> bool {
        if (ref.first == id)
            return true;
        return false;
    });
}

void EphemeralStorage::list(Response &resp)
{
    dao::genListHttpResponse(resp, [&](auto& vec) -> void {
        for_each(mp.begin(), mp.end(), [&](auto& it)->void {
            vec.push_back(it.first);
        });
    });
}