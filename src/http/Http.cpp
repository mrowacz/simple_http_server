#include "Http.h"
#include "LogEngine.h"

namespace http {

    using namespace std;
    namespace logging = boost::log;
    using namespace logging::trivial;

    const string CRLF = "\r\n";

    Server::Server (Listener fn) :
            listener(fn) {

    }

    void Server::run()
    {
        t = std::thread(std::bind(&Server::listen, this, "0.0.0.0", 8080));

    }

    void Server::stop()
    {
        {
            std::lock_guard<std::mutex> lock(m);
            _flag = true;
        }
        t.join();

    }

    void free_context (uv_handle_t* handle) {
      auto* context = reinterpret_cast<Context*>(handle->data);
      context->writes.clear();
      free(context);
    }

    //
    // Events
    //
    template <class Type>
    void attachEvents(Type* instance, http_parser_settings& settings) {

      // http parser callback types
      static function<int(http_parser* parser)> on_message_complete;

      static auto callback = instance->listener;
      // called once a connection has been made and the message is complete.
      on_message_complete = [&](http_parser* parser) -> int {
          return instance->complete(parser, callback);
          return 0;
      };

      // called after the url has been parsed.
      settings.on_url =
              [](http_parser* parser, const char* at, size_t len) -> int {
                  Context* context = static_cast<Context*>(parser->data);
                  if (at && context) { context->url = string(at, len); }
                  return 0;
              };

      // called when there are either fields or values in the request.
        settings.on_header_field =
                [](http_parser* parser, const char* at, size_t length) -> int {
                    Context* context = static_cast<Context*>(parser->data);
                    if (at && context) {
                        context->cHeaderStr = string(at, length);
                    }
                    return 0;
                };

      // called when header value is given
      settings.on_header_value =
              [](http_parser* parser, const char* at, size_t length) -> int {
                  Context* context = static_cast<Context*>(parser->data);
                  if (at && context && !context->cHeaderStr.compare("Content-Type")) {
                      context->headers.insert({context->cHeaderStr, string(at, length)});
                  }

                  return 0;
              };

      // called once all fields and values have been parsed.
      settings.on_headers_complete =
              [](http_parser* parser) -> int {
                  Context* context = static_cast<Context*>(parser->data);
                  context->method = string(http_method_str((enum http_method) parser->method));
                  return 0;
              };

      // called when there is a body for the request.
      settings.on_body =
              [](http_parser* parser, const char* at, size_t len) -> int {
                  Context* context = static_cast<Context*>(parser->data);
                  if (at && context && (int) len > -1) {
                    context->body << string(at, len);
                  }
                  return 0;
              };

      // called after all other events.
      settings.on_message_complete =
              [](http_parser* parser) -> int {
                  return on_message_complete(parser);
              };
    }

    template void attachEvents<Server>(Server* instance, http_parser_settings& settings);

    //
    // Response.
    //
    void Response::setHeader (const string key, const string val) {
      headersSet = true;
      if (writtenOrEnded) throw runtime_error("Can not set headers after write");

      if (key == "Content-Length") {
        contentLengthSet = true;
      }
      headers.insert({ key, val });
    }


    void Response::setStatus (int code) {

        BOOST_LOG_SEV(slog::lg, trace) << "setStatus " << code;
        statusSet = true;
        if (writtenOrEnded) throw runtime_error("Can not set status after write");
        statusCode = code;
        switch(code) {
            case http_status::HTTP_STATUS_OK:
                statusAdjective = "Ok";
                break;
            case http_status::HTTP_STATUS_CREATED:
                statusAdjective = "Created";
                break;
            case http_status::HTTP_STATUS_NO_CONTENT:
                statusAdjective = "No Content";
                break;
            case http_status::HTTP_STATUS_BAD_REQUEST:
                statusAdjective = "Bad Request";
                break;
            case http_status::HTTP_STATUS_PAYLOAD_TOO_LARGE:
                statusAdjective = "Request Entity Too Large";
                break;
            case http_status::HTTP_STATUS_NOT_FOUND:
                statusAdjective = "Not Found";
                break;
            default:
                BOOST_LOG_SEV(slog::lg, fatal) << "Unhandled http code " << code;
                throw runtime_error("Unhandled http code!");
        }
    }

    void Response::writeOrEnd(string str, bool end) {

      if (ended) throw runtime_error("Can not write after end");

      stringstream ss;

      if (!writtenOrEnded) {
        ss << "HTTP/1.1 " << statusCode << " " << statusAdjective << CRLF;

        for (auto &header : headers) {
          ss << header.first << ": " << header.second << CRLF;
        }
        ss << CRLF;
        writtenOrEnded = true;
      }

      bool isChunked = headers.count("Transfer-Encoding")
                       && headers["Transfer-Encoding"] == "chunked";

      if (isChunked) {
        ss << std::hex << str.size()
           << std::dec << CRLF << str << CRLF;
      }
      else {
        ss << str;
      }

      if (isChunked && end) {
        ss << "0" << CRLF << CRLF;
      }

      str = ss.str();

      // response buffer
      uv_buf_t resbuf = {
              .base = (char*) str.c_str(),
              .len = str.size()
      };

      Context* context = static_cast<Context*>(this->parser.data);

      auto id = write_count++;

      uv_write_t write_req;
      context->writes.insert({ id, write_req });

      if (end) {

        ended = true;

        uv_write(&context->writes.at(id), (uv_stream_t*) &context->handle, &resbuf, 1,
                 [](uv_write_t* req, int status) {
                     if (!uv_is_closing((uv_handle_t*) req->handle)) {
                       uv_close((uv_handle_t*) req->handle, free_context);
                     }
                 }
        );
      }
      else {
        uv_write(&context->writes.at(id), (uv_stream_t*) &context->handle, &resbuf, 1, NULL);
      }
    }


    void Response::write(string s) {
      this->writeOrEnd(s, false);
    }


    void Response::end(string s) {
      this->writeOrEnd(s, true);
    }


    void Response::end() {
      this->writeOrEnd("", true);
    }

}

