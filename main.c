#include "core.h"

const char* words = "This is SPARTAAAAAAA";
int start = 0;

struct Life_game_grid
{
    short previous;
    short current;
} grid[30][50];

rdr_t *setup_render_data(void);
void setup_program(GLuint *program_id);
obj_t* setup_object(obj_t *source);
vertex* load_vertex_data(void);
void grid_handler(m_action_t* mouse);
void start_calculating_life_game(SDL_KeyboardEvent key);
void refresh_grid(void);
void life_game(void);
int grid_check_circular(int row, int column);

int main(int argc, char** argv)
{
    pthread_t core_thread;
    core_attr_t core_info;
    rdr_t* render_data;
    int width = 1000, height = 600;

    core_thread = core_init();
    set_core_attribute(CORE_SET_TITLE, (void*)words);
    set_core_attribute(CORE_SET_WIDTH, &width);
    set_core_attribute(CORE_SET_HEIGHT, &height);
    core_attach_render_routine(setup_program);
    core_attach_mouse_routine(grid_handler);
    core_attach_keyboard_routine(start_calculating_life_game);
    core_start();
    render_data = setup_render_data();
    core_load_render_data(render_data);
    core_info = get_core_attribute(CORE_WINDOW_RESOLUTION);
    fprintf (stderr,"Current resolution %dx%d\n", core_info.resolution[0], core_info.resolution[1]);
    while (core_check_state())
    {
        if (start)
            life_game();
        SDL_Delay(300);
    }
    pthread_join(core_thread, NULL);
    return 0;
}

rdr_t* setup_render_data(void)
{
//    vertex* vertices;
//    obj_t* render_object;
    rdr_t* render_data;
    obj_t* square;

    render_data = (rdr_t*)calloc(1, sizeof(rdr_t));
    render_data->objects = (obj_t**)calloc(1, sizeof(obj_t*));
    render_data->asociation = (short*)calloc(1, sizeof(short));
    square = load_mesh("square.txt");
    render_data->objects[0] = setup_object(square);
    render_data->asociation[0] = 0;
    render_data->draw_calls = 1;
    return render_data;
}

void setup_program(GLuint* program_id)
{
    GLuint shaders[2];

    fprintf (stderr, "This routine was called\n");

    shaders[1] = create_shader("FragmentShader.glsl",GL_FRAGMENT_SHADER);
    shaders[0] = create_shader("VertexShader.glsl",GL_VERTEX_SHADER);
    program_id[0] = create_gl_program(shaders, 2, GL_TRUE);
}

vertex* load_vertex_data(void)
{
    vertex* vertices;
    float first[4] = {-1,-1,0,1}, second[4] = {-1,1,0,1}, third[4] = {1,1,0,1}, fourth[4] = {1,-1,0,1};
    float color1[4] = {1,0,0,1}, color2[4] = {0,1,0,1}, color3[4] = {0,0,1,1}, color4[4] = {1,0,1,1};

    vertices = (vertex*)calloc(4,sizeof(vertex));
    memcpy(vertices[0].coord, first, 4 * sizeof(float));
    memcpy(vertices[0].color, color1, 4 * sizeof(float));
    memcpy(vertices[1].coord, second, 4 * sizeof(float));
    memcpy(vertices[1].color, color2, 4 * sizeof(float));
    memcpy(vertices[2].coord, third, 4 * sizeof(float));
    memcpy(vertices[2].color, color3, 4 * sizeof(float));
    memcpy(vertices[3].coord, fourth, 4 * sizeof(float));
    memcpy(vertices[3].color, color4, 4 * sizeof(float));
    return vertices;
}

obj_t* setup_object(obj_t* source)
{
    obj_t* object;
    int i, j;
    float offset_x, offset_y;

    object = (obj_t*)calloc(1, sizeof(obj_t));
    object->vertices = (vertex*)calloc(6000, sizeof(vertex));
    object->indices = (unsigned short*)calloc(9000, sizeof(short));
    object->vertex_count = 0;
    object->indices_count = 0;
    object->converted = 0;

    for (i = 0; i < 30; i++)
    {
        for (j = 0; j < 50; j++)
        {
            append_to_object(object, source, offset_x, offset_y);
            offset_x += 20;
        }
        offset_y += 20;
        offset_x = 0;
    }
    return object;
}

void grid_handler(m_action_t *mouse)
{
    int x, y;
    if (mouse->action_id == 11)
    {
        x = ceil((mouse->x)/20);
        y = ceil((mouse->y)/20);
        grid[y][x].current = 1 - grid[y][x].current;
        fprintf (stderr,"Grid item %d*%d state %d\n", y, x, grid[y][x].current);
        refresh_grid();
    }
}

void refresh_grid(void)
{
    int i, j;
    rdr_t* render_data = core_get_render_data();

    for (i = 0; i < 30; i++)
        for (j = 0; j < 50; j++)
            if (grid[29 - i][j].current)
            {
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 2].color[1] = 0.0;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 2].color[0] = 1.0;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 1].color[1] = 0.0;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 1].color[0] = 1.0;
            }
            else
            {
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 2].color[1] = 0.5;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 2].color[0] = 0.0;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 1].color[1] = 0.5;
                render_data->objects[0]->vertices[i * 200 + (j + 1) * 4 - 1].color[0] = 0.0;
            }
    core_load_render_data(render_data);
}

void life_game(void)
{
    static int i, j, neighbours_n;

    for (i = 0; i < 30; i++)
        for (j = 0; j < 50; j++)
            grid[i][j].previous = grid[i][j].current;
    for (i = 0; i < 30; i++)
        for (j = 0; j < 50; j++)
        {
            neighbours_n = grid_check_circular(i, j);
            if (grid[i][j].previous == 0)
                if (neighbours_n == 3)
                    grid[i][j].current = 1;
            if (grid[i][j].previous == 1)
                if (neighbours_n < 2 || neighbours_n > 3)
                    grid[i][j].current = 0;
        }
    refresh_grid();
}

int grid_check_circular(int y, int x)
{
    int neighbours_n = 0;
    int x_c, y_c, i, j, width = 50, height = 30;
    for (i = -1; i < 2; i++)
        for (j = -1; j < 2; j++)
        {
            x_c = x + i;
            y_c = y + j;
            if (x_c < 0)
                x_c = width - 1;
            if (x_c > width - 1)
                x_c = 0;
            if (y_c < 0)
                y_c = height - 1;
            if (y_c > height - 1)
                y_c = 0;
            if (grid[y_c][x_c].previous == 1)
                neighbours_n ++;
        }
    neighbours_n = neighbours_n - grid[y][x].previous;
    return neighbours_n;
}

void start_calculating_life_game(SDL_KeyboardEvent key)
{
    if (key.keysym.scancode == SDL_SCANCODE_SPACE && key.type == SDL_KEYUP)
    {
        start = 1 - start;
        if (start)
            fprintf (stderr, "Started\n");
        else
            fprintf (stderr, "Stopped\n");
    }
}
