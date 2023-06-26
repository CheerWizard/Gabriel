#include <core/time.h>

namespace gl {

    Time operator+(const Time &t1, const Time &t2) {
        return t1.m_millis + t2.m_millis;
    }

    Time operator-(const Time &t1, const Time &t2) {
        return t1.m_millis - t2.m_millis;
    }

    Time operator*(const Time &t1, const Time &t2) {
        return t1.m_millis * t2.m_millis;
    }

    Time operator/(const Time &t1, const Time &t2) {
        return t1.m_millis / t2.m_millis;
    }

}