#include "buffer.h"

bool buffer_push(struct Buffer *buf, const void *item, int *id) {
    if (buf == NULL) {
        fprintf(stderr, "Error: Buffer struct is NULL.\n");
        return false;
    }
    if (item == NULL) {
        fprintf(stderr, "Error: Item is NULL.\n");
        return false;
    }

    int index = 0;
    while (index < buf->cap) {
        bool in_use = *((bool *)((char *)buf->data + index * buf->elem_size));
        if (!in_use) {
            break;
        }
        index++;
    }

    if (index == buf->cap) {
        if (buf->inc_size <= 0) {
            fprintf(stderr, "Error: Buffer Increment Size %i is invalid.",
                    buf->inc_size);
            return false;
        }
        if (buf->elem_size <= 0) {
            fprintf(stderr, "Error: Buffer Element Size %i is invalid.",
                    buf->inc_size);
            return false;
        }
        void *new_data = calloc((size_t)buf->cap + (size_t)buf->inc_size,
                                (size_t)buf->elem_size);
        if (new_data == NULL) {
            fprintf(stderr, "Error in calloc of new data.\n");
            return false;
        }

        if (buf->data) {
            memcpy(new_data, buf->data, (size_t)(buf->cap * buf->elem_size));
            free(buf->data);
        }

        buf->data = new_data;
        buf->cap += buf->inc_size;
    }

    void *dest = (char *)buf->data + index * buf->elem_size;
    memcpy(dest, item, (size_t)buf->elem_size);

    *id = index;

    return true;
}

void buffer_free(struct Buffer *buf, int id) {
    void *item = (char *)buf->data + id * buf->elem_size;
    memset(item, 0, (size_t)buf->elem_size);
}

bool buffer_check_id(struct Buffer *buf, int id) {
    return *((bool *)((char *)buf->data + id * buf->elem_size));
}
