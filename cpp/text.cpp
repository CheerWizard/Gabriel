#include <text.h>

namespace gl {

    void Text::init() {
        drawable.type = GL_TRIANGLES;
        drawable.strips = 1;

        drawable.vao.init();
        drawable.vao.bind();

        vertices.reserve(100);
        drawable.vbo.init<CharVertices>(100, Vertex2dUV::format, BufferAllocType::DYNAMIC);

        indices.reserve(600); // 6 indices per character
        drawable.ibo.init(600, BufferAllocType::DYNAMIC);
    }

    void Text::free() {
        drawable.free();
    }

    void Text::update() {
        size_t string_length = buffer.size();
        auto& alphabet = style.font->alphabet;
        float padding_x = style.padding.x;
        float padding_y = style.padding.y;
        vertices.clear();
        indices.clear();

        for (int i = 0 ; i < string_length ; i++) {
            const Character& character = alphabet[buffer[i]];

            const CharVertices& char_vertices = character.vertices;

            glm::vec2 char_pos_0 = { char_vertices.v0.pos.x + padding_x, char_vertices.v0.pos.y + padding_y };
            glm::vec2 char_pos_1 = { char_vertices.v1.pos.x + padding_x, char_vertices.v1.pos.y + padding_y };
            glm::vec2 char_pos_2 = { char_vertices.v2.pos.x + padding_x, char_vertices.v2.pos.y + padding_y };
            glm::vec2 char_pos_3 = { char_vertices.v3.pos.x + padding_x, char_vertices.v3.pos.y + padding_y };
            padding_x++;

            vertices.emplace_back(
                    char_pos_0, char_vertices.v0.uv,
                    char_pos_1, char_vertices.v1.uv,
                    char_pos_2, char_vertices.v2.uv,
                    char_pos_3, char_vertices.v3.uv
            );

            u32 index_offset = 4 * i;
            indices.emplace_back(index_offset);
            indices.emplace_back(index_offset + 1);
            indices.emplace_back(index_offset + 3);
            indices.emplace_back(index_offset + 1);
            indices.emplace_back(index_offset + 2);
            indices.emplace_back(index_offset + 3);
        }

        int index_count = (int) indices.size();
        drawable.vertices_per_strip = index_count;
        drawable.vbo.update(vertices);
        drawable.ibo.update(indices.data(), index_count);
    }

    void Text::draw() {
        drawable.draw();
    }

    void TextShader::update(Style &style) {
        const auto& texture = style.font->buffer;
        texture.bind();
        texture.activate(0);

        set_uniform_args("text_color", style.color);
    }

    void Text2dRenderer::init() {
        shader.add_vertex_stage("shaders/text/text2d.vert");
        shader.add_fragment_stage("shaders/text/text2d.frag");
        shader.complete();
    }

    void Text2dRenderer::free() {
        shader.free();
    }

    void Text2dRenderer::begin() {
        shader.use();
    }

    void Text2dRenderer::render(Text2d &text) {
        text.transform.update(shader);
        shader.update(text.style);
        text.draw();
    }

    void Text3dRenderer::init() {
        shader.add_vertex_stage("shaders/text/text3d.vert");
        shader.add_fragment_stage("shaders/text/text3d.frag");
        shader.complete();
    }

    void Text3dRenderer::free() {
        shader.free();
    }

    void Text3dRenderer::begin() {
        shader.use();
    }

    void Text3dRenderer::render(Text3d &text) {
        text.transform.update(shader);
        shader.update(text.style);
        text.draw();
    }

}
