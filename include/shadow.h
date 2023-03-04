#pragma once

#include <shader.h>
#include <buffers.h>
#include <transform.h>
#include <draw.h>
#include <model_loader.h>

namespace gl {

    struct ShadowVertex final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct ShadowMeshVertices final {
        ShadowVertex* data;

        inline float* to_float() const { return (float*) &data->pos.x; }
    };

    struct ShadowMesh final {
        ShadowMeshVertices vertices;
        u32 vertex_count;
        u32* indices;
        u32 index_count;
    };

    struct ShadowModel final {
        std::vector<ShadowMesh> meshes;
    };

    struct ShadowDrawableModel final {
        ShadowModel model;
        DrawableElements elements;

        void init(const io::DrawableModel& src);
        void free();
    };

    struct ShadowDrawableModels final {
        ShadowModel model;
        std::vector<DrawableElements> elements;

        void init(const io::DrawableModels& src);
        void free();
    };

    void shadow_end();

    struct DirectShadow final {
        int width = 1024;
        int height = 1024;
        glm::vec3 direction;
        glm::mat4 light_space;

        void init();

        static void free();

        void begin() const;
        static void end();

        void update();
        void update(Shader& shader);

        void draw(Transform& transform, const DrawableElements& drawable);
    };

    struct PointShadow final {
        int width = 1024;
        int height = 1024;
        glm::vec3 position;
        float far_plane = 25;

        void init();

        static void free();

        void begin() const;
        static void end();

        void update();
        void update(Shader& shader);

        void draw(Transform& transform, const DrawableElements& drawable);
    };

}