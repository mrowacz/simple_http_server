// Copyright 2017 Lukasz Czerwinski mrowacz@gmail.com
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.


#include <regex>
#include "HttpRouter.h"
#include "Templates.h"

using namespace router;
using namespace std;

// you can declare following paths
// /api/objects/:id
// /api/objects/:id/list/:pattern
// look unit tests
HttpPath::HttpPath(std::string path)
{
    smatch m;
    regex catch_re("(:[^/]+)");
    string fieldRegex = "([a-zA-Z0-9]{1,100})";

    // set cb's
    for_each(methods.begin(), methods.end(), [](auto& entry) { entry = nullptr; });

    // create path catch regex
    pathRegExpr = regex(string("^" + regex_replace(path, catch_re, fieldRegex) + "$"));
    // create map of attributues
    int insertNum = 0;
    while (regex_search(path, m, catch_re)) {
        insertionOrderMap.insert({string(m[0]).substr(1), insertNum++});
        auto residue = m.suffix().str();
        if (residue.empty())
            break;
        else path = residue.substr(1);
    }
}

bool HttpPath::match(string& url)
{
    smatch pieces_match;

    attributeValues.clear();
    try {
        if (regex_match(url, pieces_match, pathRegExpr))
        {
            for (int i = 1; i < pieces_match.size(); i++)
                attributeValues.push_back(pieces_match[i]);
            return true;
        }
    } catch (regex_error& e) {
        cerr << "HttpPath::match" << e.code() << endl;
    }

    attributeValues.resize(insertionOrderMap.size());
    return false;
}

void HttpPath::handle(http::Request& req, http::Response& res)
{
    auto it = methodMap.find(req.method);
    if (it != methodMap.end()) {
        auto methodEnum = it->second;
        auto cb = methods[templates::toUType(methodEnum)];
        cb(req, res, this);
    }
}

std::string& HttpPath::operator[](std::string key)
{
    return attributeValues[insertionOrderMap[key]];
}

HttpPath& HttpPath::set(Method method, MethodCallback cb)
{
    methods[templates::toUType(method)] = cb;
}

const map<string, Method> HttpPath::methodMap = {
        { "GET", Method::GET },
        { "POST", Method::POST },
        { "PUT", Method::PUT },
        { "DELETE", Method::DELETE }
};