//
// Created by mrowacz on 21.09.17.
//

#ifndef WP_INTERVIEW_LOGENGINE_H
#define WP_INTERVIEW_LOGENGINE_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "Config.h"

namespace slog {

    namespace logging = boost::log;
    namespace src = boost::log::sources;
    namespace sinks = boost::log::sinks;
    namespace keywords = boost::log::keywords;
    using namespace logging::trivial;

    static src::severity_logger< severity_level > lg;

    void init();
    void disable();
    // start logging into file server_%N.log
    // in directory where server is executed
    void logToFile();

}
#endif //WP_INTERVIEW_LOGENGINE_H
