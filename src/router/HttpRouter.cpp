//
// Created by mrowacz on 18.09.17.
//

#include "HttpRouter.h"
#include "LogEngine.h"

using namespace router;
using namespace std;

namespace logging = boost::log;
using namespace logging::trivial;

void Router::route(http::Request& req, http::Response& res)
{
    auto it = std::find_if(vec.begin(), vec.end(), [&](auto& entry) -> bool {
        BOOST_LOG_SEV(slog::lg, trace) << "route(): " << req.url;
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
