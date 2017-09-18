//
// Created by mrowacz on 18.09.17.
//

#ifndef WP_INTERVIEW_HTTPROUTER_H
#define WP_INTERVIEW_HTTPROUTER_H

#include <regex>
#include <string>
#include <memory>
#include <exception>
#include "http/Http.h"
#include "Templates.h"

namespace router {

    enum class Method : std::uint8_t { GET, POST, PUT, DELETE, METHODS_COUNT };

    class HttpPath {
        friend class Router;
    protected:
        // ======= methods =======
        // typedef method callback
        typedef std::function<void (http::Request& req, http::Response& res, HttpPath *ptr)> MethodCallback;
        // check does url matches path
        bool match(std::string& url);
        void handle(http::Request& req, http::Response& res);
        // ======= variables =======
        // map method for easier navigation through method string
        static const std::map<std::string, Method> methodMap;
        // callbacks array container
        static constexpr int METHODS_ARRAY_SIZE = templates::toUType(Method::METHODS_COUNT);
        std::array<MethodCallback, METHODS_ARRAY_SIZE> methods;
        // regex vars
        // path regex
        std::regex pathRegExpr;
        // attribute tags handling vars
        std::map<const std::string, int> insertionOrderMap;
        std::vector<std::string> attributeValues;
    public:
        HttpPath() = delete;
        HttpPath(std::string path);
        HttpPath& set(Method method, MethodCallback cb);
        std::string& operator[](std::string key);
    };

    class Router {
    public:
        Router() {};
        void route(http::Request& req, http::Response& res);
        Router& addPath(std::unique_ptr<HttpPath>& path);
    private:
        std::vector<std::unique_ptr<HttpPath>> vec;
    };

    enum class router_error : std::uint32_t {
        bad_request
    };

    class router_exception : public std::exception {
    public:
        router_exception(router_error error) : err(error) {}
        router_error code() { return err; }
    private:
        router_error err;
    };

}


#endif //WP_INTERVIEW_HTTPROUTER_H
