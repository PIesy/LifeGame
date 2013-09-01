#include "math_ext.h"

float to_deg(float rad)
{
    return (rad/pi)*180.0;
}

float to_rad(float deg)
{
    return (deg/180.0)*pi;
}

mx_t multiply_matrix(mx_t *a, mx_t *b)
{
    mx_t result;
    int i = 0, j = 0;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result.data[i][j] =
                a->data[i][0] * b->data[0][j] +
                a->data[i][1] * b->data[1][j] +
                a->data[i][2] * b->data[2][j] +
                a->data[i][3] * b->data[3][j];
    return result;
}

mx_t to_new_coord_system(mx_t *source, float x, float y, float z)
{
    mx_t translation = E;
    translation.data[0][3] = x;
    translation.data[1][3] = y;
    translation.data[2][3] = z;
    return multiply_matrix(source, &translation);
}

mx_t scale(mx_t *source, float x, float y, float z)
{
    mx_t translation = E;
    translation.data[0][0] = x;
    translation.data[1][1] = y;
    translation.data[2][2] = z;
    return multiply_matrix(source, &translation);
}

mx_t rotate_about_x(mx_t *source, float angle)
{
    mx_t rotation = E;
    float cos_x = cos(angle), sin_x = sin(angle);
    rotation.data[1][1] = cos_x;
    rotation.data[1][2] = -sin_x;
    rotation.data[2][1] = sin_x;
    rotation.data[2][1] = -cos_x;
    return multiply_matrix(source, &rotation);
}

mx_t rotate_about_y(mx_t *source, float angle)
{
    mx_t rotation = E;
    float cos_x = cos(angle), sin_x = sin(angle);
    rotation.data[0][0] = cos_x;
    rotation.data[0][2] = sin_x;
    rotation.data[2][0] = -sin_x;
    rotation.data[2][2] = cos_x;
    return multiply_matrix(source, &rotation);
}

mx_t rotate_about_z(mx_t *source, float angle)
{
    mx_t rotation = E;
    float cos_x = cos(angle), sin_x = sin(angle);
    rotation.data[0][0] = cos_x;
    rotation.data[0][1] = -sin_x;
    rotation.data[1][0] = sin_x;
    rotation.data[1][1] = -cos_x;
    return multiply_matrix(source, &rotation);
}

mx_t projection(float fov, float aspect, float near_plane, float far_plane)
{
    mx_t out = NULL_M;
    float y_scale = ctg((to_rad(fov/2))),
        x_scale = y_scale / aspect,
        frustum_length = far_plane - near_plane;
    out.data[0][0] = x_scale;
    out.data[1][1] = y_scale;
    out.data[2][3] = -((far_plane + near_plane) / frustum_length);
    out.data[3][3] = -1;
    out.data[2][4] = -((2 * near_plane * far_plane) / frustum_length);
    return out;
}
