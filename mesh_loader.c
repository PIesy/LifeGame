#include "mesh_loader.h"

void read_mesh_data(obj_t *object, FILE* file);

obj_t* load_mesh(const char* file_path)
{
    FILE* file;
    obj_t* mesh_object;

    if (!(file = fopen(file_path, "r")))
    {
        fprintf (stderr, "Mesh file is unreachable.\n");
        return NULL;
    }
    mesh_object = (obj_t*)calloc(1,sizeof(obj_t));
    read_mesh_data(mesh_object, file);
    return mesh_object;
}

void read_mesh_data(obj_t *object, FILE *file)
{
    char buff[1000];
    int i = 0, a = 0;
    int end = 0;
    int type = 0;
    int count;

    while (!end)
    {
        fgets(buff, 1000, file);
        *(strchr(buff, '\n')) = '\0';
        if (buff[0] == '<')
        {
            if (!strcmp("<end>", buff))
                end = 1;
            i = 0;
            type ++;
            continue;
        }
        switch (type){
        case 1:
            count = atoi(buff);
            object->vertex_count = count;
            object->vertices = (vertex*)calloc(count, sizeof(vertex));
            break;
        case 2:
            sscanf(buff, "%f %f %f %f", &(object->vertices[i].coord[0]), &(object->vertices[i].coord[1]), &(object->vertices[i].coord[2]), &(object->vertices[i].coord[3]));
            i++;
            break;
        case 3:
            sscanf(buff, "%f %f %f %f", &(object->vertices[i].color[0]), &(object->vertices[i].color[1]), &(object->vertices[i].color[2]), &(object->vertices[i].color[3]));
            i++;
            break;
        case 4:
            count = atoi(buff);
            object->indices_count = count;
            object->indices = (unsigned short*)calloc(count, sizeof(short));
            break;
        case 5:
            while (sscanf(buff + i + a, "%hu", &(object->indices[i])) > 0)
            {
                a ++;
                i ++;
            }
            break;
        case 6:
            object->coord_space = atoi(buff);
            break;
        }
    }
}
