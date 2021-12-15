struct Vector2f {
    float x, y;
};

struct Vector2i {
    int x, y;
};

struct Color {
    uchar r, g, b, a;
};

struct Vertex {
    struct Vector2f pos;
    struct Color color;
    struct Vector2f texCoords;
};

inline void
calcMandelbrod (float x_0, float y_0, struct Vector2f C, int count_iters, int* res) {
    float x = x_0, y = y_0;
    float tmp_x = 0, tmp_y = 0;

    while (count_iters--) {
        tmp_x = x * x - y * y + C.x;
        tmp_y = 2 * x * y + C.y;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            *res = count_iters;
            return;
        }
    }

    *res = 0;
    return;
}

__kernel void
drawMandelbrod (__global struct Vertex* verts,
                float scale_factor,
                float shift_x,
                float shift_y,
                struct Vector2f C,
                int count_iters)
{
    const uint ix = get_global_id (0);
    const uint iy = get_global_id (1);

    float x_0 = scale_factor * ix + shift_x;
    float y_0 = scale_factor * iy + shift_y;

    calcMandelbrod (x_0, y_0, C, count_iters, &count_iters);

    // count_iters *= 255;

    struct Vertex vertex = {
        .pos = {
            .x = ix,
            .y = iy
        },

        .color = {
            .r = 255 * count_iters * 0.0,
            .g = 255 * count_iters * 0.0,
            .b = 25.5 * count_iters * 0.9,
            .a = 255
        }
    };

    verts[ix * get_global_size (1) + iy] = vertex;
}

inline void
calcMandelbrod3 (float x_0, float y_0, int count_iters, int* res) {
    float x = x_0, y = y_0;
    float tmp_x = 0, tmp_y = 0;

    while (count_iters--) {
        tmp_x = x*x*x - 3 * x * y*y + x_0;
        tmp_y = 3 * x*x * y - y*y*y + y_0;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            *res = count_iters;
            return;
        }
    }

    *res = 0;
    return;
}

__kernel void
drawMandelbrod3 (__global struct Vertex* verts,
                 float scale_factor,
                 float shift_x,
                 float shift_y,
                 int count_iters)
{
    const uint ix = get_global_id (0);
    const uint iy = get_global_id (1);

    float x_0 = scale_factor * ix + shift_x;
    float y_0 = scale_factor * iy + shift_y;

    calcMandelbrod3 (x_0, y_0, count_iters, &count_iters);

    // count_iters *= 255;

    struct Vertex vertex = {
        .pos = {
            .x = ix,
            .y = iy
        },

        .color = {
            .r = 255 * count_iters * 0.0,
            .g = 255 * count_iters * 0.0,
            .b = 255 * count_iters * 0.9,
            .a = 255
        }
    };

    verts[ix * get_global_size (1) + iy] = vertex;
}