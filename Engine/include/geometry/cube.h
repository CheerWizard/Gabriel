#pragma once

#include <geometry/geometry.h>

namespace gl {

    template<typename T>
    struct GABRIEL_API CubeVertices final {

        T v0 = { { -0.5f,0.5f,-0.5f } };
        T v1 = { { -0.5f,-0.5f,-0.5f } };
        T v2 = { { 0.5f,-0.5f,-0.5f } };
        T v3 = { { 0.5f,0.5f,-0.5f } };

        T v4 = { { -0.5f, 0.5f, 0.5f } };
        T v5 = { { -0.5f,-0.5f,0.5f } };
        T v6 = { { 0.5f,-0.5f,0.5f } };
        T v7 = { { 0.5f,0.5f,0.5f } };

        T v8 = { { 0.5f,0.5f,-0.5f } };
        T v9 = { { 0.5f,-0.5f,-0.5f } };
        T v10 = { { 0.5f,-0.5f,0.5f } };
        T v11 = { { 0.5f,0.5f,0.5f } };

        T v12 = { { -0.5f,0.5f,-0.5f } };
        T v13 = { { -0.5f,-0.5f,-0.5f } };
        T v14 = { { -0.5f,-0.5f,0.5f } };
        T v15 = { { -0.5f,0.5f,0.5f } };

        T v16 = { { -0.5f,0.5f,0.5f } };
        T v17 = { { -0.5f,0.5f,-0.5f } };
        T v18 = { { 0.5f,0.5f,-0.5f } };
        T v19 = { { 0.5f,0.5f,0.5f } };

        T v20 = { { -0.5f,-0.5f,0.5f } };
        T v21 = { { -0.5f,-0.5f,-0.5f } };
        T v22 = { { 0.5f,-0.5f,-0.5f } };
        T v23 = { { 0.5f,-0.5f,0.5f } };

        inline size_t size() const { return sizeof(CubeVertices<T>); }
        inline float* toFloat() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    static void initUV(CubeVertices<T>& vertices) {
        vertices.v0.UV = {0, 0 };
        vertices.v1.UV = {0, 1 };
        vertices.v2.UV = {1, 1 };
        vertices.v3.UV = {1, 0 };

        vertices.v4.UV = {0, 0 };
        vertices.v5.UV = {0, 1 };
        vertices.v6.UV = {1, 1 };
        vertices.v7.UV = {1, 0 };

        vertices.v8.UV = {0, 0 };
        vertices.v9.UV = {0, 1 };
        vertices.v10.UV = {1, 1 };
        vertices.v11.UV = {1, 0 };

        vertices.v12.UV = {0, 0 };
        vertices.v13.UV = {0, 1 };
        vertices.v14.UV = {1, 1 };
        vertices.v15.UV = {1, 0 };

        vertices.v16.UV = {0, 0 };
        vertices.v17.UV = {0, 1 };
        vertices.v18.UV = {1, 1 };
        vertices.v19.UV = {1, 0 };

        vertices.v20.UV = {0, 0 };
        vertices.v21.UV = {0, 1 };
        vertices.v22.UV = {1, 1 };
        vertices.v23.UV = {1, 0 };
    }

    template<typename T>
    static void initNormal(CubeVertices<T>& vertices) {
        glm::vec3 normal1 = -glm::normalize(glm::cross(
                vertices.v1.pos - vertices.v0.pos,
                vertices.v3.pos - vertices.v0.pos
        ));

        glm::vec3 normal2 = glm::normalize(glm::cross(
                vertices.v5.pos - vertices.v4.pos,
                vertices.v7.pos - vertices.v4.pos
        ));

        glm::vec3 normal3 = -glm::normalize(glm::cross(
                vertices.v9.pos - vertices.v8.pos,
                vertices.v11.pos - vertices.v8.pos
        ));

        glm::vec3 normal4 = glm::normalize(glm::cross(
                vertices.v13.pos - vertices.v12.pos,
                vertices.v15.pos - vertices.v12.pos
        ));

        glm::vec3 normal5 = -glm::normalize(glm::cross(
                vertices.v17.pos - vertices.v16.pos,
                vertices.v19.pos - vertices.v16.pos
        ));

        glm::vec3 normal6 = glm::normalize(glm::cross(
                vertices.v21.pos - vertices.v20.pos,
                vertices.v23.pos - vertices.v20.pos
        ));

        vertices.v0.NORMAL = normal1;
        vertices.v1.NORMAL = normal1;
        vertices.v2.NORMAL = normal1;
        vertices.v3.NORMAL = normal1;

        vertices.v4.NORMAL = normal2;
        vertices.v5.NORMAL = normal2;
        vertices.v6.NORMAL = normal2;
        vertices.v7.NORMAL = normal2;

        vertices.v8.NORMAL = normal3;
        vertices.v9.NORMAL = normal3;
        vertices.v10.NORMAL = normal3;
        vertices.v11.NORMAL = normal3;

        vertices.v12.NORMAL = normal4;
        vertices.v13.NORMAL = normal4;
        vertices.v14.NORMAL = normal4;
        vertices.v15.NORMAL = normal4;

        vertices.v16.NORMAL = normal5;
        vertices.v17.NORMAL = normal5;
        vertices.v18.NORMAL = normal5;
        vertices.v19.NORMAL = normal5;

        vertices.v20.NORMAL = normal6;
        vertices.v21.NORMAL = normal6;
        vertices.v22.NORMAL = normal6;
        vertices.v23.NORMAL = normal6;
    }

