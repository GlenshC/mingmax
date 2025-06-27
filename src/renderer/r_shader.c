#include <stdio.h>
#include <glad/gl.h>
#include "util/string.h"
#include "renderer/r_shader.h"


static unsigned int _shader_compile(const char *shaderSource, GLenum type);
static Shader _shader_link(unsigned int vertID, unsigned int fragID);

// Load and compile a vertex + fragment shader from file
Shader shader_create(const char* vertPath, const char* fragPath)
{
    char *vertSource = string_read_file(vertPath);
    char *fragSource = string_read_file(fragPath);
    
    unsigned int vertID = _shader_compile(vertSource, GL_VERTEX_SHADER);
    unsigned int fragID = _shader_compile(fragSource, GL_FRAGMENT_SHADER);
    if (vertID == 0 || fragID == 0)
    {
       glDeleteShader(vertID);
       glDeleteShader(fragID);
       return 0;
    }
    Shader shaderID = _shader_link(vertID, fragID);
    
   return shaderID;
}

// Bind this shader as active
void shader_bind(Shader shader)
{
    glUseProgram(shader);
}

// Unbind current shader
void shader_unbind(void)
{
    glUseProgram(0);
}

// Destroy (delete) shader program
void shader_destroy(Shader shader)
{
    glDeleteProgram(shader);
}

static unsigned int _shader_compile(const char *shaderSource, GLenum type)
{
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        int infoLen = 0;
        char infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, &infoLen, infoLog);
        fprintf(stderr, "Shader compilation error: %s\n", infoLog);
        return 0;
    }
    printf("Shader compiled.\n");
    return shaderID;
}

static Shader _shader_link(unsigned int vertID, unsigned int fragID)
{
    Shader shaderID = glCreateProgram();
    glAttachShader(shaderID, vertID);
    glAttachShader(shaderID, fragID);
    glLinkProgram(shaderID);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    int status;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &status);
    if (!status)
    {
        int infoLen = 0;
        char infoLog[1024];
        glGetProgramInfoLog(shaderID, 1024, &infoLen, infoLog);
        fprintf(stderr, "Shader linking error: %s\n", infoLog);
        return 0;
    }
    printf("Shader linked.\n");
    return shaderID;
}

void shader_set_uniform_mat4(Shader shader, const char* name, const mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, (float *)value);
}

void shader_set_uniform_vec2(Shader shader, const char* name, const vec2 value)
{
    glUniform2fv(glGetUniformLocation(shader, name), 1, (float *)value);
}

void shader_set_uniform_vec4(Shader shader, const char* name, const vec4 value)
{
    glUniform4fv(glGetUniformLocation(shader, name), 1, (float *)value);
}

void shader_set_uniform_float(Shader shader, const char *name, const float value)
{
    glUniform1f(glGetUniformLocation(shader, name), value);
}

void shader_set_uniform_int(Shader shader, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(shader, name), value);
}