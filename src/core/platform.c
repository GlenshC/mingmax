#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "platform.h"

GWindow p_window;

int platform_init(const char *title, int width, int height)
{
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Request OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and OpenGL context
    p_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!p_window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(p_window);

    // Load OpenGL functions via GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(p_window);
        glfwTerminate();
        return -1;
    }

    // Print OpenGL version
    printf("OpenGL %s\n", glGetString(GL_VERSION));

    // Set the viewport
    glViewport(0, 0, width, height);

    return 0;
}

GWindow platform_get_window()
{
    return (GWindow)p_window;
}

void platform_shutdown(void)
{
    glfwDestroyWindow(p_window);
    glfwTerminate();
}
// Main Loop Control
int platform_should_close(void)
{
   return glfwWindowShouldClose(p_window);
}

void platform_poll_events(void)
{
    glfwPollEvents();
}

void platform_swap_buffers(void)
{
    glfwSwapBuffers(p_window);
}

void platform_set_vsync(int enabled)
{
    glfwSwapInterval(enabled); 
}

double platform_get_time()
{
    return glfwGetTime();
}

void platform_sleep(double delay)
{
    glfwWaitEventsTimeout(delay);
}