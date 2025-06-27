#pragma once

#include "renderer/r_shader.h" // expose shader to game layer
#include "renderer/r_batch.h"

// Initialize renderer and default state
void renderer_init(void);
void renderer2D_init(void);
// Clean up renderer
void renderer_shutdown(void);

// Per-frame render begin/end (optional for future batching)
void renderer_begin(void);
void renderer_end(void);

// Temporary: draw a hardcoded triangle
unsigned int render_test_triangle_init();
void renderer_draw_test_triangle(Shader shader);