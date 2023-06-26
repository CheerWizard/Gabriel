#pragma once

#include <network/io/serialization.h>

namespace gl {

    template<typename T>
    struct NetworkQueue final {

        NetworkQueue() = default;

        NetworkQueue(const size_t size) {
            m_queue.reserve(sizeof(T) * size);
        }

        ~NetworkQueue() {
            m_queue.clear();
        }

        void resize(const size_t size);

        void reserve(const size_t capacity);

        void clear();

        void push(T&& item);

        void push(const T& item);

        T& front();

        T& back();

        void pop();

        bool empty();

        inline typename std::vector<T>::const_iterator begin() {
            return m_queue.begin();
        }

        inline typename std::vector<T>::const_iterator end() {
            return m_queue.end();
        }

        inline typename std::vector<T>::const_iterator rbegin() {
            return m_queue.rbegin();
        }

        inline typename std::vector<T>::const_iterator rend() {
            return m_queue.rend();
        }

        inline T& operator [](int i) {
            return m_queue[i];
        }

    private:
        std::vector<T> m_queue;
        size_t m_cursor = -1;
        std::mutex m_mutex;
    };

    template<typename T>
    void NetworkQueue<T>::resize(const size_t size) {
        std::lock_guard lock(m_mutex);
        m_queue.resize(size);
        m_cursor = m_queue.size() - 1;
    }

    template<typename T>
    void NetworkQueue<T>::reserve(const size_t capacity) {
        std::lock_guard lock(m_mutex);
        m_queue.reserve(capacity);
    }

    template<typename T>
    void NetworkQueue<T>::clear() {
        std::lock_guard lock(m_mutex);
        m_queue.clear();
    }

    template<typename T>
    void NetworkQueue<T>::push(T &&item) {
        std::lock_guard lock(m_mutex);
        m_queue.emplace_back(item);
        m_cursor++;
    }

    template<typename T>
    void NetworkQueue<T>::push(const T &item) {
        std::lock_guard lock(m_mutex);
        m_queue.emplace_back(item);
        m_cursor++;
    }

    template<typename T>
    void NetworkQueue<T>::pop() {
        std::lock_guard lock(m_mutex);
        m_queue.erase(m_queue.begin() + m_cursor);
        m_cursor--;
    }

    template<typename T>
    T& NetworkQueue<T>::front() {
        std::lock_guard lock(m_mutex);
        return m_queue.front();
    }

    template<typename T>
    T& NetworkQueue<T>::back() {
        std::lock_guard lock(m_mutex);
        return m_queue.back();
    }

    template<typename T>
    bool NetworkQueue<T>::empty() {
        std::lock_guard lock(m_mutex);
        return m_queue.empty();
    }

}