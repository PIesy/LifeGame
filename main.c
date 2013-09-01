#include "core.h"

const char* words = "This is SPARTAAAAAAA";

rdr_t *setup_render_data(void);
void setup_program(GLuint *program_id);
obj_t* setup_object(vertex* vertices);
vertex* load_vertex_data(void);

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
    core_start();
    render_data = setup_render_data();
    core_load_render_data(render_data);
    core_info = get_core_attribute(CORE_WINDOW_RESOLUTION);
    fprintf (stderr,"Current resolution %dx%d\n", core_info.resolution[0], core_info.resolution[1]);
    pthread_join(core_thread, NULL);
    return 0;
}

rdr_t* setup_render_data(void)
{
//    vertex* vertices;
//    obj_t* render_object;
    rdr_t* render_data;

    render_data = (rdr_t*)calloc(1, sizeof(rdr_t));
    render_data->objects = (obj_t**)calloc(1, sizeof(obj_t*));
    render_data->asociation = (short*)calloc(1, sizeof(short));
//    vertices = load_vertex_data();
//    render_object = setup_object(vertices);
    render_data->objects[0] = load_mesh("square.txt");
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

obj_t* setup_object(vertex *vertices)
{
    obj_t* object;
    unsigned short indices[6] = {0,1,2,0,2,3};

    object = (obj_t*)calloc(1, sizeof(obj_t));
    object->vertices = vertices;
    object->vertices_size = 4*sizeof(vertex);
    object->indices = (unsigned short*)calloc(6, sizeof(short));
    memcpy(object->indices, indices, 6 * sizeof(unsigned short));
    return object;
}
