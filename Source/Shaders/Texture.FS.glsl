#version 330 core

layout(location = 0) out vec4 out_color;
  
in vec2 texCoord;

in vec3 world_position;
in vec3 world_normal;

// uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform sampler2D textureImg0;

void main()
{
    // compute world space vectors
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

	// define ambient light component
	float ambient_light = 0.75;
	float lightColor = 1.0;

	// compute diffuse light component
	float getLight = (dot(world_normal, L) > 0) ? 1 : 0;
	float diffuse_light = 3 * material_kd * lightColor * max(dot(world_normal, L), 0);	

	// compute specular light component
	float specular_light = material_ks * lightColor * getLight * pow(max(dot(world_normal, H), 0), material_shininess);

	float d = distance(light_position, world_position);
	float attenuation =  1 / (d * d);

	// compute light
	float light = ambient_light + attenuation * (diffuse_light + specular_light);

	// send color light output to fragment shader
    out_color = texture(textureImg0, texCoord) * light;
}