#version 330 core

in vec2 v_uv;
flat in float v_tex_index;

out vec4 FragColor;

// Bind up to 16 2D textures (indexed via v_tex_index)
uniform sampler2D u_textures[16];

void main() {
    int index = int(v_tex_index);

    if (index < 0 || index >= 16) {
        // Invalid texture index fallback
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); // magenta = error
    } else {
        FragColor = texture(u_textures[index], v_uv);
    }
}