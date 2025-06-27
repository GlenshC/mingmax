#pragma once

typedef struct GLFWwindow *GWindow;

extern GWindow p_window;
// Initialization and shutdown

int platform_init(const char* title, int width, int height);
void platform_shutdown(void);
// Main Loop Control

GWindow platform_get_window();
int  platform_should_close(void);
void platform_poll_events(void);
void platform_swap_buffers(void);
void platform_set_vsync(int enabled);
double platform_get_time();
void platform_sleep(double delay);

