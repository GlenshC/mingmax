#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

layout(location = 2) in vec2 i_position;
layout(location = 3) in vec2 i_scale;
layout(location = 4) in float i_rotation;
layout(location = 5) in uint i_tex_uv_indexes;
layout(location = 6) in uint i_flags;

uniform mat4 u_projection;

uniform vec4 u_atlas_info[16]; // per texture: (atlas_w, atlas_h, tile_w, tile_h)

out vec2 v_uv;
flat out float v_tex_index;

void main() {
    vec2 tex_uv_bits = unpackHalf2x16(i_tex_uv_indexes);

    uint i_uv_index = (i_tex_uv_indexes & 0xffffu); 
    uint i_texture_id = ((i_tex_uv_indexes >> 16) & 0xffffu);

    // int i_flip_uv = int(i_flags); 
    int i_flip_uv = 0;
    
    vec4 info = u_atlas_info[i_texture_id];

    float atlas_w = info.x;
    float atlas_h = info.y;
    float tile_w  = info.z;
    float tile_h  = info.w;
    
    int columns = int(atlas_w / tile_w);
    int rows    = int(atlas_h / tile_h);

    // NEW LOGIC: Start from top-left, go down each row
    float col = mod(float(i_uv_index), float(columns));
    float row = floor(float(i_uv_index) / float(columns));
    row = float(rows - 1) - row; // Flip Y (top to bottom)

    // Calculate UV region
    vec2 uv_min = vec2(col * tile_w, row * tile_h) / vec2(atlas_w, atlas_h);
    vec2 uv_max = uv_min + vec2(tile_w, tile_h) / vec2(atlas_w, atlas_h);

    float flipped_uv_x = (i_flip_uv == 1) ? (1.0 - a_uv.x) : a_uv.x;
    v_uv = mix(uv_min, uv_max, vec2(flipped_uv_x, a_uv.y));
    
    v_tex_index = float(i_texture_id);

    // Transform
    vec2 scaled = a_pos * i_scale;
    float c = cos(i_rotation);
    float s = sin(i_rotation);
    vec2 rotated = mat2(c, -s, s, c) * scaled;
    vec2 world_pos = rotated + i_position;

    gl_Position = u_projection * vec4(world_pos, 0.0, 1.0);
}