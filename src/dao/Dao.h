//
// Created by l.czerwinski on 9/14/17.
//

#ifndef WP_INTERVIEW_DAO_H
#define WP_INTERVIEW_DAO_H

#include <vector>
#include <sstream>
#include <exception>
#include <functional>

namespace dao {
    constexpr int ONE_MB = 1024 * 1024;

    class Dao {
    public:
        virtual ~Dao() {}
        virtual void create(const std::string& id, const std::string& payload, const std::string& type) = 0;
        virtual void del(const std::string& id) = 0;
        // elements of tuple :
        // 0 - type
        // 1 - payload
        virtual std::tuple<std::string, std::string> get(const std::string& id) = 0;
        virtual std::string list() = 0;
        virtual void clear() = 0;
    };

    enum class dao_error : std::uint32_t {
        not_found,
        object_too_large,
        empty_content,
        empty_payload,
        internal_error
    };

    class dao_exception : public std::exception {
    public:
        dao_error code();
        dao_exception(dao_error error, const char *msg);
        dao_exception(dao_error error);
        dao_exception(const char *msg);

        const char* what() const throw()
        {
            return message.c_str();
        }
    private:
        std::string message;
        dao_error err;
    };

    inline std::string genListStr(
        std::function<void(std::vector<std::string> &vec)> func)
    {
        std::stringstream ss;
        ss << "[";
        std::vector<std::string> vec;

        // run query function
        func(vec);

        for (int i = 0; i < vec.size(); i++) {
            ss << "\"" << vec[i] << "\"";
            if (i != vec.size() - 1)
                ss << ", ";
        }
        ss << "]";
        return ss.str();
    }
}


#endif //WP_INTERVIEW_DAO_H
