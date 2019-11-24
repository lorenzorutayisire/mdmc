#version 450

in vec3 position;
in vec2 uv;
in float texture_id;
in vec4 tint;

uniform mat4 u_transform;
uniform mat4 u_camera;

out vec2 v_uv;
flat out float v_texture_id;
flat out vec4 v_tint;

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
	v_uv = uv;
	v_texture_id = texture_id;
	v_tint = tint;
}
