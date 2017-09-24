// Copyright 2017 Lukasz Czerwinski mrowacz@gmail.com
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.


#include "dao/Dao.h"
#include "LogEngine.h"
#include "HttpRouter.h"
#include "ServerError.h"

using namespace router;
using namespace std;

void Router::route(http::Request& req, http::Response& res)
{
    auto it = std::find_if(vec.begin(), vec.end(), [&](auto& entry) -> bool {
        return entry->match(req.url);
    });

    if (it != vec.end()) {
        try {
            it->get()->handle(req, res);
        } catch (dao::dao_exception& e) {
            WARN() << "dao_exception "
                   << templates::toUType(e.code());
            ServerError::handleDaoException(e, res);
        }
    } else throw router::router_exception(router_error::bad_request);
}

Router& Router::addPath(std::unique_ptr<HttpPath>& path)
{
    vec.push_back(std::move(path));
}
