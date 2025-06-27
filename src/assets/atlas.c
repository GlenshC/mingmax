#include <stdio.h>
#include <glad/gl.h>

#include "assets/atlas.h"
#include "cglm/cglm.h"

static const char *atlas_slots[] = {
    "u_atlas_info[0]",
    "u_atlas_info[1]",
    "u_atlas_info[2]",
    "u_atlas_info[3]",
    "u_atlas_info[4]",
    "u_atlas_info[5]",
    "u_atlas_info[6]",
    "u_atlas_info[7]",
    "u_atlas_info[8]",
    "u_atlas_info[9]",
    "u_atlas_info[10]",
    "u_atlas_info[11]",
    "u_atlas_info[12]",
    "u_atlas_info[13]",
    "u_atlas_info[14]",
    "u_atlas_info[15]",
};

static const char *texture_slots[] = {
    "u_textures[0]",
    "u_textures[1]",
    "u_textures[2]",
    "u_textures[3]",
    "u_textures[4]",
    "u_textures[5]",
    "u_textures[6]",
    "u_textures[7]",
    "u_textures[8]",
    "u_textures[9]",
    "u_textures[10]",
    "u_textures[11]",
    "u_textures[12]",
    "u_textures[13]",
    "u_textures[14]",
    "u_textures[15]",
};

void atlas_create(TextureAtlas * atlas, const char *image_path, const int tile_width, const int tile_height)
{
    int atlas_width, atlas_height;
    atlas->texture = texture2D_load(image_path, GL_RGBA, &atlas_width, &atlas_height);
    atlas->tile_count = (atlas_width/tile_width) * (atlas_height/tile_height);

    atlas->tile_width = tile_width;
    atlas->tile_height = tile_height;
    atlas->atlas_width = atlas_width;
    atlas->atlas_height = atlas_height;    
}

void atlas_bind(Shader shader, TextureAtlas *atlas, const uint32_t slot)
{
    texture2D_bind(atlas->texture, slot);
    shader_set_uniform_int(shader, texture_slots[slot], slot);
    shader_set_uniform_vec4(shader, atlas_slots[slot], (float*)atlas);
}

void atlas_destroy(TextureAtlas* atlas)
{
    texture_free(&atlas->texture);
    atlas->tile_count = 0;
}