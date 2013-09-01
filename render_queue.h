#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include "renderer.h"

#ifndef MAX_QUEUES_COUNT
#define MAX_QUEUES_COUNT 5
#endif

#ifndef MAX_QUEUES_LENGTH
#define MAX_QUEUES_LENGTH SHRT_MAX
#endif

typedef struct Queue_object
{
    rdr_t* data;
    struct Queue_object* next;
} queue_object;

typedef struct Render_queue
{
    queue_object* current;
    queue_object* last;
    short count;
    short max_count;
    short threshold;
} rqueue_t;

int create_render_queue(void);
int delete_render_queue(int queue_id);
int render_queue_enqueue(rdr_t* data, int queue_id);
int render_queue_dequeue(rdr_t **container, int queue_id);

rqueue_t* queues[MAX_QUEUES_COUNT];

#endif // RENDER_QUEUE_H
