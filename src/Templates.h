//
// Created by mrowacz on 19.09.17.
//

#ifndef WP_INTERVIEW_TEMPLATES_H
#define WP_INTERVIEW_TEMPLATES_H

namespace templates {
    template<typename E>
    constexpr auto
    toUType(E enumerator) noexcept {
        return static_cast<std::underlying_type_t <E>>(enumerator);
    }
}

#endif //WP_INTERVIEW_TEMPLATES_H
