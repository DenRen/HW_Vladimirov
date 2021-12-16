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

void fillVertex (__global struct Vertex* vertex,
                 uint ix,
                 uint iy,
                 int count_iters)
{
    struct Vertex tmp = {
        .pos = {
            .x = ix,
            .y = iy
        },

        .color = {
            .r = 25.5 * count_iters * 0.1,
            .g = 25.5 * count_iters * 0.4,
            .b = 25.5 * count_iters * 0.9,
            .a = 255
        }
    };   

    *vertex = tmp;
}

inline void
calcMandelbrod2 (float x_0,
                float y_0,
                struct Vector2f C,
                int count_iters,
                int is_mand,
                int* res)
{
    float x = x_0, y = y_0;
    float tmp_x = 0, tmp_y = 0;

    x_0 *= is_mand;
    y_0 *= is_mand;

    while (count_iters--) {
        tmp_x = x * x - y * y + x_0 + C.x;
        tmp_y = 2 * x * y + y_0 + C.y;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            *res = count_iters;
            return;
        }
    }

    *res = 0;
    return;
} // calcMandelbrod2

__kernel void
drawMandelbrod2 (__global struct Vertex* verts,
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

    calcMandelbrod2 (x_0, y_0, C, count_iters, 1, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawMandelbrod

__kernel void
drawJulia2 (__global struct Vertex* verts,
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

    calcMandelbrod2 (x_0, y_0, C, count_iters, 0, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawJulia

inline void
calcMandelbrod3 (float x_0,
                 float y_0,
                 struct Vector2f C,
                 int count_iters,
                 int is_mand,
                 int* res)
{
    float x = x_0, y = y_0;
    float tmp_x = 0, tmp_y = 0;

    x_0 *= is_mand;
    y_0 *= is_mand;

    while (count_iters--) {
        tmp_x = x*x*x - 3 * x * y*y + x_0 + C.x;
        tmp_y = 3 * x*x * y - y*y*y + y_0 + C.y;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            *res = count_iters;
            return;
        }
    }

    *res = 0;
    return;
} // calcMandelbrod3

__kernel void
drawMandelbrod3 (__global struct Vertex* verts,
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

    calcMandelbrod3 (x_0, y_0, C, count_iters, 1, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawMandelbrod3

__kernel void
drawJulia3 (__global struct Vertex* verts,
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

    calcMandelbrod3 (x_0, y_0, C, count_iters, 0, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawJulia3

inline void
calcMandelbrod4 (float x_0,
                 float y_0,
                 struct Vector2f C,
                 int count_iters,
                 int is_mand,
                 int* res)
{
    float x = x_0, y = y_0;
    float tmp_x = 0, tmp_y = 0;

    x_0 *= is_mand;
    y_0 *= is_mand;

    while (count_iters--) {
        float x2 = x * x, y2 = y * y;

        tmp_x = x2 - y2;
        tmp_x = tmp_x * tmp_x - 4 * x2 * y2 + x_0 + C.x;
        tmp_y = 4 * x * y * (x2 - y2) + y_0 + C.y;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            *res = count_iters;
            return;
        }
    }

    *res = 0;
    return;
} // calcMandelbrod4

__kernel void
drawMandelbrod4 (__global struct Vertex* verts,
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

    calcMandelbrod4 (x_0, y_0, C, count_iters, 1, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawMandelbrod4

__kernel void
drawJulia4 (__global struct Vertex* verts,
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

    calcMandelbrod4 (x_0, y_0, C, count_iters, 0, &count_iters);

    fillVertex (&verts[ix * get_global_size (1) + iy], ix, iy, count_iters);
} // drawJulia4