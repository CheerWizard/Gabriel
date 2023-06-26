#pragma once

namespace gl {

    typedef const std::chrono::duration<double, std::milli> TimeMillis;

    struct GABRIEL_API Time final {

        Time() = default;
        Time(const double millis) : m_millis(millis) {}

        inline float millis() const {
            return m_millis;
        }

        inline float seconds() const {
            return m_millis * 0.001f;
        }

        inline float minutes() const {
            return 60 * seconds();
        }

        inline float hours() const {
            return 3600 * seconds();
        }

        friend Time operator+(const Time& t1, const Time& t2);
        friend Time operator-(const Time& t1, const Time& t2);
        friend Time operator*(const Time& t1, const Time& t2);
        friend Time operator/(const Time& t1, const Time& t2);

    private:
        float m_millis = 0.0f;
    };

}