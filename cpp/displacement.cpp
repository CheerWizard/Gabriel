#include <displacement.h>
#include <math_functions.h>

namespace gl {

    void DisplacementImageMixer::add_image(const char* filepath, bool flip_uv, PixelType pixel_type) {
        images.emplace_back(ImageReader::read(filepath, flip_uv, pixel_type));
    }

    void DisplacementImageMixer::mix(float min_height, float max_height) {
        if (!displacement_map) {
            print_err("Displacement map is NULL!");
            return;
        }

        int width = mixed_image.width;
        int height = mixed_image.height;
        int channels = mixed_image.channels;
        float displacement_map_ratio = (float) displacement_map->rows / (float) height;
        mixed_image.pixel_type = PixelType::U8;
        mixed_image.pixels = malloc(width * height * channels);
        u8* pixels = (u8*) mixed_image.pixels;
        size_t image_size = images.size();

        for (int y = 0 ; y < height ; y++) {
            for (int x = 0 ; x < width ; x++) {
                float interpolated_height = displacement_map->get_interpolated_height(
                        (float) x * displacement_map_ratio,
                        (float) y * displacement_map_ratio
                );
                float red = 0;
                float green = 0;
                float blue = 0;

                for (int i = 0 ; i < image_size ; i++) {
                    auto& image = images[i];
                    glm::vec4 color = image.get_color(x, y);
                    float blend_factor = region_percent(i, interpolated_height);

                    red += blend_factor * color.r;
                    green += blend_factor * color.g;
                    blue += blend_factor * color.b;
                }

                if (red >= 255.0f || green >= 255.0f || blue >= 255.0f) {
                    print_err("RGB out of bounds!");
                    return;
                }

                pixels[0] = (u8) red;
                pixels[1] = (u8) green;
                pixels[2] = (u8) blue;
                pixels += 3;
            }
        }
    }

    float DisplacementImageMixer::region_percent(int tile, float height) {
        return 0;
    }

