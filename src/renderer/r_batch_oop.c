#include <stdio.h>

#include <stddef.h>
#include <stdint.h>

#include <glad/gl.h>
#include "renderer/r_batch_oop.h"

static const float quad_vertices[] = {
    //  x      y     u     v
    -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.0f, 1.0f   // Top-left
};

static const uint32_t quad_indices[] = {
    0, 1, 2,
    2, 3, 0
};

struct {
    SpriteData instances[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    uint16_t index_to_handle[MAX_SPRITES];
    uint16_t free_handles[MAX_SPRITES];
    int free_count;
    int count;
} batch_static;

struct {
    SpriteData instances[MAX_SPRITES];
    uint16_t handle_to_index[MAX_SPRITES];
    uint16_t index_to_handle[MAX_SPRITES];
    uint16_t uv_min_index[MAX_SPRITES];
    uint16_t uv_max_index[MAX_SPRITES];
    uint16_t free_handles[MAX_SPRITES];
    int free_count;
    int count;
} batch_anim;

GLuint vao, vbo_quad, vbo_instance, ibo;
GLuint vao_anim, vbo_anim_instance;

void renderer2D_create_anim(const SpriteInstanceAnim *instance)
{
    int index = batch_anim.count++; 
    int handle = index;

    if (batch_anim.free_count >= 0)
        handle = batch_anim.free_handles[--(batch_static.free_count)];

    batch_anim.handle_to_index[handle] = index;
    batch_anim.index_to_handle[index] = handle;
    batch_anim.uv_min_index[index] = instance->uv_min_index;
    batch_anim.uv_max_index[index] = instance->uv_max_index;
    batch_anim.instances[index] = instance->sprite;
}

void renderer2D_create_static(const SpriteInstance *instance)
{
    int index = batch_static.count++; 
    int handle = index;

    if (batch_static.free_count >= 0)
        handle = batch_static.free_handles[--(batch_static.free_count)];

    batch_static.handle_to_index[handle] = index;
    batch_static.index_to_handle[index] = handle;
    batch_static.instances[index] = instance->sprite;
}

void renderer2D_remove(const int handle)
{
    int index = batch_static.handle_to_index[handle];
    int last_index = --batch_static.count;

    if (index != last_index)
    {
        batch_static.free_handles[batch_static.free_count++] = (uint16_t)handle;
        batch_static.instances[index] = batch_static.instances[last_index];
        int last_handle = batch_static.index_to_handle[last_index];
        batch_static.handle_to_index[last_handle] = index;
        batch_static.index_to_handle[index] = last_handle;

        glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
        glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &batch_static.instances[index]);
    }
}

void renderer2D_update_anim(SpriteInstanceAnim *instance)
{
    int index = batch_anim.handle_to_index[instance->handle];
    instance->sprite.uv_index++;
    if ((instance->sprite.uv_index) > instance->uv_max_index)
        instance->sprite.uv_index = instance->uv_min_index;

    batch_anim.instances[index] = instance->sprite;
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    // glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &batch_anim.instances[index]);
}

void renderer2D_update_static(const SpriteInstance *instance)
{
    int index = batch_static.handle_to_index[instance->handle];
    batch_static.instances[index] = instance->sprite;
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    // glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &batch_static.instances[index]);
}

void renderer2D_push_static(const int handle)
{
    int index = batch_static.handle_to_index[handle];
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &batch_static.instances[index]);
}

void renderer2D_flush()
{
    renderer2D_flush_static();
    renderer2D_flush_anim();
}

void renderer2D_flush_static(void)
{
    // glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch_static.count * sizeof(SpriteData), batch_static.instances);
    // glBindVertexArray(0);
}

void renderer2D_flush_anim(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch_anim.count * sizeof(SpriteData), batch_anim.instances);
}

void renderer2D_draw(void)
{
    renderer2D_draw_anim();
    renderer2D_draw_static();
}

void renderer2D_draw_static(void)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batch_static.count);

    glBindVertexArray(0);
}

void renderer2D_draw_anim(void)
{
    glBindVertexArray(vao_anim);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batch_anim.count);

    glBindVertexArray(0);
}

void renderer2D_batch_init(void)
{
    renderer2D_batch_init_static();    
    renderer2D_batch_init_anim();
}

void renderer2D_batch_init_anim(void)
{
    glGenVertexArrays(1, &vao_anim);
    glBindVertexArray(vao_anim);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);

    // a_pos (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // a_uv (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Index buffer ---
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // --- Instance buffer (dynamic) ---
    glGenBuffers(1, &vbo_anim_instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(SpriteData), NULL, GL_DYNAMIC_DRAW);

    // i_position
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, position));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // i_scale
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, scale));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // i_rotation
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, rotation));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // i_texture_id
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, texture_id));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    // i_uv_index
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, uv_index));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void renderer2D_batch_init_static(void)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // --- Static quad buffer (shared) ---
    glGenBuffers(1, &vbo_quad);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // a_pos (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // a_uv (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Index buffer ---
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    // --- Instance buffer (dynamic) ---
    glGenBuffers(1, &vbo_instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(SpriteData), NULL, GL_DYNAMIC_DRAW);

    // i_position
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, position));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // i_scale
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, scale));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // i_rotation
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, rotation));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // i_texture_id
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, texture_id));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    // i_uv_index
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteData), (void*)offsetof(SpriteData, uv_index));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}
