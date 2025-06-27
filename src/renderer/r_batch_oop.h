#pragma once

#define MAX_SPRITES 1000

typedef struct SpriteData{
    float position[2];
    float scale[2];
    float rotation;
    float texture_id;
    float uv_index;
} SpriteData;

typedef struct SpriteInstance {
    SpriteData sprite;
    int handle;
} SpriteInstance;

// TODO pack values and bitwise unpack in gpu
// TODO make the atlas start on the top left

typedef struct SpriteInstanceAnim {
    SpriteData sprite;
    int handle;
    int uv_min_index;
    int uv_max_index;
} SpriteInstanceAnim;


void renderer2D_create_static(const SpriteInstance *instance);
void renderer2D_create_anim(const SpriteInstanceAnim *instance);

void renderer2D_update_static(const SpriteInstance *instance);
void renderer2D_update_anim(SpriteInstanceAnim *instance);
void renderer2D_remove(const int handle);

void renderer2D_push_static(const int handle);

void renderer2D_flush(void);
void renderer2D_flush_static(void);
void renderer2D_flush_anim(void);

void renderer2D_draw(void);
void renderer2D_draw_static(void);
void renderer2D_draw_anim(void);

void renderer2D_batch_init(void);
void renderer2D_batch_init_anim(void);
void renderer2D_batch_init_static(void);