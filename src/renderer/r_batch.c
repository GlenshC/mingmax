#include <stdio.h>

#include <stddef.h>
#include <stdint.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/clock.h"
#include "renderer/r_batch.h"

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

SpriteBuffer_S r_buffer_sprite_s = {};
SpriteBuffer_A r_buffer_sprite_a = {};

static GLuint vao, vbo_quad, vbo_instance, ibo;
static GLuint vao_anim, vbo_anim_instance;

// create
Sprite_A renderer2D_create_anim(void)
{
    Sprite_A index = r_buffer_sprite_a.count++; 
    Sprite_A handle = index;

    if (r_buffer_sprite_a.free_count >= 0)
        handle = r_buffer_sprite_a.free_handles[--(r_buffer_sprite_s.free_count)];

    r_buffer_sprite_a.handle_to_index[handle] = index;
    r_buffer_sprite_a.index_to_handle[index] = handle;
    return handle;
}

Sprite_S renderer2D_create_static(void)
{
    Sprite_S index = r_buffer_sprite_s.count++; 
    Sprite_S handle = index;

    if (r_buffer_sprite_s.free_count >= 0)
        handle = r_buffer_sprite_s.free_handles[--(r_buffer_sprite_s.free_count)];

    r_buffer_sprite_s.handle_to_index[handle] = index;
    r_buffer_sprite_s.index_to_handle[index] = handle;
    return handle;
}

// remove
void renderer2D_remove_anim(const Sprite_A handle)
{
    int index = r_buffer_sprite_a.handle_to_index[handle];
    int last_index = --r_buffer_sprite_a.count;

    if (index != last_index)
    {
        r_buffer_sprite_a.free_handles[r_buffer_sprite_a.free_count++] = (uint16_t)handle;
        r_buffer_sprite_a.instances[index] = r_buffer_sprite_a.instances[last_index];
        int last_handle = r_buffer_sprite_a.index_to_handle[last_index];
        r_buffer_sprite_a.handle_to_index[last_handle] = index;
        r_buffer_sprite_a.index_to_handle[index] = last_handle;

        #ifndef RENDERER_MANUAL_FLUSH_UPDATE
            glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_a.instances[index]);
        #endif
    }
}

void renderer2D_remove_static(const Sprite_S handle)
{
    int index = r_buffer_sprite_s.handle_to_index[handle];
    int last_index = --r_buffer_sprite_s.count;

    if (index != last_index)
    {
        r_buffer_sprite_s.free_handles[r_buffer_sprite_s.free_count++] = (uint16_t)handle;
        r_buffer_sprite_s.instances[index] = r_buffer_sprite_s.instances[last_index];
        int last_handle = r_buffer_sprite_s.index_to_handle[last_index];
        r_buffer_sprite_s.handle_to_index[last_handle] = index;
        r_buffer_sprite_s.index_to_handle[index] = last_handle;

        #ifndef RENDERER_MANUAL_FLUSH_UPDATE
            glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_s.instances[index]);
        #endif
    }
}

void renderer2D_update_all_anim(void)
{
    static double last_time = 0.0;
    double current_time = clock_get_time();
    if ((current_time - last_time) >= (1.0/2.0)) 
    {
        // printf("%lf\n", last_time);
        last_time = current_time;
        SpriteData *instances = r_buffer_sprite_a.instances;
        const uint16_t *uv_min = r_buffer_sprite_a.uv_index;
        const uint16_t *uv_max_offset = r_buffer_sprite_a.uv_max_offset;
        for (int index = 0; index < r_buffer_sprite_a.count; index++)
        {
            if ((++(instances[index].uv_index) - uv_min[index]) > uv_max_offset[index])
            {
                instances[index].uv_index =  uv_min[index];
            }
        }
        //
        #ifndef RENDERER_MANUAL_FLUSH_UPDATE
            glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
            glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(SpriteData), r_buffer_sprite_a.instances);
        #endif
    }

}

// update instance (single flush)
void renderer2D_push_static(const Sprite_S handle)
{
    int index = r_buffer_sprite_s.handle_to_index[handle];
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_s.instances[index]);
}

void renderer2D_push_anim(const Sprite_A handle)
{
    int index = r_buffer_sprite_a.handle_to_index[handle];
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_a.instances[index]);
    // sizeof(SpriteData);
    // sizeof(SpriteBuffer_A);
    // (void *)offsetof(SpriteBuffer_A, index_to_handle);
    // (void *)offsetof(SpriteBuffer_A, handle_to_index);
    // sizeof(SpriteBuffer_S);
}

// flush buffer
void renderer2D_flush()
{
    renderer2D_flush_static();
    renderer2D_flush_anim();
}

void renderer2D_flush_static(void)
{
    // glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(SpriteData), r_buffer_sprite_s.instances);
    // glBindVertexArray(0);
}

void renderer2D_flush_anim(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(SpriteData), r_buffer_sprite_a.instances);
}

// draw
void renderer2D_draw(void)
{
    renderer2D_draw_anim();
    renderer2D_draw_static();
}

void renderer2D_draw_static(void)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, r_buffer_sprite_s.count);

    glBindVertexArray(0);
}

void renderer2D_draw_anim(void)
{
    glBindVertexArray(vao_anim);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, r_buffer_sprite_a.count);

    glBindVertexArray(0);
}

// initialization
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

    // i_uv_index
    // i_texture_id
    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(SpriteData), (void*)offsetof(SpriteData, uv_index));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    
    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(SpriteData), (void*)(offsetof(SpriteData, bit_field)));
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

    // i_uv_index
    // i_texture_id
    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(SpriteData), (void*)offsetof(SpriteData, uv_index));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    
    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(SpriteData), (void*)(offsetof(SpriteData, bit_field)));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}
