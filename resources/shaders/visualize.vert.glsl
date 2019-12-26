#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 color;

layout(location = 4) uniform mat4 u_transform;
layout(location = 5) uniform mat4 u_camera;

out vec2 v_tex_coord;
out vec4 v_color;

void main()
{
	gl_Position = u_camera * u_transform * vec4(position, 1.0);
	v_tex_coord = tex_coord;
	v_color = color;
}
