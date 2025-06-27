#pragma once
#include <stdint.h>

#define RENDERER_MANUAL_FLUSH_UPDATE
#define MAX_SPRITES 1000

typedef uint16_t Sprite_S, Sprite_A;
// TODO pack values and bitwise unpack in gpu
// TODO make the atlas start on the top left

typedef struct SpriteDataBits{
    uint32_t 
    flip: 1;
} SpriteDataBits;

typedef struct SpriteData{
    float position[2]; // 0
    float scale[2]; // 8
    float rotation; // 16
    uint16_t uv_index; // 20
    uint16_t tex_index; // 20
    SpriteDataBits bit_field; // 20 
} SpriteData;

typedef struct SpriteBuffer_S {
    SpriteData instances[MAX_SPRITES];
    Sprite_S index_to_handle[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    Sprite_S free_handles[MAX_SPRITES];
    int free_count;
    int count;
} SpriteBuffer_S;

typedef struct SpriteBuffer_A {
    SpriteData instances[MAX_SPRITES];
    Sprite_A index_to_handle[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    uint16_t uv_index[MAX_SPRITES];
    uint16_t uv_max_offset[MAX_SPRITES];
    Sprite_A free_handles[MAX_SPRITES];
    float frame_timer[MAX_SPRITES];
    uint8_t frame_rate[MAX_SPRITES];
    uint16_t free_count;
    uint16_t count;
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