#ifndef PARSER_H
#define PARSER_H

#include "game.h"

bool str_to_u8(const char *str, Uint8 *value, const char *orig_str);
bool str_to_i(const char *str, int *value, const char *orig_str);
bool str_to_ul(const char *str, unsigned long *value, const char *orig_str);
bool str_to_f(const char *str, float *value, const char *orig_str);
bool str_to_id(Buffer *buff, const char *str, int *index, const char *orig_str);
bool int_to_sval(SdlServer *s, int value);
bool float_to_sval(SdlServer *s, float value);
bool str_to_field(const char *str, RectField *field, const char *orig_str);
bool str_to_pos(const char *str, RectField *field, const char *orig_str);
bool str_to_flip(const char *str, SDL_RendererFlip *flip, const char *orig_str);
bool get_scancode(const char *input, SDL_Scancode *code, const char *orig_str);
bool parse_line(SdlServer *s);

#endif
