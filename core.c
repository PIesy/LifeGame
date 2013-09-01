#include "core_defs.h"

pthread_t core_init(void)
{
    int* render_queue_id;
    pthread_t main_thread;

    render_queue_id = (int*)calloc(1,sizeof(int));
    SDL_Init(SDL_INIT_VIDEO);
    *render_queue_id = create_render_queue();

    pthread_create(&main_thread, NULL, core_thread, render_queue_id);
    return main_thread;
}

void core_data_init(core_t* core_data)
{
    core_data->graphics_data.window_data.title = NULL;
    core_data->graphics_data.window_data.width = 800;
    core_data->graphics_data.window_data.height = 600;
    core_data->exit_flags.keyboard_handler_term = 0;
    core_data->exit_flags.mouse_handler_term = 0;
    core_data->exit_flags.render_term = 0;
    core_data->exit_flags.terminate = 0;
    core_data->sync.keyboard_ready = 1;
    core_data->sync.mouse_ready = 1;
    core_data->sync.window_ready = 1;
    core_data->events.event_flags.new_event = 0;
    core_data->events.event_flags.new_keyboard_event = 0;
    core_data->events.event_flags.new_mouse_event = 0;
    core_data->events.event_flags.new_window_event = 0;
    core_data->sideloaded_routines.keyboard_routine = NULL;
    core_data->sideloaded_routines.mouse_routine = NULL;
    core_data->sideloaded_routines.window_routine = NULL;
    core_data->sideloaded_routines.render_routine = NULL;
    core_data->sideloaded_routines.rroutine_set = 0;
    core_data->sideloaded_routines.wroutine_set = 0;
    core_data->sideloaded_routines.mroutine_set = 0;
    core_data->sideloaded_routines.kroutine_set = 0;
    core_data->start = 0;
}

