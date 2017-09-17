#include "Http.h"

#include <regex>
#include "Config.h"

namespace http {

    int Server::complete (http_parser* parser) {

        Context* context = reinterpret_cast<Context*>(parser->data);
        Request req;
        Response res;

        req.url = context->url;
        req.method = context->method;
        req.body = std::move(context->body);
        req.headers = std::move(context->headers);
        res.parser = *parser;

        BOOST_LOG_SEV(lg, info) << req.method << ": " << req.url;
        auto it = find_if(restVec.begin(), restVec.end(), [&](auto &v) -> bool {
            auto path = v.get()->getPath().get();

            if (path->compare(req.url))
                return true;
            return false;
        }
        );

        if (it != restVec.end())
        {
            auto inst = (*it).get();
            auto cb = inst->getMethods().find(req.method)->second;
            cb(req, res);
            res.end();
        }
        else {
            // TODO finish that
            std::__throw_future_error(0);
        }
        return 0;
    }

    int Server::listen (const char* ip, int port) {

        //
        // parser settings needs to be static.
        //
        //
        static http_parser_settings settings;
        attachEvents(this, settings);

        int status = 0;
        int cores = sysconf(_SC_NPROCESSORS_ONLN);

        std::stringstream cores_string;
        cores_string << cores;
        setenv("UV_THREADPOOL_SIZE", cores_string.str().c_str(), 1);

        struct sockaddr_in address;

        static function<void(uv_stream_t* socket, int status)> on_connect;
        static function<void(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)> read;

        UV_LOOP = uv_default_loop();
        uv_tcp_init(UV_LOOP, &socket_);

        //
        // @TODO - Not sure exactly how to use this,
        // after the initial timeout, it just
        // seems to kill the server.
        //
        //uv_tcp_keepalive(&socket_,1,60);

        status = uv_ip4_addr(ip, port, &address);
        ASSERT_STATUS(status, "Resolve Address");

        status = uv_tcp_bind(&socket_, (const struct sockaddr*) &address, 0);
        ASSERT_STATUS(status, "Bind");

        // called once a connection is made.
        on_connect = [&](uv_stream_t* handle, int status) {
            Context* context = new Context();

            // init tcp handle
            uv_tcp_init(UV_LOOP, &context->handle);

            // init http parser
            http_parser_init(&context->parser, HTTP_REQUEST);

            // client reference for parser routines
            context->parser.data = context;

            // client reference for handle data on requests
            context->handle.data = context;

            // accept connection passing in refernce to the client handle
            uv_accept(handle, (uv_stream_t*) &context->handle);

            // called for every read
            read = [&](uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
                ssize_t parsed;
                Context* context = static_cast<Context*>(tcp->data);

                if (nread >= 0) {
                    parsed = (ssize_t) http_parser_execute(&context->parser,
                                                           &settings,
                                                           buf->base,
                                                           nread);
                    // close handle
                    if (parsed < nread) {
                        uv_close((uv_handle_t*) &context->handle, free_context);
                    }
                } else {
                    if (nread != UV_EOF) {
                        // @TODO - debug error
                    }

                    // close handle
                    uv_close((uv_handle_t*) &context->handle, free_context);
                }

                // free request buffer data
                free(buf->base);
            };

            // allocate memory and attempt to read.
            uv_read_start((uv_stream_t*) &context->handle,
                    // allocator
                          [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
                              *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
                          },

                    // reader
                          [](uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
                              read(tcp, nread, buf);
                          });
        };

        status = uv_listen((uv_stream_t*) &socket_, MAX_WRITE_HANDLES,
                // listener
                           [](uv_stream_t* socket, int status) {
                               on_connect(socket, status);
                           });

        ASSERT_STATUS(status, "Listen");

        // init loop
        _flag = false;
        while(uv_run(UV_LOOP, UV_RUN_NOWAIT))
        {
            {
                std::lock_guard<std::mutex> lock(m);
                if (_flag)
                    break;
            }
        }
        uv_stop(UV_LOOP);
        return 0;
    }

    Server& Server::registerPath(unique_ptr<RestPath> &ptr)
    {
        this->restVec.push_back(move(ptr));
        BOOST_LOG_SEV(lg, info) << "Registered path="
                                << *this->restVec.back()->getPath();
        return *this;
    }

    Server::Server() : m_port(SERVER_DEFAULT_PORT)
    {
        logging::add_file_log
                (
                        keywords::file_name = "server_%N.log",
                        keywords::auto_flush = true,
                        keywords::rotation_size = 10 * 1024 * 1024,
                        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                        keywords::format = "[%TimeStamp%]: >> %Message%"
                );

        logging::core::get()->set_filter
                (
                        logging::trivial::severity >= logging::trivial::info
                );

        logging::add_common_attributes();
    }

    Server& Server::enableStdLogs(bool enable)
    {
        if (enable) {
            logging::add_console_log
                    (
                            std::cout,
                            keywords::format = "[%TimeStamp%]: >> %Message%"
                    );
        }
        return *this;
    }

    Server& Server::run()
    {
        BOOST_LOG_SEV(lg, info) << "Server Version: "
                                << version::VERSION_MAJOR
                                << "."
                                << version::VERSION_MINOR;
        BOOST_LOG_SEV(lg, info) << "Build SHA1: "
                                << version::VERSION_SHA1;
        t = std::thread(std::bind(&Server::listen, this, "0.0.0.0", m_port));
        return *this;
    }

    Server& Server::port(int port)
    {
        this->m_port = port;
        return *this;
    }

    Server& Server::stop()
    {
        BOOST_LOG_SEV(lg, info) << "Server stop() executed";
        {
            std::lock_guard<std::mutex> lock(m);
            _flag = true;
        }
        t.join();

        BOOST_LOG_SEV(lg, info) << "Server stopped ...";
        return *this;
    }
} // namespace http

