#include <cube.h>

namespace gl {

    template<typename T>
    static void init_uv(cube_vertices<T>& vertices) {
        vertices.v0.uv = { 0, 0 };
        vertices.v1.uv = { 0, 1 };
        vertices.v2.uv = { 1, 1 };
        vertices.v3.uv = { 1, 0 };

        vertices.v4.uv = { 0, 0 };
        vertices.v5.uv = { 0, 1 };
        vertices.v6.uv = { 1, 1 };
        vertices.v7.uv = { 1, 0 };

        vertices.v8.uv = { 0, 0 };
        vertices.v9.uv = { 0, 1 };
        vertices.v10.uv = { 1, 1 };
        vertices.v11.uv = { 1, 0 };

        vertices.v12.uv = { 0, 0 };
        vertices.v13.uv = { 0, 1 };
        vertices.v14.uv = { 1, 1 };
        vertices.v15.uv = { 1, 0 };

        vertices.v16.uv = { 0, 0 };
        vertices.v17.uv = { 0, 1 };
        vertices.v18.uv = { 1, 1 };
        vertices.v19.uv = { 1, 0 };

        vertices.v20.uv = { 0, 0 };
        vertices.v21.uv = { 0, 1 };
        vertices.v22.uv = { 1, 1 };
        vertices.v23.uv = { 1, 0 };
    }

    template<typename T>
    static void init_normal(cube_vertices<T>& vertices) {
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

        vertices.v0.normal = normal1;
        vertices.v1.normal = normal1;
        vertices.v2.normal = normal1;
        vertices.v3.normal = normal1;

        vertices.v4.normal = normal2;
        vertices.v5.normal = normal2;
        vertices.v6.normal = normal2;
        vertices.v7.normal = normal2;

        vertices.v8.normal = normal3;
        vertices.v9.normal = normal3;
        vertices.v10.normal = normal3;
        vertices.v11.normal = normal3;

        vertices.v12.normal = normal4;
        vertices.v13.normal = normal4;
        vertices.v14.normal = normal4;
        vertices.v15.normal = normal4;

        vertices.v16.normal = normal5;
        vertices.v17.normal = normal5;
        vertices.v18.normal = normal5;
        vertices.v19.normal = normal5;

        vertices.v20.normal = normal6;
        vertices.v21.normal = normal6;
        vertices.v22.normal = normal6;
        vertices.v23.normal = normal6;
    }

    cube_uv cube_uv_init(drawable_elements& drawable)
    {
        cube_uv new_cube;
        init_uv(new_cube.vertices);
        cube_init<vertex_uv>(drawable, new_cube);
        return new_cube;
    }

    cube_uv_normal cube_uv_normal_init(drawable_elements& drawable)
    {
        cube_uv_normal new_cube;
        init_uv(new_cube.vertices);
        init_normal(new_cube.vertices);
        cube_init<vertex_uv_normal>(drawable, new_cube);
        return new_cube;
    }

    cube_solid cube_solid_init(drawable_elements& drawable, const glm::vec4& fill_color)
    {
        cube_solid new_cube;
        cube_vertices<vertex_solid>& vertices = new_cube.vertices;

        vertices.v0.color = fill_color;
        vertices.v1.color = fill_color;
        vertices.v2.color = fill_color;
        vertices.v3.color = fill_color;
        vertices.v4.color = fill_color;
        vertices.v5.color = fill_color;
        vertices.v6.color = fill_color;
        vertices.v7.color = fill_color;
        vertices.v8.color = fill_color;
        vertices.v9.color = fill_color;
        vertices.v10.color = fill_color;
        vertices.v11.color = fill_color;
        vertices.v12.color = fill_color;
        vertices.v13.color = fill_color;
        vertices.v14.color = fill_color;
        vertices.v15.color = fill_color;
        vertices.v16.color = fill_color;
        vertices.v17.color = fill_color;
        vertices.v18.color = fill_color;
        vertices.v19.color = fill_color;
        vertices.v20.color = fill_color;
        vertices.v21.color = fill_color;
        vertices.v22.color = fill_color;
        vertices.v23.color = fill_color;

        cube_init<vertex_solid>(drawable, new_cube);

        return new_cube;
    }

    template<typename T>
    static void init_tbn(cube_vertices<T>& vertices) {
        init_tbn(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        init_tbn(&vertices.v4, &vertices.v5, &vertices.v6, &vertices.v7);
        init_tbn(&vertices.v8, &vertices.v9, &vertices.v10, &vertices.v11);
        init_tbn(&vertices.v12, &vertices.v13, &vertices.v14, &vertices.v15);
        init_tbn(&vertices.v16, &vertices.v17, &vertices.v18, &vertices.v19);
        init_tbn(&vertices.v20, &vertices.v21, &vertices.v22, &vertices.v23);
    }

    cube_tbn cube_tbn_init(drawable_elements& drawable) {
        cube_tbn cube;
        init_uv(cube.vertices);
        init_normal(cube.vertices);
        init_tbn(cube.vertices);
        cube_init(drawable, cube);
        return cube;
    }

}