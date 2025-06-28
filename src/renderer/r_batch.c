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

static GLuint vao, vbo_quad, vbo_instance[5], ibo;
static GLuint vao_anim, vbo_anim_instance[5];

// create
Sprite_A renderer2D_create_anim(void)
{
    Sprite_A index = (Sprite_A)r_buffer_sprite_a.count++; 
    Sprite_A handle = index;

    if (r_buffer_sprite_a.free_count > 0)
        handle = r_buffer_sprite_a.free_handles[--(r_buffer_sprite_a.free_count)];

    r_buffer_sprite_a.handle_to_index[handle] = index;
    r_buffer_sprite_a.index_to_handle[index] = handle;
    return handle;
}

Sprite_S renderer2D_create_static(void)
{
    uint16_t index = (uint16_t)r_buffer_sprite_s.count++; 
    Sprite_S handle = (Sprite_S)index;

    if (r_buffer_sprite_s.free_count >= 0)
        handle = r_buffer_sprite_s.free_handles[--(r_buffer_sprite_s.free_count)];

    r_buffer_sprite_s.handle_to_index[handle] = index;
    r_buffer_sprite_s.index_to_handle[index] = handle;
    return handle;
    // return 1;
}

// remove
void renderer2D_remove_anim(const Sprite_A handle)
{
    int index = r_buffer_sprite_a.handle_to_index[handle];
    int last_index = --r_buffer_sprite_a.count;

    if (index != last_index)
    {
        r_buffer_sprite_a.free_handles[r_buffer_sprite_a.free_count++] = (uint16_t)handle;

        // r_buffer_sprite_a.instances[index] = r_buffer_sprite_a.instances[last_index];
        r_buffer_sprite_a.instances.position[index] = r_buffer_sprite_a.instances.position[last_index];
        r_buffer_sprite_a.instances.scale[index] = r_buffer_sprite_a.instances.scale[last_index];
        r_buffer_sprite_a.instances.rotation[index] = r_buffer_sprite_a.instances.rotation[last_index];
        r_buffer_sprite_a.instances.uv_tex_indexes[index] = r_buffer_sprite_a.instances.uv_tex_indexes[last_index];
        r_buffer_sprite_a.instances.bit_field[index] = r_buffer_sprite_a.instances.bit_field[last_index];
        
        int last_handle = r_buffer_sprite_a.index_to_handle[last_index];
        r_buffer_sprite_a.handle_to_index[last_handle] = (uint16_t)index;
        r_buffer_sprite_a.index_to_handle[index] = (Sprite_A)last_handle;

        #ifndef RENDERER_MANUAL_FLUSH_UPDATE
            glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_a.instances[index]);
        #endif
    }
}

void renderer2D_remove_static(const Sprite_S handle)
{
    // (void)handle;
    int index = r_buffer_sprite_s.handle_to_index[handle];
    int last_index = --r_buffer_sprite_s.count;

    if (index != last_index)
    {
        r_buffer_sprite_s.free_handles[r_buffer_sprite_s.free_count++] = (uint16_t)handle;

        r_buffer_sprite_s.instances.position[index] = r_buffer_sprite_s.instances.position[last_index];
        r_buffer_sprite_s.instances.scale[index] = r_buffer_sprite_s.instances.scale[last_index];
        r_buffer_sprite_s.instances.rotation[index] = r_buffer_sprite_s.instances.rotation[last_index];
        r_buffer_sprite_s.instances.uv_tex_indexes[index] = r_buffer_sprite_s.instances.uv_tex_indexes[last_index];
        r_buffer_sprite_s.instances.bit_field[index] = r_buffer_sprite_s.instances.bit_field[last_index];

        int last_handle = r_buffer_sprite_s.index_to_handle[last_index];
        r_buffer_sprite_s.handle_to_index[last_handle] = (uint16_t)index;
        r_buffer_sprite_s.index_to_handle[index] = (Sprite_S)last_handle;

        #ifndef RENDERER_MANUAL_FLUSH_UPDATE
            glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(SpriteData), sizeof(SpriteData), &r_buffer_sprite_s.instances[index]);
        #endif
    }
}

