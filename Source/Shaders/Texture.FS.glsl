#version 330 core

layout(location = 0) out vec4 out_color;
  
in vec2 TexCoord;

uniform sampler2D textureImg0;

void main()
{
    out_color = texture(textureImg0, TexCoord);
}