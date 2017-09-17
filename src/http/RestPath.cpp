//
// Created by mrowacz on 13.09.17.
//
#include "Http.h"
#include "../HttpLib.h"

namespace http {

    RestPath* RestPath::addMethod(RestMethod method, method_cb cb)
    {
        auto methodItr = find_if(at.begin(), at.end(), [&](auto tp)-> bool
            {
                return method == static_cast<RestMethod>
                                 (get<lib::toUType(RestField::METHOD_NAME)>(tp));
            }
        );
        this->mp.insert(pair<string, method_cb>(
                std::get<lib::toUType(RestField::METHOD_STR_NAME)>(*methodItr),
                cb
        ));
        return this;
    }

    RestPath* RestPath::setPathStr(string path)
    {
        this->path = make_unique<string>(path);
        return this;
    }

    const map<string, RestPath::method_cb> &RestPath::getMethods() const {
        return mp;
    }

    const unique_ptr<string> &RestPath::getPath() const {
        return path;
    }

    string RestPath::objectName(const string& pathName, const string& url)
    {
        if (pathName.length() >= url.length())
            return "";
        return url.substr(pathName.length(), url.length() - pathName.length());
    }

}
