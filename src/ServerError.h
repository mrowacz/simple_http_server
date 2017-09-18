//
// Created by l.czerwinski on 9/21/17.
//

#ifndef WP_INTERVIEW_SERVERERROR_H
#define WP_INTERVIEW_SERVERERROR_H

#include <iostream>
#include <exception>

#include "dao/Dao.h"
#include "http/Http.h"
#include "router/HttpRouter.h"

namespace ServerError {
    void handleDaoException(dao::dao_exception& e, http::Response& resp)
    {
        switch(e.code()) {
            case dao::dao_error::not_found:
                resp.setStatus(http_status::HTTP_STATUS_NOT_FOUND);
                break;
            case dao::dao_error::object_too_large:
                resp.setStatus(http_status::HTTP_STATUS_PAYLOAD_TOO_LARGE);
                break;
            case dao::dao_error::empty_content:
                resp.setStatus(http_status::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);
                break;
            case dao::dao_error::empty_payload:
                resp.setStatus(http_status::HTTP_STATUS_NO_CONTENT);
                break;
            case dao::dao_error::internal_error:
                resp.setStatus(http_status::HTTP_STATUS_INTERNAL_SERVER_ERROR);
                break;
            default:
                throw std::runtime_error("Unhandled server exception!");
        }
        resp.end();
    }

    void handleRouterException(router::router_exception& e, http::Response& resp)
    {
        switch(e.code()) {
            case router::router_error::bad_request:
                resp.setStatus(http_status::HTTP_STATUS_BAD_REQUEST);
                break;
            default:
                throw std::runtime_error("Unhandled router exception");
        }
        resp.end();
    }
};
#endif //WP_INTERVIEW_SERVERERROR_H
