#include <glad/gl.h>
#include "core/app.h"
#include "core/game.h"
#include "core/platform.h"
#include "core/clock.h"
#include "renderer/renderer.h"

void app_run(const char *title, int width, int height)
{
    platform_init(title, width, height);
    clock_init();
    renderer_init();

    game_init();

    while (!platform_should_close()) {
        clock_update();
        // input_update();
        const double delta_time = c_time_state.delta_time;
        while (c_time_state.tick_accumulator >= c_clock_init.tick_interval)
        {
            game_update(delta_time);
            c_time_state.tick_accumulator -= c_clock_init.tick_interval;
        }
        
        // platform_clear_screen(...);
        game_render();
        
        platform_swap_buffers();
        platform_poll_events();
    }

    // game_shutdown();
    // renderer_shutdown();
    platform_shutdown();
}

