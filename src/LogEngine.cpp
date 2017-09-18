//
// Created by mrowacz on 21.09.17.
//

#include "LogEngine.h"

using namespace slog;

void slog::init() {

    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::trace
            );
    logging::add_common_attributes();
    logging::add_console_log
            (
                    std::cout,
                    keywords::format = "[%TimeStamp%]: >> %Message%"
            );

    BOOST_LOG_SEV(lg, info) << "Server Version: "
                            << version::VERSION_MAJOR
                            << "."
                            << version::VERSION_MINOR;
    BOOST_LOG_SEV(lg, info) << "Build SHA1: "
                            << version::VERSION_SHA1;
}

void slog::logToFile()
{
    logging::add_file_log
            (
                    keywords::file_name = "server_%N.log",
                    keywords::auto_flush = true,
                    keywords::rotation_size = 10 * 1024 * 1024,
                    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                    keywords::format = "[%TimeStamp%][%File%]: >> %Message%"
            );
}

void slog::disable() {
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::fatal
            );
}