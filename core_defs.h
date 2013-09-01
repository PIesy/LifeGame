#ifndef CORE_DEFS_H
#define CORE_DEFS_H

typedef struct Core_data core_t;

#include "core.h"
#include "renderer.h"

#ifndef MAX_CORE_THREADS
#define MAX_CORE_THREADS 10
#endif
#ifndef MAX_GL_PROGRAMS
#define MAX_GL_PROGRAMS 2
#endif
#ifndef MAX_GL_SHADERS
#define MAX_GL_SHADERS 5
#endif
#define START 1
#define STOP 2
#define RENDER_ROUTINE 1
#define MOUSE_ROUTINE 2
#define KEYBOARD_ROUTINE 3
#define RENDER_DATA 8

typedef struct GL_data
{
    GLuint program_id[MAX_GL_PROGRAMS];
    GLuint shader_id[MAX_GL_SHADERS];
    GLuint VBO[5];
    GLuint IBO[5];
    GLuint UBO[5];
    GLuint VAO[5];
    int indices_count;
    rdr_t* current_render_data;
} gl_data_t;

typedef struct Window_data
{
    SDL_Window* window_id;
    int current_render_queue;
    char* title;
    int width;
    int height;
} wdata_t;

typedef struct Graphics_data
{
    gl_data_t gl_data;
    wdata_t window_data;
} vis_t;

typedef struct Event_flags
{
    int new_event;
    int new_mouse_event;
    int new_keyboard_event;
    int new_window_event;
} ev_flags_t;

typedef struct Events
{
    SDL_Event mouse_event;
    SDL_Event keyboard_event;
    SDL_Event window_event;
    ev_flags_t event_flags;
} ev_t;

typedef struct Input_sync
{
    int window_ready;
    int mouse_ready;
    int keyboard_ready;
} syncin_t;

typedef struct Exit_flags
{
    int terminate;
    int render_term;
    int keyboard_handler_term;
    int mouse_handler_term;
} term_t;

typedef struct Sideloaded_routines
{
    void (*window_routine)(SDL_WindowEvent);
    void (*mouse_routine)(m_action_t*);
    void (*keyboard_routine)(SDL_KeyboardEvent);
    void (*render_routine)(GLuint*);
    short wroutine_set;
    short mroutine_set;
    short kroutine_set;
    short rroutine_set;
} f_ptr_t;

typedef struct Core_data
{
    vis_t graphics_data;
    ev_t events;
    syncin_t sync;
    term_t exit_flags;
    pthread_t core_thread_pool[MAX_CORE_THREADS];
    f_ptr_t sideloaded_routines;
    int start;
} core_t;

void* core_thread(void* render_queue_id);
void* render(void* input);
void* keyboard_handler(void* input);
void* mouse_handler(void* input);
void* window_handler(void* input);
void* request_handler(void* input);
void core_data_init(core_t* core_data);
void core_loop(core_t* core_data);
void setup_glcontext(SDL_GLContext* context, SDL_Window* window_id);
m_action_t mouse_actions(SDL_Event* event);

struct Request_data
{
    int mode;
    int attr_name;
    void* data;
    void (*fun)(void*);
} data_request;

#endif // CORE_DEFS_H
