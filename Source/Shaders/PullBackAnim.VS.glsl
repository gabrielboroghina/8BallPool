#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 world_position;
out vec3 world_normal;

uniform float pull_back_dist;
uniform vec3 pull_back_dir;

out vec2 texCoord;

void main()
{
    // pass v_texture_coord as output to Fragment Shader
    texCoord = v_texture_coord;

    // compute world space vertex position and normal
	world_position = (Model * vec4(v_position, 1)).xyz;
	world_normal = normalize((Model * vec4(v_normal, 1)).xyz);

    // compute position after pull back animation
    vec3 pullBack = pull_back_dir * pull_back_dist;
    mat4 PullBackTranslation = transpose(mat4(1, 0, 0, pullBack.x,
                                              0, 1, 0, pullBack.y,
                                              0, 0, 1, pullBack.z,
                                              0, 0, 0, 1));

    gl_Position = Projection * View * (PullBackTranslation * Model) * vec4(v_position, 1.0);
}
