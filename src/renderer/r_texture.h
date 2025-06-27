// File: r_texture.h

#pragma once

#include <stdint.h>

typedef unsigned int Texture;
/* typedef struct Texture {
    uint32_t texID;
    int width;
    int height;
} Texture; */

// #define tex2D_bind(tex, slot) texture2D_bind(tex->texID, slot)

Texture texture2D_load(const char* path, const uint32_t format, int *width, int *height);
void    texture2D_bind(const Texture texID, const uint32_t slot);
void    texture_free(Texture *tex);
