#ifndef NODEUV_HTTP_H
#define NODEUV_HTTP_H

#include <map>
#include <mutex>
#include <array>
#include <tuple>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <unistd.h>

#include "uv.h"
#include "uri.h"
#include "http_parser.h"

#define MAX_WRITE_HANDLES 1000

#define ASSERT_STATUS(status, msg) \
  if (status != 0) { \
    std::cerr << msg << ": " << uv_err_name(status); \
    exit(1); \
  }

namespace http {

    using namespace std;

    namespace logging = boost::log;
    namespace src = boost::log::sources;
    namespace sinks = boost::log::sinks;
    namespace keywords = boost::log::keywords;
    using namespace logging::trivial;

    static src::severity_logger< severity_level > lg;

    template <class Type> class Buffer;
    template <class Type> class IStream;

    class Request;
    class Response;
    class Server;
    class Context;
    class RestPath;

    extern const string CRLF;
    extern void free_context (uv_handle_t*);

    template <class Type>
    extern void attachEvents(Type* instance, http_parser_settings& settings);

    template <class Type>
    class Buffer : public stringbuf {

        friend class Request;
        friend class Response;

        Type* stream;

        Buffer<Type> (ostream& str) {};
        ~Buffer () {};
        virtual int sync () {

            string out = str();
            std::ostringstream buf;
            buf << out;
            out = buf.str();
            stream->writeOrEnd(out, true);
            buf.flush();
            str("");
            return 0;
        }
    };


    template <class Type>
    class IStream : virtual public ostream {

    public:
        IStream () { };
    };


    constexpr auto HTTP_HEADER_CONTENT_TYPE = "Content-Type";

    class Request {
    public:
        string url;
        string method;
        string status_code;
        stringstream body;
        map<const string, string> headers;
        Request() {}
        ~Request() {}
    };


    class Response : public IStream<Response> {

        friend class Buffer<class Response>;
        friend class Server;

        stringstream stream;
        Buffer<Response> buffer;

        void writeOrEnd(string, bool);

        int write_count = 0;
        bool writtenOrEnded = false;
        bool ended = false;
        bool headersSet = false;
        bool statusSet = false;
        bool contentLengthSet = false;

    public:

        http_parser parser;

        int statusCode = 200;
        string body = "";
        string statusAdjective = "OK";
        map<const string, const string> headers;

        void setHeader (const string, const string);
        void setStatus (int);
        void setStatus (int, string);

        void write (string);
        void end (string);
        void end ();

        Response() :
                IStream(),
                ostream(&buffer),
                buffer(stream) {
            buffer.stream = this;
        }
        ~Response() {
        }
    };

    /*
      // @TODO
      // Maybe have each op call write
      //
      inline Response &operator << (Response &res, string s) {
      res.write(s);
      return res;
    }*/


    class Context : public Request {

    public:
        map<int, uv_write_t> writes;
        uv_tcp_t handle;
        uv_connect_t connect_req;
        uv_write_t write_req;
        http_parser parser;

        // additional field to store current header name
        string hName;
    };

    constexpr int SERVER_DEFAULT_PORT = 8080;
    class Server {

        template<typename Type>
        friend void attachEvents(Type* instance, http_parser_settings& settings);
        friend class Response;

    private:
        uv_loop_t* UV_LOOP;
        uv_tcp_t socket_;
        std::thread t;
        mutable std::mutex m;
        bool _flag;
        int m_port;

        vector<unique_ptr<RestPath>> restVec;

        int complete(http_parser* parser);
        int listen (const char*, int);

    public:
        Server ();
        ~Server() {}
        Server(const Server &other) = delete;
        Server& run();
        Server& stop();
        Server& enableStdLogs(bool enable);
        Server& port(int port);
        Server& registerPath(RestPath&& ptr);
        Server& registerPath(unique_ptr<RestPath> &ptr);
    };


    // REST SECTION
    enum class RestMethod : uint8_t { POST, GET, PUT, DELETE };
    typedef tuple<RestMethod , string> restTuple;
    enum class RestField : uint8_t { METHOD_NAME, METHOD_STR_NAME };

    static array<restTuple, 4> const at = {
            make_tuple(RestMethod::POST, "POST"),
            make_tuple(RestMethod::GET, "GET"),
            make_tuple(RestMethod::PUT, "PUT"),
            make_tuple(RestMethod::DELETE, "DELETE")
    };

    class RestPath {
    public:
        typedef function<void (Request &req, Response &res)> method_cb;
        static string objectName(const string& pathName, const string& url);

        void handle(Request &req, Response &res);
        RestPath *addMethod(RestMethod method, method_cb cb);
        RestPath *setPathStr(string path);

        RestPath() = default;
        ~RestPath() {}

        const map<string, method_cb> &getMethods() const;
        const unique_ptr<string> &getPath() const;

    private:
        map<string, method_cb> mp;
        unique_ptr<string> path;
    };

} // namespace http

#endif

