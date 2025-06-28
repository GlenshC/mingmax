#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/init.h"
#include "core/game.h"
#include "core/platform.h"
#include "core/clock.h"

#include "assets/atlas.h"
#include "renderer/renderer.h"

#define SIM_NUMBERS (MAX_SPRITES_A - 1)

TextureAtlas spritesheet;
Shader shader;
// Sprite_A sprite1;
// uint16_t sprite1_index;

const ClockInit c_clock_init = { .tick_interval = 256 };
mat4 u_model = GLM_MAT4_IDENTITY_INIT;
vec3 s_position;

void update_fps_counter() {
    static double last_time = 0.0;
    static int frame_count = 0;
    double current_time = clock_get_raw_time();
    frame_count++;
    if (current_time - last_time >= 1.0) {
        printf("FPS: %-10d TPS: %-10d EA-count: %-10d ES-count: %-10d\n", 
            clock_get_fps(), frame_count, r_buffer_sprite_a.count, r_buffer_sprite_s.count);
        
        frame_count = 0;
        last_time = current_time;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)sizeof(r_buffer_sprite_a);
    (void) window, (void)scancode, (void)mods;
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
    {
        // renderer2D_remove(sprite.handle);
    }
}
void game_init(void)
{    
    glfwSetKeyCallback(p_window, key_callback);
    glClearColor(.2f, .3f, .3f, 1.0f);
    platform_set_vsync(0);
    shader = shader_create("./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
    atlas_create(&spritesheet, "./assets/sprites/debug.png", 32, 32);

    printf("atlas: aw%.1f, ah%.1f, tw%d, th%d \n", spritesheet.atlas_width, spritesheet.atlas_height, 32, 32);

    // sprite1 = renderer2D_create_anim();
    // sprite1_index = get_index_sprite_a(sprite1);
    // r_buffer_sprite_a.instances.scale[sprite1_index] = (vec2_f){0.5f,0.5f};
    // r_buffer_sprite_a.instances.uv_tex_indexes[sprite1_index].uv_index = 0;
    // r_buffer_sprite_a.uv_index[sprite1_index] = 0;
    // r_buffer_sprite_a.uv_max_offset[sprite1_index] = 10;
    // r_buffer_sprite_a.tick_rate[sprite1_index] = 256;

    for (int i = 0; i < SIM_NUMBERS; i++)
    {
        int s_index = i;
        (renderer2D_create_anim());
        r_buffer_sprite_a.instances.scale[s_index] = (vec2_f) {0.1f, 0.1f};
        r_buffer_sprite_a.instances.position[s_index] = (vec2_f) {fmod(0.1 * i, 2.0), fmod((floor((0.1 * i)) / 200.0), 2.0)};
    }

    renderer2D_flush_anim();
}

void game_update(float dTime)
{
    (void)dTime;
    const float mv_speed = 5.0;
    float dX = 0, dY = 0;
    // vec2_f *positions = r_buffer_sprite_a.instances.position;
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
    }
    if (glfwGetKey(p_window, GLFW_KEY_D))
    {
        dX += 0.001f * mv_speed;
    }

    if (dX != 0 || dY != 0)
    {
        s_position[0] += dX;
        s_position[1] += dY;
        glm_mat4_identity(u_model);
        glm_translate(u_model, s_position);
    //     for (int i = 0; i < SIM_NUMBERS + 1; i++)
    //     {
    //         positions[i].x += dX;
    //         positions[i].y += dY;
    //     }
    }
    // renderer2D_update_all_anim();
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
    shader_set_uniform_mat4(shader, "u_model", u_model);

    // renderer2D_flush_static();
    renderer2D_draw_anim();
}

void game_shutdown(void)
{

}
