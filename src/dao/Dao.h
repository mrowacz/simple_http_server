//
// Created by l.czerwinski on 9/14/17.
//

#ifndef WP_INTERVIEW_DAO_H
#define WP_INTERVIEW_DAO_H

#include <sstream>
#include "../http/Http.h"

namespace dao {

    using namespace std;
    using namespace http;

    constexpr int ONE_MB = 1024 * 1024;
    constexpr int MAX_OBJECT_NAME_LENGTH = 100;

    class Dao {
    public:
        virtual ~Dao() {}
        virtual void create(const std::string id, std::stringstream& ss,
                            const string contentType, Response& resp) = 0;
        virtual void del(const string id, Response& resp) = 0;
        virtual void get(const string id, Response& resp) = 0;

        virtual void list(Response& resp) = 0;
    };

    inline void genListHttpResponse(http::Response &resp,
        std::function<void(std::vector<string> &vec)> func)
    {
        stringstream ss;
        ss << "[";
        vector<string> vec;

        // run query function
        func(vec);

        for (int i = 0; i < vec.size(); i++)
        {
            ss << "\"" << vec[i] << "\"";
            if (i != vec.size() -1)
                ss << ", ";
        }
        ss << "]";
        resp.body += ss.str();
        resp.setStatus(http_status::HTTP_STATUS_OK);
    }
}


#endif //WP_INTERVIEW_DAO_H
