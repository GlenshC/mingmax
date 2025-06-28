#pragma once
#include <stdint.h>

#define RENDERER_MANUAL_FLUSH_UPDATE
#define MAX_SPRITES_A 100
#define MAX_SPRITES_S 100

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

typedef struct SpriteData_S{
    vec2_f          position[MAX_SPRITES_S];
    vec2_f          scale[MAX_SPRITES_S];
    float           rotation[MAX_SPRITES_S]; // 16
    SpriteIndexes   uv_tex_indexes[MAX_SPRITES_S]; // 20
    SpriteDataBits  bit_field[MAX_SPRITES_S]; // 20 
} SpriteData_S;

typedef struct SpriteBuffer_S {
    SpriteData_S instances;
    Sprite_S index_to_handle[MAX_SPRITES_S];
    uint16_t handle_to_index[MAX_SPRITES_S];
    Sprite_S free_handles[MAX_SPRITES_S];
    int free_count;
    int count;
} SpriteBuffer_S;

typedef struct SpriteData_A{
    vec2_f          position[MAX_SPRITES_A];
    vec2_f          scale[MAX_SPRITES_A];
    float           rotation[MAX_SPRITES_A]; // 16
    SpriteIndexes   uv_tex_indexes[MAX_SPRITES_A]; // 20
    SpriteDataBits  bit_field[MAX_SPRITES_A]; // 20 
} SpriteData_A;

typedef struct SpriteBuffer_A {
    SpriteData_A instances;
    Sprite_A index_to_handle[MAX_SPRITES_A];
    uint16_t handle_to_index[MAX_SPRITES_A];
    uint16_t uv_index[MAX_SPRITES_A];
    uint16_t uv_max_offset[MAX_SPRITES_A];
    Sprite_A free_handles[MAX_SPRITES_A];
    uint16_t tick_rate[MAX_SPRITES_A];
    uint16_t last_tick[MAX_SPRITES_A];
    int free_count;
    int count;
    // float frame_timer[MAX_SPRITES_A];
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