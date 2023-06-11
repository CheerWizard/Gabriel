#pragma once

namespace gl {

    struct BinaryStream final {

        BinaryStream() = default;
        BinaryStream(size_t capacity);
        BinaryStream(std::string& string);
        ~BinaryStream();

        template<class T>
        void add(T& primitive);

        void addString(std::string& string);

        template<class T>
        void add(std::vector<T>& vector);

        template<class T>
        void get(T& primitive);

        void getString(std::string& string);

        template<class T>
        void get(std::vector<T>& vector);

        void write(const char* filepath);
        void read(const char* filepath);

        void seek(size_t index);

    private:
        void add(void* data, size_t newSize);
        void get(void* data, size_t size);

    private:
        std::vector<u8> mBuffer;
        size_t mCursor = 0;
    };

    template<class T>
    void BinaryStream::add(T& primitive) {
        add(&primitive, sizeof(primitive));
    }

    template<class T>
    void BinaryStream::add(std::vector<T>& vector) {
        size_t size = vector.size();
        add(size);
        add(vector.data(), size * sizeof(T));
    }

    template<class T>
    void BinaryStream::get(T& primitive) {
        get(&primitive, sizeof(primitive));
    }

    template<class T>
    void BinaryStream::get(std::vector<T>& vector) {
        size_t size = 0;
        get(size);
        vector.resize(size);
        get(vector.data(), size * sizeof(T));
    }

    #define serialization() \
    void serialize(BinaryStream& stream); \
    void deserialize(BinaryStream& stream);

    template<class Serializable>
    struct Serializer final {
        static void serialize(const char* filepath, Serializable& serializable);
        static void deserialize(const char* filepath, Serializable& serializable);
    };

    template<class Serializable>
    void Serializer<Serializable>::serialize(const char* filepath, Serializable& serializable) {
        BinaryStream stream;
        serializable.serialize(stream);
        try {
            stream.write(filepath);
        } catch (const std::exception& e) {
            error("Failed to serialize into {0}", filepath);
            error(e.what());
        }
    }

    template<class Serializable>
    void Serializer<Serializable>::deserialize(const char* filepath, Serializable& serializable) {
        try {
            BinaryStream stream;
            stream.read(filepath);
            stream.seek(0);
            serializable.deserialize(stream);
        } catch (const std::exception& e) {
            error("Failed to deserialize from {0}", filepath);
            error(e.what());
        }
    }

}