#pragma once
#include <stdint.h>

#define RENDERER_MANUAL_FLUSH_UPDATE
#define MAX_SPRITES 1000

#define VBO_POSITION    0
#define VBO_SCALE       1
#define VBO_ROTATION    2
#define VBO_UV_TEX      3
#define VBO_BIT_FIELD   4

typedef uint16_t Sprite_S, Sprite_A;
// TODO pack values and bitwise unpack in gpu
// TODO make the atlas start on the top left

typedef struct SpriteDataBits{
    uint32_t 
    flip: 1;
} SpriteDataBits;

typedef struct SpriteIndexes {
    uint16_t        uv_index;
    uint16_t        tex_index;
} SpriteIndexes;

typedef struct vec2_f {
    float x;
    float y;
} vec2_f;

typedef struct SpriteData{
    vec2_f          position[MAX_SPRITES];
    vec2_f          scale[MAX_SPRITES];
    float           rotation[MAX_SPRITES]; // 16
    SpriteIndexes   uv_tex_indexes[MAX_SPRITES]; // 20
    SpriteDataBits  bit_field[MAX_SPRITES]; // 20 
} SpriteData;

typedef struct SpriteBuffer_S {
    SpriteData instances;
    Sprite_S index_to_handle[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    Sprite_S free_handles[MAX_SPRITES];
    int free_count;
    int count;
} SpriteBuffer_S;

typedef struct SpriteBuffer_A {
    SpriteData instances;
    Sprite_A index_to_handle[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    uint16_t uv_index[MAX_SPRITES];
    uint16_t uv_max_offset[MAX_SPRITES];
    Sprite_A free_handles[MAX_SPRITES];
    uint16_t tick_rate[MAX_SPRITES];
    uint16_t last_tick[MAX_SPRITES];
    uint16_t free_count;
    uint16_t count;
    // float frame_timer[MAX_SPRITES];
    // uint8_t 
    // uint8_t 
} SpriteBuffer_A;

// access the sprites here
extern SpriteBuffer_S r_buffer_sprite_s;
extern SpriteBuffer_A r_buffer_sprite_a;

#define get_index_sprite_s(handle) r_buffer_sprite_s.handle_to_index[(handle)]
#define get_index_sprite_a(handle) r_buffer_sprite_a.handle_to_index[(handle)]

Sprite_S renderer2D_create_static(void);
Sprite_A renderer2D_create_anim(void);

void renderer2D_flush(void);
void renderer2D_flush_static(void);
void renderer2D_flush_anim(void);

void renderer2D_remove_static(const Sprite_S handle);
void renderer2D_remove_anim(const Sprite_A handle);

void renderer2D_update_all_anim(void);

void renderer2D_push_static(const Sprite_S handle);
void renderer2D_push_anim(const Sprite_A handle);

void renderer2D_draw(void);
void renderer2D_draw_static(void);
void renderer2D_draw_anim(void);

void renderer2D_batch_init(void);
void renderer2D_batch_init_anim(void);
void renderer2D_batch_init_static(void);