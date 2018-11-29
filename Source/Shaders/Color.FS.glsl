#version 330

uniform vec3 color;

layout(location = 0) out vec4 out_color;

in vec3 world_position;
in vec3 world_normal;

// uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

float GetLight() 
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
	float diffuse_light = 10 * material_kd * lightColor * max(dot(world_normal, L), 0);	

	// compute specular light component
	float specular_light = material_ks * lightColor * getLight * pow(max(dot(world_normal, H), 0), material_shininess);

	float d = distance(light_position, world_position);
	float attenuation =  1 / (d * d);

	// compute light
	return ambient_light + attenuation * (diffuse_light + specular_light);
}

void main()
{
	out_color = vec4(abs(color), 1) * GetLight();
}