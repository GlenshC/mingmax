#include "core/clock.h"
#include "core/platform.h"

ClockStateTime c_time_state = {};
ClockStateFPS c_fps_state = {};
// Initializes the clock system (call once at startup)
void clock_init(void)
{
    c_time_state.time_start = platform_get_time();
    c_time_state.time_now = platform_get_time();
    c_time_state.time_last = c_time_state.time_now;
}

// Call once every frame to update timing
void clock_update(void)
{
    c_time_state.time_last = c_time_state.time_now;
    c_time_state.time_now = platform_get_time(); 
    c_time_state.delta_time = c_time_state.time_now - c_time_state.time_last;
    c_time_state.tick_accumulator += c_time_state.delta_time;
    c_time_state.elapsed_time = c_time_state.time_now - c_time_state.time_start;


    c_fps_state.fps_timer += c_time_state.delta_time;
    c_fps_state.frame_count++;

    if (c_fps_state.fps_timer >= 1.0) {
        c_fps_state.fps = c_fps_state.frame_count;
        c_fps_state.frame_count = 0;
        c_fps_state.fps_timer = 0.0;
    }
}

int clock_tick(void)
{
    return (c_time_state.tick_accumulator >= c_clock_init.tick_interval) ? 
        (c_time_state.tick_accumulator -= c_clock_init.tick_interval, 1) : 0;
}

// Returns delta time in seconds (time between last two frames)
float clock_get_delta_time(void)
{
    return c_time_state.delta_time;
}

// Returns total elapsed time since engine start (in seconds)
double clock_get_time(void)
{
    return c_time_state.elapsed_time;
}

// Returns FPS (frames per second)
int clock_get_fps(void)
{
    return c_fps_state.fps;
}

void clock_set_target_fps(int fps) {
    c_fps_state.target_fps = fps;
    c_fps_state.target_frame_time = fps > 0 ? 1.0 / (double)fps : 0.0;
}

int clock_get_target_fps(void) {
    return c_fps_state.target_fps;
}

void clock_wait_for_frame_end(void) {
    if (c_fps_state.target_fps <= 0) return;

    double end_time = platform_get_time();
    double frame_duration = end_time - c_time_state.time_now;

    double delay = c_fps_state.target_frame_time - frame_duration;
    if (delay > 0) {
        platform_sleep(delay); // Use glfwWaitEventsTimeout or similar
    }
}
