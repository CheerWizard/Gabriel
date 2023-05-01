#pragma once

#include <draw.h>
#include <geometry.h>

#include <functional>

namespace gl {

    struct DisplacementMap {
        int rows;
        int columns;
        float min = 0.0f;
        float max = 1.0f;
        std::vector<float> map;

        DisplacementMap() = default;

        DisplacementMap(int rows, int columns) : rows(rows), columns(columns) {
            map.resize(rows * columns);
        }

        inline int size() const { return rows * columns; }

        inline float& get(int row, int col) {
            return map[row * rows + col];
        }

        inline float& operator [](int i) { return map[i]; }

        void normalize();

        float get_interpolated_height(float x, float y);
    };

    struct DisplacementRange final {
        float min = 0;
        float median = 0.5f;
        float max = 1;
    };

    struct DisplacementImage final {
        Image image;
        DisplacementRange range;

        DisplacementImage() = default;
        DisplacementImage(const Image& image) : image(image) {}
        DisplacementImage(const Image& image, const DisplacementRange& range) : image(image), range(range) {}
    };

    struct DisplacementImageMixer final {
        std::vector<DisplacementImage> displacement_images;
        Image mixed_image;
        DisplacementMap* displacement_map = null;

        void add_image(const DisplacementRange& range, const char* filepath, bool flip_uv = false, PixelType pixel_type = PixelType::U8);
        void mix(int width, int height);

    private:
        void resize_images(int width, int height);
        float region_percent(int tile, float height);
    };

    struct HeightMap : DisplacementMap {
        explicit HeightMap(const Image& image);
    };

    struct FaultFormation : DisplacementMap {
        FaultFormation(int columns, int rows, int iterations, float min_height, float max_height, float filter);
    private:
        void apply_fir_filter(float filter);
    };

    struct MidPointFormation : DisplacementMap {
        MidPointFormation(int row, int columns, float roughness, float min_height, float max_height);

    private:
        void diamond_step(int rect_size, float height);
        void square_step(int rect_size, float height);
    };

    template<typename T>
    struct Displacement {
        DisplacementMap map;
        float scale = 1.0f;

        void free();

        void set_origin_vertices(Vertices<T>* vertices);

        void displace(DrawableElements& drawable);

    private:
        Vertices<T>* vertices = null;
        Vertices<T> origin_vertices;
    };

    template<typename T>
    void Displacement<T>::set_origin_vertices(Vertices<T>* vertices) {
        this->vertices = vertices;
        origin_vertices.copy_from(vertices);
    }

    template<typename T>
    void Displacement<T>::displace(DrawableElements& drawable) {
        // displace vertices Y positions
        auto& displaced_vertices = *vertices;
        int size = map.size();
        float s = scale;

        for (int i = 0 ; i < size ; i++) {
            auto& displaced_v = displaced_vertices[i];
            auto& origin_v = origin_vertices[i];
            displaced_v.pos = origin_v.pos + map[i] * s * glm::normalize(origin_v.normal);
        }

        drawable.vao.bind();
        drawable.vbo.update(*vertices);
    }

    template<typename T>
    void Displacement<T>::free() {
        origin_vertices.free();
    }

    struct DisplacementTBN : Displacement<VertexTBN>, ecs::Component<DisplacementTBN> {};

}