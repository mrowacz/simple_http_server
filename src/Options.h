//
// Created by mrowacz on 17.09.17.
//

#ifndef WP_INTERVIEW_OPTIONS_H
#define WP_INTERVIEW_OPTIONS_H

#include <iostream>
#include <boost/program_options.hpp>

namespace server_options {

    namespace po = boost::program_options;

    constexpr const char * OPTION_QUIET = "quiet";
    constexpr const char * OPTION_PORT = "port";
    constexpr const char * OPTION_DB = "db";
    constexpr const char * OPTION_STDOUT_LOG = "disable-stdout";
    constexpr const char * OPTION_FILE_LOG = "disable-filelog";

    constexpr const char * DB_MEMORY_MODE = "memory";
    constexpr const char * DB_SQLITE3_MODE = "sqlite3";

    struct dbtype {
        enum class mode { MEMORY, SQLITE3, UNKNOWN };
        dbtype() {}
        dbtype(std::string const& val):
                value(val)
        {
            if (!val.compare(DB_MEMORY_MODE))
                md = mode::MEMORY;
            else if (!val.compare(DB_SQLITE3_MODE))
                md = mode::SQLITE3;
        }

        mode md;
        std::string value;
    };

    void validate(boost::any& v,
                  std::vector<std::string> const& values,
                  dbtype* /* target_type */,
                  int)
    {
        using namespace boost::program_options;

        // Make sure no previous assignment to 'v' was made.
        validators::check_first_occurrence(v);

        // Extract the first string from 'values'. If there is more than
        // one string, it's an error, and exception will be thrown.
        std::string const& s = validators::get_single_string(values);

        if (s == DB_SQLITE3_MODE || s == DB_MEMORY_MODE) {
            v = boost::any(dbtype(s));
        } else {
            throw validation_error(validation_error::invalid_option_value);
        }
    }

    std::tuple<int, dbtype, bool, bool, bool>
    process_program_options(const int argc, const char *const argv[])
    {
        bool quietFlag = false;
        bool silentStd = false;
        bool silentFileLog = false;
        int port = 0;
        dbtype dbType(DB_MEMORY_MODE);

        po::options_description desc("Usage");
        desc.add_options()
                (
                        "help,h",
                        "Show help. If given, show help on the specified topic."
                )
                (
                        "quiet,q",
                        "Remove debugs"
                )
                (
                        "disable-stdout",
                        "Disable logging to the stdout"
                )
                (
                        "disable-filelog",
                        "Disable logging to the file"
                )
                (
                        "port,p",
                        po::value<int>()->required(),
                        "Server port number"
                )
                (
                        "db",
                        po::value<dbtype>()->required(),
                        "Choose place where data will be stored: memory or sqlite3"
                );

        po::variables_map args;

        try {
            po::store(
                    po::parse_command_line(argc, argv, desc),
                    args
            );
            if (argc == 1 || args.count("help")) {
                std::cout << desc << std::endl;
                exit( EXIT_SUCCESS );
            }
            if (args.count(OPTION_QUIET))
                quietFlag = true;
            if (args.count(OPTION_PORT))
                port = args[OPTION_PORT].as<int>();
            if (args.count(OPTION_DB))
                dbType = args[OPTION_DB].as<dbtype>();
            if (args.count(OPTION_STDOUT_LOG))
                silentStd = true;
            if (args.count(OPTION_FILE_LOG))
                silentFileLog = true;
        }
        catch (po::error const& e) {
            std::cerr << e.what() << '\n';
            exit( EXIT_FAILURE );
        }
        po::notify(args);
        return std::make_tuple(port, dbType, quietFlag, silentStd, silentFileLog);
    }
}

#endif //WP_INTERVIEW_OPTIONS_H
