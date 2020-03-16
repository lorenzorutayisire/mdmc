#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in float tint_index;

uniform mat4 u_camera;
uniform mat4 u_transform;

out vec2 v_uv;
flat out float v_tint_index; 

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1);
	v_uv = uv;
	v_tint_index = tint_index;
}
