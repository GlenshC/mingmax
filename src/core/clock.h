#pragma once

#include "core/init/init_clock.h"
#include <stdint.h>
typedef struct ClockStateTime 
{
    double time_last;       // Last frame timestamp
    double time_now;        // Current timestamp
    double elapsed_time;    // Total time since clock_init() 
    double time_start;
    double tick_accumulator;
    float  delta_time;      // Time between last two frames (in seconds)
    uint16_t current_tick;
} ClockStateTime; 

typedef struct ClockStateFPS 
{
    double target_frame_time;
    double fps_timer;
    int fps;
    int frame_count;
    int target_fps;
} ClockStateFPS;

extern ClockStateTime c_time_state;
extern ClockStateFPS c_fps_state;

// Initialize clock system
void  clock_init(void);

// Call once per frame to update timing state
void  clock_update(void);

// Get delta time (time between current and previous frame) in seconds
float clock_get_delta_time(void);

// Get total elapsed time since engine start in seconds
double clock_get_time(void);

// Get current frames per second (FPS)
int   clock_get_fps(void);

// Set a target framerate cap (e.g., 60)
void  clock_set_target_fps(int fps);

// Get current target FPS value
int   clock_get_target_fps(void);

// Optionally wait to maintain target frame rate
void  clock_wait_for_frame_end(void);

void clock_tick(void);

double clock_get_raw_time(void);