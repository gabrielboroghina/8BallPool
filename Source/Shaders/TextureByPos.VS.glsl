#version 330

layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 v_pos_out;

void main()
{
    // pass v_pos_out as output to Fragment Shader
    v_pos_out = v_position;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