void renderer2D_update_all_anim(void)
{
    SpriteIndexes *instances = r_buffer_sprite_a.instances.uv_tex_indexes;
    const uint16_t *uv_min = r_buffer_sprite_a.uv_index;
    const uint16_t *uv_max_offset = r_buffer_sprite_a.uv_max_offset;
    const uint16_t *tick_rate = r_buffer_sprite_a.tick_rate;
    uint16_t *last_tick = r_buffer_sprite_a.last_tick;
    const uint16_t current_tick = c_time_state.current_tick;  
    uint16_t elapsed_ticks;  
    for (int index = 0; index < r_buffer_sprite_a.count; index++)
    {
        elapsed_ticks = current_tick - last_tick[index];
        if (elapsed_ticks >= tick_rate[index])
        {
            last_tick[index] = current_tick; 
            if ((++(instances[index].uv_index) - uv_min[index]) > uv_max_offset[index])
            {
                instances[index].uv_index = uv_min[index];
            }
        }
    }
#ifndef RENDERER_MANUAL_FLUSH_UPDATE
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(SpriteData), r_buffer_sprite_a.instances);
#endif

}

// update instance (single flush)
void renderer2D_push_static(const Sprite_S handle)
{
    // (void)handle;
    int index = r_buffer_sprite_s.handle_to_index[handle];

    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_POSITION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_s.instances.position), &r_buffer_sprite_s.instances.position[index]);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_SCALE]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_s.instances.scale), &r_buffer_sprite_s.instances.scale[index]);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_ROTATION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_s.instances.rotation), &r_buffer_sprite_s.instances.rotation[index]);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_UV_TEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_s.instances.uv_tex_indexes), &r_buffer_sprite_s.instances.uv_tex_indexes[index]);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_BIT_FIELD]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_s.instances.bit_field), &r_buffer_sprite_s.instances.bit_field[index]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer2D_push_anim(const Sprite_A handle)
{
    int index = r_buffer_sprite_a.handle_to_index[handle];

    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_POSITION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_a.instances.position), &r_buffer_sprite_a.instances.position[index]);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_SCALE]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_a.instances.scale), &r_buffer_sprite_a.instances.scale[index]);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_ROTATION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_a.instances.rotation), &r_buffer_sprite_a.instances.rotation[index]);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_UV_TEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_a.instances.uv_tex_indexes), &r_buffer_sprite_a.instances.uv_tex_indexes[index]);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_BIT_FIELD]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(r_buffer_sprite_a.instances.bit_field), &r_buffer_sprite_a.instances.bit_field[index]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_POSITION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(*r_buffer_sprite_s.instances.position), r_buffer_sprite_s.instances.position);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_SCALE]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(*r_buffer_sprite_s.instances.scale), r_buffer_sprite_s.instances.scale);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_ROTATION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(*r_buffer_sprite_s.instances.rotation), r_buffer_sprite_s.instances.rotation);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_UV_TEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(*r_buffer_sprite_s.instances.uv_tex_indexes), r_buffer_sprite_s.instances.uv_tex_indexes);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_BIT_FIELD]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_s.count * sizeof(*r_buffer_sprite_s.instances.bit_field), r_buffer_sprite_s.instances.bit_field);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer2D_flush_anim(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_POSITION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(*r_buffer_sprite_a.instances.position), r_buffer_sprite_a.instances.position);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_SCALE]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(*r_buffer_sprite_a.instances.scale), r_buffer_sprite_a.instances.scale);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_ROTATION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(*r_buffer_sprite_a.instances.rotation), r_buffer_sprite_a.instances.rotation);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_UV_TEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(*r_buffer_sprite_a.instances.uv_tex_indexes), r_buffer_sprite_a.instances.uv_tex_indexes);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_BIT_FIELD]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_buffer_sprite_a.count * sizeof(*r_buffer_sprite_a.instances.bit_field), r_buffer_sprite_a.instances.bit_field);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, r_buffer_sprite_s.count);

    glBindVertexArray(0);
}

void renderer2D_draw_anim(void)
{
    glBindVertexArray(vao_anim);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance);
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
    glGenBuffers(5, vbo_anim_instance);

    // i_position
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_POSITION]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_A * sizeof(vec2_f), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2_f), (void *)(0));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_SCALE]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_A * sizeof(vec2_f), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2_f), (void *)(0));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_ROTATION]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_A * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_UV_TEX]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_A * sizeof(SpriteIndexes), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(SpriteIndexes), (void *)(0));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_anim_instance[VBO_BIT_FIELD]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_A * sizeof(SpriteDataBits), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(SpriteDataBits), (void *)(0));
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
    glGenBuffers(5, vbo_instance);

    // i_position
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_POSITION]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_S * sizeof(vec2_f), NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2_f), (void *)(0));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // i_scale
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_SCALE]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_S * sizeof(vec2_f), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2_f), (void *)(0));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // i_rotation
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_ROTATION]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_S * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // i_uv_index
    // i_texture_id
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_UV_TEX]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_S * sizeof(SpriteIndexes), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(SpriteIndexes), (void *)(0));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    
    // bit_field
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance[VBO_BIT_FIELD]);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES_S * sizeof(SpriteDataBits), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(SpriteDataBits), (void *)(0));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}
