#ifndef CORE_H
#define CORE_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <stddef.h>
#include "math_ext.h"
#include "optlib.h"
#include "render_defs.h"
#include "mesh_loader.h"

#define MAX_CORE_THREADS 10
#define CORE_WINDOW_HANDLE 1
#define CORE_WINDOW_RESOLUTION 2
#define CORE_CURRENT_RENDER_QUEUE 3
#define CORE_SET_TITLE 1
#define CORE_SET_HEIGHT 2
#define CORE_SET_WIDTH 3

typedef struct Mouse_actions
{
    int action_id;
    int x;
    int y;
} m_action_t;

typedef union core_attribute
{
    SDL_Window* window_id;
    unsigned int resolution[2];
    int current_render_queue_id;
} core_attr_t;

pthread_t core_init(void);
core_attr_t get_core_attribute(int attribute);
rdr_t* core_get_render_data(void);
void set_core_attribute(int attribute, void* value);
void core_load_render_data(rdr_t* render_data);
void core_attach_mouse_routine(void (*fun)(m_action_t*));
void core_attach_render_routine(void (*fun)(GLuint*));
void core_attach_keyboard_routine(void (*fun)(SDL_KeyboardEvent));
void core_start(void);
void core_stop(void);
int core_check_state(void);

#endif // CORE_H
