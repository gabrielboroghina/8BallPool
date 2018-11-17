#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 v_pos_out;

uniform sampler2D textureImg0;

void main()
{
	vec2 texc = vec2(v_pos_out.x, v_pos_out.z);
    out_color = texture(textureImg0, texc);
}