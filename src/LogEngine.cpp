// Copyright 2017 Lukasz Czerwinski mrowacz@gmail.com
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.

#include "LogEngine.h"

void slog::init() {
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::trace
            );
    logging::add_common_attributes();
}

void slog::logToStdOut()
{
    logging::add_console_log
            (
                    std::cout,
                    keywords::format = "[%TimeStamp%]: >> %Message%"
            );
}

void slog::logToFile() {
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
