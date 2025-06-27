// File: atlas.h

#pragma once

#include "renderer/r_texture.h"
#include "renderer/r_shader.h"

typedef struct TextureAtlas {
    float atlas_width;
    float atlas_height;
    float tile_width;
    float tile_height;
    int tile_count;
    Texture texture;
} TextureAtlas;

void atlas_bind(Shader shader, TextureAtlas *atlas, const uint32_t slot);
void atlas_create(TextureAtlas *atlas, const char* image_path, int tile_width, int tile_height);
void         atlas_destroy(TextureAtlas* atlas);

// Calculate UV min/max for a tile index (if needed outside shader)
// void atlas_get_uv(const TextureAtlas* atlas, int index, float* out_uv_min, float* out_uv_max);