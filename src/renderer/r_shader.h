#pragma once

#include "cglm/cglm.h"
// Shader handle (opaque to users, just an integer ID)
typedef unsigned int Shader; // OpenGL program ID

// Load and compile a vertex + fragment shader from file
Shader shader_create(const char* vert_path, const char* frag_path);

// Bind this shader as active
void shader_bind(Shader shader);

// Unbind current shader
void shader_unbind(void);

// Destroy (delete) shader program
void shader_destroy(Shader shader);

void shader_set_uniform_mat4(Shader shader, const char* name, const mat4 mat);
void shader_set_uniform_vec4(Shader shader, const char* name, const vec4 value);
void shader_set_uniform_vec2(Shader shader, const char* name, const vec2 value);
void shader_set_uniform_float(Shader shader, const char *name, const float value);
void shader_set_uniform_int(Shader shader, const char* name, int value);