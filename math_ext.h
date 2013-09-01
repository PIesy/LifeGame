#ifndef MATH_EXT_H
#define MATH_EXT_H

#include <math.h>

#define ctg(x) ((1.0)/(tan(x)))

typedef struct matrix
{
    float data[4][4];
} mx_t;

float to_rad(float deg);
float to_deg(float rad);
mx_t multiply_matrix(mx_t* a,mx_t* b);
mx_t to_new_coord_system(mx_t* source, float x, float y, float z);
mx_t rotate_about_x(mx_t* source, float angle);
mx_t rotate_about_y(mx_t* source, float angle);
mx_t rotate_about_z(mx_t* source, float angle);
mx_t scale(mx_t* source, float x, float y, float z);
mx_t projection(float fov, float aspect, float near_plane, float far_plane);

static const mx_t E = {{{1,0,0,0},
                       {0,1,0,0},
                       {0,0,1,0},
                       {0,0,0,1}}};
static const mx_t NULL_M = {{{0,0,0,0},
                            {0,0,0,0},
                            {0,0,0,0},
                            {0,0,0,0}}};
static const double pi = 3.14159265359;
#endif // MATH_EXT_H
