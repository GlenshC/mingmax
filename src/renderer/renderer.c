#include <glad/gl.h>
#include "renderer/renderer.h"
#include <stddef.h>

static unsigned int TESTTriangleVBO;


// Initialize renderer and default state
void renderer_init(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderer2D_init();
}

void renderer2D_init(void)
{
    renderer2D_batch_init();
}

// Clean up renderer
void renderer_shutdown(void)
{
    return;
}

// Per-frame render begin/end (optional for future batching)
void renderer_begin(void)
{
    return;
}

void renderer_end(void)
{
    return;
}

// Temporary: draw a hardcoded triangle
void renderer_draw_test_triangle(Shader shader)
{
    glUseProgram(shader);
    glBindVertexArray(TESTTriangleVBO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

unsigned int render_test_triangle_init()
{
    static unsigned int VBO = 0, VAO = 0;
    static const float triangle_vertices[] = {
        // Position        // Color
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Top (Red)
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Left (Green)
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // Right (Blue)
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *) (0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *) (sizeof(float) * 3));

    return VAO;
}