#include "render_queue.h"

static short queues_allocated = 0;

int create_render_queue(void)
{
    int queue_id = -1;

    if (queues_allocated != MAX_QUEUES_COUNT)
    {
        queues[queues_allocated] = (rqueue_t*)calloc(1,sizeof(rqueue_t));
        queues_allocated ++;
        queue_id = queues_allocated - 1;
        queues[queue_id]->max_count = MAX_QUEUES_LENGTH;
        queues[queue_id]->count = 0;
        queues[queue_id]->threshold = 100;
        queues[queue_id]->current = NULL;
        queues[queue_id]->last = NULL;
    }
    return queue_id;
}

int delete_render_queue(int queue_id)
{
    int result;

    if (queue_id > queues_allocated)
        result = 0;
    else if (queues[queue_id - 1]->count != 0)
        result = -1;
    else
    {
        free(queues[queue_id - 1]);
        queues_allocated --;
        result = 1;
    }
    return result;
}

int render_queue_enqueue(rdr_t *data, int queue_id)
{
    queue_object* object;

    queue_id ++;
    if (queue_id > queues_allocated)
        return -1;
    queue_id --;
    if (queues[queue_id]->count == queues[queue_id]->max_count)
        return 0;
    object = (queue_object*)calloc(1,sizeof(queue_object));
    object->data = data;
    object->next = NULL;
    if (queues[queue_id]->count == 0)
    {
        queues[queue_id]->current = object;
        queues[queue_id]->last = object;
        queues[queue_id]->count ++;
    }
    else
    {
        queues[queue_id]->last->next = object;
        queues[queue_id]->last = object;
        queues[queue_id]->count ++;
    }
    return 1;
}

int render_queue_dequeue(rdr_t **container, int queue_id)
{
    queue_object* temp;

    queue_id ++;
    if (queue_id > queues_allocated)
        return -1;
    queue_id --;
    if (queues[queue_id]->count == 0)
        return 0;
    if (queues[queue_id]->count == 1)
    {
        *container = queues[queue_id]->current->data;
        free(queues[queue_id]->current);
        queues[queue_id]->current = NULL;
        queues[queue_id]->last = NULL;
        queues[queue_id]->count --;
    }
    else
    {
        *container = queues[queue_id]->current->data;
        temp = queues[queue_id]->current->next;
        free(queues[queue_id]->current);
        queues[queue_id]->current = temp;
        queues[queue_id]->count --;
    }
    return 1;
}
