//
// Created by mrowacz on 15.09.17.
//

#ifndef WP_INTERVIEW_HTTPLIB_H
#define WP_INTERVIEW_HTTPLIB_H

#include <regex>
#include <vector>
#include <iostream>
#include <functional>

#include "http/Http.h"
#include "dao/Dao.h"

namespace lib
{
    using namespace std;

    template<typename E>
    constexpr auto
    toUType(E enumerator) noexcept {
        return static_cast<std::underlying_type_t <E>>(enumerator);
    }

    static regex allowed_id("[a-zA-Z0-9]+");

    inline bool checkObjectName(std::string id) {
        if (!regex_match(id.begin(), id.end(), allowed_id) ||
            id.length() > dao::MAX_OBJECT_NAME_LENGTH)
            return false;
        return true;
    }

    inline std::string getContentHeader(auto headerMap)
    {
        auto it = headerMap.find(http::HTTP_HEADER_CONTENT_TYPE);
        if (it != headerMap.end())
            return it->second;
        return "";
    }

}

#endif //WP_INTERVIEW_HTTPLIB_H
