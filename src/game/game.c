#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/init.h"
#include "core/game.h"
#include "core/platform.h"
#include "core/clock.h"

#include "assets/atlas.h"
#include "renderer/renderer.h"
TextureAtlas spritesheet;
Shader shader;

Sprite_A sprite1;
uint16_t sprite1_index;

const ClockInit c_clock_init = { .tick_interval = 256 };


void update_fps_counter() {
    static double last_time = 0.0;
    static int frame_count = 0;
    double current_time = clock_get_raw_time();
    frame_count++;
    if (current_time - last_time >= 1.0) {
        printf("FPS: %-10d TPS: %d\n", clock_get_fps(),frame_count);
        frame_count = 0;
        last_time = current_time;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void) window, (void)scancode, (void)mods; 
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
    {
        // renderer2D_remove(sprite.handle);
    }
}

void game_init(void)
{
    glfwSetKeyCallback(p_window, key_callback);
    // platform_set_vsync(1);
    shader = shader_create("./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
    atlas_create(&spritesheet, "./assets/sprites/debug.png", 32, 32);

    printf("atlas: aw%.1f, ah%.1f, tw%d, th%d \n", spritesheet.atlas_width, spritesheet.atlas_height, 32, 32);
    glClearColor(.2f, .3f, .3f, 1.0f);

    sprite1 = renderer2D_create_anim();
    sprite1_index = get_index_sprite_a(sprite1);
    r_buffer_sprite_a.instances.scale[sprite1_index] = (vec2_f){0.5f,0.5f};
    r_buffer_sprite_a.instances.uv_tex_indexes[sprite1_index].uv_index = 0; 
    r_buffer_sprite_a.uv_index[sprite1_index] = 0;
    r_buffer_sprite_a.uv_max_offset[sprite1_index] = 10;
    r_buffer_sprite_a.tick_rate[sprite1_index] = 256/6;
    
    renderer2D_flush_anim();
}

void game_update(float dTime)
{
    (void)dTime;
    const float mv_speed = 5.0;
    float dX = 0, dY = 0;
    if (glfwGetKey(p_window, GLFW_KEY_W))
    {
        dY += 0.001f * mv_speed;
    }    
    if (glfwGetKey(p_window, GLFW_KEY_S))
    {
        dY -= 0.001f * mv_speed;
    }    
    if (glfwGetKey(p_window, GLFW_KEY_A))
    {
        dX -= 0.001f * mv_speed;
        r_buffer_sprite_a.instances.bit_field[sprite1_index].flip = 1;
    }    
    if (glfwGetKey(p_window, GLFW_KEY_D))
    {
        dX += 0.001f * mv_speed;
        r_buffer_sprite_a.instances.bit_field[sprite1_index].flip = 0;
    }
     
    if (dX != 0 || dY != 0)
    {
        r_buffer_sprite_a.instances.position[sprite1_index].x += dX;
        r_buffer_sprite_a.instances.position[sprite1_index].y += dY;
    }
    renderer2D_update_all_anim();
    update_fps_counter();
}

// TODO renderer*_draw must have shader parameter
// TODO renderer animation/static dynamic
void game_render(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    shader_bind(shader);
    atlas_bind(shader, &spritesheet, 0);
    shader_set_uniform_mat4(shader, "u_projection", GLM_MAT4_IDENTITY);

    renderer2D_flush_anim();
    renderer2D_draw_anim();
}

void game_shutdown(void)
{

}