void* core_thread(void* render_queue_id)
{
    core_t core_data;
    wdata_t* window = &core_data.graphics_data.window_data;
    pthread_t* render_thread = &(core_data.core_thread_pool[1]);
    pthread_t* window_thread = &(core_data.core_thread_pool[2]);
    pthread_t* keyboard_thread = &(core_data.core_thread_pool[3]);
    pthread_t* mouse_thread = &(core_data.core_thread_pool[4]);
    pthread_t* service_thread = &(core_data.core_thread_pool[5]);

    core_data.graphics_data.window_data.current_render_queue = *((int*)render_queue_id);
    free(render_queue_id);
    core_data_init(&core_data);
    pthread_create(service_thread, NULL, request_handler, &core_data);
    while(!core_data.start)
        SDL_Delay(100);
    window->window_id = SDL_CreateWindow(window->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window->width, window->height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    pthread_create(window_thread, NULL, window_handler, &core_data);
    pthread_create(render_thread, NULL, render, &core_data);
    pthread_create(keyboard_thread, NULL, keyboard_handler, &core_data);
    pthread_create(mouse_thread, NULL, mouse_handler, &core_data);

    core_loop(&core_data);

    pthread_join(*keyboard_thread, NULL);
    pthread_join(*mouse_thread, NULL);
    pthread_join(*service_thread, NULL);
    pthread_join(*window_thread, NULL);
    return 0;
}

void core_loop(core_t *core_data)
{
    int exit = 0;
    SDL_Event temp_event;
    SDL_Event* window_event = &(core_data->events.window_event);
    SDL_Event* keyboard_event = &(core_data->events.keyboard_event);
    SDL_Event* mouse_event = &(core_data->events.mouse_event);
    ev_flags_t* event = &(core_data->events.event_flags);

    while (!exit)
    {
        event->new_event = SDL_PollEvent(&temp_event);
        if (event->new_event)
            switch (temp_event.type){
            case SDL_WINDOWEVENT:
                if (core_data->sync.window_ready)
                {
                    *window_event = temp_event;
                    core_data->sync.window_ready = 0;
                    event->new_window_event = 1;
                }
                break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                if (core_data->sync.keyboard_ready)
                {
                    *keyboard_event = temp_event;
                    core_data->sync.keyboard_ready = 0;
                    event->new_keyboard_event = 1;
                }
                break;
            case SDL_MOUSEMOTION:
            case SDL_MOUSEWHEEL:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                if (core_data->sync.mouse_ready)
                {
                    *mouse_event = temp_event;
                    core_data->sync.mouse_ready = 0;
                    event->new_mouse_event = 1;
                }
                break;
            }
        if (core_data->exit_flags.terminate == 1)
            exit = 1;
    }
}

void* window_handler(void *input)
{
    core_t* core_data = input;
    SDL_Event* window = &core_data->events.window_event;
    SDL_Event current_event;
    ev_flags_t* event = &core_data->events.event_flags;

    fprintf (stderr,"Window thread started\n");

    while(!core_data->exit_flags.terminate)
    {
        if (event->new_window_event)
        {
            current_event = *window;
            event->new_window_event = 0;
            core_data->sync.window_ready = 1;
            switch(current_event.window.event){
            case SDL_WINDOWEVENT_CLOSE:
                fprintf (stderr,"\nThis kingdom will fall!\n\n");
                core_data->exit_flags.terminate = 1;
                break;
            }
        }
        SDL_Delay(1);
    }
    pthread_join(core_data->core_thread_pool[1], NULL);
    SDL_DestroyWindow(core_data->graphics_data.window_data.window_id);
    return NULL;
}

void* render(void* input)
{
    core_t* core_data  = input;
    SDL_GLContext gl_context;

    fprintf (stderr,"Render thread started\n");

    setup_glcontext(&gl_context, core_data->graphics_data.window_data.window_id);
    core_gl_init(core_data);  
    if (core_data->sideloaded_routines.render_routine)
        core_data->sideloaded_routines.render_routine(core_data->graphics_data.gl_data.program_id);
    while(!core_data->exit_flags.terminate)
    {       
        core_gl_fetch_data(core_data);
        core_gl_draw(core_data);
        SDL_GL_SwapWindow(core_data->graphics_data.window_data.window_id);
        SDL_Delay(10);
    }
    SDL_GL_DeleteContext(gl_context);
    fprintf (stderr,"OpenGL context destroyed\n");
    core_data->exit_flags.render_term = 1;
    return NULL;
}

void setup_glcontext(SDL_GLContext* context, SDL_Window *window_id)
{
    GLenum glew_err;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    *context = SDL_GL_CreateContext(window_id);
    glewExperimental = 1;
    glew_err = glewInit();
    if (glew_err != GLEW_OK)
        fprintf (stderr,"Error %s\n",glewGetErrorString(glew_err));
}

void* request_handler(void *input)
{
    core_t* core_data = input;

    while(!core_data->exit_flags.terminate)
    {
        switch (data_request.mode){
        case 1:
            switch (data_request.attr_name){
            case CORE_WINDOW_RESOLUTION:
                ((core_attr_t*)data_request.data)->resolution[0] = core_data->graphics_data.window_data.width;
                ((core_attr_t*)data_request.data)->resolution[1] = core_data->graphics_data.window_data.height;
                break;
            case CORE_WINDOW_HANDLE:
                ((core_attr_t*)data_request.data)->window_id = core_data->graphics_data.window_data.window_id;
                break;
            case CORE_CURRENT_RENDER_QUEUE:
                ((core_attr_t*)data_request.data)->current_render_queue_id = core_data->graphics_data.window_data.current_render_queue;
                break;
            case RENDER_DATA:
                *(rdr_t**)data_request.data = core_data->graphics_data.gl_data.current_render_data;
                break;
            }
            break;
        case 2:
            switch (data_request.attr_name){
            case CORE_SET_TITLE:
                core_data->graphics_data.window_data.title = (char*)(data_request.data);
                break;
            case CORE_SET_WIDTH:
                core_data->graphics_data.window_data.width = *((int*)(data_request.data));
                break;
            case CORE_SET_HEIGHT:
                core_data->graphics_data.window_data.height = *((int*)(data_request.data));
                break;
            }
            break;
        case 3:
            switch (data_request.attr_name){
            case START:
                core_data->start = 1;
                break;
            case STOP:
                core_data->exit_flags.terminate = 1;
                break;
            }
            break;
        case 4:
            render_queue_enqueue((rdr_t*)data_request.data, core_data->graphics_data.window_data.current_render_queue);
            break;
        case 5:
            switch(data_request.attr_name){
            case RENDER_ROUTINE:
                core_data->sideloaded_routines.render_routine = data_request.fun;
                core_data->sideloaded_routines.rroutine_set = 1;
                break;
            case MOUSE_ROUTINE:
                core_data->sideloaded_routines.mouse_routine = data_request.fun;
                break;
            case KEYBOARD_ROUTINE:
                core_data->sideloaded_routines.keyboard_routine = data_request.fun;
                break;
            }
            break;
        }
        data_request.mode = -1;
        SDL_Delay(1);
    }
    data_request.mode = -2;
    fprintf (stderr,"Request thread terminated\n");
    return NULL;
}

void* keyboard_handler(void *input)
{
    core_t* core_data = input;
    SDL_Event* keyboard = &core_data->events.keyboard_event;
    SDL_Event current_event;
    ev_flags_t* event = &core_data->events.event_flags;

    fprintf (stderr,"Keyboard thread started\n");

    while(!core_data->exit_flags.terminate)
    {
        if (event->new_keyboard_event)
        {
            current_event = *keyboard;
            event->new_keyboard_event = 0;
            core_data->sync.keyboard_ready = 1;
            if (core_data->sideloaded_routines.keyboard_routine)
                core_data->sideloaded_routines.keyboard_routine(current_event.key);
            if (current_event.key.keysym.scancode == SDL_SCANCODE_Q)
            {
                fprintf (stderr,"\nDeath dispatched\n\n");
                core_data->exit_flags.terminate = 1;
            }
        }
        SDL_Delay(1);
    }
    core_data->exit_flags.keyboard_handler_term = 1;
    fprintf (stderr,"Keyboard handler terminated\n");
    return NULL;
}

void* mouse_handler(void *input)
{
    core_t* core_data = input;
    SDL_Event* mouse = &core_data->events.mouse_event;
    SDL_Event current_event;
    ev_flags_t* event = &core_data->events.event_flags;
    m_action_t mouse_action;

    fprintf (stderr,"Mouse thread started\n");

    while(!core_data->exit_flags.terminate)
    {
        if (event->new_mouse_event)
        {
            current_event = *mouse;
            event->new_mouse_event = 0;
            core_data->sync.mouse_ready = 1;
            mouse_action = mouse_actions(&current_event);
            if (core_data->sideloaded_routines.mouse_routine)
                core_data->sideloaded_routines.mouse_routine(&mouse_action);
            if (mouse_action.action_id == 23)
            {
                fprintf (stderr,"\nBombs dropped\n\n");
                core_data->exit_flags.terminate = 1;
            }
        }
        SDL_Delay(1);
    }
    core_data->exit_flags.mouse_handler_term = 1;
    fprintf (stderr,"Mouse handler terminated\n");
    return NULL;
}

m_action_t mouse_actions(SDL_Event *event)
{
    m_action_t mouse_action = {0,0,0};

    switch(event->type){
    case SDL_MOUSEMOTION:
        mouse_action.action_id += 10;
        mouse_action.x = event->motion.xrel;
        mouse_action.y = event->motion.yrel;
        break;
    case SDL_MOUSEBUTTONUP:
        mouse_action.action_id += 10;
    case SDL_MOUSEBUTTONDOWN:
        mouse_action.action_id += 10;
        mouse_action.x = event->button.x;
        mouse_action.y = event->button.y;
        switch(event->button.button){
        case SDL_BUTTON_LEFT:
            mouse_action.action_id += 1;
            break;
        case SDL_BUTTON_RIGHT:
            mouse_action.action_id += 2;
            break;
        case SDL_BUTTON_MIDDLE:
            mouse_action.action_id += 3;
            break;
        case SDL_BUTTON_X1:
            mouse_action.action_id += 4;
            break;
        case SDL_BUTTON_X2:
            mouse_action.action_id += 5;
            break;
        }
        break;
    case SDL_MOUSEWHEEL:
        mouse_action.action_id += 16;
        mouse_action.x = event->wheel.x;
        mouse_action.y = event->wheel.y;
    }
    return mouse_action;
}

void set_core_attribute(int attribute, void *value)
{
    data_request.data = value;
    data_request.attr_name = attribute;
    data_request.mode = 2;
    while (data_request.mode != -1);
}

core_attr_t get_core_attribute(int attribute)
{
    core_attr_t return_value;

    data_request.data = &return_value;
    data_request.attr_name = attribute;
    data_request.mode = 1;
    while (data_request.mode != -1);
    return return_value;
}

void core_start(void)
{
    data_request.attr_name = START;
    data_request.mode = 3;
    while (data_request.mode != -1);
}

void core_stop(void)
{
    data_request.mode = 3;
    data_request.attr_name = STOP;
    while (data_request.mode != -1);
}

void core_load_render_data(rdr_t *render_data)
{
    data_request.data = render_data;
    data_request.mode = 4;
    while (data_request.mode != -1);
}

void core_attach_render_routine(void (*fun)(GLuint *))
{
    data_request.fun = (*fun);
    data_request.attr_name = RENDER_ROUTINE;
    data_request.mode = 5;
    while (data_request.mode != -1);
}

void core_attach_mouse_routine(void (*fun)(m_action_t *))
{
    data_request.fun = (*fun);
    data_request.attr_name = MOUSE_ROUTINE;
    data_request.mode = 5;
    while (data_request.mode != -1);
}

void core_attach_keyboard_routine(void (*fun)(SDL_KeyboardEvent))
{
    data_request.fun = (*fun);
    data_request.attr_name = KEYBOARD_ROUTINE;
    data_request.mode = 5;
    while (data_request.mode != -1);
}

rdr_t* core_get_render_data(void)
{
    rdr_t* return_value;

    data_request.data = &return_value;
    data_request.attr_name = RENDER_DATA;
    data_request.mode = 1;
    while (data_request.mode != -1);
    return return_value;
}

int core_check_state(void)
{
    if (data_request.mode == -2)
        return 0;
    return 1;
}
