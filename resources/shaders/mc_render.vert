#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tile;
layout(location = 2) in vec2 uv;
layout(location = 3) in float tint;

uniform mat4 u_camera;
uniform mat4 u_transform;

out vec2 v_tile;
out vec2 v_uv;
flat out float v_tint; 

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1);
	v_tile = tile;
	v_uv = uv;
	v_tint = tint;
}
