#include <text/text.h>

namespace gl {

    void Text::init() {
        drawable.type = DrawType::TRIANGLES;
        drawable.strips = 1;

        drawable.vao.init();
        drawable.vao.bind();

        mVertices.reserve(1);
        drawable.vbo.init<CharVertices>(1, Vertex2dUV::format, BufferAllocType::DYNAMIC);

        mIndices.reserve(6); // 6 indices per character
        drawable.ibo.init(6, BufferAllocType::DYNAMIC);
    }

    void Text::free() {
        drawable.free();
    }

    void Text::update() {
        size_t stringLength = buffer.size();
        auto& alphabet = style.font->alphabet;
        float paddingX = style.padding.x;
        float paddingY = style.padding.y;
        mVertices.resize(stringLength);
        mIndices.resize(stringLength * 6);

        for (int i = 0 ; i < stringLength ; i++) {
            const Character& character = alphabet[buffer[i]];
            const CharVertices& charVertices = character.vertices;

            glm::vec2 charPos0 = {charVertices.v0.pos.x + paddingX, charVertices.v0.pos.y + paddingY };
            glm::vec2 charPos1 = {charVertices.v1.pos.x + paddingX, charVertices.v1.pos.y + paddingY };
            glm::vec2 charPos2 = {charVertices.v2.pos.x + paddingX, charVertices.v2.pos.y + paddingY };
            glm::vec2 charPos3 = {charVertices.v3.pos.x + paddingX, charVertices.v3.pos.y + paddingY };
            paddingX++;

            mVertices[i] = {
                    charPos0, charVertices.v0.uv,
                    charPos1, charVertices.v1.uv,
                    charPos2, charVertices.v2.uv,
                    charPos3, charVertices.v3.uv
            };

            u32 indexOffset = 4 * i;
            u32 index = 6 * i;
            mIndices[index] = indexOffset;
            mIndices[index + 1] = indexOffset + 1;
            mIndices[index + 2] = indexOffset + 2;
            mIndices[index + 3] = indexOffset;
            mIndices[index + 4] = indexOffset + 2;
            mIndices[index + 5] = indexOffset + 3;
        }

        drawable.vbo.tryUpdate(mVertices);
        drawable.ibo.tryUpdate(mIndices);
        drawable.verticesPerStrip = mIndices.size();
    }

    void Text::draw() {
        drawable.draw();
    }

    void TextShader::update(Style &style) {
        style.font->buffer.bindActivate(0);
        textColor.value = style.color;
        setUniform(textColor);
    }

    Text2dRenderer::Text2dRenderer() {
        mShader.addVertexStage("shaders/text/text2d.vert");
        mShader.addFragmentStage("shaders/text/text2d.frag");
        mShader.complete();
    }

    Text2dRenderer::~Text2dRenderer() {
        mShader.free();
    }

    void Text2dRenderer::begin() {
        mShader.use();
    }

    void Text2dRenderer::render(Text2d &text) {
        text.transform.update(mShader);
        mShader.update(text.style);
        text.draw();
    }

    Text3dRenderer::Text3dRenderer() {
        mShader.addVertexStage("shaders/text/text3d.vert");
        mShader.addFragmentStage("shaders/text/text3d.frag");
        mShader.complete();
    }

    Text3dRenderer::~Text3dRenderer() {
        mShader.free();
    }

    void Text3dRenderer::begin() {
        mShader.use();
    }

    void Text3dRenderer::render(Text3d &text) {
        text.transform.update(mShader);
        mShader.update(text.style);
        text.draw();
    }

}
