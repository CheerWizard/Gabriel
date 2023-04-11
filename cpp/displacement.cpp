#include <displacement.h>
#include <math_functions.h>

namespace gl {

    HeightMap::HeightMap(const Image &image) {
        rows = image.height;
        columns = image.width;
        map.resize(rows * columns);
        int channels = image.channels;
        u8* pixels = (u8*) image.pixels;

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                // retrieve texel for (i, j) uv
                u8* texel = pixels + (j + columns * i) * channels;
                // raw height at coordinate
                get(i, j) = (float) texel[0] / 255.0f;
            }
        }
    }

    FaultFormation::FaultFormation(int columns, int rows, int iterations, float min_height, float max_height, float filter) {
        this->columns = columns;
        this->rows = rows;
        map.resize(columns * rows);
        float delta_height = max_height - min_height;

        for (int i = 0 ; i < iterations ; i++) {
            float iteration_ratio = ((float) i / (float) iterations);
            float h = (max_height / delta_height) - iteration_ratio;

            int rx1 = rand() % columns;
            int rz1 = rand() % rows;
            int rx2 = rand() % columns;
            int rz2 = rand() % rows;

            int r = 0;
            while (rx1 == rx2 && rz1 == rz2) {
                rx2 = rand() % columns;
                rz2 = rand() % rows;

                if (r++ == 1000) {
                    print_err("FaultFormation::FaultFormation(): Infinite randomize loop. Breaking loop");
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

        int s = size();
        for (int i = 0 ; i < s ; i++) {
            float& h = map[i];
            h -= floor(h);
        }

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

}