    HeightMap::HeightMap(const Image &image) : DisplacementMap(image.height, image.width) {
        int channels = image.channels;
        u8* pixels = (u8*) image.pixels;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                // retrieve texel for (i, j) uv
                u8* texel = pixels + (j + columns * i) * channels;
                // raw height at coordinate
                get(i, j) = (float) texel[0] / 255.0f;
            }
        }
    }

    FaultFormation::FaultFormation(int columns, int rows, int iterations, float min_height, float max_height, float filter)
    : DisplacementMap(rows, columns) {
        float delta_height = max_height - min_height;

        for (int i = 0 ; i < iterations ; i++) {
            float iteration_ratio = ((float) i / (float) iterations);
            float h = max_height - iteration_ratio * delta_height;

            int rx1 = rand() % columns;
            int rz1 = rand() % rows;
            int rx2 = rand() % columns;
            int rz2 = rand() % rows;

            int r = 0;
            while (rx1 == rx2 && rz1 == rz2) {
                rx2 = rand() % columns;
                rz2 = rand() % rows;

                if (r++ == 1000) {
                    print_err("Infinite randomize loop. Breaking loop");
                    break;
                }
            }

            int drx = rx2 - rx1;
            int drz = rz2 - rz1;

            for (int z = 0 ; z < rows ; z++) {
                for (int x = 0 ; x < columns ; x++) {
                    int dx = x - rx1;
                    int dz = z - rz1;
                    int cross = dx * drz - drx * dz;

                    if (cross > 0) {
                        get(z, x) += h;
                    }
                }
            }
        }

        normalize(min_height, max_height);
        apply_fir_filter(filter);
    }

    void FaultFormation::apply_fir_filter(float filter) {
        // left to right
        for (int z = 0 ; z < rows ; z++) {
            float& prev = get(z, 0);
            for (int x = 1 ; x < columns ; x++) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // right to left
        for (int z = 0 ; z < rows ; z++) {
            float& prev = get(z, columns - 1);
            for (int x = columns - 2 ; x >= 0 ; x--) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // bottom to top
        for (int x = 0 ; x < columns ; x++) {
            float& prev = get(0, x);
            for (int z = 1 ; z < rows ; z++) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // top to bottom
        for (int x = 0 ; x < columns ; x++) {
            float& prev = get(rows - 1, x);
            for (int z = rows - 2 ; z >= 0 ; z--) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
    }

    MidPointFormation::MidPointFormation(int rows, int columns, float roughness, float min_height, float max_height)
    : DisplacementMap(rows, columns) {

        int rect_size = 1;
        if (rows == 1) {
            rect_size = 2;
        } else {
            while (rect_size < rows) {
                rect_size = rect_size * 2;
            }
        }

        float current_height = (float) rect_size / 2.0f;
        float height_reduce = pow(2.0f, -roughness);

        while (rect_size > 0) {
            diamond_step(rect_size, current_height);
            square_step(rect_size, current_height);
            rect_size /= 2;
            current_height *= height_reduce;
        }

        normalize(min_height, max_height);
    }

    void MidPointFormation::diamond_step(int rect_size, float height) {
        int half_rect_size = rect_size / 2;
        for (int y = 0 ; y < columns ; y += rect_size) {
            for (int x = 0 ; x < rows ; x += rect_size) {
                int next_x = (x + rect_size) % rows;
                int next_y = (y + rect_size) % columns;

                if (next_x < x) {
                    next_x = rows - 1;
                }

                if (next_y < y) {
                    next_y = columns - 1;
                }

                float top_left = get(x, y);
                float top_right = get(next_x, y);
                float bot_left = get(x, next_y);
                float bot_right = get(next_x, next_y);

                int mid_x = (x + half_rect_size) % rows;
                int mid_y = (y + half_rect_size) % columns;

                float r = random<float>(-height, height);
                float mid_point = (top_left + top_right + bot_left + bot_right) / 4.0f;

                get(mid_x, mid_y) = mid_point + r;
            }
        }
    }

    void MidPointFormation::square_step(int rect_size, float height) {
        int half_rect_size = rect_size / 2;

        for (int y = 0 ; y < columns ; y += rect_size) {
            for (int x = 0 ; x < rows ; x += rect_size) {
                int next_x = (x + rect_size) % rows;
                int next_y = (y + rect_size) % columns;

                int mid_x = (x + half_rect_size) % rows;
                int mid_y = (y + half_rect_size) % columns;

                int prev_mid_x = (x - half_rect_size + rows) % rows;
                int prev_mid_y = (y - half_rect_size + columns) % columns;

                float current_top_left = get(x, y);
                float current_top_right = get(next_x, y);
                float current_bot_left = get(x, next_y);
                float current_center = get(mid_x, mid_y);
                float prev_x_center = get(prev_mid_x, mid_y);
                float prev_y_center = get(mid_x, prev_mid_y);

                float current_mid_left = (current_top_left + current_center + current_bot_left + prev_x_center) / 4.0f + random<float>(-height, height);
                float current_mid_top = (current_top_left + current_center + current_top_right + prev_y_center) / 4.0f + random<float>(-height, height);

                get(mid_x, y) = current_mid_top;
                get(x, mid_y) = current_mid_left;
            }
        }
    }

    void DisplacementMap::get_min_max(float &min, float &max) {
        max = min = map[0];

        for (int i = 1 ; i < rows * columns ; i++) {

            if (map[i] < min) {
                min = map[i];
            }

            if (map[i] > max) {
                max = map[i];
            }

        }
    }

    void DisplacementMap::normalize(float min_range, float max_range) {
        float min, max;
        get_min_max(min, max);

        if (max <= min)
            return;

        float min_max_delta = max - min;
        float min_max_range = max_range - min_range;

        for (int i = 0 ; i < rows * columns; i++) {
            map[i] = ((map[i] - min) / min_max_delta) * min_max_range + min_range;
        }
    }

    float DisplacementMap::get_interpolated_height(float x, float y) {
        float height = get((int)x, (int)y);

        if (((int) x + 1 >= rows) || ((int)y + 1 >= columns)) {
            return height;
        }

        float height_next_x = get((int)x + 1, (int)y);
        float ratio_x = x - floorf(x);
        float height_interpolated_x = (height_next_x - height) * ratio_x + height;

        float height_next_y = get((int)x, (int)y + 1);
        float ratio_y = y - floorf(y);
        float height_interpolated_y = (height_next_y - height) * ratio_y + height;

        return (height_interpolated_x + height_interpolated_y) / 2.0f;
    }
}