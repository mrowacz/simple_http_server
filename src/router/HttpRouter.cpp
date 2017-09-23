//
// Created by mrowacz on 18.09.17.
//

#include "HttpRouter.h"

using namespace router;
using namespace std;

void Router::route(http::Request& req, http::Response& res)
{
    auto it = std::find_if(vec.begin(), vec.end(), [&](auto& entry) -> bool {
        return entry->match(req.url);
    });

    if (it != vec.end()) {
        it->get()->handle(req, res);
    } else throw router::router_exception(router_error::bad_request);
}

Router& Router::addPath(std::unique_ptr<HttpPath>& path)
{
    vec.push_back(std::move(path));
}
