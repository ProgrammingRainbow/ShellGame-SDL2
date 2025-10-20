#ifndef BUFFER_H
#define BUFFER_H

#include "main.h"

typedef struct Buffer {
        union {
                void *data;
                struct Image *images;
                struct Rect *rects;
                struct Text *texts;
                struct Sprite *sprites;
                struct Sound *sounds;
                struct Music *music;
        };
        int elem_size;
        int inc_size;
        int cap;
} Buffer;

bool buffer_push(struct Buffer *buf, const void *item, int *id);
void buffer_free(struct Buffer *buf, int index);
bool buffer_check_id(struct Buffer *buf, int id);

#endif
