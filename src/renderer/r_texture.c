#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "util/memory.h"
#include "r_texture.h"

Texture texture2D_load(const char* path, const uint32_t format, int *width, int *height)
{ 
    uint32_t texID;
    int channels, x, y;
    
    stbi_set_flip_vertically_on_load(1);
    unsigned char *imageData = stbi_load(path, &x, &y, &channels, 0);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // No mipmaps, nearest neighbor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents texture bleeding at edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } 
    stbi_image_free(imageData);

    if (width)
        *width = x;
    if (height)
        *height = y;
    
    return texID; 
}

void texture_free(Texture *tex)
{
    glDeleteTextures(1, tex);
    *tex = 0;
}

void texture2D_bind(const Texture texID, const uint32_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texID);
}