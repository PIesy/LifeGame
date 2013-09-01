#ifndef RENDERER_H
#define RENDERER_H

#include "core_defs.h"
#include "render_defs.h"
#include "render_queue.h"

void core_gl_init(core_t* core_data);
void core_gl_fetch_data(core_t* core_data);
void core_gl_draw(core_t* core_data);
#endif // RENDERER_H