    template<typename T>
    static void initTBN(CubeVertices<T>& vertices) {
        initTBN(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        initTBN(&vertices.v4, &vertices.v5, &vertices.v6, &vertices.v7);
        initTBN(&vertices.v8, &vertices.v9, &vertices.v10, &vertices.v11);
        initTBN(&vertices.v12, &vertices.v13, &vertices.v14, &vertices.v15);
        initTBN(&vertices.v16, &vertices.v17, &vertices.v18, &vertices.v19);
        initTBN(&vertices.v20, &vertices.v21, &vertices.v22, &vertices.v23);
    }

    template<typename T>
    struct Cube final {
        CubeVertices<T> vertices;
        u32 indices[36] = {
                0,1,3,
                3,1,2,
                4,5,7,
                7,5,6,
                8,9,11,
                11,9,10,
                12,13,15,
                15,13,14,
                16,17,19,
                19,17,18,
                20,21,23,
                23,21,22
        };

        void init(DrawableElements& drawable);
        void initUV(DrawableElements& drawable);
        void initUVNormal(DrawableElements& drawable);
        void initSolid(DrawableElements& drawable, const glm::vec4& fillColor);
        void initTBN(DrawableElements& drawable);
    };

    typedef Cube<VertexDefault> CubeDefault;
    typedef Cube<VertexUV> CubeUV;
    typedef Cube<VertexTBN> CubeTBN;

    template<typename T>
    void Cube<T>::init(DrawableElements& drawable)
    {
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, BufferAllocType::STATIC);
        drawable.ibo.init(indices, 36, BufferAllocType::STATIC);
        drawable.strips = 1;
        drawable.verticesPerStrip = 36;
    }

    template<typename T>
    void Cube<T>::initUV(DrawableElements &drawable) {
        gl::initUV(vertices);
        init(drawable);
    }

    template<typename T>
    void Cube<T>::initUVNormal(DrawableElements &drawable) {
        gl::initUV(vertices);
        gl::initNormal(vertices);
        init(drawable);
    }

    template<typename T>
    void Cube<T>::initSolid(DrawableElements &drawable, const glm::vec4& fillColor) {
        vertices.v0.color = fillColor;
        vertices.v1.color = fillColor;
        vertices.v2.color = fillColor;
        vertices.v3.color = fillColor;
        vertices.v4.color = fillColor;
        vertices.v5.color = fillColor;
        vertices.v6.color = fillColor;
        vertices.v7.color = fillColor;
        vertices.v8.color = fillColor;
        vertices.v9.color = fillColor;
        vertices.v10.color = fillColor;
        vertices.v11.color = fillColor;
        vertices.v12.color = fillColor;
        vertices.v13.color = fillColor;
        vertices.v14.color = fillColor;
        vertices.v15.color = fillColor;
        vertices.v16.color = fillColor;
        vertices.v17.color = fillColor;
        vertices.v18.color = fillColor;
        vertices.v19.color = fillColor;
        vertices.v20.color = fillColor;
        vertices.v21.color = fillColor;
        vertices.v22.color = fillColor;
        vertices.v23.color = fillColor;

        init(drawable);
    }

    template<typename T>
    void Cube<T>::initTBN(DrawableElements &drawable) {
        gl::initUV(vertices);
        gl::initNormal(vertices);
        gl::initTBN(vertices);
        init(drawable);
    }

}